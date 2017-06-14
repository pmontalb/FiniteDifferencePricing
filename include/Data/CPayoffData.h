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
class CPayoffData
{
public:
	std::vector<double> payoff_i;
	std::vector<double> vega_i;
	std::vector<double> rho_i;
	std::vector<double> rhoBorrow_i;

	template<EAdjointDifferentiation adjointDifferentiation>
	void Init(const size_t N) noexcept;

	template<EAdjointDifferentiation adjointDifferentiation>
	void Copy(const CPayoffData& unaliased rhs) noexcept;

	template<EAdjointDifferentiation adjointDifferentiation>
	void ZeroGreeks(const size_t i) noexcept;
};
}

#include <Data/CPayoffData.tpp>

#endif /* DATA_CPAYOFFDATA_H_ */
