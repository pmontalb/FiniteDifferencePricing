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


TEST (FDTest, AmericanOptionNoExercise)
{
	CInputData input;
	input.smoothing = false;
	input.S = 100;
	input.K = 100;
	input.r = .05;
	input.b = .05;
	input.sigma = .3;
	input.T = 2;
	input.N = 257;
	input.M = 80;

	CPricerSettings settings;
	settings.exerciseType = EExerciseType::American;
	settings.fdSettings.gridType = EGridType::Linear;
	CFDPricer<ESolverType::CrankNicolson, EAdjointDifferentiation::None> pricerAmerican(input, settings);

	CPricerSettings settings2;
	settings2.exerciseType = EExerciseType::European;
	settings2.fdSettings.gridType = EGridType::Linear;
	CFDPricer<ESolverType::CrankNicolson, EAdjointDifferentiation::None> pricerEuropean(input, settings2);

	COutputData callOutputAm, putOutputAm;
	pricerAmerican.Price(callOutputAm, putOutputAm);
	COutputData callOutputEu, putOutputEu;
	pricerEuropean.Price(callOutputEu, putOutputEu);

	// positive rates: never convenient to exercise calls
	ASSERT_NEAR(callOutputAm.price, callOutputEu.price, 6e-6);
	ASSERT_GT(putOutputAm.price, putOutputEu.price);

	input.r *= -1;
	input.b *= -1;
	CFDPricer<ESolverType::CrankNicolson, EAdjointDifferentiation::None> pricerAmerican2(input, settings);
	CFDPricer<ESolverType::CrankNicolson, EAdjointDifferentiation::None> pricerEuropean2(input, settings2);
	pricerAmerican2.Price(callOutputAm, putOutputAm);
	pricerEuropean2.Price(callOutputEu, putOutputEu);

	// negative rates: never convenient to exercise calls
	ASSERT_NEAR(putOutputAm.price, putOutputEu.price, 6e-6);
	ASSERT_GE(callOutputAm.price, callOutputEu.price);
}


TEST (FDTest, BinomialTreeConsistency)
{
	CInputData input;
	input.smoothing = true;
	input.S = 100;
	input.K = 100;
	input.r = .05;
	input.b = .05;
	input.sigma = .3;
	input.T = 2;
	input.N = 257;
	input.M = 80;

	CPricerSettings settings;
	settings.exerciseType = EExerciseType::American;
	settings.fdSettings.gridType = EGridType::Adaptive;
	CFDPricer<ESolverType::CrankNicolson, EAdjointDifferentiation::None> pricer(input, settings);

	COutputData callOutput, putOutput;
	pricer.Price(callOutput, putOutput);

	ASSERT_LE(fabs(21.198671628986794 - callOutput.price), .0155);
	ASSERT_LE(fabs(12.845527283509988 - putOutput.price) , .0203);
}


TEST (FDTest, ZeroDividendOnTimeGridSanity)
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
	input.dividends = { CDividend(.5 * input.T, 0.0) };

	CPricerSettings settings;
	settings.exerciseType = EExerciseType::American;
	settings.fdSettings.gridType = EGridType::Adaptive;
	CFDPricer<ESolverType::CrankNicolson, EAdjointDifferentiation::All> pricerDiv(input, settings);

	COutputData callOutput, putOutput;
	pricerDiv.Price(callOutput, putOutput);

	input.dividends.resize(0);
	CFDPricer<ESolverType::CrankNicolson, EAdjointDifferentiation::All> pricerNoDiv(input, settings);

	COutputData callOutput2, putOutput2;
	pricerNoDiv.Price(callOutput2, putOutput2);

	ASSERT_LE(fabs(callOutput2.price - callOutput.price), 1e-16);
	ASSERT_LE(fabs(putOutput2.price  - putOutput.price) , 1e-16);

	ASSERT_LE(fabs(callOutput2.delta - callOutput.delta), 1e-16);
	ASSERT_LE(fabs(putOutput2.delta  - putOutput.delta) , 1e-16);

	ASSERT_LE(fabs(callOutput2.gamma - callOutput.gamma), 1e-16);
	ASSERT_LE(fabs(putOutput2.gamma  - putOutput.gamma) , 1e-16);

	ASSERT_LE(fabs(callOutput2.vega - callOutput.vega), 1e-16);
	ASSERT_LE(fabs(putOutput2.vega  - putOutput.vega) , 1e-16);

	ASSERT_LE(fabs(callOutput2.rho - callOutput.rho), 1e-16);
	ASSERT_LE(fabs(putOutput2.rho  - putOutput.rho) , 1e-16);

	ASSERT_LE(fabs(callOutput2.rhoBorrow - callOutput.rhoBorrow), 1e-16);
	ASSERT_LE(fabs(putOutput2.rhoBorrow  - putOutput.rhoBorrow) , 1e-16);

	ASSERT_LE(fabs(callOutput2.theta - callOutput.theta), 1e-16);
	ASSERT_LE(fabs(putOutput2.theta  - putOutput.theta) , 1e-16);

	ASSERT_LE(fabs(callOutput2.theta2 - callOutput.theta2), 1e-16);
	ASSERT_LE(fabs(putOutput2.theta2  - putOutput.theta2) , 1e-16);

	ASSERT_LE(fabs(callOutput2.charm - callOutput.charm), 1e-16);
	ASSERT_LE(fabs(putOutput2.charm  - putOutput.charm) , 1e-16);
}

