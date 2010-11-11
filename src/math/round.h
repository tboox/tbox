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
 * \file		round.h
 *
 */
#ifndef TB_MATH_ROUND_H
#define TB_MATH_ROUND_H

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ////////////////////////////////////////////////////////////////////////
 * macros
 */

#define TB_MATH_IROUND(x) 					((x) > 0? (tb_int32_t)(tb_uint32_t)((x) + 0.5) : (tb_int32_t)(tb_uint32_t)((x) - 0.5))
#define TB_MATH_IFLOOR(x) 					((x) > 0? (tb_int32_t)(tb_uint32_t)(x) : (tb_int32_t)(tb_uint32_t)((x) - 0.9999999999))
#define TB_MATH_ICEIL(x) 					((x) > 0? (tb_int32_t)(tb_uint32_t)((x) + 0.9999999999) : (tb_int32_t)(tb_uint32_t)(x))


#endif

