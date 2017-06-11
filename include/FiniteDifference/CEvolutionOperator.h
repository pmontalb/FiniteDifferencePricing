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

namespace fdpricing
{

enum class ESolverType
{
	Null,
	ExplicitEuler,
	ImplicitEuler,
	CrankNicolson
};

template<ESolverType T>
class CEvolutionOperator
{
public:
	CEvolutionOperator(const CInputData& __restrict__ input,
			const double lowerFactor = 1e-3, const double upperFactor = 10.0, const EGridType gridType = EGridType::Adaptive) noexcept
		: input(input),
		  grid(input.S, lowerFactor * input.S, upperFactor * input.S, gridType, input.N),
		  L(input, grid),
		  dt(input.T / input.M),
		  A(L)
	{
		switch (T)
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

				B = std::make_shared<CTridiagonalOperator>(L);
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

	void Apply(std::vector<double>& __restrict__ x) noexcept
	{
		switch (T)
		{
			case ESolverType::ExplicitEuler:
				A.Dot(x);
				break;
			case ESolverType::ImplicitEuler:
				A.Solve(x);
				break;
			case ESolverType::CrankNicolson:
			{
				A.Solve(x);
				B->Dot(x);
				break;
			}
			default:
				break;
		}
	}

private:
	const CInputData& __restrict__ input;
	const CGrid grid;

	// Space Discretization
	const CTridiagonalOperator L;

	// Space-Time Discretization
	const double dt;
	CTridiagonalOperator A; // right operator
	std::shared_ptr<CTridiagonalOperator> B; // left operator
};

} /* namespace fdpricing */

#endif /* FINITEDIFFERENCE_CEVOLUTIONOPERATOR_H_ */
