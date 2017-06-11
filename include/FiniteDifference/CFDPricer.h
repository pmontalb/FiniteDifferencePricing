/*
 * CFDPricer.h
 *
 *  Created on: 11 Jun 2017
 *      Author: raiden
 */

#ifndef FINITEDIFFERENCE_CFDPRICER_H_
#define FINITEDIFFERENCE_CFDPRICER_H_

#include <FiniteDifference/CEvolutionOperator.h>
#include <Data/ECalculationType.h>
#include <Data/EAdjointDifferentiation.h>

namespace fdpricing
{

template <ESolverType solverType, ECalculationType calculationType, EAdjointDifferentiation adjointDifferentiation>
class CFDPricer
{
public:
	CFDPricer(const CInputData& __restrict__ input);
	virtual ~CFDPricer();

private:
	CEvolutionOperator<solverType, adjointDifferentiation> u;
};

} /* namespace fdpricing */

#endif /* FINITEDIFFERENCE_CFDPRICER_H_ */
