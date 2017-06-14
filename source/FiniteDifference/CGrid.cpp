/*
 * CGrid.cpp
 *
 *  Created on: 11 Jun 2017
 *      Author: raiden
 */

#include <cstdio>
#include <FiniteDifference/CGrid.h>

namespace fdpricing
{

template<>
void CGrid::Make<EGridType::Linear>() noexcept
{
	data.resize(N);

	const size_t halfN = (N - 1) >> 1;
	const double dx0 = (x0 - lb) / halfN;
	const double dx1 = (ub - x0) / ((N - halfN) - 1);

	data[0] = lb;
	for (size_t i = 1; i <= halfN; ++i)
		data[i] = data[i - 1] + dx0;

	for (size_t i = halfN + 1; i < N; ++i)
		data[i] = data[i - 1] + dx1;
}


template<>
void CGrid::Make<EGridType::Logarithmic>() noexcept
{
	data.resize(N);

	const size_t halfN = (N - 1) >> 1;
	const double dx0 = pow(x0 / lb, 1.0 / halfN);
	const double dx1 = pow(ub / x0, 1.0 / (N - halfN - 1));

	data[0] = lb;
	for (size_t i = 1; i <= halfN; ++i)
		data[i] = data[i - 1] * dx0;

	for (size_t i = halfN + 1; i < N; ++i)
		data[i] = data[i - 1] * dx1;
}

/**
 * Tavella-Randall Grid: http://www.thierry-roncalli.com/download/pde.pdf
 */
template<>
void CGrid::Make<EGridType::Adaptive>() noexcept
{
	data.resize(N, x0);

	// First work out the grid
	const double alpha = .2 * (ub - lb);
	const double oneOverAlpha = 1.0 / alpha;
	const double c1 = asinh((lb - x0) * oneOverAlpha);
	const double c2 = asinh((ub - x0) * oneOverAlpha);

	const double oneOverN = 1.0 / (N - 1);
	for(size_t i = 0; i < N; ++i)
	{
		const double uniformDistribution = i * oneOverN;
		data[i] += alpha * sinh(c2 * uniformDistribution + c1 * (1.0 - uniformDistribution));
	}

	// Then rescale it in order to center it (boundaries stay)
	const double scalingFactor = x0 / data[N >> 1];
	for(size_t i = 1; i < N - 1; ++i)
		data[i] *= scalingFactor;
}

CGrid::CGrid(const double x0, const double lb, const double ub, const EGridType gridType, const size_t N) noexcept
		: N(N), x0(x0), lb(lb), ub(ub), gridType(gridType)
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
	switch (gridType)
	{
		case EGridType::Linear:
			Make<EGridType::Linear>();
			break;
		case EGridType::Logarithmic:
			Make<EGridType::Logarithmic>();
			break;
		case EGridType::Adaptive:
			Make<EGridType::Adaptive>();
			break;
		default:
			break;
	}
}

CGrid::CGrid(const CGrid& unaliased rhs) noexcept
	: N(rhs.N), x0(rhs.x0), lb(rhs.lb), ub(rhs.ub), gridType(rhs.gridType), data(rhs.data)
{
}

CGrid::CGrid(const CGrid&& unaliased rhs) noexcept
	: N(rhs.N), x0(rhs.x0), lb(rhs.lb), ub(rhs.ub), gridType(rhs.gridType), data(rhs.data)
{
}


} /* namespace fdpricing */
