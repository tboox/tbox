/*!The Treasure Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author		ruki
 * @file		rand.h
 * @ingroup 	math
 *
 */
#ifndef TB_MATH_RAND_H
#define TB_MATH_RAND_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the rand mode
typedef enum __tb_rand_mode_t
{
	TB_RAND_MODE_NULL 		= 0
,	TB_RAND_MODE_LINEAR 	= 1

}tb_rand_mode_t;

// the rand type
typedef struct __tb_rand_t
{
	tb_size_t 		mode;

}tb_rand_t;

// the linear rand type
typedef struct __tb_rand_linear_t
{
	// the rand base
	tb_rand_t 		base;

	// the rand seed
	tb_uint32_t 	seed;

	// the rand data
	tb_uint32_t 	data;

}tb_rand_linear_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

// the global rand
tb_bool_t 			tb_rand_init(tb_noarg_t);
tb_void_t 			tb_rand_exit(tb_noarg_t);

tb_void_t 			tb_rand_seed(tb_uint32_t seed);
tb_void_t 			tb_rand_clear(tb_noarg_t);

tb_uint32_t 		tb_rand_uint32(tb_uint32_t b, tb_uint32_t e);
tb_sint32_t 		tb_rand_sint32(tb_sint32_t b, tb_sint32_t e);

#ifdef TB_CONFIG_TYPE_FLOAT
tb_float_t 			tb_rand_float(tb_float_t b, tb_float_t e);
#endif

// the linear rand
tb_rand_linear_t* 	tb_rand_linear_init(tb_uint32_t seed);
tb_void_t 			tb_rand_linear_exit(tb_rand_linear_t* rand);

tb_void_t 			tb_rand_linear_seed(tb_rand_linear_t* rand, tb_uint32_t seed);
tb_void_t 			tb_rand_linear_clear(tb_rand_linear_t* rand);

tb_uint32_t 		tb_rand_linear_uint32(tb_rand_linear_t* rand, tb_uint32_t b, tb_uint32_t e);
tb_sint32_t 		tb_rand_linear_sint32(tb_rand_linear_t* rand, tb_sint32_t b, tb_sint32_t e);

#ifdef TB_CONFIG_TYPE_FLOAT
tb_float_t 			tb_rand_linear_float(tb_rand_linear_t* rand, tb_float_t b, tb_float_t e);
#endif

#endif

