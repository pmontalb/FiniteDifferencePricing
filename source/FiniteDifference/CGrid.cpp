/*
 * CGrid.cpp
 *
 *  Created on: 22 Jun 2017
 *      Author: raiden
 */

#include <FiniteDifference/CGrid.h>

namespace fdpricing
{

template<>
void CGrid<EGridType::Linear>::Make() noexcept
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
void CGrid<EGridType::Logarithmic>::Make() noexcept
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
void CGrid<EGridType::Adaptive>::Make() noexcept
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

}
