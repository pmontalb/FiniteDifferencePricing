/*
 * COutputData.h
 *
 *  Created on: 18 Dec 2016
 *      Author: raiden
 */

#ifndef DATA_COUTPUTDATA_H_
#define DATA_COUTPUTDATA_H_

namespace fdpricing
{
class COutputData
{
public:
	COutputData() = default;
	~COutputData() = default;
	COutputData(const COutputData& rhs) = default;
	COutputData(COutputData&& rhs) = default;

	double price = 0.0;
	double delta = 0.0;
	double gamma = 0.0;
	double vega = 0.0;
	double rho = 0.0;
	double rhoBorrow = 0.0;
	double theta = 0.0;
	double theta2 = 0.0;
	double charm = 0.0;
};
}
#endif /* DATA_COUTPUTDATA_H_ */
