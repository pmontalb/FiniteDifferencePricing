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

	COutputData callOutput, putOutput;
	pricer.Price(callOutput, putOutput);

	EXPECT_LE(fabs(bsC - callOutput.price), 0.00281162);
	EXPECT_LE(fabs(bsP - putOutput.price) , 0.00207582);

	EXPECT_LE(fabs(bsV - callOutput.vega), 0.438633);
	EXPECT_LE(fabs(bsV - putOutput.vega), 0.440376);

	EXPECT_LE(fabs(bsRBC - callOutput.rhoBorrow), 1.25332);
	EXPECT_LE(fabs(bsRBP - putOutput.rhoBorrow), 1.25323);

	printf("R(BS=(%g %g) FD=(%g %g) E=(%g %g))\n", bsRC, bsRP, callOutput.rho, putOutput.rho, fabs(bsRC - callOutput.rho), fabs(bsRP - putOutput.rho));
}
