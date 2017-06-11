/*
 * CStats.h
 *
 *  Created on: 18 Dec 2016
 *      Author: raiden
 */

#ifndef UTILITIES_CSTATS_H_
#define UTILITIES_CSTATS_H_

#include <cmath>

namespace fdpricing
{
class CStats
{
public:
	static constexpr double sqrtOneOver2Pi { .5 * M_2_SQRTPIl * M_SQRT1_2l };

	/**
	 * Abramovitz-Stegun (7.1.26) approximation of a normal CDF
	 */
	static double normCdf(double x);
	static double normPdf(double x);
};
}

#endif /* UTILITIES_CSTATS_H_ */
