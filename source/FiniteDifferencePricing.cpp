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
#include <chrono>
#include <valgrind/callgrind.h>
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

void Profile()
{
	const size_t iterations = 100;
	const size_t nDivs = 8;
	const double dtDivs = .25;

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
	settings.exerciseType = EExerciseType::American;
	settings.fdSettings.gridType = EGridType::Adaptive;

	auto started = std::chrono::high_resolution_clock::now();

	CALLGRIND_START_INSTRUMENTATION;

	for (size_t iter = 0; iter < iterations; ++iter)
	{
		for (size_t n = 0; n < nDivs; ++n)
		{
			input.dividends.resize(n);
			for (size_t m = 0; m < n; ++m)
				input.dividends[m] = CDividend(0.001 + dtDivs, 1.0);

			CFDPricer<ESolverType::CrankNicolson, EAdjointDifferentiation::All> pricer(input, settings);
			COutputData callOutput, putOutput;
			pricer.Price(callOutput, putOutput);
		}
	}

	CALLGRIND_STOP_INSTRUMENTATION;
	CALLGRIND_DUMP_STATS;

	auto done = std::chrono::high_resolution_clock::now();
	double avgTime = std::chrono::duration_cast<std::chrono::milliseconds>(done - started).count();
	avgTime /= (iterations * nDivs);

	printf("Avg Time(ms) Per Option: %.5f\n", avgTime);
	printf("Opt/Sec: %.5f\n", iterations / (.001 * avgTime));
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
	if(cmdOptionExists(argv, argv+argc, "-profile"))
	{
		Profile();
	}


	// main

	return 0;
}
