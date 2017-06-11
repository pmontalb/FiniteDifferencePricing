/*
 * ESolverType.h
 *
 *  Created on: 11 Jun 2017
 *      Author: raiden
 */

#ifndef DATA_ESOLVERTYPE_H_
#define DATA_ESOLVERTYPE_H_

namespace fdpricing
{

enum class ESolverType
{
	Null,
	ExplicitEuler,
	ImplicitEuler,
	CrankNicolson
};

}

#endif /* DATA_ESOLVERTYPE_H_ */
