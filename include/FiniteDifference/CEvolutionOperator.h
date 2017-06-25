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
};

/**
 * This class is a wrapper of CTridiagonalOperator for facilitating the operations (i.e. solve and dot product)
 */
template<ESolverType solverType=ESolverType::CrankNicolson,
		EGridType gridType=EGridType::Adaptive,
		EAdjointDifferentiation adjointDifferentiation=EAdjointDifferentiation::All>
class CEvolutionOperator
{
public:
	CEvolutionOperator(const CInputData& unaliased input, const CFiniteDifferenceSettings& unaliased settings) noexcept;

	/**
	 * Pseudo copy constructor: the copy is allowed only if dt needs to change (e.g. when a dividend occurs between time grid points)
	 */
	CEvolutionOperator(const CEvolutionOperator& rhs, const double dt) noexcept;

	virtual ~CEvolutionOperator() = default;

	/**
	 * Make this class not copy-/move-able
	 */
	CEvolutionOperator(const CEvolutionOperator& rhs) = delete;
	CEvolutionOperator(const CEvolutionOperator&& rhs) = delete;
	CEvolutionOperator& operator=(const CEvolutionOperator& rhs) = delete;
	CEvolutionOperator& operator=(const CEvolutionOperator&& rhs) = delete;

	/**
	 * Apply left/right operators to the input vector
	 */
	void Apply(CPayoffData& unaliased x) noexcept;

	const CGrid<gridType>& GetGrid() const noexcept
	{
		return grid;
	}

	double GetDt() const noexcept
	{
		return dt;
	}

private:
	const CGrid<gridType> grid;

	// Space Discretization
	const CTridiagonalOperator<gridType, adjointDifferentiation> L;

	// Space-Time Discretization
	const double dt;
	CTridiagonalOperator<gridType,adjointDifferentiation> A; // right operator
	std::unique_ptr<CTridiagonalOperator<gridType, adjointDifferentiation>> B; // left operator

	void ctor() noexcept;
};

} /* namespace fdpricing */

#include <FiniteDifference/CEvolutionOperator.tpp>

#endif /* FINITEDIFFERENCE_CEVOLUTIONOPERATOR_H_ */
