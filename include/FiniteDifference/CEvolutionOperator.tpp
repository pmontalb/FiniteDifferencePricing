/*
 * CEvolutionOperator.tpp
 *
 *  Created on: 13 Jun 2017
 *      Author: raiden
 */

#include <Flags.h>

namespace fdpricing
{

template<ESolverType solverType, EGridType gridType, EAdjointDifferentiation adjointDifferentiation>
CEvolutionOperator<solverType, gridType, adjointDifferentiation>::CEvolutionOperator(const CInputData& unaliased input, const CFiniteDifferenceSettings& unaliased settings) noexcept
	: grid(input.S, settings.lowerFactor * input.S, settings.upperFactor * input.S, input.N),
	  L(input, grid),
	  dt(input.T / input.M),
	  A(L)
{
	ctor();
}

template<ESolverType solverType, EGridType gridType, EAdjointDifferentiation adjointDifferentiation>
CEvolutionOperator<solverType, gridType, adjointDifferentiation>::CEvolutionOperator(const CEvolutionOperator& rhs, const double dt) noexcept
	: grid(rhs.grid), L(rhs.L), dt(dt), A(L)
{
	ctor();
}

template<ESolverType solverType, EGridType gridType, EAdjointDifferentiation adjointDifferentiation>
void CEvolutionOperator<solverType, gridType, adjointDifferentiation>::ctor() noexcept
{
	switch (solverType)
	{
		case ESolverType::ExplicitEuler:
			A.Add(1.0, dt);
			break;
		case ESolverType::ImplicitEuler:
			A.Add(1.0, -dt);
			break;
		case ESolverType::CrankNicolson:
		{
			B = std::make_unique<CTridiagonalOperator<gridType, adjointDifferentiation>>(L);

			const double halfDt = .5 * dt;
			A.Add(1.0, -halfDt);
			B->Add(1.0, halfDt);
			break;
		}
		default:
			break;
	}
}

template<ESolverType solverType, EGridType gridType, EAdjointDifferentiation adjointDifferentiation>
void CEvolutionOperator<solverType, gridType, adjointDifferentiation>::Apply(CPayoffData& unaliased x) noexcept
{
	switch (solverType)
	{
		case ESolverType::ExplicitEuler:
			A.Dot(x);
			break;
		case ESolverType::ImplicitEuler:
			A.Solve(x);
			break;
		case ESolverType::CrankNicolson:
		{
			B->Dot(x);
			A.Solve(x);
			break;
		}
		default:
			break;
	}
}

}
