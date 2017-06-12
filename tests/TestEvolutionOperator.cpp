/*
 * TestGrid.cpp
 *
 *  Created on: 11 Jun 2017
 *      Author: raiden
 */


#include <gtest/gtest.h>
#include <FiniteDifference/CEvolutionOperator.h>

using namespace fdpricing;


TEST (TridiagonalOperator, ExplicitEulerBase)
{
	CInputData inputData;
	inputData.S = 100.0;
	inputData.b = .002;
	inputData.sigma = .03;
	inputData.N = 20;
	inputData.T = 1.0;
	inputData.M = 10;

	CFiniteDifferenceSettings settings;
	CEvolutionOperator<ESolverType::ExplicitEuler, EAdjointDifferentiation::None> u(inputData, settings);

	CPayoffData payoffData;
	payoffData.payoff_i.resize(inputData.N + 1);
	for (size_t i = 0; i < payoffData.payoff_i.size(); ++i)
		payoffData.payoff_i[i] = 5.0 + i;
	payoffData.payoff_i[inputData.N / 2] = 2.0;
	std::vector<double> xCopy(payoffData.payoff_i);

	u.Apply(payoffData);

	CGrid grid(inputData.S, 1e-3 * inputData.S, 10.0 * inputData.S, EGridType::Adaptive, inputData.N);
	const double dt = inputData.T / inputData.M;
	for (size_t i = 1; i <= inputData.N - 1; ++i)
	{
		const double dxPlus  = grid.Get(i + 1) - grid.Get(i);
		const double dxMinus = grid.Get(i)     - grid.Get(i - 1);
		const double dx = dxPlus + dxMinus;

		const double drift = inputData.b * grid.Get(i);
		const double volatility = inputData.sigma * inputData.sigma * grid.Get(i) * grid.Get(i);

		const double mMinus = dt * (-dxPlus * drift + volatility) / (dxMinus * dx);
		const double mPlus  = dt * (dxMinus * drift + volatility) / (dxPlus  * dx);
		const double mZero  = 1.0 - mPlus - mMinus;

		const double x_i = mMinus * xCopy[i - 1] + mZero * xCopy[i] + mPlus * xCopy[i + 1];
		ASSERT_NEAR(x_i, payoffData.payoff_i[i], 1e-12);
	}
}


TEST (TridiagonalOperator, ExplicitEulerVega)
{
	const double dSigma = 1e-4;

	CInputData inputData;
	inputData.S = 100.0;
	inputData.b = .002;
	inputData.sigma = .03;
	inputData.N = 128;
	inputData.T = 1.0;
	inputData.M = 100;

	CFiniteDifferenceSettings settings;
	CEvolutionOperator<ESolverType::ExplicitEuler, EAdjointDifferentiation::Vega> u(inputData, settings);

	CInputData inputDataPlus(inputData);
	inputDataPlus.sigma += dSigma;
	CEvolutionOperator<ESolverType::ExplicitEuler, EAdjointDifferentiation::None> uPlus(inputDataPlus, settings);

	CInputData inputDataMinus(inputData);
	inputDataMinus.sigma -= dSigma;
	CEvolutionOperator<ESolverType::ExplicitEuler, EAdjointDifferentiation::None> uMinus(inputDataMinus, settings);

	CPayoffData payoffData;
	payoffData.payoff_i.resize(inputData.N + 1, 0.0);
	payoffData.payoff_i[64] = 1.0;
	CPayoffData payoffDataPlus(payoffData), payoffDataMinus(payoffData);

	payoffData.vega_i.resize(inputData.N + 1, 0.0);
	u.Apply(payoffData);

	uPlus.Apply(payoffDataPlus);
	uMinus.Apply(payoffDataMinus);

	std::vector<double> vega(inputData.N + 1);
	ASSERT_EQ(vega.size(), payoffData.payoff_i.size());

	for (size_t i = 0; i <= inputData.N; ++i)
	{
		vega[i] = 1.0 / (2.0 * dSigma) * (payoffDataPlus.payoff_i[i] - payoffDataMinus.payoff_i[i]);
		ASSERT_NEAR(vega[i], payoffData.vega_i[i], 1e-6);
	}
}


TEST (TridiagonalOperator, ExplicitEulerRhoBorrow)
{
	const double db = 1e-4;

	CInputData inputData;
	inputData.S = 100.0;
	inputData.b = .002;
	inputData.sigma = .03;
	inputData.N = 128;
	inputData.T = 1.0;
	inputData.M = 100;

	CFiniteDifferenceSettings settings;
	CEvolutionOperator<ESolverType::ExplicitEuler, EAdjointDifferentiation::Rho> u(inputData, settings);

	CInputData inputDataPlus(inputData);
	inputDataPlus.b += db;
	CEvolutionOperator<ESolverType::ExplicitEuler, EAdjointDifferentiation::None> uPlus(inputDataPlus, settings);

	CInputData inputDataMinus(inputData);
	inputDataMinus.b -= db;
	CEvolutionOperator<ESolverType::ExplicitEuler, EAdjointDifferentiation::None> uMinus(inputDataMinus, settings);

	CPayoffData payoffData;
	payoffData.payoff_i.resize(inputData.N + 1, 0.0);
	payoffData.payoff_i[64] = 1.0;
	CPayoffData payoffDataPlus(payoffData), payoffDataMinus(payoffData);

	payoffData.rhoBorrow_i.resize(inputData.N + 1, 0.0);
	u.Apply(payoffData);

	uPlus.Apply(payoffDataPlus);
	uMinus.Apply(payoffDataMinus);

	std::vector<double> rhoBorrow(inputData.N + 1);
	ASSERT_EQ(rhoBorrow.size(), payoffData.payoff_i.size());

	for (size_t i = 0; i <= inputData.N; ++i)
	{
		rhoBorrow[i] = 1.0 / (2.0 * db) * (payoffDataPlus.payoff_i[i] - payoffDataMinus.payoff_i[i]);
		ASSERT_NEAR(rhoBorrow[i], payoffData.rhoBorrow_i[i], 1e-6);
	}
}
