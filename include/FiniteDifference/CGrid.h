/*
 * CGrid.h
 *
 *  Created on: 11 Jun 2017
 *      Author: raiden
 */

#ifndef FINITEDIFFERENCE_CGRID_H_
#define FINITEDIFFERENCE_CGRID_H_

#include <vector>
#include <cmath>
#include <stddef.h>
#include <Flags.h>

namespace fdpricing
{

enum class EGridType
{
	Null,
	Linear,
	Logarithmic,
	Adaptive
};

class CGrid
{
public:
	/**
	 * x0: central point, grid will always pass through x0
	 * lb: lower bound
	 * ub: upper bound
	 * N : # of points - 1
	 */
	CGrid(const double x0, const double lb, const double ub, const EGridType gridType, const size_t N) noexcept;

	CGrid(const CGrid& unaliased rhs) noexcept;
	CGrid(const CGrid&& unaliased rhs) noexcept;

	virtual ~CGrid() = default;

	const double& Get(const size_t i) const noexcept
	{
#ifdef DEBUG
		if (i >= data.size())
		{
			printf("******** Out of bounds ********: %zu(%zu)\n", i, data.size());
			return data[i];
		}
#endif
		return data[i];
	}

	size_t size() const noexcept
	{
		return N;
	}

	const size_t N;
	const double x0;
	const double lb;
	const double ub;
	const EGridType gridType;

private:
	template<EGridType>
	void Make() noexcept;

	std::vector<double> data;
};


} /* namespace fdpricing */

#endif /* FINITEDIFFERENCE_CGRID_H_ */
