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

template<EAdjointDifferentiation adjointDifferentiation>
CTridiagonalOperator<adjointDifferentiation>::CTridiagonalOperator(const size_t N)
	: N(N), matrix(N + 1)
{
	switch (adjointDifferentiation)
	{
		case EAdjointDifferentiation::Vega:
			matrixVega.resize(N + 1);
			break;
		case EAdjointDifferentiation::Rho:
			matrixRhoBorrow.resize(N + 1);
			break;
		case EAdjointDifferentiation::All:
			matrixVega.resize(N + 1);
			matrixRhoBorrow.resize(N + 1);
			break;
		default:
			break;
	}
}

template<EAdjointDifferentiation T>
CTridiagonalOperator<T>::CTridiagonalOperator(const CInputData& __restrict__ input, const CGrid& __restrict__ grid)
	: CTridiagonalOperator(input.N)
{
	Make(input, grid);
}

template<EAdjointDifferentiation T>
CTridiagonalOperator<T>::CTridiagonalOperator(const CTridiagonalOperator& __restrict__ rhs)
	: N(rhs.N), matrix(rhs.matrix)
{

}

template<EAdjointDifferentiation adjointDifferentiation>
void CTridiagonalOperator<adjointDifferentiation>::Add(const double alpha, const double beta) noexcept
{
	for (size_t i = 0; i <= N; ++i)
	{
		matrix[i].Set(details::Zero, alpha + beta * matrix[i].Get(details::Zero));
		matrix[i].Set(details::Plus,         beta * matrix[i].Get(details::Plus));
		matrix[i].Set(details::Minus,        beta * matrix[i].Get(details::Minus));

		switch (adjointDifferentiation)
		{
			case EAdjointDifferentiation::Vega:
				matrixVega[i].Set(details::Zero,  beta * matrixVega[i].Get(details::Zero));
				matrixVega[i].Set(details::Plus,  beta * matrixVega[i].Get(details::Plus));
				matrixVega[i].Set(details::Minus, beta * matrixVega[i].Get(details::Minus));
				break;
			case EAdjointDifferentiation::Rho:
				matrixRhoBorrow[i].Set(details::Zero,  beta * matrixRhoBorrow[i].Get(details::Zero));
				matrixRhoBorrow[i].Set(details::Plus,  beta * matrixRhoBorrow[i].Get(details::Plus));
				matrixRhoBorrow[i].Set(details::Minus, beta * matrixRhoBorrow[i].Get(details::Minus));
				break;
			case EAdjointDifferentiation::All:
				matrixVega[i].Set(details::Zero,  beta * matrixVega[i].Get(details::Zero));
				matrixVega[i].Set(details::Plus,  beta * matrixVega[i].Get(details::Plus));
				matrixVega[i].Set(details::Minus, beta * matrixVega[i].Get(details::Minus));

				matrixRhoBorrow[i].Set(details::Zero,  beta * matrixRhoBorrow[i].Get(details::Zero));
				matrixRhoBorrow[i].Set(details::Plus,  beta * matrixRhoBorrow[i].Get(details::Plus));
				matrixRhoBorrow[i].Set(details::Minus, beta * matrixRhoBorrow[i].Get(details::Minus));
				break;
			default:
				break;
		}
	}
}

template<EAdjointDifferentiation T>
void CTridiagonalOperator<T>::Dot(CPayoffData& __restrict__ payoffData) const noexcept
{
	CPayoffData payoffDataCopy;
	payoffDataCopy.Copy<T>(payoffData);
	Dot(payoffData, payoffDataCopy);
}

template<EAdjointDifferentiation adjointDifferentiation>
void CTridiagonalOperator<adjointDifferentiation>::Dot(CPayoffData& __restrict__ out, const CPayoffData& __restrict__ in) const noexcept
{
#ifdef DEBUG
	if (out.payoff_i.size() != (N + 1) || in.payoff_i.size() != (N + 1))
	{
		printf("WRONG SIZE");
		return;
	}
#endif

	Dot(out.payoff_i, in.payoff_i, matrix);

/*	switch (adjointDifferentiation)
	{
		case EAdjointDifferentiation::Vega:
			Dot(out.vega_i, in.vega_i, matrixVega);
			break;
		case EAdjointDifferentiation::Rho:
			Dot(out.rhoBorrow_i, in.rhoBorrow_i, matrixRhoBorrow);
			break;
		case EAdjointDifferentiation::All:
			Dot(out.vega_i, in.vega_i, matrixVega);
			Dot(out.rhoBorrow_i, in.rhoBorrow_i, matrixRhoBorrow);
			break;
		default:
			break;
	}*/
}

