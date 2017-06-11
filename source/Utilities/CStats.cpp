/*
 * CStats.cpp
 *
 *  Created on: 18 Dec 2016
 *      Author: raiden
 */

#include <Utilities/CStats.h>

namespace fdpricing
{
double CStats::normCdf(double x)
{
	constexpr double a1 = { 0.254829592  };
	constexpr double a2 = { -0.284496736 };
	constexpr double a3 = { 1.421413741  };
	constexpr double a4 = { -1.453152027 };
	constexpr double a5 = { 1.061405429  };
	constexpr double p  = { 0.3275911    };

	// Save the sign of x
	int sign(1);
	if (x < 0)
	{
		x *= -M_SQRT1_2l;
		sign = -1;
	}
	else
		x *= M_SQRT1_2l;

	const double t(1.0 / (1.0 + p * x));
	const double y(1.0 - (((((a5 * t + a4) * t) + a3) * t + a2) * t + a1) * t * exp(-x * x));

	return 0.5 * (1.0 + sign * y);
}

double CStats::normPdf(double x)
{
	return sqrtOneOver2Pi * exp(-0.5 * x * x);
}
}

