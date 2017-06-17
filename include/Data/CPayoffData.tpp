/*
 * CPayoffData.tpp
 *
 *  Created on: 14 Jun 2017
 *      Author: raiden
 */

#include <Flags.h>

namespace fdpricing
{

template<EAdjointDifferentiation adjointDifferentiation>
void CPayoffData::Init(const size_t N) noexcept
{
	payoff_i.resize(N);
	switch (adjointDifferentiation) {
		case EAdjointDifferentiation::Vega:
			vega_i.resize(N);
			break;
		case EAdjointDifferentiation::Rho:
			rho_i.resize(N);
			rhoBorrow_i.resize(N);
			break;
		case EAdjointDifferentiation::All:
			vega_i.resize(N);
			rho_i.resize(N);
			rhoBorrow_i.resize(N);
			break;
		default:
			break;
	}
}

template<EAdjointDifferentiation adjointDifferentiation>
void CPayoffData::Copy(const CPayoffData& unaliased rhs) noexcept
{
	payoff_i = rhs.payoff_i;
	switch (adjointDifferentiation)
	{
		case EAdjointDifferentiation::Vega:
			vega_i = rhs.vega_i;
			break;
		case EAdjointDifferentiation::Rho:
			rho_i = rhs.rho_i;
			rhoBorrow_i = rhs.rhoBorrow_i;
			break;
		case EAdjointDifferentiation::All:
			vega_i = rhs.vega_i;
			rho_i = rhs.rho_i;
			rhoBorrow_i = rhs.rhoBorrow_i;
			break;
		default:
			break;
	}
}

template<EAdjointDifferentiation adjointDifferentiation>
void CPayoffData::ZeroGreeks(const size_t i) noexcept
{
	switch (adjointDifferentiation)
	{
		case EAdjointDifferentiation::Vega:
			vega_i[i] = 0.0;
			break;
		case EAdjointDifferentiation::Rho:
			rho_i[0] = 0.0;
			rhoBorrow_i[0] = 0.0;
			break;
		case EAdjointDifferentiation::All:
			vega_i[i] = 0.0;
			rho_i[0] = 0.0;
			rhoBorrow_i[0] = 0.0;
			break;
		default:
			break;
	}
}

template<EAdjointDifferentiation adjointDifferentiation>
void CPayoffData::Lerp(const size_t i, const size_t j, const double w0, const double w1) noexcept
{
	payoff_i[i] = w0 * payoff_i[j - 1] + w1 * payoff_i[j];

	switch (adjointDifferentiation)
	{
		case EAdjointDifferentiation::Vega:
			vega_i[i] = w0 * vega_i[j - 1] + w1 * vega_i[j];
			break;
		case EAdjointDifferentiation::Rho:
			rho_i[i] = w0 * rho_i[j - 1] + w1 * rho_i[j];
			rhoBorrow_i[i] = w0 * rhoBorrow_i[j - 1] + w1 * rhoBorrow_i[j];
			break;
		case EAdjointDifferentiation::All:
			vega_i[i] = w0 * vega_i[j - 1] + w1 * vega_i[j];
			rho_i[i] = w0 * rho_i[j - 1] + w1 * rho_i[j];
			rhoBorrow_i[i] = w0 * rhoBorrow_i[j - 1] + w1 * rhoBorrow_i[j];
			break;
		default:
			break;
	}
}

}
