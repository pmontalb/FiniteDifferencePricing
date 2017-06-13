/*
 * CInputData.cpp
 *
 *  Created on: 18 Dec 2016
 *      Author: raiden
 */

#include <Data/CInputData.h>

namespace fdpricing
{
CInputData::CInputData() noexcept
		: CInputData(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0)
{
}

CInputData::CInputData(const CInputData& __restrict__ rhs) noexcept
{
	*this = rhs;
}

CInputData& CInputData::operator=(const CInputData& __restrict__ rhs) noexcept
{
	if (this != &rhs)
	{
		S = rhs.S;
		K = rhs.K;
		r = rhs.r;
		b = rhs.b;
		T = rhs.T;
		sigma = rhs.sigma;
		N = rhs.N;
		M = rhs.M;
		smoothing = rhs.smoothing;
		acceleration = rhs.acceleration;
		dividends = rhs.dividends;
	}

	return *this;
}

CInputData::CInputData(const double S, const double K, const double r, const double b, const double T, const double sigma, int N) noexcept
		: S(S), K(K), r(r), b(b), T(T), sigma(sigma), N(N & 1 ? N : (N + 1)), M(N), smoothing(false), acceleration(false)
{
}

CInputData::CInputData(const double S, const double K, const double r, const double T, const double sigma, int N) noexcept
		: CInputData(S, K, r, r, T, sigma, N)
{
}

CInputData::CInputData(const double S, const double K, const double r, const double T, const double sigma, int N, const std::vector<CDividend>& __restrict__ dividends) noexcept
		: CInputData(S, K, r, r, T, sigma, N, dividends)
{
}

CInputData::CInputData(const double S, const double K, const double r, const double b, const double T, const double sigma, int N, const std::vector<CDividend>& __restrict__ dividends) noexcept
		: S(S), K(K), r(r), b(b), T(T), sigma(sigma), dividends(dividends), N(N & 1 ? N : (N + 1)), M(N), smoothing(false), acceleration(false)
{
}
}
