/*
 * TestFDPricer.cpp
 *
 *  Created on: 14 Jun 2017
 *      Author: raiden
 */

#include <cmath>
#include <gtest/gtest.h>
#include <BlackScholes/CBlackScholes.h>
#include <FiniteDifference/CFDPricer.h>

using namespace fdpricing;


TEST (FDTest, BlackScholesConsistency)
{
	CInputData input;
	input.smoothing = true;
	input.S = 100;
	input.K = 100;
	input.r = .05;
	input.b = .02;
	input.sigma = .3;
	input.T = 2;
	input.N = 129;
	input.M = 80;

	CBlackScholes bs(input);

	CPricerSettings settings;
	settings.exerciseType = EExerciseType::European;
	settings.fdSettings.gridType = EGridType::Adaptive;
	CFDPricer<ESolverType::CrankNicolson, EAdjointDifferentiation::All> pricer(input, settings);

	double bsC = bs.Value<EOptionType::Call>();
	double bsP = bs.Value<EOptionType::Put>();
	double bsV = bs.Vega();
	double bsRBC = bs.RhoBorrow<EOptionType::Call>();
	double bsRBP = bs.RhoBorrow<EOptionType::Put>();
	double bsRC = bs.Rho<EOptionType::Call>();
	double bsRP = bs.Rho<EOptionType::Put>();
	double bsDC = bs.Delta<EOptionType::Call>();
	double bsDP = bs.Delta<EOptionType::Put>();
	double bsG = bs.Gamma();

	COutputData callOutput, putOutput;
	pricer.Price(callOutput, putOutput);

	ASSERT_LE(fabs(bsC - callOutput.price), 0.00281162);
	ASSERT_LE(fabs(bsP - putOutput.price) , 0.00207582);

	ASSERT_LE(fabs(bsV - callOutput.vega), 0.438633);
	ASSERT_LE(fabs(bsV - putOutput.vega), 0.440376);

	ASSERT_LE(fabs(bsRC - callOutput.rho), 0.00562323);
	ASSERT_LE(fabs(bsRP - putOutput.rho), 0.00415164);

	ASSERT_LE(fabs(bsRBC - callOutput.rhoBorrow), 0.034);
	ASSERT_LE(fabs(bsRBP - putOutput.rhoBorrow), 0.0172);

	ASSERT_LE(fabs(bsRBC - callOutput.rhoBorrow), 0.034);
	ASSERT_LE(fabs(bsRBP - putOutput.rhoBorrow), 0.0172);

	ASSERT_LE(fabs(bsDC - callOutput.delta), 0.000174);
	ASSERT_LE(fabs(bsDP - putOutput.delta), 9.8392e-05);

	ASSERT_LE(fabs(bsG - callOutput.gamma), 5.40176e-06);
	ASSERT_LE(fabs(bsG - putOutput.gamma), 5.63647e-07);

	// TODO: implement bs theta and compare it
}