TEST (FDTest, ZeroDividendSanity)
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
	input.dividends = { CDividend(.5123456789 * input.T, 0.0) };

	CPricerSettings settings;
	settings.exerciseType = EExerciseType::American;
	settings.fdSettings.gridType = EGridType::Adaptive;
	CFDPricer<ESolverType::CrankNicolson, EAdjointDifferentiation::All> pricerDiv(input, settings);

	COutputData callOutput, putOutput;
	pricerDiv.Price(callOutput, putOutput);

	input.dividends.resize(0);
	CFDPricer<ESolverType::CrankNicolson, EAdjointDifferentiation::All> pricerNoDiv(input, settings);

	COutputData callOutput2, putOutput2;
	pricerNoDiv.Price(callOutput2, putOutput2);

	EXPECT_LE(fabs(callOutput2.price - callOutput.price), 7e-6);
	EXPECT_LE(fabs(putOutput2.price  - putOutput.price) , 7e-6);

	EXPECT_LE(fabs(callOutput2.delta - callOutput.delta), 7e-6);
	EXPECT_LE(fabs(putOutput2.delta  - putOutput.delta) , 7e-6);

	EXPECT_LE(fabs(callOutput2.gamma - callOutput.gamma), 7e-6);
	EXPECT_LE(fabs(putOutput2.gamma  - putOutput.gamma) , 7e-6);

	EXPECT_LE(fabs(callOutput2.vega - callOutput.vega), 1.8e-6);
	EXPECT_LE(fabs(putOutput2.vega  - putOutput.vega) , 3e-4);

	EXPECT_LE(fabs(callOutput2.rho - callOutput.rho), 7e-6);
	EXPECT_LE(fabs(putOutput2.rho  - putOutput.rho) , 7e-6);

	EXPECT_LE(fabs(callOutput2.rhoBorrow - callOutput.rhoBorrow), 5e-5);
	EXPECT_LE(fabs(putOutput2.rhoBorrow  - putOutput.rhoBorrow) , 5e-5);

	EXPECT_LE(fabs(callOutput2.theta - callOutput.theta), 7e-6);
	EXPECT_LE(fabs(putOutput2.theta  - putOutput.theta) , 7e-6);

	EXPECT_LE(fabs(callOutput2.theta2 - callOutput.theta2), 7e-6);
	EXPECT_LE(fabs(putOutput2.theta2  - putOutput.theta2) , 7e-6);

	EXPECT_LE(fabs(callOutput2.charm - callOutput.charm), 7e-6);
	EXPECT_LE(fabs(putOutput2.charm  - putOutput.charm) , 7e-6);
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
	input3.N = 2 * input.N + 1;
	input3.M = input.M;
	CFDPricer<ESolverType::CrankNicolson, EAdjointDifferentiation::All> pricer2NM(input3, settings);
	COutputData callOutput3, putOutput3;
	pricer2NM.Price(callOutput3, putOutput3);

	CInputData input4(input);
	input4.N = 2 * input.N + 1;
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

/**
 * http://doc.utwente.nl/58556/1/Vellekoop06efficient.pdf
 */
