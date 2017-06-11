/*
 * CTridiagonalOperator.h
 *
 *  Created on: 11 Jun 2017
 *      Author: raiden
 */

#ifndef FINITEDIFFERENCE_CTRIDIAGONALOPERATOR_H_
#define FINITEDIFFERENCE_CTRIDIAGONALOPERATOR_H_

#include <vector>
#include <array>
#include <stddef.h>

#include <FiniteDifference/CGrid.h>
#include <Data/CInputData.h>

namespace details
{

enum ETridiagIndex
{
	Minus = 0,
	Zero = 1,
	Plus = 2
};

class Triple
{
public:
	Triple(const double sub, const double diag, const double super) : data({sub, diag, super}) {};
	Triple() noexcept: Triple(0.0, 0.0, 0.0){};
	Triple(const Triple& rhs) : data(rhs.data) {}

	const double& Get(const ETridiagIndex idx) const noexcept
	{
		return data[idx];
	}

	void Set(const ETridiagIndex idx, const double val) noexcept
	{
		data[idx] = val;
	}
private:
	std::array<double, 3> data;
};

}


namespace fdpricing
{

class CTridiagonalOperator
{
public:
	CTridiagonalOperator(const size_t N);
	CTridiagonalOperator(const CInputData& __restrict__ input, const CGrid& __restrict__ grid);
	CTridiagonalOperator(const CTridiagonalOperator& __restrict rhs);

	virtual ~CTridiagonalOperator() = default;
	CTridiagonalOperator& operator=(const CTridiagonalOperator& rhs) = delete;
	CTridiagonalOperator& operator=(const CTridiagonalOperator&& rhs) = delete;

	/**
	 * Compute LHS = diag(alpha) + beta * RHS
	 */
	void Add(const double alpha, const double beta) noexcept;

	void Dot(std::vector<double>& __restrict__ x) const noexcept;

	/**
	 * Thomas Algorithm: https://en.wikibooks.org/wiki/Algorithm_Implementation/Linear_Algebra/Tridiagonal_matrix_algorithm
	 *
	 * x: containts input/output
	 */
	void Solve(std::vector<double>& __restrict__ x) noexcept;

private:
	const size_t N;
	std::vector<details::Triple> matrix;
	std::vector<double> solve_cache;

	/**
	 * Set the operator according to the second order uneven mesh finite difference
	 */
	void Make(const CInputData& __restrict__ input, const CGrid& __restrict__ grid) noexcept;

	void Dot(std::vector<double>& __restrict__ out, const std::vector<double>& __restrict__ in) const noexcept;
};

} /* namespace fdpricing */

#endif /* FINITEDIFFERENCE_CTRIDIAGONALOPERATOR_H_ */
