/*
 * Flags.h
 *
 *  Created on: 14 Jun 2017
 *      Author: raiden
 */

#ifndef FLAGS_H_
#define FLAGS_H_

/**
 * Activate debug checks and printf's
 */
//#define DEBUG

/**
 * Tells compilers that pointers are not aliased
 */
#define unaliased __restrict__

/**
 * These two macros are here just for experimenting the effects of these two keywords
 */
#define const const
#define noexcept noexcept

#endif /* FLAGS_H_ */
