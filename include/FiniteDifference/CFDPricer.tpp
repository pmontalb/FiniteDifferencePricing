/*
 * CFDPricer.tpp
 *
 *  Created on: 13 Jun 2017
 *      Author: raiden
 */

#include <Flags.h>

namespace fdpricing
{

template <ESolverType solverType, EAdjointDifferentiation adjointDifferentiation>
CFDPricer<solverType, adjointDifferentiation>::CFDPricer(const CInputData& unaliased input,
															const CPricerSettings& unaliased settings) noexcept
		: input(input), settings(settings),
		  calculateCall(settings.calculationType == ECalculationType::All || settings.calculationType == ECalculationType::CallOnly),
		  calculatePut(settings.calculationType == ECalculationType::All || settings.calculationType == ECalculationType::PutOnly),
		  accelerateCall(input.acceleration && input.b - input.r > 0.0),
		  acceleratePut(!accelerateCall),
		  u(input, settings.fdSettings)
{
	cache.discountFactor = exp(-input.r * u.GetDt());

	if (calculateCall)
		callData.Init<adjointDifferentiation>(input.N);

	if (calculatePut)
		putData.Init<adjointDifferentiation>(input.N);

	UpdateDelegates();
}

template <ESolverType solverType, EAdjointDifferentiation adjointDifferentiation>
void CFDPricer<solverType, adjointDifferentiation>::UpdateDelegates() noexcept
{
	switch (settings.calculationType)
	{
		case ECalculationType::All:
			exerciseDelegate = &CFDPricer<solverType, adjointDifferentiation>::Exercise<ECalculationType::All>;
			smoothingDelegate = &CFDPricer<solverType, adjointDifferentiation>::PayoffSmoothing<ECalculationType::All>;
			discountDelegate = &CFDPricer<solverType, adjointDifferentiation>::RollBack<ECalculationType::All>;
			jumpConditionDelegate = &CFDPricer<solverType, adjointDifferentiation>::ApplyJumpCondition<ECalculationType::All>;
			refinedSmoothingDelegate = &CFDPricer<solverType, adjointDifferentiation>::RefinedPayoffSmoothing<ECalculationType::All>;
			break;
		case ECalculationType::CallOnly:
			exerciseDelegate = &CFDPricer<solverType, adjointDifferentiation>::Exercise<ECalculationType::CallOnly>;
			smoothingDelegate = &CFDPricer<solverType, adjointDifferentiation>::PayoffSmoothing<ECalculationType::CallOnly>;
			discountDelegate = &CFDPricer<solverType, adjointDifferentiation>::RollBack<ECalculationType::CallOnly>;
			jumpConditionDelegate = &CFDPricer<solverType, adjointDifferentiation>::ApplyJumpCondition<ECalculationType::CallOnly>;
			refinedSmoothingDelegate = &CFDPricer<solverType, adjointDifferentiation>::RefinedPayoffSmoothing<ECalculationType::CallOnly>;
			break;
		case ECalculationType::PutOnly:
			exerciseDelegate = &CFDPricer<solverType, adjointDifferentiation>::Exercise<ECalculationType::PutOnly>;
			smoothingDelegate = &CFDPricer<solverType, adjointDifferentiation>::PayoffSmoothing<ECalculationType::PutOnly>;
			discountDelegate = &CFDPricer<solverType, adjointDifferentiation>::RollBack<ECalculationType::PutOnly>;
			jumpConditionDelegate = &CFDPricer<solverType, adjointDifferentiation>::ApplyJumpCondition<ECalculationType::PutOnly>;
			refinedSmoothingDelegate = &CFDPricer<solverType, adjointDifferentiation>::RefinedPayoffSmoothing<ECalculationType::PutOnly>;
			break;
		default:
			printf("WRONG SETTINGS");
			return;
	}
}

template <ESolverType solverType, EAdjointDifferentiation adjointDifferentiation>
template<ECalculationType calculationType>
void CFDPricer<solverType, adjointDifferentiation>::Exercise()
{
	const auto& grid = u.GetGrid();

	for (size_t i = 0; i < input.N; ++i)
	{
		const double intrinsicValue = grid.Get(i) - input.K;
		switch (calculationType)
		{
			case ECalculationType::All:
				if (intrinsicValue > callData.payoff_i[i])
				{
					callData.payoff_i[i] = intrinsicValue;
					callData.ZeroGreeks<adjointDifferentiation>(i);
				}
				if (-intrinsicValue > putData.payoff_i[i])
				{
					putData.payoff_i[i] = -intrinsicValue;
					putData.ZeroGreeks<adjointDifferentiation>(i);
				}
				break;
			case ECalculationType::CallOnly:
				if (intrinsicValue > callData.payoff_i[i])
				{
					callData.payoff_i[i] = intrinsicValue;
					callData.ZeroGreeks<adjointDifferentiation>(i);
				}
				break;
			case ECalculationType::PutOnly:
				if (-intrinsicValue > putData.payoff_i[i])
				{
					putData.payoff_i[i] = -intrinsicValue;
					putData.ZeroGreeks<adjointDifferentiation>(i);
				}
				break;
			default:
				break;
		}
	}
}

template <ESolverType solverType, EAdjointDifferentiation adjointDifferentiation>
template<ECalculationType calculationType>
void CFDPricer<solverType, adjointDifferentiation>::PayoffSmoothing()
{
	const auto& grid = u.GetGrid();

	cache.T = u.GetDt();
	cache.sqrtDt = sqrt(cache.T);
	cache.sigmaSqrtDt = input.sigma * cache.sqrtDt;
	cache.growthFactor   = exp( input.b * u.GetDt());
	cache.growthFactorTimesDiscountFactor = cache.discountFactor * cache.growthFactor;
	CBlackScholes bs(input, cache);

	for (size_t i = 0; i < input.N; ++i)
	{
		bs.Update(grid.Get(i));

		SmoothingWorker<calculationType>(i, bs);
	}
}

template <ESolverType solverType, EAdjointDifferentiation adjointDifferentiation>
template<ECalculationType calculationType>
void CFDPricer<solverType, adjointDifferentiation>::RefinedPayoffSmoothing(const double previousTime, const double currentTime, const CDividend& unaliased dividend) noexcept
{
	const double dtAfter  = currentTime - dividend.time;
	const double dtBefore = dividend.time - previousTime;

	const double dfAfter = exp(-input.r * dtAfter);
	const double dfBefore = exp(-input.r * dtBefore); // TODO: maybe to division?

	const double currentDf = cache.discountFactor;

	const auto& grid = u.GetGrid();

	cache.T = dtAfter;
	cache.discountFactor = dfAfter;
	cache.sqrtDt = sqrt(cache.T);
	cache.sigmaSqrtDt = input.sigma * cache.sqrtDt;
	cache.growthFactor   = exp( input.b * dtAfter);
	cache.growthFactorTimesDiscountFactor = cache.discountFactor * cache.growthFactor;
	CBlackScholes bs(input, cache);

	for (size_t i = 0; i < input.N; ++i)
	{
		double shiftedValue = grid.Get(i) - dividend.dividend;
		if (shiftedValue <= 0.0)
			shiftedValue = 1e-7;

		bs.Update(shiftedValue);

		SmoothingWorker<calculationType>(i, bs);
	}

	cache.discountFactor = currentDf;
	if (settings.exerciseType == EExerciseType::American)
		(this->*exerciseDelegate)();

	CEvolutionOperator<solverType, adjointDifferentiation> uBefore(u, dtBefore);

	//(this->*jumpConditionDelegate)(dividend.dividend);

	if (settings.exerciseType == EExerciseType::American)
		(this->*exerciseDelegate)();

	if (calculateCall)
		uBefore.Apply(callData);
	if (calculatePut)
		uBefore.Apply(putData);

	(this->*discountDelegate)(dtBefore, dfBefore);
	if (settings.exerciseType == EExerciseType::American)
		(this->*exerciseDelegate)();
}


template <ESolverType solverType, EAdjointDifferentiation adjointDifferentiation>
template<ECalculationType calculationType>
void CFDPricer<solverType, adjointDifferentiation>::SmoothingWorker(const size_t i, CBlackScholes& unaliased bs) noexcept
{
	switch (calculationType)
	{
		case ECalculationType::All:
			#if(1) // ******* All *******

			callData.payoff_i[i] = bs.Value<EOptionType::Call>();
			putData.payoff_i[i] = bs.Value<EOptionType::Put>();

			switch (adjointDifferentiation)
			{
				case EAdjointDifferentiation::All:
					callData.vega_i[i] = putData.vega_i[i] = bs.Vega();

					callData.rho_i[i] = - u.GetDt() * callData.payoff_i[i];
					putData.rho_i[i] = - u.GetDt() * putData.payoff_i[i];
					callData.rhoBorrow_i[i] = bs.RhoBorrow<EOptionType::Call>();
					putData.rhoBorrow_i[i] = bs.RhoBorrow<EOptionType::Put>();
					break;
				case EAdjointDifferentiation::Vega:
					callData.vega_i[i] = putData.vega_i[i] = bs.Vega();
					break;
				case EAdjointDifferentiation::Rho:
					callData.rho_i[i] = - u.GetDt() * callData.payoff_i[i];
					putData.rho_i[i] = - u.GetDt() * putData.payoff_i[i];
					callData.rhoBorrow_i[i] = bs.RhoBorrow<EOptionType::Call>();
					putData.rhoBorrow_i[i] = bs.RhoBorrow<EOptionType::Put>();
					break;
				default:
					break;
			}

			#endif

			break;

		case ECalculationType::CallOnly:
			#if(1) // ****** Call *******

			callData.payoff_i[i] = bs.Value<EOptionType::Call>();

			switch (adjointDifferentiation)
			{
				case EAdjointDifferentiation::All:
					callData.vega_i[i] = bs.Vega();

					callData.rho_i[i] = - u.GetDt() * callData.payoff_i[i];
					callData.rhoBorrow_i[i] = bs.RhoBorrow<EOptionType::Call>();
					break;
				case EAdjointDifferentiation::Vega:
					callData.vega_i[i] = bs.Vega();
					break;
				case EAdjointDifferentiation::Rho:
					callData.rho_i[i] = - u.GetDt() * callData.payoff_i[i];
					callData.rhoBorrow_i[i] = bs.RhoBorrow<EOptionType::Call>();
					break;
				default:
					break;
			}

			#endif

			break;
		case ECalculationType::PutOnly:
			#if(1) // ******* Put *******

			putData.payoff_i[i] = bs.Value<EOptionType::Put>();

			switch (adjointDifferentiation)
			{
				case EAdjointDifferentiation::All:
					putData.vega_i[i] = bs.Vega();

					putData.rho_i[i] = - u.GetDt() * putData.payoff_i[i];
					putData.rhoBorrow_i[i] = bs.RhoBorrow<EOptionType::Put>();
					break;
				case EAdjointDifferentiation::Vega:
					putData.vega_i[i] = bs.Vega();
					break;
				case EAdjointDifferentiation::Rho:
					putData.rho_i[i] = - u.GetDt() * putData.payoff_i[i];
					putData.rhoBorrow_i[i] = bs.RhoBorrow<EOptionType::Put>();
					break;
				default:
					break;
			}

			#endif

			break;
		default:
			break;
	}
}


template <ESolverType solverType, EAdjointDifferentiation adjointDifferentiation>
template<ECalculationType calculationType>
void CFDPricer<solverType, adjointDifferentiation>::RollBack(const double dt, const double df)
{
	for (size_t i = 0; i < input.N; ++i)
	{
		switch (calculationType)
		{
			case ECalculationType::All:
				callData.payoff_i[i] *= df;
				putData.payoff_i[i] *= df;

				switch (adjointDifferentiation)
				{
					case EAdjointDifferentiation::All:
						callData.vega_i[i] *= df;
						callData.rhoBorrow_i[i] *= df;
						callData.rho_i[i] = -dt * callData.payoff_i[i] + callData.rho_i[i] * df;

						putData.vega_i[i] *= df;
						putData.rhoBorrow_i[i] *= df;
						putData.rho_i[i] = -dt * putData.payoff_i[i] + putData.rho_i[i] * df;
						break;
					case EAdjointDifferentiation::Vega:
						callData.vega_i[i] *= df;
						putData.vega_i[i] *= df;
						break;
					case EAdjointDifferentiation::Rho:
						callData.rhoBorrow_i[i] *= df;
						callData.rho_i[i] = -dt * callData.payoff_i[i] + callData.rho_i[i] * df;
						putData.rhoBorrow_i[i] *= df;
						putData.rho_i[i] = -dt * putData.payoff_i[i] + putData.rho_i[i] * df;
						break;
					default:
						break;
				}

				break;

			case ECalculationType::CallOnly:
				callData.payoff_i[i] *= df;

				switch (adjointDifferentiation)
				{
					case EAdjointDifferentiation::All:
						callData.vega_i[i] *= df;
						callData.rhoBorrow_i[i] *= df;
						callData.rho_i[i] = -dt * callData.payoff_i[i] + callData.rho_i[i] * df;
						break;
					case EAdjointDifferentiation::Vega:
						callData.vega_i[i] *= df;
						break;
					case EAdjointDifferentiation::Rho:
						callData.rhoBorrow_i[i] *= df;
						callData.rho_i[i] = -dt * callData.payoff_i[i] + callData.rho_i[i] * df;
						break;
					default:
						break;
				}

				break;
			case ECalculationType::PutOnly:
				putData.payoff_i[i] *= df;

				switch (adjointDifferentiation)
				{
					case EAdjointDifferentiation::All:
						putData.vega_i[i] *= df;
						putData.rhoBorrow_i[i] *= df;
						putData.rho_i[i] = -dt * putData.payoff_i[i] + putData.rho_i[i] * df;
						break;
					case EAdjointDifferentiation::Vega:
						putData.vega_i[i] *= df;
						break;
					case EAdjointDifferentiation::Rho:
						putData.rhoBorrow_i[i] *= df;
						putData.rho_i[i] = -dt * putData.payoff_i[i] + putData.rho_i[i] * df;
						break;
					default:
						break;
				}

				break;
			default:
				break;
		}
	}
}


template <ESolverType solverType, EAdjointDifferentiation adjointDifferentiation>
void CFDPricer<solverType, adjointDifferentiation>::BackwardInduction() noexcept
{
	if (calculateCall)
		u.Apply(callData);
	if (calculatePut)
		u.Apply(putData);

	(this->*discountDelegate)(u.GetDt(), cache.discountFactor);

	if (settings.exerciseType == EExerciseType::American)
		(this->*exerciseDelegate)();
}


template <ESolverType solverType, EAdjointDifferentiation adjointDifferentiation>
void CFDPricer<solverType, adjointDifferentiation>::RefinedBackwardInduction(const double previousTime, const double currentTime, const CDividend& unaliased dividend) noexcept
{
	const double dtAfter  = currentTime - dividend.time;
	const double dtBefore = dividend.time - previousTime;

	// if a dividend occurs exactly on a time grid point, there's no need to calculate additional operators
	if (fabs(dtAfter) <= 1e-12 || fabs(dtBefore) <= 1e-12)
	{
		BackwardInduction();
		(this->*jumpConditionDelegate)(dividend.dividend);

		if (settings.exerciseType == EExerciseType::American)
			(this->*exerciseDelegate)();

		return;
	}

	const double dfAfter = exp(-input.r * dtAfter);
	const double dfBefore = exp(-input.r * dtBefore); // TODO: maybe to division?

#ifdef DEBUG

	if (dtAfter <= 0.0)
	{
		printf("*** WRONG DIVIDEND TIME ***\n");
		return;
	}
	if (dtBefore <= 0.0)
	{
		printf("*** WRONG DIVIDEND TIME ***\n");
		return;
	}

#endif

	CEvolutionOperator<solverType, adjointDifferentiation> uAfter(u, dtAfter);
	CEvolutionOperator<solverType, adjointDifferentiation> uBefore(u, dtBefore);

	if (calculateCall)
		uAfter.Apply(callData);
	if (calculatePut)
		uAfter.Apply(putData);

	(this->*discountDelegate)(dtAfter, dfAfter);

	if (settings.exerciseType == EExerciseType::American)
		(this->*exerciseDelegate)();

	(this->*jumpConditionDelegate)(dividend.dividend);

	if (settings.exerciseType == EExerciseType::American)
		(this->*exerciseDelegate)();

	if (calculateCall)
		uBefore.Apply(callData);
	if (calculatePut)
		uBefore.Apply(putData);

	(this->*discountDelegate)(dtBefore, dfBefore);
	if (settings.exerciseType == EExerciseType::American)
		(this->*exerciseDelegate)();
}

template <ESolverType solverType, EAdjointDifferentiation adjointDifferentiation>
template<ECalculationType calculationType>
void CFDPricer<solverType, adjointDifferentiation>::ApplyJumpCondition(const double shift) noexcept
{
#ifdef DEBUG
	if (shift <= 0.0)
	{
		printf("*** WRONG DIVIDEND ***\n");
		return;
	}
#endif

	const auto& grid = u.GetGrid();

	size_t j = input.N - 1;
	double w0 = shift / (grid.Get(input.N - 1) - grid.Get(input.N - 2));  // weight to attribute at point j - 1
	double w1 = 1.0 - w0;
	for (size_t i = input.N; i --> 0 ;)
	{
		const double shiftedValue = grid.Get(i) - shift;
		if (shiftedValue <= 0.0)
			return;

		// since grid is monotonically increasing, this while-loop ensures
		// that grid[j] and grid[j + 1] bracket shiftedValue
		// Also, shiftedValue < grid[N - 1], so j \in { 0, ..., N - 1 }
		while (j > 1 && grid.Get(j - 1) >= shiftedValue)
			--j;

		w0 = (grid.Get(j) - shiftedValue) / (grid.Get(j) - grid.Get(j - 1));  // weight to attribute at point j - 1
		w1 = 1.0 - w0;

#ifdef DEBUG
		if (j != 1 && (w0 < 0.0 || w1 < 0.0))
		{
			printf("*** NEGATIVE WEIGHTS ***\n");
			return;
		}
#endif

		switch (calculationType)
		{
			case ECalculationType::All:
				callData.Lerp<adjointDifferentiation>(i, j, w0, w1);
				putData.Lerp<adjointDifferentiation>(i, j, w0, w1);
				break;
			case ECalculationType::CallOnly:
				callData.Lerp<adjointDifferentiation>(i, j, w0, w1);
				break;
			case ECalculationType::PutOnly:
				putData.Lerp<adjointDifferentiation>(i, j, w0, w1);
				break;
			default:
				break;
		}
	}
}

template <ESolverType solverType, EAdjointDifferentiation adjointDifferentiation>
void CFDPricer<solverType, adjointDifferentiation>::PayoffInitialise(size_t& unaliased m) noexcept
{
	if (m != input.M)
		return;

	if (input.smoothing)
	{
		(this->*smoothingDelegate)();
		--m;
	}
	else
		(this->*exerciseDelegate)();
}

template <ESolverType solverType, EAdjointDifferentiation adjointDifferentiation>
void CFDPricer<solverType, adjointDifferentiation>::RefinedPayoffInitialise(size_t& unaliased m) noexcept
{
	if (m != input.M)
		return;

	double currentTime = input.T;
	double previousTime = currentTime - u.GetDt();
	size_t divIdx = input.dividends.size() - 1;

	if (input.smoothing)
	{
		(this->*refinedSmoothingDelegate)(previousTime, currentTime, input.dividends[divIdx]);
		--m;
	}
	else
	{
		(this->*exerciseDelegate)();
		RefinedBackwardInduction(previousTime, currentTime, input.dividends[divIdx]);
	}
}


template <ESolverType solverType, EAdjointDifferentiation adjointDifferentiation>
void CFDPricer<solverType, adjointDifferentiation>::Price(COutputData& unaliased callOutput, COutputData& unaliased putOutput) noexcept
{
	size_t m = input.M;
	// TODO: acceleration!
	{
		// accelerate
		// modify m
	}

	std::array<double, 6> callLeavesDt, putLeavesDt;

	if (input.dividends.size())
	{
		double currentTime = m * u.GetDt();
		double previousTime = currentTime - u.GetDt();
		size_t divIdx = input.dividends.size() - 1;

		if (input.dividends[divIdx].time >= previousTime && input.dividends[divIdx].time < currentTime)
			RefinedPayoffInitialise(m);
		else
			PayoffInitialise(m);

		for (; m --> 0 ;)
		{
			currentTime = previousTime;
			previousTime -= u.GetDt();

			if (input.dividends[divIdx].time >= previousTime && input.dividends[divIdx].time < currentTime)
			{
				RefinedBackwardInduction(previousTime, currentTime, input.dividends[divIdx]);
				if (divIdx != 0)
					--divIdx;
			}
			else
				BackwardInduction();

			if (m < 3)
				SaveLeaves(m, callLeavesDt, putLeavesDt);
		}
	}
	else
	{
		PayoffInitialise(m);

		for (; m --> 0 ;)
		{
			BackwardInduction();

			if (m < 3)
				SaveLeaves(m, callLeavesDt, putLeavesDt);
		}
	}

	ComputeGreeks(callOutput, putOutput, callLeavesDt, putLeavesDt);
	SetOutput(callOutput, putOutput);
}

template <ESolverType solverType, EAdjointDifferentiation adjointDifferentiation>
void CFDPricer<solverType, adjointDifferentiation>::SaveLeaves(const size_t m, std::array<double, 6>& unaliased callLeavesDt, std::array<double, 6>& unaliased putLeavesDt) const noexcept
{
	if (m == 0)
		return;

	const size_t idx = 3 * (m - 1);
	if (calculateCall)
	{
		callLeavesDt[idx]     = callData.payoff_i[(input.N >> 1) - 1];
		callLeavesDt[idx + 1] = callData.payoff_i[input.N >> 1];
		callLeavesDt[idx + 2] = callData.payoff_i[(input.N >> 1) + 1];
	}

	if (calculatePut)
	{
		putLeavesDt[idx]     = putData.payoff_i[(input.N >> 1) - 1];
		putLeavesDt[idx + 1] = putData.payoff_i[input.N >> 1];
		putLeavesDt[idx + 2] = putData.payoff_i[(input.N >> 1) + 1];
	}
}

template <ESolverType solverType, EAdjointDifferentiation adjointDifferentiation>
void CFDPricer<solverType, adjointDifferentiation>::SetOutput(COutputData& unaliased callOutput, COutputData& unaliased putOutput) const noexcept
{
	if (calculateCall)
	{
		callOutput.price = callData.payoff_i[input.N >> 1];

		switch (adjointDifferentiation)
		{
			case EAdjointDifferentiation::All:
				callOutput.vega = callData.vega_i[input.N >> 1];
				callOutput.rhoBorrow = callData.rhoBorrow_i[input.N >> 1];
				callOutput.rho = callData.rho_i[input.N >> 1];
				break;
			case EAdjointDifferentiation::Vega:
				callOutput.vega = callData.vega_i[input.N >> 1];
				break;
			case EAdjointDifferentiation::Rho:
				callOutput.rhoBorrow = callData.rhoBorrow_i[input.N >> 1];
				callOutput.rho = callData.rho_i[input.N >> 1];
				break;
			default:
				break;
		}

	}

	if (calculatePut)
	{
		putOutput.price = putData.payoff_i[input.N >> 1];

		switch (adjointDifferentiation)
		{
			case EAdjointDifferentiation::All:
				putOutput.vega = putData.vega_i[input.N >> 1];
				putOutput.rhoBorrow = putData.rhoBorrow_i[input.N >> 1];
				putOutput.rho = putData.rho_i[input.N >> 1];
				break;
			case EAdjointDifferentiation::Vega:
				putOutput.vega = putData.vega_i[input.N >> 1];
				break;
			case EAdjointDifferentiation::Rho:
				putOutput.rhoBorrow = putData.rhoBorrow_i[input.N >> 1];
				putOutput.rho = putData.rho_i[input.N >> 1];
				break;
			default:
				break;
		}
	}
}

template <ESolverType solverType, EAdjointDifferentiation adjointDifferentiation>
void CFDPricer<solverType, adjointDifferentiation>::ComputeGreeks(COutputData& unaliased callOutput, COutputData& unaliased putOutput, const std::array<double, 6>& unaliased callLeavesDt, const std::array<double, 6>& unaliased putLeavesDt) const noexcept
{
	const auto& grid = u.GetGrid();

	const double dxPlus  = grid.Get((input.N >> 1) + 1) - grid.Get((input.N >> 1));
	const double dxMinus = grid.Get((input.N >> 1))     - grid.Get((input.N >> 1) - 1);
	const double dx = dxPlus + dxMinus;

	const double b0 = 1.0 / (dx * dxMinus);
	const double b2 = 1.0 / (dx * dxPlus);
	const double b1 = -b0 - b2;

	const double a0 = -dxPlus * b0;
	const double a2 =  dxMinus * b2;
	const double a1 = -a0 - a2;

	double oneOverHalfDt = 1.0 / u.GetDt();
	const double oneOverDt2 = oneOverHalfDt * oneOverHalfDt;
	oneOverHalfDt = .5;

	if (calculateCall)
	{
		callOutput.delta = a0 * callData.payoff_i[(input.N >> 1) - 1] + a1 * callData.payoff_i[input.N >> 1] + a2 * callData.payoff_i[(input.N >> 1) + 1];
		callOutput.gamma = 2.0 * (b0 * callData.payoff_i[(input.N >> 1) - 1] + b1 * callData.payoff_i[input.N >> 1] + b2 * callData.payoff_i[(input.N >> 1) + 1]);

		callOutput.theta  = oneOverHalfDt  * (callLeavesDt[4]                         - callData.payoff_i[input.N >> 1]);
		callOutput.theta2 = oneOverDt2 *     (callLeavesDt[4] - 2.0 * callLeavesDt[1] + callData.payoff_i[input.N >> 1]);

		const double delta_2dt = a0 * callLeavesDt[3] + a1 * callLeavesDt[4] + a2 * callLeavesDt[5];
		callOutput.charm = oneOverHalfDt * (delta_2dt - callOutput.delta);
	}

	if (calculatePut)
	{
		putOutput.delta = a0 * putData.payoff_i[(input.N >> 1) - 1] + a1 * putData.payoff_i[input.N >> 1] + a2 * putData.payoff_i[(input.N >> 1) + 1];
		putOutput.gamma = 2.0 * (b0 * putData.payoff_i[(input.N >> 1) - 1] + b1 * putData.payoff_i[input.N >> 1] + b2 * putData.payoff_i[(input.N >> 1) + 1]);

		putOutput.theta  = oneOverHalfDt  * (putLeavesDt[4]                        - putData.payoff_i[input.N >> 1]);
		putOutput.theta2 = oneOverDt2 *     (putLeavesDt[4] - 2.0 * putLeavesDt[1] + putData.payoff_i[input.N >> 1]);

		const double delta_2dt = a0 * putLeavesDt[3] + a1 * putLeavesDt[4] + a2 * putLeavesDt[5];
		putOutput.charm = oneOverHalfDt * (delta_2dt - putOutput.delta);
	}
}

}
