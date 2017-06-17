/*
 * CTridiagonalOperator.tpp
 *
 *  Created on: 11 Jun 2017
 *      Author: raiden
 */

#include <Flags.h>

namespace fdpricing
{

template<EAdjointDifferentiation adjointDifferentiation>
CTridiagonalOperator<adjointDifferentiation>::CTridiagonalOperator(const size_t N) noexcept
	: N(N), matrix(N)
{
	switch (adjointDifferentiation)
	{
		case EAdjointDifferentiation::Vega:
			matrixVega.resize(N);
			break;
		case EAdjointDifferentiation::Rho:
			matrixRhoBorrow.resize(N);
			break;
		case EAdjointDifferentiation::All:
			matrixVega.resize(N);
			matrixRhoBorrow.resize(N);
			break;
		default:
			break;
	}
}

template<EAdjointDifferentiation T>
CTridiagonalOperator<T>::CTridiagonalOperator(const CInputData& unaliased input, const CGrid& unaliased grid) noexcept
	: CTridiagonalOperator(input.N)
{
	Make(input, grid);
}

template<EAdjointDifferentiation T>
CTridiagonalOperator<T>::CTridiagonalOperator(const CTridiagonalOperator& unaliased rhs) noexcept
	: N(rhs.N), matrix(rhs.matrix), matrixVega(rhs.matrixVega), matrixRhoBorrow(rhs.matrixRhoBorrow)
{

}

template<EAdjointDifferentiation adjointDifferentiation>
void CTridiagonalOperator<adjointDifferentiation>::Add(const double alpha, const double beta) noexcept
{
	for (size_t i = 0; i < N; ++i)
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

template<EAdjointDifferentiation adjointDifferentiation>
void CTridiagonalOperator<adjointDifferentiation>::Dot(CPayoffData& unaliased out) const noexcept
{
#ifdef DEBUG
	if (out.payoff_i.size() != N)
	{
		printf("*** WRONG PAYOFF SIZE ***\n");
		return;
	}
#endif

	// x_{n} = A \cdot x_{n + 1}
	// Therefore:
	// v_{n} = J \cdot x_{n + 1} + A \cdot v_{n + 1}
	switch (adjointDifferentiation)
	{
		case EAdjointDifferentiation::Vega:
			Dot(matrix, out.vega_i);  // A \cdot v_{n + 1}
			Add(out.vega_i, 1.0, matrixVega, out.payoff_i);  // J \cdot x_{n + 1}
			break;
		case EAdjointDifferentiation::Rho:
			Dot(matrix, out.rho_i);

			Dot(matrix, out.rhoBorrow_i);
			Add(out.rhoBorrow_i, 1.0, matrixRhoBorrow, out.payoff_i);
			break;
		case EAdjointDifferentiation::All:
			Dot(matrix, out.vega_i);
			Add(out.vega_i, 1.0, matrixVega, out.payoff_i);

			Dot(matrix, out.rho_i);

			Dot(matrix, out.rhoBorrow_i);
			Add(out.rhoBorrow_i, 1.0, matrixRhoBorrow, out.payoff_i);
			break;
		default:
			break;
	}

	// Only now we can update the payoff
	Dot(matrix, out.payoff_i);
}

template<EAdjointDifferentiation T>
void CTridiagonalOperator<T>::Add(std::vector<double>& unaliased out, const double factor, const details::Matrix& unaliased A, const std::vector<double>& unaliased x) const noexcept
{
	out[0] += factor * (A[0].Get(details::Zero) * x[0] + A[0].Get(details::Plus) * x[1]);

	for(size_t i = 1; i < N - 1; ++i)
		out[i] += factor * (A[i].Get(details::Minus) * x[i - 1] + A[i].Get(details::Zero) * x[i] + A[i].Get(details::Plus) * x[i + 1]);

	out[N - 1] += factor * (A[N - 1].Get(details::Minus) * x[N - 2] + A[N - 1].Get(details::Zero) * x[N - 1]);
}

template<EAdjointDifferentiation T>
void CTridiagonalOperator<T>::Dot(const details::Matrix& unaliased A, std::vector<double>& unaliased x) const noexcept
{
	std::array<double, 2> cache = { x[0], 0.0 };
	x[0] = A[0].Get(details::Zero) * x[0] + A[0].Get(details::Plus) * x[1];

	for(size_t i = 1; i < N - 1; ++i)
	{
		cache[i & 1] = x[i];
		x[i] = A[i].Get(details::Minus) * cache[(i - 1) & 1] + A[i].Get(details::Zero) * x[i] + A[i].Get(details::Plus) * x[i + 1];
	}

	x[N - 1] = A[N - 1].Get(details::Minus) * cache[(N - 2) & 1] + A[N - 1].Get(details::Zero) * x[N - 1];
}

template<EAdjointDifferentiation adjointDifferentiation>
void CTridiagonalOperator<adjointDifferentiation>::Solve(CPayoffData& unaliased out) noexcept
{
#ifdef DEBUG
	if (out.payoff_i.size() != N)
	{
		printf("*** WRONG PAYOFF SIZE ***\n");
		return;
	}
#endif

	// First we update the payoff
	Solve(out.payoff_i, matrix);

	// A \cdot x_{n} = x_{n + 1}
	// Therefore:
	// A \cdot v_{n} = v_{n + 1} - J \cdot x_{n}
	switch (adjointDifferentiation)
	{
		case EAdjointDifferentiation::Vega:
			Add(out.vega_i, -1.0, matrixVega, out.payoff_i);
			Solve(out.vega_i, matrix);
			break;
		case EAdjointDifferentiation::Rho:
			Solve(out.rho_i, matrix);

			Add(out.rhoBorrow_i, -1.0, matrixRhoBorrow, out.payoff_i);
			Solve(out.rhoBorrow_i, matrix);
			break;
		case EAdjointDifferentiation::All:
			Add(out.vega_i, -1.0, matrixVega, out.payoff_i);
			Solve(out.vega_i, matrix);

			Solve(out.rho_i, matrix);

			Add(out.rhoBorrow_i, -1.0, matrixRhoBorrow, out.payoff_i);
			Solve(out.rhoBorrow_i, matrix);
			break;
		default:
			break;
	}
}

template<EAdjointDifferentiation T>
void CTridiagonalOperator<T>::Solve(std::vector<double>& unaliased x, const details::Matrix& unaliased mat) noexcept
{
#ifdef DEBUG
	if (x.size() != N)
	{
		printf("*** WRONG VECTOR SIZE***\n");
		return;
	}
#endif

    if (!solve_cache.size())
    	solve_cache.resize(N);

    for (size_t i = 0; i < N - 1; ++i)
    	solve_cache[i] = mat[i].Get(details::Plus);

    solve_cache[0] = mat[0].Get(details::Plus) / mat[0].Get(details::Zero);
    x[0] = x[0] / mat[0].Get(details::Zero);

    for (size_t i = 1; i < N; ++i)
    {
        const double m = 1.0 / (mat[i].Get(details::Zero) - mat[i].Get(details::Minus) * solve_cache[i - 1]);
        solve_cache[i] = mat[i].Get(details::Plus) * m;
        x[i] = (x[i] - mat[i].Get(details::Minus) * x[i - 1]) * m;
    }

    for (size_t i = N - 1; i--> 0 ;)
        x[i] -= solve_cache[i] * x[i + 1];
}

template<EAdjointDifferentiation adjointDifferentiation>
void CTridiagonalOperator<adjointDifferentiation>::Make(const CInputData& unaliased input, const CGrid& unaliased grid) noexcept
{
#ifdef DEBUG
	if (matrix.size() != N)
	{
		printf("WRONG MATRIX SIZE");
		return;
	}
	if (grid.size() != N)
	{
		printf("WRONG GRID SIZE: N=%zu, gN=%zu", N, grid.size());
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

	for (size_t i = 1; i < N - 1; ++i)
	{
		const double dxPlus  = grid.Get(i + 1) - grid.Get(i);
		const double dxMinus = grid.Get(i)     - grid.Get(i - 1);
		const double dx = dxPlus + dxMinus;

		const double drift = input.b * grid.Get(i);
		const double volatility = sigma2 * grid.Get(i) * grid.Get(i);

		matrix[i].Set(details::Minus, (-dxPlus * drift + volatility) / (dxMinus * dx));
		matrix[i].Set(details::Plus,  (dxMinus * drift + volatility) / (dxPlus  * dx));

		#ifdef DEBUG

		if (matrix[i].Get(details::Minus) <= 0.0)
		{
			printf("******* Up factor too big(%g): increase grid size, mu=%g sigma^2=%g *******\n", matrix[i].Get(details::Minus), drift, volatility);
			return;
		}
		if (matrix[i].Get(details::Plus) <= 0.0)
		{
			printf("******* Down factor too big(%g): increase grid size, mu=%g sigma^2=%g *******\n", matrix[i].Get(details::Plus), drift, volatility);
			return;
		}

		#endif

		matrix[i].Set(details::Zero, -matrix[i].Get(details::Minus) - matrix[i].Get(details::Plus));

		switch (adjointDifferentiation)
		{
			case EAdjointDifferentiation::Vega:
				{
					const double dVolDSigma = 2.0 * input.sigma * grid.Get(i) * grid.Get(i);
					matrixVega[i].Set(details::Minus, dVolDSigma / (dxMinus * dx));
					matrixVega[i].Set(details::Plus,  dVolDSigma / (dxPlus * dx));
					matrixVega[i].Set(details::Zero, -matrixVega[i].Get(details::Minus) - matrixVega[i].Get(details::Plus));
				}
				break;
			case EAdjointDifferentiation::Rho:
					matrixRhoBorrow[i].Set(details::Minus, -dxPlus  * grid.Get(i) / (dxMinus * dx));
					matrixRhoBorrow[i].Set(details::Plus,   dxMinus * grid.Get(i) / (dxPlus * dx));
					matrixRhoBorrow[i].Set(details::Zero, -matrixRhoBorrow[i].Get(details::Minus) - matrixRhoBorrow[i].Get(details::Plus));
				break;
			case EAdjointDifferentiation::All:
				{
					const double dVolDSigma = 2.0 * input.sigma * grid.Get(i) * grid.Get(i);
					matrixVega[i].Set(details::Minus, dVolDSigma / (dxMinus * dx));
					matrixVega[i].Set(details::Plus,  dVolDSigma / (dxPlus * dx));
					matrixVega[i].Set(details::Zero, -matrixVega[i].Get(details::Minus) - matrixVega[i].Get(details::Plus));

					matrixRhoBorrow[i].Set(details::Minus, -dxPlus  * grid.Get(i) / (dxMinus * dx));
					matrixRhoBorrow[i].Set(details::Plus,   dxMinus * grid.Get(i) / (dxPlus * dx));
					matrixRhoBorrow[i].Set(details::Zero, -matrixRhoBorrow[i].Get(details::Minus) - matrixRhoBorrow[i].Get(details::Plus));
				}
				break;
			default:
				break;
		}
	}

	// Right BC: zero drift
	dx = grid.Get(N - 1) - grid.Get(N - 2);
	volatility = sigma2 * grid.Get(N - 1) * grid.Get(N - 1);
	matrix[N - 1].Set(details::Zero, -volatility / (dx * dx));
	matrix[N - 1].Set(details::Minus, -matrix[N - 1].Get(details::Zero));

	switch (adjointDifferentiation)
	{
		case EAdjointDifferentiation::Vega:
		case EAdjointDifferentiation::All:
			{
				const double dVolDSigma = 2.0 * input.sigma * grid.Get(N - 1) * grid.Get(N - 1);
				matrixVega[N - 1].Set(details::Zero, -dVolDSigma / (dx * dx));
				matrixVega[N - 1].Set(details::Plus, -matrixVega[N - 1].Get(details::Zero));
			}
			break;
		default:
			break;
	}
}

} /* namespace fdpricing */
