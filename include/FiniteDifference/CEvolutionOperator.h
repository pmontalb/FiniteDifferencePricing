/*
 * CEvolutionOperator.h
 *
 *  Created on: 11 Jun 2017
 *      Author: raiden
 */

#ifndef FINITEDIFFERENCE_CEVOLUTIONOPERATOR_H_
#define FINITEDIFFERENCE_CEVOLUTIONOPERATOR_H_

#include <memory>

#include <FiniteDifference/CTridiagonalOperator.h>
#include <FiniteDifference/CGrid.h>
#include <Data/CInputData.h>
#include <Data/ESolverType.h>

namespace fdpricing
{

struct CFiniteDifferenceSettings
{
	double lowerFactor = 1e-3;
	double upperFactor = 10.0;
	EGridType gridType = EGridType::Adaptive;
};

template<ESolverType solverType, EAdjointDifferentiation adjointDifferentiation>
class CEvolutionOperator
{
public:
	CEvolutionOperator(const CInputData& __restrict__ input, const CFiniteDifferenceSettings& __restrict__ settings) noexcept
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

	virtual ~CEvolutionOperator() = default;

	CEvolutionOperator(const CEvolutionOperator& rhs) = delete;
	CEvolutionOperator(const CEvolutionOperator&& rhs) = delete;

	void Apply(CPayoffData& __restrict__ x) noexcept
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

private:
	const CInputData& __restrict__ input;
	const CFiniteDifferenceSettings& __restrict__ settings;
	const CGrid grid;

	// Space Discretization
	const CTridiagonalOperator<adjointDifferentiation> L;

	// Space-Time Discretization
	const double dt;
	CTridiagonalOperator<adjointDifferentiation> A; // right operator
	std::shared_ptr<CTridiagonalOperator<adjointDifferentiation>> B; // left operator
};

} /* namespace fdpricing */

#endif /* FINITEDIFFERENCE_CEVOLUTIONOPERATOR_H_ */
