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
#include <Flags.h>

namespace fdpricing
{
class CBlackScholes
{
public:
	CBlackScholes(const CInputData& unaliased input) noexcept;
	CBlackScholes(const CInputData& unaliased input, const details::CCacheData& unaliased cachedData) noexcept;
	~CBlackScholes() = default;

	/**
	 * Update all cached quantities that depend on underlying value
	 */
	void Update(const double S) noexcept;

	void Price(COutputData& unaliased callOutput, COutputData& unaliased putOutput) const noexcept;

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
	const CInputData& unaliased input;
	double d1;
	double d2;
	double Nd1;
	double Nd2;
	double NminusD1;
	double NminusD2;
	double Pd1;

	/**
	 * d1 = d1Addend + log(currentS) / (sigma * sqrt(T))
	 */
	double d1Addend;

	/**
	 * Cached value for the underlying
	 */
	double currentS;

	details::CCacheData cacheData;

	double oneOverSigmaSqrtTtimesGrowthFactorTimesDiscountFactor;
	double sqrtTtimesGrowthFactorTimesDiscountFactor;

	double oneOverSigmaSqrtT;
	double halfSigma2;
	double TtimesGrowthFactorTimesDiscountFactor;

	/**
	 * Precomputes quantities that do not depend on the underlying value
	 */
	void Make();
};
}

#endif /* BLACKSCHOLES_CBLACKSCHOLES_H_ */
