/*
 * CBlackScholes.h
 *
 *  Created on: 18 Dec 2016
 *      Author: raiden
 */

#ifndef BLACKSCHOLES_CBLACKSCHOLES_H_
#define BLACKSCHOLES_CBLACKSCHOLES_H_

#include <Data/CInputData.h>
#include <Data/COutputData.h>
#include <Data/CCacheData.h>
#include <Data/EOptionType.h>

namespace fdpricing
{
class CBlackScholes
{
public:
	CBlackScholes(const CInputData& __restrict__ input) noexcept;
	CBlackScholes(const CInputData& __restrict__ input, const details::CCacheData& __restrict__ cachedData) noexcept;
	~CBlackScholes() = default;

	void Update(const double S) noexcept;

	void Price(COutputData& __restrict__ callOutput, COutputData& __restrict__ putOutput) const noexcept;

	template<EOptionType>
	double Value() const noexcept;

	template<EOptionType>
	double Delta() const noexcept;

	double Gamma() const noexcept;

	double Vega() const noexcept;

	template<EOptionType>
	double Rho() const noexcept;

	template<EOptionType>
	double RhoBorrow() const noexcept;

protected:
	const CInputData& __restrict__ input;
	double d1;
	double d2;
	double Nd1;
	double Nd2;
	double NminusD1;
	double NminusD2;
	double Pd1;
	double d1Addend;

	details::CCacheData cacheData;

	double oneOverSigmaSqrtTtimesGrowthFactorTimesDiscountFactor;
	double sqrtTtimesGrowthFactorTimesDiscountFactor;

	double oneOverSigmaSqrtT;
	double halfSigma2;
	double TtimesGrowthFactorTimesDiscountFactor;

	void Make();
};
}

#endif /* BLACKSCHOLES_CBLACKSCHOLES_H_ */
