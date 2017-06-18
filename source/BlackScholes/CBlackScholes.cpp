/*
 * CBlackScholes.cpp
 *
 *  Created on: 18 Dec 2016
 *      Author: raiden
 */

#include <Utilities/CStats.h>
#include <BlackScholes/CBlackScholes.h>

#include <Flags.h>

namespace fdpricing
{
CBlackScholes::CBlackScholes(const CInputData & unaliased input) noexcept
		: input(input), currentS(input.S)
{
	cacheData.T = input.T;

	const double sqrtT = sqrt(cacheData.T);

	cacheData.sigmaSqrtDt = input.sigma * sqrtT;
	oneOverSigmaSqrtT = 1.0 / cacheData.sigmaSqrtDt;

	cacheData.discountFactor = exp(-input.r * cacheData.T);
	cacheData.growthFactor   = exp( input.b * cacheData.T);

	cacheData.growthFactorTimesDiscountFactor = cacheData.growthFactor * cacheData.discountFactor;
	oneOverSigmaSqrtTtimesGrowthFactorTimesDiscountFactor = oneOverSigmaSqrtT * cacheData.growthFactorTimesDiscountFactor;
	sqrtTtimesGrowthFactorTimesDiscountFactor = cacheData.growthFactorTimesDiscountFactor * sqrtT;
	TtimesGrowthFactorTimesDiscountFactor = cacheData.T * cacheData.growthFactorTimesDiscountFactor;

	Make();
	Update(currentS);
}

CBlackScholes::CBlackScholes(const CInputData & unaliased input, const details::CCacheData& unaliased cacheData) noexcept
		: input(input), currentS(input.S), cacheData(cacheData)
{
	oneOverSigmaSqrtT = 1.0 / cacheData.sigmaSqrtDt;
	oneOverSigmaSqrtTtimesGrowthFactorTimesDiscountFactor = oneOverSigmaSqrtT * cacheData.growthFactorTimesDiscountFactor;
	sqrtTtimesGrowthFactorTimesDiscountFactor = cacheData.growthFactorTimesDiscountFactor * cacheData.sqrtDt;
	TtimesGrowthFactorTimesDiscountFactor = cacheData.T * cacheData.growthFactorTimesDiscountFactor;

	Make();
	Update(currentS);
}

void CBlackScholes::Update(double S) noexcept
{
	currentS = S;

	d1 = oneOverSigmaSqrtT * log(currentS) + d1Addend;
	d2 = d1 - cacheData.sigmaSqrtDt;

	Nd1 = CStats::normCdf(d1);
	NminusD1 = 1.0 - Nd1;

	Nd2 = CStats::normCdf(d2);
	NminusD2 = 1.0 - Nd2;

	Pd1 = CStats::normPdf(d1);
}

void CBlackScholes::Make()
{
	halfSigma2 = .5 * input.sigma * input.sigma;

	d1Addend = ((input.b + halfSigma2) * cacheData.T - log(input.K)) * oneOverSigmaSqrtT;
}

template<>
double CBlackScholes::Value<EOptionType::Call>() const noexcept
{
	return cacheData.discountFactor * (currentS * cacheData.growthFactor * Nd1 - input.K * Nd2);
}

template<>
double CBlackScholes::Value<EOptionType::Put>() const noexcept
{
	return cacheData.discountFactor * (-currentS * cacheData.growthFactor * NminusD1 + input.K * NminusD2);
}

template<>
double CBlackScholes::Delta<EOptionType::Call>() const noexcept
{
	return cacheData.growthFactorTimesDiscountFactor * Nd1;
}

template<>
double CBlackScholes::Delta<EOptionType::Put>() const noexcept
{
	return -cacheData.growthFactorTimesDiscountFactor * NminusD1;
}

double CBlackScholes::Gamma() const noexcept
{
	return 1.0 / currentS * Pd1 * oneOverSigmaSqrtTtimesGrowthFactorTimesDiscountFactor;
}

double CBlackScholes::Vega() const noexcept
{
	return currentS * sqrtTtimesGrowthFactorTimesDiscountFactor * Pd1;
}

template<>
double CBlackScholes::Rho<EOptionType::Call>() const noexcept
{
	return -cacheData.T * Value<EOptionType::Call>();
}

template<>
double CBlackScholes::Rho<EOptionType::Put>() const noexcept
{
	return -cacheData.T * Value<EOptionType::Put>();
}

template<>
double CBlackScholes::RhoBorrow<EOptionType::Call>() const noexcept
{
	return TtimesGrowthFactorTimesDiscountFactor * currentS * Nd1;
}

template<>
double CBlackScholes::RhoBorrow<EOptionType::Put>() const noexcept
{
	return -TtimesGrowthFactorTimesDiscountFactor * currentS * NminusD1;
}

void CBlackScholes::Price(COutputData& unaliased callOutput, COutputData& unaliased putOutput) const noexcept
{
	callOutput.price = Value<EOptionType::Call>();
	callOutput.delta = Delta<EOptionType::Call>();
	callOutput.gamma = Gamma();
	callOutput.vega = Vega();
	callOutput.rho = Rho<EOptionType::Call>();
	callOutput.rhoBorrow = RhoBorrow<EOptionType::Call>();

	putOutput.price = Value<EOptionType::Put>();
	putOutput.delta = Delta<EOptionType::Put>();
	putOutput.gamma = callOutput.gamma;
	putOutput.vega = callOutput.vega;
	putOutput.rho = Rho<EOptionType::Put>();
	putOutput.rhoBorrow = RhoBorrow<EOptionType::Put>();
}
}