TEST (FDTest, Convergence)
{
	CInputData input;
	input.smoothing = true;
	input.S = 100;
	input.K = 100;
	input.r = .05;
	input.b = .02;
	input.sigma = .3;
	input.T = 2;
	CPricerSettings settings;
	settings.exerciseType = EExerciseType::European;
	settings.fdSettings.gridType = EGridType::Adaptive;
	CBlackScholes bs(input);
	double bsC = bs.Value<EOptionType::Call>();
	double bsP = bs.Value<EOptionType::Put>();
	double bsV = bs.Vega();
	double bsRBC = bs.RhoBorrow<EOptionType::Call>();
	double bsRBP = bs.RhoBorrow<EOptionType::Put>();
	double bsRC = bs.Rho<EOptionType::Call>();
	double bsRP = bs.Rho<EOptionType::Put>();
	double bsDC = bs.Delta<EOptionType::Call>();
	double bsDP = bs.Delta<EOptionType::Put>();
	//double bsG = bs.Gamma();  // gamma error is already low, and it won't benefit from N, M increasing

	input.N = 129;
	input.M = 80;
	CFDPricer<ESolverType::CrankNicolson, EAdjointDifferentiation::All> pricerNM(input, settings);
	COutputData callOutput, putOutput;
	pricerNM.Price(callOutput, putOutput);

	CInputData input2(input);
	input2.N = 129;
	input2.M = 2 * input.M;
	CFDPricer<ESolverType::CrankNicolson, EAdjointDifferentiation::All> pricerN2M(input2, settings);
	COutputData callOutput2, putOutput2;
	pricerN2M.Price(callOutput2, putOutput2);

	CInputData input3(input);
	input3.N = 2 * input.N;
	input3.M = input.M;
	CFDPricer<ESolverType::CrankNicolson, EAdjointDifferentiation::All> pricer2NM(input3, settings);
	COutputData callOutput3, putOutput3;
	pricer2NM.Price(callOutput3, putOutput3);

	CInputData input4(input);
	input4.N = 2 * input.N;
	input4.M = 2 * input.M;
	CFDPricer<ESolverType::CrankNicolson, EAdjointDifferentiation::All> pricer2N2M(input3, settings);
	COutputData callOutput4, putOutput4;
	pricer2N2M.Price(callOutput4, putOutput4);

	//ASSERT_LE(fabs(bsC - callOutput2.price), fabs(bsC - callOutput.price));
	ASSERT_LE(fabs(bsC - callOutput3.price), fabs(bsC - callOutput.price));
	ASSERT_LE(fabs(bsC - callOutput4.price), fabs(bsC - callOutput.price));
	ASSERT_LE(fabs(bsC - callOutput4.price), fabs(bsC - callOutput2.price));
	ASSERT_LE(fabs(bsC - callOutput4.price), fabs(bsC - callOutput3.price));

	//ASSERT_LE(fabs(bsP - putOutput2.price), fabs(bsP - putOutput.price));
	ASSERT_LE(fabs(bsP - putOutput3.price), fabs(bsP - putOutput.price));
	ASSERT_LE(fabs(bsP - putOutput4.price), fabs(bsP - putOutput.price));
	ASSERT_LE(fabs(bsP - putOutput4.price), fabs(bsP - putOutput2.price));
	ASSERT_LE(fabs(bsP - putOutput4.price), fabs(bsP - putOutput3.price));

	//ASSERT_LE(fabs(bsDC - callOutput2.delta), fabs(bsDC - callOutput.delta));
	ASSERT_LE(fabs(bsDC - callOutput3.delta), fabs(bsDC - callOutput.delta));
	ASSERT_LE(fabs(bsDC - callOutput4.delta), fabs(bsDC - callOutput.delta));
	ASSERT_LE(fabs(bsDC - callOutput4.delta), fabs(bsDC - callOutput2.delta));
	ASSERT_LE(fabs(bsDC - callOutput4.delta), fabs(bsDC - callOutput3.delta));

	//ASSERT_LE(fabs(bsDP - putOutput2.delta), fabs(bsDP - putOutput.delta));
	ASSERT_LE(fabs(bsDP - putOutput3.delta), fabs(bsDP - putOutput.delta));
	ASSERT_LE(fabs(bsDP - putOutput4.delta), fabs(bsDP - putOutput.delta));
	ASSERT_LE(fabs(bsDP - putOutput4.delta), fabs(bsDP - putOutput2.delta));
	ASSERT_LE(fabs(bsDP - putOutput4.delta), fabs(bsDP - putOutput3.delta));

	//ASSERT_LE(fabs(bsG - callOutput2.gamma), fabs(bsG - callOutput.gamma));
	//ASSERT_LE(fabs(bsG / callOutput3.gamma - 1.0), fabs(bsG / callOutput.gamma - 1.0));
	//ASSERT_LE(fabs(bsG - callOutput4.gamma), fabs(bsG - callOutput.gamma));
	//ASSERT_LE(fabs(bsG - callOutput4.gamma), fabs(bsG - callOutput2.gamma));
	//ASSERT_LE(fabs(bsG - callOutput4.gamma), fabs(bsG - callOutput3.gamma));

	//ASSERT_LE(fabs(bsG - putOutput2.gamma), fabs(bsG - putOutput.gamma));
	//ASSERT_LE(fabs(bsG - putOutput3.gamma), fabs(bsG - putOutput.gamma));
	//ASSERT_LE(fabs(bsG - putOutput4.gamma), fabs(bsG - putOutput.gamma));
	//ASSERT_LE(fabs(bsG - putOutput4.gamma), fabs(bsG - putOutput2.gamma));
	//ASSERT_LE(fabs(bsG - putOutput4.gamma), fabs(bsG - putOutput3.gamma));

	//ASSERT_LE(fabs(bsV - callOutput2.vega), fabs(bsV - callOutput.vega));
	//ASSERT_LE(fabs(bsV - callOutput3.vega), fabs(bsV - callOutput.vega));
	//EXPECT_LE(fabs(bsV - callOutput4.vega), fabs(bsV - callOutput.vega));
	//EXPECT_LE(fabs(bsV - callOutput4.vega), fabs(bsV - callOutput2.vega));
	EXPECT_LE(fabs(bsV - callOutput4.vega), fabs(bsV - callOutput3.vega));

	//EXPECT_LE(fabs(bsV - putOutput2.vega), fabs(bsV - putOutput.vega));
	EXPECT_LE(fabs(bsV - putOutput3.vega), fabs(bsV - putOutput.vega));
	EXPECT_LE(fabs(bsV - putOutput4.vega), fabs(bsV - putOutput.vega));
	EXPECT_LE(fabs(bsV - putOutput4.vega), fabs(bsV - putOutput2.vega));
	EXPECT_LE(fabs(bsV - putOutput4.vega), fabs(bsV - putOutput3.vega));

	//EXPECT_LE(fabs(bsRC - callOutput2.rho), fabs(bsRC - callOutput.rho));
	EXPECT_LE(fabs(bsRC - callOutput3.rho), fabs(bsRC - callOutput.rho));
	EXPECT_LE(fabs(bsRC - callOutput4.rho), fabs(bsRC - callOutput.rho));
	EXPECT_LE(fabs(bsRC - callOutput4.rho), fabs(bsRC - callOutput2.rho));
	EXPECT_LE(fabs(bsRC - callOutput4.rho), fabs(bsRC - callOutput3.rho));

	//EXPECT_LE(fabs(bsRP - putOutput2.rho), fabs(bsRP - putOutput.rho));
	EXPECT_LE(fabs(bsRP - putOutput3.rho), fabs(bsRP - putOutput.rho));
	EXPECT_LE(fabs(bsRP - putOutput4.rho), fabs(bsRP - putOutput.rho));
	EXPECT_LE(fabs(bsRP - putOutput4.rho), fabs(bsRP - putOutput2.rho));
	EXPECT_LE(fabs(bsRP - putOutput4.rho), fabs(bsRP - putOutput3.rho));

	//EXPECT_LE(fabs(bsRBC - callOutput2.rhoBorrow), fabs(bsRBC - callOutput.rhoBorrow));
	EXPECT_LE(fabs(bsRBC - callOutput3.rhoBorrow), fabs(bsRBC - callOutput.rhoBorrow));
	EXPECT_LE(fabs(bsRBC - callOutput4.rhoBorrow), fabs(bsRBC - callOutput.rhoBorrow));
	EXPECT_LE(fabs(bsRBC - callOutput4.rhoBorrow), fabs(bsRBC - callOutput2.rhoBorrow));
	EXPECT_LE(fabs(bsRBC - callOutput4.rhoBorrow), fabs(bsRBC - callOutput3.rhoBorrow));

	//EXPECT_LE(fabs(bsRBP - putOutput2.rhoBorrow), fabs(bsRBP - putOutput.rhoBorrow));
	EXPECT_LE(fabs(bsRBP - putOutput3.rhoBorrow), fabs(bsRBP - putOutput.rhoBorrow));
	EXPECT_LE(fabs(bsRBP - putOutput4.rhoBorrow), fabs(bsRBP - putOutput.rhoBorrow));
	EXPECT_LE(fabs(bsRBP - putOutput4.rhoBorrow), fabs(bsRBP - putOutput2.rhoBorrow));
	EXPECT_LE(fabs(bsRBP - putOutput4.rhoBorrow), fabs(bsRBP - putOutput3.rhoBorrow));
}
