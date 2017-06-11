/*
 * CTridiagonalOperator.cpp
 *
 *  Created on: 11 Jun 2017
 *      Author: raiden
 */

#include <FiniteDifference/CTridiagonalOperator.h>

namespace fdpricing
{

CTridiagonalOperator::CTridiagonalOperator(const size_t N)
	: N(N), matrix(N)
{
	
}

void CTridiagonalOperator::Dot(std::vector<double>& __restrict__ out, const std::vector<double>& __restrict__ in) const noexcept
{
#ifdef DEBUG
	if (in.size() != N)
	{
		printf("WRONG SIZE");
		throw;
	}
#endif

	out.resize(N);

	out[0] = matrix[0].Get(details::Zero) * in[0] + matrix[0].Get(details::Plus) * in[1];

	for(size_t i = 1; i < N - 1; ++i)
		out[i] = matrix[i].Get(details::Minus) * in[i - 1] + matrix[i].Get(details::Zero) * in[i] + matrix[i].Get(details::Plus) * in[i + 1];

	out[N - 1] = matrix[N - 1].Get(details::Minus) * in[N - 2] + matrix[N - 1].Get(details::Zero) * in[N - 1];
}

void CTridiagonalOperator::Solve(std::vector<double>& __restrict__ x) noexcept
{
#ifdef DEBUG
	if (x.size() != N)
	{
		printf("WRONG SIZE");
		throw;
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

} /* namespace fdpricing */
