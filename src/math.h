/*!The Tiny Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with TGraphic; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		math.h
 *
 */
#ifndef TB_MATH_H
#define TB_MATH_H

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <math.h>
#include <stdlib.h>

/* ////////////////////////////////////////////////////////////////////////
 * macros
 */

// min & max
#define TB_MATH_MAX(a, b) 					(((a) > (b))? (a) : (b))
#define TB_MATH_MIN(a, b) 					(((a) < (b))? (a) : (b))
#define TB_MATH_MAX_MIN(max, min, a, b)		((a) > (b)? ((*max) = (a), (*min) = (b)) : ((*max) = (b), (*min) = (a)))

// random
#define TB_MATH_RAND() 						rand()
#define TB_MATH_RAND_MAX 					(RAND_MAX)

// functions
#define TB_MATH_SIN(x) 						(sin((x)))
#define TB_MATH_COS(x) 						(cos((x)))
#define TB_MATH_TAN(x) 						(tan((x)))
#define TB_MATH_ASIN(x) 					(asin((x)))
#define TB_MATH_ACOS(x) 					(acos((x)))
#define TB_MATH_ATAN(x) 					(atan((x)))
#define TB_MATH_ATAN2(a, b) 				(atan2((a), (b)))
#define TB_MATH_LOG(x) 						(log((x)))
#define TB_MATH_EXP(x) 						(exp((x)))
#define TB_MATH_POW(a, b) 					(pow(a, b))
#define TB_MATH_ABS(x) 						(abs((x)))
#define TB_MATH_FABS(x) 					(fabs((x)))
#define TB_MATH_SQRT(x) 					(sqrt((x)))
#define TB_MATH_FLOOR(x) 					(floor((x)))
#define TB_MATH_CEIL(x) 					(ceil((x)))
#define TB_MATH_ROUND(x) 					(round((x)))
#define TB_MATH_IS_NAN(val) 				(isnan((val)))
#define TB_MATH_IS_INF(val) 				(isinf((val)))

// constants
#define TB_MATH_CONST_NAN 					(NAN)
#define TB_MATH_CONST_INFINITY 				(INFINITY)
#define TB_MATH_CONST_MAX_VALUE 			(1.79769313486232e+308)
#define TB_MATH_CONST_MIN_VALUE 			(4.94065645841247e-324)
#define TB_MATH_CONST_E 					(2.71828182845905)
#define TB_MATH_CONST_LN10 					(2.302585092994046)
#define TB_MATH_CONST_LN2 					(0.6931471805599453)
#define TB_MATH_CONST_LOG10E 				(0.4342944819032518)
#define TB_MATH_CONST_LOG2E 				(1.442695040888963387)
#define TB_MATH_CONST_PI 					(3.141592653589793)
#define TB_MATH_CONST_SQRT1_2 				(0.7071067811865476)
#define TB_MATH_CONST_SQRT2 				(1.4142135623730951)

// align
#define TB_MATH_ALIGN(size, boundary)		(((size) + ((boundary) - 1)) & ~((boundary) - 1))

// swap
#define TB_MATH_SWAP_INT(a, b) 				((a) ^= (b) ^= (a) ^= (b))

/* ////////////////////////////////////////////////////////////////////////
 * interfaces
 */



#endif

