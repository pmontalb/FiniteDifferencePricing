/*
 * CTreeData.h
 *
 *  Created on: 18 Dec 2016
 *      Author: raiden
 */

#ifndef DATA_CTREEDATA_H_
#define DATA_CTREEDATA_H_

#include <vector>
#include <array>

#include <Data/EAdjointDifferentiation.h>

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
	void Copy(const CPayoffData& __restrict__ rhs) noexcept
	{
		payoff_i = rhs.payoff_i;
		switch (adjointDifferentiation) {
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
};
}


#endif /* DATA_CTREEDATA_H_ */
