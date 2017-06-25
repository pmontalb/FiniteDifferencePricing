/*
 * CFDPricer.h
 *
 *  Created on: 11 Jun 2017
 *      Author: raiden
 */

#ifndef FINITEDIFFERENCE_CFDPRICER_H_
#define FINITEDIFFERENCE_CFDPRICER_H_

#include <FiniteDifference/CEvolutionOperator.h>
#include <Data/ECalculationType.h>
#include <Data/EAdjointDifferentiation.h>
#include <Data/EExerciseType.h>
#include <Data/CCacheData.h>
#include <Data/COutputData.h>
#include <BlackScholes/CBlackScholes.h>
#include <Flags.h>

namespace fdpricing
{

struct CPricerSettings
{
	EExerciseType exerciseType = EExerciseType::American;
	ECalculationType calculationType = ECalculationType::All;
	CFiniteDifferenceSettings fdSettings = CFiniteDifferenceSettings();
};

template <ESolverType solverType=ESolverType::CrankNicolson,
		EGridType gridType=EGridType::Adaptive,
		EAdjointDifferentiation adjointDifferentiation=EAdjointDifferentiation::All>
class CFDPricer
{
public:
	CFDPricer(const CInputData& unaliased input, const CPricerSettings& unaliased settings) noexcept;

	/**
	 * Do not copy this class: instead one should work out how to update quantities when sigma/T changes
	 */
	CFDPricer(const CFDPricer& rhs) = delete;
	CFDPricer(const CFDPricer&& rhs) = delete;
	CFDPricer& operator=(const CFDPricer& rhs) = delete;
	CFDPricer& operator=(const CFDPricer&& rhs) = delete;

	virtual ~CFDPricer() = default;

	void Price(COutputData& unaliased callOutput, COutputData& unaliased putOutput) noexcept;

private:
	const CInputData& unaliased input;
	const CPricerSettings& unaliased settings;
	const bool calculateCall;
	const bool calculatePut;

	/**
	 * Loops through the dividends: it's updated once the BI is done
	 */
	size_t divIdx;

	/**
	 * Used for storing discount factor and for additional Black-Scholes caching
	 */
	details::CCacheData cache;

	CPayoffData callData;
	CPayoffData putData;

	typedef CFDPricer<solverType, gridType, adjointDifferentiation> Pricer;
	typedef CEvolutionOperator<solverType, gridType, adjointDifferentiation> Operator;
	/**
	 * Space-Time Discretization operator
	 */
	Operator u;

	/**
	 * This defines a vector of 6 elements:
	 *
	 *
	 *   x		  x		...
	 *
	 *   x		  x		...
	 *
	 *   2		  5		...		N / 2 + 1
	 *
	 *   1		  4		...		N / 2
	 *
	 *   0		  3		...		N / 2 - 1
	 *
	 *   x		  x		...
	 *
	 *   x 		  x		...
	 *
	 *   x		  x		...
	 *
	 *
	 * t = 0	t = dt
	 *
	 * These vectors are used for calculating theta, theta2 and charm
	 */
	typedef std::array<double, 6> TimeLeaves;

	/**
	 * These delegates are function pointers that point to the correct templated version. Defined ad initialization and from UpdateDelegates
	 */
	typedef void (Pricer::*InitializerDelegate)();
	InitializerDelegate exerciseDelegate;
	InitializerDelegate smoothingDelegate;

	typedef void (Pricer::*AccelerationDelegate)(size_t& unaliased m,
			COutputData& unaliased callOutput, COutputData& unaliased putOutput, TimeLeaves& unaliased callLeavesDt, TimeLeaves& unaliased putLeavesDt);
	AccelerationDelegate accelerationDelegate;

	typedef void (Pricer::*RollBackDelegate)(const double dt, const double df);
	RollBackDelegate discountDelegate;

	typedef void (Pricer::*JumpConditionDelegate)(const double shift);
	JumpConditionDelegate jumpConditionDelegate;