TEST (FDTest, VellekoopPage280Table1)
{
	CInputData input;
	input.S = 100;
	input.r = .05;
	input.b = .05;
	input.sigma = .3;
	input.T = 1.0;
	input.N = 2049;
	input.M = 200;
	input.smoothing = true;
	input.acceleration = false;

	auto RE = [&]()
	{
		COutputData callOutput;
		COutputData putOutput;

		CPricerSettings settings;
		settings.exerciseType = EExerciseType::American;
		settings.fdSettings.gridType = EGridType::Adaptive;
		settings.calculationType = ECalculationType::CallOnly;
		CFDPricer<ESolverType::CrankNicolson, EAdjointDifferentiation::None> pricer(input, settings);
		pricer.Price(callOutput, putOutput);

		double pn = callOutput.price;

		return pn;
	};

	input.K = 70;
	input.dividends = std::vector<CDividend>({ CDividend(.1, 7.0) });
	EXPECT_LE(fabs(RE() - 30.38), 0.01);
	input.K = 100;
	EXPECT_LE(fabs(RE() - 10.29), 0.01);
	input.K = 130;
	EXPECT_LE(fabs(RE() - 3.00), 0.01);

	input.K = 70;
	input.dividends = std::vector<CDividend>({ CDividend(.5, 7.0) });
	EXPECT_LE(fabs(RE() - 32.13), 0.01);
	input.K = 100;
	EXPECT_LE(fabs(RE() - 11.33), 0.01);
	input.K = 130;
	EXPECT_LE(fabs(RE() - 3.28), 0.01);

	input.K = 70;
	input.dividends = std::vector<CDividend>({ CDividend(.9, 7.0) });
	EXPECT_LE(fabs(RE() - 33.92), 0.01);
	input.K = 100;
	EXPECT_LE(fabs(RE() - 13.49), 0.01);
	input.K = 130;
	EXPECT_LE(fabs(RE() - 4.17), 0.01);
}

/**
 * http://doc.utwente.nl/58556/1/Vellekoop06efficient.pdf
 */
TEST (FDTest, VellekoopPage282Table3)
{
	CInputData input;
	input.S = 100;
	input.r = .06;
	input.b = .06;
	input.sigma = .25;
	input.T = 7;
	input.N = 4057;
	input.M = 200;
	input.smoothing = true;
	input.acceleration = false;
	input.dividends.resize(7);

	auto RE = [&]()
	{
		COutputData callOutput;
		COutputData putOutput;

		CPricerSettings settings;
		settings.exerciseType = EExerciseType::American;
		settings.fdSettings.gridType = EGridType::Adaptive;
		settings.calculationType = ECalculationType::CallOnly;
		CFDPricer<ESolverType::CrankNicolson, EAdjointDifferentiation::None> pricer(input, settings);
		pricer.Price(callOutput, putOutput);

		double pn = callOutput.price;

		return pn;
	};

	input.K = 70;
	double t0 = .1;
	for (size_t i = 0; i < 7; i++)
		input.dividends[i] = CDividend(t0 + i, std::min(8.0, 6.0 + .5 * i));
	EXPECT_LE(fabs(RE() - 31.14), 0.01);
	input.K = 100;
	EXPECT_LE(fabs(RE() - 18.32), 0.01);
	input.K = 130;
	EXPECT_LE(fabs(RE() - 12.48), 0.01);

	input.K = 70;
	t0 = .5;
	for (size_t i = 0; i < 7; i++)
		input.dividends[i] = CDividend(t0 + i, std::min(8.0, 6.0 + .5 * i));
	EXPECT_LE(fabs(RE() - 33.47), 0.01);
	input.K = 100;
	EXPECT_LE(fabs(RE() - 20.04), 0.01);
	input.K = 130;
	EXPECT_LE(fabs(RE() - 13.75), 0.01);

	input.K = 70;
	t0 = .9;
	for (size_t i = 0; i < 7; i++)
		input.dividends[i] = CDividend(t0 + i, std::min(8.0, 6.0 + .5 * i));
	EXPECT_LE(fabs(RE() - 35.52), 0.01);
	input.K = 100;
	EXPECT_LE(fabs(RE() - 21.86), 0.01);
	input.K = 130;
	EXPECT_LE(fabs(RE() - 15.21), 0.01);
}
