/*
 * CEvolutionOperator.tpp
 *
 *  Created on: 13 Jun 2017
 *      Author: raiden
 */

namespace fdpricing
{

template<ESolverType solverType, EAdjointDifferentiation adjointDifferentiation>
CEvolutionOperator<solverType, adjointDifferentiation>::CEvolutionOperator(const CInputData& __restrict__ input, const CFiniteDifferenceSettings& __restrict__ settings) noexcept
	: input(input),
	  settings(settings),
	  grid(input.S, settings.lowerFactor * input.S, settings.upperFactor * input.S, settings.gridType, input.N),
	  L(input, grid),
	  dt(input.T / input.M),
	  A(L)
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
			const double halfDt = .5 * dt;
			A.Add(1.0, -halfDt);

			B = std::make_shared<CTridiagonalOperator<adjointDifferentiation>>(L);
			B->Add(1.0, halfDt);
			break;
		}
		default:
			break;
	}
}

template<ESolverType solverType, EAdjointDifferentiation adjointDifferentiation>
void CEvolutionOperator<solverType, adjointDifferentiation>::Apply(CPayoffData& __restrict__ x) noexcept
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
