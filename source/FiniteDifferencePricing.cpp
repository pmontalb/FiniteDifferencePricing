//============================================================================
// Name        : FiniteDifferencePricing.cpp
// Author      : pmontalb
// Version     :
// Copyright   : 
// Description :
//============================================================================

#include <iostream>
#include <string>
#include <algorithm>
#include <gtest/gtest.h>

#include <FiniteDifference/CFDPricer.h>
#include <Utilities/CPlotter.h>

char* getCmdOption(char ** begin, char ** end, const std::string& option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

void PlotConvergence()
{
	using namespace fdpricing;

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
	CPricerSettings settings;
	settings.exerciseType = EExerciseType::European;
	settings.fdSettings.gridType = EGridType::Adaptive;

	std::vector<size_t> x;
	std::vector<double> y;
	for (size_t i = 0; i < 100; ++i)
	{
		x.push_back(i);
		const double td = .9 * input.T + i * (.9999 * input.T - .9 * input.T) / 99;
		const double dv = 10.0;
		input.dividends.resize(1);
		input.dividends[0] = CDividend(td, dv);
		CFDPricer<ESolverType::CrankNicolson, EAdjointDifferentiation::All> pricer(input, settings);
		COutputData callOutput, putOutput;
		pricer.Price(callOutput, putOutput);

		y.push_back(callOutput.price);
	}

	CPlotter plotter;
	plotter.plot(y, x);
}

int main(int argc, char * argv[])
{
	if(cmdOptionExists(argv, argv+argc, "-test"))
	{
		::testing::InitGoogleTest(&argc, argv);
		return RUN_ALL_TESTS();
	}
	if(cmdOptionExists(argv, argv+argc, "-conv"))
	{
		PlotConvergence();
	}


	// main

	return 0;
}
