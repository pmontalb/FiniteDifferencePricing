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

namespace fdpricing
{
class CTreeData
{
public:
	std::vector<double> payoff_i;
	std::vector<double> vega_i;
	std::vector<double> rho_i;
	std::vector<double> rhoBorrow_i;

	std::array<double, 6> leaves[6];
};
}


#endif /* DATA_CTREEDATA_H_ */
