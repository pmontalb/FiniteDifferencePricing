/*
 * CTreeData.h
 *
 *  Created on: 18 Dec 2016
 *      Author: raiden
 */

#ifndef DATA_CPAYOFFDATA_H_
#define DATA_CPAYOFFDATA_H_

#include <vector>
#include <array>

#include <Data/EAdjointDifferentiation.h>
#include <Flags.h>

namespace fdpricing
{
/**
 * This class stores the payoff and its differentiation w.r.t. sigma, r and b
 */
class CPayoffData
{
public:
	std::vector<double> payoff_i;
	std::vector<double> vega_i;
	std::vector<double> rho_i;
	std::vector<double> rhoBorrow_i;

	/**
	 * Initialise vectors
	 */
	template<EAdjointDifferentiation adjointDifferentiation>
	void Init(const size_t N) noexcept;

	/**
	 * Copy only requested quantities
	 */
	template<EAdjointDifferentiation adjointDifferentiation>
	void Copy(const CPayoffData& unaliased rhs) noexcept;

	/**
	 * Set to zero the requested quantities
	 */
	template<EAdjointDifferentiation adjointDifferentiation>
	void ZeroGreeks(const size_t i) noexcept;

	/**
	 * Linear Interpolation
	 *
	 * i : current value to interpolate
	 * j : left interpolation value
	 * w0: j-th weight
	 * w1: (j + 1)-th weight
	 */
	template<EAdjointDifferentiation adjointDifferentiation>
	void Lerp(const size_t i, const size_t j, const double w0, const double w1) noexcept;
};
}

#include <Data/CPayoffData.tpp>

#endif /* DATA_CPAYOFFDATA_H_ */
