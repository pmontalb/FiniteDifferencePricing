/*
 * CDividend.h
 *
 *  Created on: 24 Dec 2016
 *      Author: raiden
 */

#ifndef DATA_CDIVIDEND_H_
#define DATA_CDIVIDEND_H_

namespace fdpricing
{
class CDividend
{
public:
	CDividend(const double t = 0, const double d = 0) noexcept;

	CDividend(const CDividend& __restrict__ rhs) noexcept;
	CDividend(const CDividend&& __restrict__ rhs) noexcept;
	CDividend& operator=(const CDividend& __restrict__ rhs) noexcept;
	CDividend& operator=(const CDividend&& __restrict__ rhs) noexcept;

	virtual ~CDividend() = default;

	double time;
	double dividend;
};
}
#endif /* DATA_CDIVIDEND_H_ */
