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
	CEvolutionOperator(const CInputData& __restrict__ input, const CFiniteDifferenceSettings& __restrict__ settings) noexcept;

	virtual ~CEvolutionOperator() = default;

	CEvolutionOperator(const CEvolutionOperator& rhs) = delete;
	CEvolutionOperator(const CEvolutionOperator&& rhs) = delete;

	void Apply(CPayoffData& __restrict__ x) noexcept;

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

#include <FiniteDifference/CEvolutionOperator.tpp>

#endif /* FINITEDIFFERENCE_CEVOLUTIONOPERATOR_H_ */
