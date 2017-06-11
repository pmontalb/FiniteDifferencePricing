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

namespace fdpricing
{
class CInputData
{
public:
	CInputData() noexcept;

	CInputData(const CInputData& __restrict__ rhs) noexcept;
	CInputData(const CInputData&& __restrict__ rhs) noexcept;

	CInputData& operator=(const CInputData& __restrict__ rhs) noexcept;
	CInputData& operator=(const CInputData&& __restrict__ rhs) noexcept;

	CInputData(const double S, const double K, const double r, const double b, const double T, const double sigma, const int N) noexcept;
	CInputData(const double S, const double K, const double r, const double T, const double sigma, int N) noexcept;

	CInputData(const double S, const double K, const double r, const double T, const double sigma, int N, const std::vector<CDividend>& __restrict__ dividends) noexcept;
	CInputData(const double S, const double K, const double r, const double b, const double T, const double sigma, int N, const std::vector<CDividend>& __restrict__ dividends) noexcept;
	~CInputData() = default;

	double S;
	double K;
	double r;
	double b;
	double T;
	double sigma;
	std::vector<CDividend> dividends;
	size_t N;

	bool smoothing;
	bool acceleration;
};
}

#endif /* DATA_CINPUTDATA_H_ */
