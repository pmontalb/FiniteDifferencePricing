/*
 * TestGrid.cpp
 *
 *  Created on: 11 Jun 2017
 *      Author: raiden
 */


#include <gtest/gtest.h>
#include <FiniteDifference/CEvolutionOperator.h>

using namespace fdpricing;


TEST (TridiagonalOperator, ExplicitEuler)
{
	CInputData inputData;
	inputData.S = 100.0;
	inputData.b = .002;
	inputData.sigma = .03;
	inputData.N = 128;
	inputData.T = 1.0;
	inputData.M = 100;

	CFiniteDifferenceSettings settings;
	CEvolutionOperator<ESolverType::ExplicitEuler> u(inputData, settings);

	std::vector<double> x(inputData.N + 1, 0.0);
	x[64] = 1.0;
	std::vector<double> xCopy(x);

	u.Apply(x);

	for (size_t i = 0; i < x.size(); ++i)
	{
		if (i != 63 && i != 64 && i != 65)
			ASSERT_TRUE(x[i] == 0);
		else
			ASSERT_TRUE(x[i] != 0);
	}

	CGrid grid(inputData.S, 1e-3 * inputData.S, 10.0 * inputData.S, EGridType::Adaptive, inputData.N);
	const double dt = inputData.T / inputData.M;
	for (size_t i = 63; i <= 65; ++i)
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
		ASSERT_NEAR(x_i, x[i], 1e-12);
	}
}
