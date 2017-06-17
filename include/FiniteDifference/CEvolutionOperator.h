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
#include <Flags.h>

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
	CEvolutionOperator(const CInputData& unaliased input, const CFiniteDifferenceSettings& unaliased settings) noexcept;
	CEvolutionOperator(const CEvolutionOperator& rhs, const double dt) noexcept;

	virtual ~CEvolutionOperator() = default;

	CEvolutionOperator(const CEvolutionOperator& rhs) = delete;
	CEvolutionOperator(const CEvolutionOperator&& rhs) = delete;
	CEvolutionOperator& operator=(const CEvolutionOperator& rhs) = delete;
	CEvolutionOperator& operator=(const CEvolutionOperator&& rhs) = delete;

	void Apply(CPayoffData& unaliased x) noexcept;

	const CGrid& GetGrid() const noexcept
	{
		return grid;
	}

	double GetDt() const noexcept
	{
		return dt;
	}

private:
	const CGrid grid;

	// Space Discretization
	const CTridiagonalOperator<adjointDifferentiation> L;

	// Space-Time Discretization
	const double dt;
	CTridiagonalOperator<adjointDifferentiation> A; // right operator
	std::unique_ptr<CTridiagonalOperator<adjointDifferentiation>> B; // left operator

	void ctor() noexcept;
};

} /* namespace fdpricing */

#include <FiniteDifference/CEvolutionOperator.tpp>

#endif /* FINITEDIFFERENCE_CEVOLUTIONOPERATOR_H_ */
