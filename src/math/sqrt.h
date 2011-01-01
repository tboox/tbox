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
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		sqrt.h
 *
 */
#ifndef TB_MATH_SQRT_H
#define TB_MATH_SQRT_H

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ////////////////////////////////////////////////////////////////////////
 * macros
 */

// isqrti(x) 
#define TB_MATH_ISQRTI(x) 					tb_math_isqrti(x)

// isqrtf(x) 
#define TB_MATH_ISQRTF(x) 					tb_math_isqrti(TB_MATH_IROUND(x))

// fsqrtf(x) 
#define TB_MATH_FSQRTF(x) 					sqrt(x)

/* ////////////////////////////////////////////////////////////////////////
 * implements
 */
tb_uint32_t tb_math_isqrti(tb_uint32_t x);


#endif

