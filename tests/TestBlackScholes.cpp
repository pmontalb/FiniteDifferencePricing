/*
 * TestBlackScholes.cpp
 *
 *  Created on: 18 Dec 2016
 *      Author: raiden
 */

#include <cmath>
#include <gtest/gtest.h>
#include <BlackScholes/CBlackScholes.h>

using namespace fdpricing;


TEST (BlackScholesTest, PutCallParity)
{
	CInputData input;
	input.S = 100;
	input.K = 100;
	input.r = .05;
	input.b = .02;
	input.sigma = .3;
	input.T = 2;

	CBlackScholes bs(input);

	for (int i = 0; i < 20; ++i)
	{
		double S = 80 * (1 + .025 * i);
		bs.Update(S);
		double C = bs.Value<EOptionType::Call>();
		double P = bs.Value<EOptionType::Put>();
		double F = (S * exp(input.b * input.T) - input.K) * exp(-input.r * input.T);

		ASSERT_FLOAT_EQ(F, C - P);
	}
}

TEST (BlackScholesTest, PutCallParityDelta)
{
	CInputData input;
	input.S = 100;
	input.K = 100;
	input.r = .05;
	input.b = .02;
	input.sigma = .3;
	input.T = 2;

	CBlackScholes bs(input);

	for (int i = 0; i < 20; ++i)
	{
		bs.Update(80 * (1 + .025 * i));
		double C = bs.Delta<EOptionType::Call>();
		double P = bs.Delta<EOptionType::Put>();
		double expected = exp(input.b * input.T) * exp(-input.r * input.T);

		ASSERT_FLOAT_EQ(expected, C - P);
	}
}

TEST (BlackScholesTest, Gamma)
{
	CInputData input;
	input.S = 100;
	input.K = 100;
	input.r = .05;
	input.b = .02;
	input.sigma = .3;
	input.T = 2;

	CBlackScholes bs(input);

	double G = bs.Gamma();
	double expected = 0.0084494501;

	ASSERT_FLOAT_EQ(expected, G);
}

TEST (BlackScholesTest, Vega)
{
	CInputData input;
	input.S = 100;
	input.K = 100;
	input.r = .05;
	input.b = .02;
	input.sigma = .3;
	input.T = 2;

	CBlackScholes bs(input);

	double V = bs.Vega();
	double expected = 50.696697;

	ASSERT_FLOAT_EQ(expected, V);
}


