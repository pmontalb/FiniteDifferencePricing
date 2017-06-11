/*
 * TestGrid.cpp
 *
 *  Created on: 11 Jun 2017
 *      Author: raiden
 */


#include <gtest/gtest.h>
#include <FiniteDifference/CGrid.h>

using namespace fdpricing;


TEST (GridTest, LinearGrid)
{
	CGrid grid(50.0, 20.0, 100.0, EGridType::Linear, 100);
	ASSERT_NEAR(grid.Get(0), grid.lb, 1e-12);
	ASSERT_NEAR(grid.Get(grid.N / 2), grid.x0, 1e-12);
	ASSERT_NEAR(grid.Get(grid.N), grid.ub, 1e-12);

	CGrid grid2(50.0, 20.0, 100.0, EGridType::Linear, 101);
	ASSERT_NEAR(grid.Get(0), grid.lb, 1e-12);
	ASSERT_NEAR(grid.Get(grid.N / 2), grid.x0, 1e-12);
	ASSERT_NEAR(grid.Get(grid.N), grid.ub, 1e-12);
}

TEST (GridTest, LogarithmicGrid)
{
	CGrid grid(50.0, 20.0, 100.0, EGridType::Logarithmic, 100);
	ASSERT_NEAR(grid.Get(0), grid.lb, 1e-12);
	ASSERT_NEAR(grid.Get(grid.N / 2), grid.x0, 1e-12);
	ASSERT_NEAR(grid.Get(grid.N), grid.ub, 1e-12);

	CGrid grid2(50.0, 20.0, 100.0, EGridType::Logarithmic, 101);
	ASSERT_NEAR(grid.Get(0), grid.lb, 1e-12);
	ASSERT_NEAR(grid.Get(grid.N / 2), grid.x0, 1e-12);
	ASSERT_NEAR(grid.Get(grid.N), grid.ub, 1e-12);
}

TEST (GridTest, AdaptiveGrid)
{
	CGrid grid(50.0, 20.0, 100.0, EGridType::Adaptive, 100);
	ASSERT_NEAR(grid.Get(0), grid.lb, 1e-12);
	ASSERT_NEAR(grid.Get(grid.N), grid.ub, 1e-12);

	CGrid grid2(50.0, 20.0, 100.0, EGridType::Adaptive, 101);
	ASSERT_NEAR(grid.Get(0), grid.lb, 1e-12);
	ASSERT_NEAR(grid.Get(grid.N), grid.ub, 1e-12);
}
