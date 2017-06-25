/*
 * CInputData.h
 *
 *  Created on: 18 Dec 2016
 *      Author: raiden
 */

#ifndef DATA_CINPUTDATA_H_
#define DATA_CINPUTDATA_H_

#include <vector>
#include <stdlib.h>

#include <Data/CDividend.h>
#include <Flags.h>

namespace fdpricing
{
class CInputData
{
public:
	CInputData() noexcept;

	CInputData(const CInputData& unaliased rhs) noexcept;
	CInputData(const CInputData&& unaliased rhs) noexcept;

	CInputData& operator=(const CInputData& unaliased rhs) noexcept;
	CInputData& operator=(const CInputData&& unaliased rhs) noexcept;

	CInputData(const double S, const double K, const double r, const double b, const double T, const double sigma, const int N) noexcept;
	CInputData(const double S, const double K, const double r, const double T, const double sigma, int N) noexcept;

	CInputData(const double S, const double K, const double r, const double T, const double sigma, int N, const std::vector<CDividend>& unaliased dividends) noexcept;
	CInputData(const double S, const double K, const double r, const double b, const double T, const double sigma, int N, const std::vector<CDividend>& unaliased dividends) noexcept;
	~CInputData() = default;

	double S;
	double K;
	double r;
	double b;
	double T;
	double sigma;
	std::vector<CDividend> dividends;

	/**
	 * Number of space grid points
	 */
	size_t N;

	/**
	 * Number of time  grid points
	 */
	size_t M;

	/**
	 * Payoff smoothing: if true, the last Backward Induction step will be smoothed with Black-Scholes
	 */
	bool smoothing;

	/**
	 * Acceleration: if true, it tries (depending on interest rates) to smooth the payoff until last ex-dividend date
	 */
	bool acceleration;
};
}

#endif /* DATA_CINPUTDATA_H_ */
