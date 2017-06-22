/*
 * CGrid.cpp
 *
 *  Created on: 11 Jun 2017
 *      Author: raiden
 */

#include <cstdio>

namespace fdpricing
{

template<EGridType gridType>
CGrid<gridType>::CGrid(const double x0, const double lb, const double ub, const size_t N) noexcept
		: N(N), x0(x0), lb(lb), ub(ub)
{
#ifdef DEBUG
	if (x0 >= ub || x0 <= lb)
	{
		printf("WRONG BOUNDARIES");
		return;
	}
	if (!(N & 1))
	{
		printf("NEED EVEN # of POINTS");
		return;
	}
#endif

	Make();
}

template<EGridType gridType>
CGrid<gridType>::CGrid(const CGrid& unaliased rhs) noexcept
	: N(rhs.N), x0(rhs.x0), lb(rhs.lb), ub(rhs.ub), data(rhs.data)
{
}

template<EGridType gridType>
CGrid<gridType>::CGrid(const CGrid&& unaliased rhs) noexcept
	: N(rhs.N), x0(rhs.x0), lb(rhs.lb), ub(rhs.ub), data(rhs.data)
{
}

} /* namespace fdpricing */