	typedef void (Pricer::*RefinedSmoothingDelegate)(const double previousTime, const double currentTime, const CDividend& unaliased dividend);
	RefinedSmoothingDelegate refinedSmoothingDelegate;

	typedef void (Pricer::*ApplyOperatorDelegate)(Operator& unaliased u);
	ApplyOperatorDelegate applyOperatorDelegate;

	typedef void (Pricer::*SetOutputDelegate)(COutputData& unaliased callOutput, COutputData& unaliased putOutput) const;
	SetOutputDelegate setOutputDelegate;

	typedef void (Pricer::*ComputeGreeksDelegate)(COutputData& unaliased callOutput, COutputData& unaliased putOutput, const TimeLeaves& unaliased callLeavesDt, const TimeLeaves& unaliased putLeavesDt) const;
	ComputeGreeksDelegate computeGreeksDelegate;

	void UpdateDelegates(const CPricerSettings& unaliased settings, const bool accelerateCall, const bool acceleratePut) noexcept;

	/**
	 * Define the initial condition
	 */
	void PayoffInitialise(size_t& unaliased m) noexcept;
	void RefinedPayoffInitialise(size_t& unaliased m) noexcept;

	/**
	 * Apply the american exercise condition
	 */
	template<ECalculationType calculationType>
	void Exercise();

	/**
	 * Advance backward in time applying the evolution operator
	 */
	template<ECalculationType calculationType>
	void ApplyOperator(Operator& unaliased u);

	/**
	 * Accelerate the relevant option using Black-Scholes and synchronize the other one at the same time grid point
	 */
	template<ECalculationType calculationType>
	void Accelerate(size_t& unaliased m, COutputData& unaliased callOutput, COutputData& unaliased putOutput, TimeLeaves& unaliased callLeavesDt, TimeLeaves& unaliased putLeavesDt);

	/**
	 * Main Backward Induction routine that advance (backwards) from end to start
	 */
	void PriceUntil(size_t start, const size_t end, TimeLeaves& unaliased callLeavesDt, TimeLeaves& unaliased putLeavesDt) noexcept;

	template<ECalculationType calculationType>
	void PayoffSmoothing();
	template<ECalculationType calculationType>
	void RefinedPayoffSmoothing(const double previousTime, const double currentTime, const CDividend& unaliased dividend) noexcept;
	template<ECalculationType calculationType>
	void SmoothingWorker(const size_t i, CBlackScholes& unaliased bs, const double dt) noexcept;

	/**
	 * Apply discount factor
	 */
	template<ECalculationType calculationType>
	void RollBack(const double dt, const double df);

	void BackwardInduction() noexcept;
	void RefinedBackwardInduction(const double previousTime, const double currentTime, const CDividend& unaliased dividend) noexcept;

	/**
	 * The jump condition is given from the cash dividends. If they fall off the grid, a linear interpolation is used
	 */
	template<ECalculationType calculationType>
	void ApplyJumpCondition(const double shift) noexcept;

	template <ECalculationType>
	void SetOutput(COutputData& unaliased callOutput, COutputData& unaliased putOutput) const;
	void SaveLeaves(const size_t m, TimeLeaves& unaliased callLeavesDt, TimeLeaves& unaliased putLeavesDt) const noexcept;

	/**
	 * Usual 2nd order FD derivatives: http://www.m-hikari.com/ijma/ijma-password-2009/ijma-password17-20-2009/bhadauriaIJMA17-20-2009.pdf
	 */
	template <ECalculationType>
	void ComputeGreeks(COutputData& unaliased callOutput, COutputData& unaliased putOutput, const TimeLeaves& unaliased callLeavesDt, const TimeLeaves& unaliased putLeavesDt) const;
};

} /* namespace fdpricing */

#include <FiniteDifference/CFDPricer.tpp>

#endif /* FINITEDIFFERENCE_CFDPRICER_H_ */
