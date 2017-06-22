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
	CGrid<EGridType::Linear> grid(50.0, 20.0, 100.0, 101);
	ASSERT_NEAR(grid.Get(0), grid.lb, 1e-12);
	ASSERT_NEAR(grid.Get(grid.N / 2), grid.x0, 1e-12);
	ASSERT_NEAR(grid.Get(grid.N - 1), grid.ub, 1e-12);
}

TEST (GridTest, LogarithmicGrid)
{
	CGrid<EGridType::Logarithmic> grid(50.0, 20.0, 100.0, 101);
	ASSERT_NEAR(grid.Get(0), grid.lb, 1e-12);
	ASSERT_NEAR(grid.Get(grid.N / 2), grid.x0, 1e-12);
	ASSERT_NEAR(grid.Get(grid.N - 1), grid.ub, 1e-12);
}

TEST (GridTest, AdaptiveGrid)
{
	CGrid<EGridType::Adaptive> grid(50.0, 20.0, 100.0, 101);
	ASSERT_NEAR(grid.Get(0), grid.lb, 1e-12);
	ASSERT_NEAR(grid.Get(grid.N / 2), grid.x0, 1e-12);
	ASSERT_NEAR(grid.Get(grid.N - 1), grid.ub, 1e-12);
}
