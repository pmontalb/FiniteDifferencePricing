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
#include <thread>
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

enum class EProfileMethod
{
	SingleThreaded,
	MultiThreaded,
};

template <EProfileMethod profileMethod>
void ProfileWorker(const size_t iterations, const size_t nDivs, const bool smoothing, const bool acceleration) noexcept
{
	using namespace fdpricing;

	CInputData input;
	input.smoothing = smoothing;
	input.acceleration = acceleration;
	input.S = 100;
	input.K = 100;
	input.r = .05;
	input.b = .02;
	input.sigma = .3;
	input.T = 7;
	input.N = 129;
	input.M = 80;
	input.dividends.resize(nDivs);
	for (size_t m = 0; m < nDivs; ++m)
		input.dividends[m] = CDividend(0.001 + .25 * m, 1.0);

	CPricerSettings settings;
	settings.calculationType = ECalculationType::All;
	settings.exerciseType = EExerciseType::European;
	settings.fdSettings.gridType = EGridType::Adaptive;

	CALLGRIND_START_INSTRUMENTATION;

	switch(profileMethod)
	{
		case EProfileMethod::SingleThreaded:
			for (size_t iter = 0; iter < iterations; ++iter)
			{
				CFDPricer<ESolverType::CrankNicolson, EAdjointDifferentiation::All> pricer(input, settings);
				COutputData callOutput, putOutput;
				pricer.Price(callOutput, putOutput);
			}
		break;

		case EProfileMethod::MultiThreaded:
		{
			size_t nThreads = std::thread::hardware_concurrency();
			std::vector<std::thread> threads(nThreads);
			const size_t optionsPerThread = static_cast<size_t>(iterations / nThreads);

			size_t start = 0;
			for (size_t i = 0; i < nThreads - 1; ++i)
			{
				start = i * optionsPerThread;
				threads[i] = std::thread([&]()
						{
							for (size_t iter = start; iter < start + optionsPerThread; ++iter)
							{
								CFDPricer<ESolverType::CrankNicolson, EAdjointDifferentiation::All> pricer(input, settings);
								COutputData callOutput, putOutput;
								pricer.Price(callOutput, putOutput);
							}
						});
			}
			threads[nThreads - 1] = std::thread([&]()
					{
						for (size_t iter = optionsPerThread * (nThreads - 2); iter < iterations; ++iter)
						{
							CFDPricer<ESolverType::CrankNicolson, EAdjointDifferentiation::All> pricer(input, settings);
							COutputData callOutput, putOutput;
							pricer.Price(callOutput, putOutput);
						}
					});

			for (size_t i = 0; i < nThreads; ++i)
				threads[i].join();
		}
		break;
	}

	CALLGRIND_STOP_INSTRUMENTATION;
	CALLGRIND_DUMP_STATS;
}

void Profile(const size_t iterations = 100,
		const size_t nDivs = 8,
		const bool smoothing = true,
		const bool acceleration = true,
		const EProfileMethod profileMethod = EProfileMethod::SingleThreaded) noexcept
{
	auto started = std::chrono::high_resolution_clock::now();

	if (profileMethod == EProfileMethod::SingleThreaded)
		ProfileWorker<EProfileMethod::SingleThreaded>(iterations, nDivs, smoothing, acceleration);
	if (profileMethod == EProfileMethod::MultiThreaded)
		ProfileWorker<EProfileMethod::MultiThreaded>(iterations, nDivs, smoothing, acceleration);

	auto done = std::chrono::high_resolution_clock::now();
	double avgTime = std::chrono::duration_cast<std::chrono::milliseconds>(done - started).count();
	avgTime /= iterations;

	printf("--------- SMOOTH=%d - ACCEL=%d - %zu DIVIDENDS (out of %zu iterations)  ---------\n", smoothing, acceleration, nDivs, iterations);
	printf("\n\t* Avg Time(ms) Per Option: %.5f\n", avgTime);
	printf("\n\t* Opt/Sec: %.5f\n", iterations / (.001 * avgTime));
	printf("\n----------------------------------------------------------\n");

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
		size_t nIterations = 100;
		size_t nDivs = 8;
		EProfileMethod profileMethod = EProfileMethod::SingleThreaded;
		bool smoothing = false;
		bool acceleration = false;

		if (cmdOptionExists(argv, argv+argc, "-iter"))
			nIterations = std::atoi(getCmdOption(argv, argv + argc, "-iter"));
		if (cmdOptionExists(argv, argv+argc, "-divs"))
			nDivs = std::atoi(getCmdOption(argv, argv + argc, "-divs"));
		if (cmdOptionExists(argv, argv+argc, "-smooth"))
			smoothing = true;
		if (cmdOptionExists(argv, argv+argc, "-acc"))
			acceleration = true;
		if (cmdOptionExists(argv, argv+argc, "-method"))
		{
			const auto method = std::string(getCmdOption(argv, argv + argc, "-method"));
			if (method == "single")
			{
				printf("============== SINGLE THREADED ==============\n");
				profileMethod = EProfileMethod::SingleThreaded;
			}
			if (method == "multi")
			{
				printf("============== MULTI-THREADED ==============\n");
				profileMethod = EProfileMethod::MultiThreaded;
			}
		}
		Profile(nIterations, nDivs, smoothing, acceleration, profileMethod);
	}


	// main

	return 0;
}
