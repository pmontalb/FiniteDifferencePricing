/*
 * CDividend.cpp
 *
 *  Created on: 24 Dec 2016
 *      Author: raiden
 */

#include <Data/CDividend.h>

namespace fdpricing
{
CDividend::CDividend(const double t, const double d) noexcept
		: time(t), dividend(d)
{
}

CDividend::CDividend(const CDividend& __restrict__ rhs) noexcept
{
	*this = rhs;
}

CDividend::CDividend(const CDividend&& __restrict__ rhs) noexcept
{
	*this = rhs;
}

CDividend& CDividend::operator =(const CDividend& __restrict__ rhs) noexcept
{
	if (this != &rhs)
	{
		time = rhs.time;
		dividend = rhs.dividend;
	}

	return *this;
}

CDividend& CDividend::operator =(const CDividend&& __restrict__ rhs) noexcept
{
	time = rhs.time;
	dividend = rhs.dividend;

	return *this;
}
}
