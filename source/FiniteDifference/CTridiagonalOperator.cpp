/*
 * CTridiagonalOperator.cpp
 *
 *  Created on: 11 Jun 2017
 *      Author: raiden
 */

#include <FiniteDifference/CTridiagonalOperator.h>

#define DEBUG

namespace fdpricing
{

CTridiagonalOperator::CTridiagonalOperator(const size_t N)
	: N(N), matrix(N + 1)
{
}

CTridiagonalOperator::CTridiagonalOperator(const CInputData& __restrict__ input, const CGrid& __restrict__ grid)
	: CTridiagonalOperator(input.N)
{
	Make(input, grid);
}

CTridiagonalOperator::CTridiagonalOperator(const CTridiagonalOperator& __restrict__ rhs)
	: N(rhs.N), matrix(rhs.matrix)
{

}

void CTridiagonalOperator::Add(const double alpha, const double beta) noexcept
{
	for (size_t i = 0; i <= N; ++i)
	{
		matrix[i].Set(details::Zero, alpha + beta * matrix[i].Get(details::Zero));
		matrix[i].Set(details::Plus,         beta * matrix[i].Get(details::Plus));
		matrix[i].Set(details::Minus,        beta * matrix[i].Get(details::Minus));
	}
}

void CTridiagonalOperator::Dot(std::vector<double>& __restrict__ x) const noexcept
{
	const std::vector<double> xCopy(x);
	Dot(x, xCopy);
}

void CTridiagonalOperator::Dot(std::vector<double>& __restrict__ out, const std::vector<double>& __restrict__ in) const noexcept
{
#ifdef DEBUG
	if (out.size() != (N + 1) || in.size() != (N + 1))
	{
		printf("WRONG SIZE");
		return;
	}
#endif

	out[0] = matrix[0].Get(details::Zero) * in[0] + matrix[0].Get(details::Plus) * in[1];

	for(size_t i = 1; i < N; ++i)
		out[i] = matrix[i].Get(details::Minus) * in[i - 1] + matrix[i].Get(details::Zero) * in[i] + matrix[i].Get(details::Plus) * in[i + 1];

	out[N] = matrix[N].Get(details::Minus) * in[N - 1] + matrix[N].Get(details::Zero) * in[N];
}

void CTridiagonalOperator::Solve(std::vector<double>& __restrict__ x) noexcept
{
#ifdef DEBUG
	if (x.size() != N + 1)
	{
		printf("WRONG SIZE");
		return;
	}
#endif

    if (!solve_cache.size())
    {
    	solve_cache.resize(N - 1);
    	for (size_t i = 0; i < N - 1; ++i)
    		solve_cache[i] = matrix[i].Get(details::Plus);
    }

    solve_cache[0] = matrix[0].Get(details::Plus) / matrix[0].Get(details::Zero);
    x[0] = x[0] / matrix[0].Get(details::Zero);

    for (size_t i = 1; i < N; ++i)
    {
        const double m = 1.0 / (matrix[i].Get(details::Zero) - matrix[i].Get(details::Minus) * solve_cache[i - 1]);
        solve_cache[i] = matrix[i].Get(details::Plus) * m;
        x[i] = (x[i] - matrix[i].Get(details::Minus) * x[i - 1]) * m;
    }

    for (size_t i = N - 2; ; --i)
        x[i] -= solve_cache[i] * x[i + 1];
}

void CTridiagonalOperator::Make(const CInputData& __restrict__ input, const CGrid& __restrict__ grid) noexcept
{
#ifdef DEBUG
	if (matrix.size() != N + 1)
	{
		printf("WRONG MATRIX SIZE");
		return;
	}
	if (grid.size() != N + 1)
	{
		printf("WRONG GRID SIZE");
		return;
	}
#endif
	const double sigma2 = input.sigma * input.sigma;

	// Left BC: zero drift
	double dx = grid.Get(1) - grid.Get(0);
	double volatility = sigma2 * grid.Get(0) * grid.Get(0);
	matrix[0].Set(details::Zero, -volatility / (dx * dx));
	matrix[0].Set(details::Plus, -matrix[0].Get(details::Zero));

	for (size_t i = 1; i < N; ++i)
	{
		const double dxPlus  = grid.Get(i + 1) - grid.Get(i);
		const double dxMinus = grid.Get(i)     - grid.Get(i - 1);
		const double dx = dxPlus + dxMinus;

		const double drift = input.b * grid.Get(i);
		const double volatility = sigma2 * grid.Get(i) * grid.Get(i);

		matrix[i].Set(details::Minus, (-dxPlus * drift + volatility) / (dxMinus * dx));
		matrix[i].Set(details::Plus,  (dxMinus * drift + volatility) / (dxPlus  * dx));

		matrix[i].Set(details::Zero, -matrix[i].Get(details::Minus) - matrix[i].Get(details::Plus));
	}

	// Right BC: zero drift
	dx = grid.Get(N) - grid.Get(N - 1);
	volatility = sigma2 * grid.Get(N) * grid.Get(N);
	matrix[N].Set(details::Zero, -volatility / (dx * dx));
	matrix[N].Set(details::Minus, -matrix[N].Get(details::Zero));
}

} /* namespace fdpricing */
