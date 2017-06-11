/*
 * CBlackScholes.cpp
 *
 *  Created on: 18 Dec 2016
 *      Author: raiden
 */

#include <Utilities/CStats.h>
#include <BlackScholes/CBlackScholes.h>

namespace fdpricing
{
CBlackScholes::CBlackScholes(const CInputData & __restrict__ input) noexcept
		: input(input)
{
	const double sqrtT = sqrt(input.T);

	cacheData.sigmaSqrtDt = input.sigma * sqrtT;
	oneOverSigmaSqrtT = 1.0 / cacheData.sigmaSqrtDt;

	cacheData.discountFactor = exp(-input.r * input.T);
	cacheData.growthFactor   = exp( input.b * input.T);

	cacheData.growthFactorTimesDiscountFactor = cacheData.growthFactor * cacheData.discountFactor;
	oneOverSigmaSqrtTtimesGrowthFactorTimesDiscountFactor = oneOverSigmaSqrtT * cacheData.growthFactorTimesDiscountFactor;
	sqrtTtimesGrowthFactorTimesDiscountFactor = cacheData.growthFactorTimesDiscountFactor * sqrtT;
	TtimesGrowthFactorTimesDiscountFactor = input.T * cacheData.growthFactorTimesDiscountFactor;

	Make();
	Update(input.S);
}

CBlackScholes::CBlackScholes(const CInputData & __restrict__ input, const details::CCacheData& __restrict__ cacheData) noexcept
		: input(input), cacheData(cacheData)
{
	oneOverSigmaSqrtT = 1.0 / cacheData.sigmaSqrtDt;
	oneOverSigmaSqrtTtimesGrowthFactorTimesDiscountFactor = oneOverSigmaSqrtT * cacheData.growthFactorTimesDiscountFactor;
	sqrtTtimesGrowthFactorTimesDiscountFactor = cacheData.growthFactorTimesDiscountFactor * cacheData.sqrtDt;
	TtimesGrowthFactorTimesDiscountFactor = input.T * cacheData.growthFactorTimesDiscountFactor;

	Make();
}

void CBlackScholes::Update(double S) noexcept
{
	d1 = oneOverSigmaSqrtT * log(S) + d1Addend;
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

	d1Addend = ((input.b + halfSigma2) * input.T - log(input.K)) * oneOverSigmaSqrtT;
}

template<>
double CBlackScholes::Value<EOptionType::Call>() const noexcept
{
	return cacheData.discountFactor * (input.S * cacheData.growthFactor * Nd1 - input.K * Nd2);
}

template<>
double CBlackScholes::Value<EOptionType::Put>() const noexcept
{
	return cacheData.discountFactor * (-input.S * cacheData.growthFactor * NminusD1 + input.K * NminusD2);
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
	return 1.0 / input.S * Pd1 * oneOverSigmaSqrtTtimesGrowthFactorTimesDiscountFactor;
}

double CBlackScholes::Vega() const noexcept
{
	return input.S * sqrtTtimesGrowthFactorTimesDiscountFactor * Pd1;
}

template<>
double CBlackScholes::Rho<EOptionType::Call>() const noexcept
{
	return -input.T * Value<EOptionType::Call>();
}

template<>
double CBlackScholes::Rho<EOptionType::Put>() const noexcept
{
	return -input.T * Value<EOptionType::Put>();
}

template<>
double CBlackScholes::RhoBorrow<EOptionType::Call>() const noexcept
{
	return TtimesGrowthFactorTimesDiscountFactor * input.S * Nd1;
}

template<>
double CBlackScholes::RhoBorrow<EOptionType::Put>() const noexcept
{
	return -TtimesGrowthFactorTimesDiscountFactor * input.S * NminusD1;
}

void CBlackScholes::Price(COutputData& __restrict__ callOutput, COutputData& __restrict__ putOutput) const noexcept
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