template<EAdjointDifferentiation T>
void CTridiagonalOperator<T>::Dot(std::vector<double>& __restrict__ out, const std::vector<double>& __restrict__ in, const details::Matrix& __restrict__ m) const noexcept
{
	out[0] = m[0].Get(details::Zero) * in[0] + m[0].Get(details::Plus) * in[1];

	for(size_t i = 1; i < N; ++i)
		out[i] = m[i].Get(details::Minus) * in[i - 1] + m[i].Get(details::Zero) * in[i] + m[i].Get(details::Plus) * in[i + 1];

	out[N] = m[N].Get(details::Minus) * in[N - 1] + m[N].Get(details::Zero) * in[N];
}

template<EAdjointDifferentiation adjointDifferentiation>
void CTridiagonalOperator<adjointDifferentiation>::Solve(CPayoffData& __restrict__ out) noexcept
{
	Solve(out.payoff_i, matrix);

/*	switch (adjointDifferentiation)
	{
		case EAdjointDifferentiation::Vega:
			Solve(out.vega_i, matrixVega);
			break;
		case EAdjointDifferentiation::Rho:
			Solve(out.rhoBorrow_i, matrixRhoBorrow);
			break;
		case EAdjointDifferentiation::All:
			Solve(out.vega_i, matrixVega);
			Solve(out.rhoBorrow_i, matrixRhoBorrow);
			break;
		default:
			break;
	}*/
}

template<EAdjointDifferentiation T>
void CTridiagonalOperator<T>::Solve(std::vector<double>& __restrict__ x, const details::Matrix& __restrict__ mat) noexcept
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
    	for (size_t i = 0; i <= N - 1; ++i)
    		solve_cache[i] = mat[i].Get(details::Plus);
    }

    solve_cache[0] = mat[0].Get(details::Plus) / mat[0].Get(details::Zero);
    x[0] = x[0] / mat[0].Get(details::Zero);

    for (size_t i = 1; i <= N; ++i)
    {
        const double m = 1.0 / (mat[i].Get(details::Zero) - mat[i].Get(details::Minus) * solve_cache[i - 1]);
        solve_cache[i] = mat[i].Get(details::Plus) * m;
        x[i] = (x[i] - mat[i].Get(details::Minus) * x[i - 1]) * m;
    }

    for (size_t i = N - 1; ; --i)
        x[i] -= solve_cache[i] * x[i + 1];
}

template<EAdjointDifferentiation adjointDifferentiation>
void CTridiagonalOperator<adjointDifferentiation>::Make(const CInputData& __restrict__ input, const CGrid& __restrict__ grid) noexcept
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

	switch (adjointDifferentiation)
	{
		case EAdjointDifferentiation::Vega:
		case EAdjointDifferentiation::All:
			{
				const double dVolDSigma = 2.0 * input.sigma * grid.Get(0) * grid.Get(0);
				matrixVega[0].Set(details::Zero, -dVolDSigma / (dx * dx));
				matrixVega[0].Set(details::Plus, -matrixVega[0].Get(details::Zero));
			}
			break;
		default:
			break;
	}

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

		switch (adjointDifferentiation)
		{
			case EAdjointDifferentiation::Vega:
				{
					const double dVolDSigma = 2.0 * input.sigma * grid.Get(i) * grid.Get(i);
					matrixVega[i].Set(details::Minus, dVolDSigma / (dxMinus * dx));
					matrixVega[i].Set(details::Plus,  dVolDSigma / (dxPlus * dx));
				}
				break;
			case EAdjointDifferentiation::Rho:
					matrixRhoBorrow[i].Set(details::Minus, -dxPlus  * grid.Get(i) / (dxMinus * dx));
					matrixRhoBorrow[i].Set(details::Plus,   dxMinus * grid.Get(i) / (dxPlus * dx));
				break;
			case EAdjointDifferentiation::All:
				{
					const double dVolDSigma = 2.0 * input.sigma * grid.Get(i) * grid.Get(i);
					matrixVega[i].Set(details::Minus, dVolDSigma / (dxMinus * dx));
					matrixVega[i].Set(details::Plus,  dVolDSigma / (dxPlus * dx));

					matrixRhoBorrow[i].Set(details::Minus, -dxPlus  * grid.Get(i) / (dxMinus * dx));
					matrixRhoBorrow[i].Set(details::Plus,   dxMinus * grid.Get(i) / (dxPlus * dx));
				}
				break;
			default:
				break;
		}
	}

	// Right BC: zero drift
	dx = grid.Get(N) - grid.Get(N - 1);
	volatility = sigma2 * grid.Get(N) * grid.Get(N);
	matrix[N].Set(details::Zero, -volatility / (dx * dx));
	matrix[N].Set(details::Minus, -matrix[N].Get(details::Zero));

	switch (adjointDifferentiation)
	{
		case EAdjointDifferentiation::Vega:
		case EAdjointDifferentiation::All:
			{
				const double dVolDSigma = 2.0 * input.sigma * grid.Get(N) * grid.Get(N);
				matrixVega[N].Set(details::Zero, -dVolDSigma / (dx * dx));
				matrixVega[N].Set(details::Plus, -matrixVega[N].Get(details::Zero));
			}
			break;
		default:
			break;
	}
}

} /* namespace fdpricing */
