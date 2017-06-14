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
#include <Data/CPayoffData.h>
#include <Data/EAdjointDifferentiation.h>
#include <Flags.h>

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

typedef std::vector<Triple> Matrix;

}


namespace fdpricing
{

template<EAdjointDifferentiation adjointDifferentiation>
class CTridiagonalOperator
{
public:
	CTridiagonalOperator(const size_t N) noexcept;
	CTridiagonalOperator(const CInputData& unaliased input, const CGrid& unaliased grid) noexcept;
	CTridiagonalOperator(const CTridiagonalOperator& __restrict rhs) noexcept;

	virtual ~CTridiagonalOperator() = default;
	CTridiagonalOperator& operator=(const CTridiagonalOperator& rhs) = delete;
	CTridiagonalOperator& operator=(const CTridiagonalOperator&& rhs) = delete;

	/**
	 * Compute LHS = diag(alpha) + beta * RHS
	 */
	void Add(const double alpha, const double beta) noexcept;

	void Dot(CPayoffData& unaliased payoffData) const noexcept;

	/**
	 * Thomas Algorithm: https://en.wikibooks.org/wiki/Algorithm_Implementation/Linear_Algebra/Tridiagonal_matrix_algorithm
	 *
	 * x: containts input/output
	 */
	void Solve(CPayoffData& unaliased payoffData) noexcept;

private:
	const size_t N;
	details::Matrix matrix;
	details::Matrix matrixVega;
	details::Matrix matrixRhoBorrow;

	std::vector<double> solve_cache;

	/**
	 * Set the operator according to the second order uneven mesh finite difference
	 */
	void Make(const CInputData& unaliased input, const CGrid& unaliased grid) noexcept;

	 /**
	  * Compute out += alpha * A * x
	  * */
	void Add(std::vector<double>& unaliased out, const double alpha, const details::Matrix& unaliased A, const std::vector<double>& unaliased x) const noexcept;

	void Dot(const details::Matrix& unaliased A, std::vector<double>& unaliased x) const noexcept;

	void Solve(std::vector<double>& unaliased x, const details::Matrix& unaliased m) noexcept;
};

} /* namespace fdpricing */

#endif /* FINITEDIFFERENCE_CTRIDIAGONALOPERATOR_H_ */

#include <FiniteDifference/CTridiagonalOperator.tpp>
