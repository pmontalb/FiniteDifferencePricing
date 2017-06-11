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

	const double& Get(const ETridiagIndex idx) const noexcept
	{
		return data[idx];
	}

	void Set(const double val, const ETridiagIndex idx) noexcept
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

	virtual ~CTridiagonalOperator() = default;
	CTridiagonalOperator(const CTridiagonalOperator& rhs) = delete;
	CTridiagonalOperator(const CTridiagonalOperator&& rhs) = delete;
	CTridiagonalOperator& operator=(const CTridiagonalOperator& rhs) = delete;
	CTridiagonalOperator& operator=(const CTridiagonalOperator&& rhs) = delete;

	void Dot(std::vector<double>& __restrict__ out, const std::vector<double>& __restrict__ in) const noexcept;

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
};

} /* namespace fdpricing */

#endif /* FINITEDIFFERENCE_CTRIDIAGONALOPERATOR_H_ */
