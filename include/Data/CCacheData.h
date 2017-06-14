/*
 * CCacheData.h
 *
 *  Created on: 11 Jun 2017
 *      Author: raiden
 */

#ifndef DATA_CCACHEDATA_H_
#define DATA_CCACHEDATA_H_

namespace details
{
class CCacheData
{
public:
	CCacheData() = default;

	CCacheData(const double sqrtDt, const double sigmaSqrtDt, const double growthFactor, const double discountFactor, const double growthFactorTimesDiscountFactor)
		: sqrtDt(sqrtDt),
		  sigmaSqrtDt(sigmaSqrtDt),
		  growthFactor(growthFactor),
		  discountFactor(discountFactor),
		  growthFactorTimesDiscountFactor(growthFactorTimesDiscountFactor)
	{
	};

	~CCacheData() = default;
	CCacheData(const CCacheData& rhs) = default;
	CCacheData(CCacheData&& rhs) = default;

	double T = 0.0;
	double sqrtDt = 0.0;
	double sigmaSqrtDt = 0.0;
	double growthFactor = 0.0;
	double discountFactor = 0.0;
	double growthFactorTimesDiscountFactor = 0.0;
};
}



#endif /* CCACHEDATA_H_ */
