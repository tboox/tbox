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
 * @file		linear.c
 * @ingroup 	math
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME 			"random_linear"
#define TB_TRACE_MODULE_DEBUG 			(1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "random.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the linear random type
typedef struct __tb_random_linear_t
{
	// the base
	tb_random_t 			base;

	// the seed
	tb_size_t 				seed;

	// the value
	tb_size_t 				value;

}tb_random_linear_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_random_linear_t* tb_random_linear_cast(tb_random_t* random)
{
	// check
	tb_assert_and_check_return_val(random && random->type == TB_RANDOM_GENERATOR_TYPE_LINEAR, tb_null);

	// the random
	return (tb_random_linear_t*)random;
}
static tb_void_t tb_random_linear_exit(tb_random_t* random)
{
	// exit it
	if (random) tb_free((tb_pointer_t)random);
}
static tb_void_t tb_random_linear_seed(tb_random_t* random, tb_size_t seed)
{
	// check
	tb_random_linear_t* lrandom = tb_random_linear_cast(random);
	tb_assert_and_check_return(lrandom);
	
	// update seed
	lrandom->seed 	= seed;
	lrandom->value 	= seed;
}
static tb_void_t tb_random_linear_clear(tb_random_t* random)
{
	// check
	tb_random_linear_t* lrandom = tb_random_linear_cast(random);
	tb_assert_and_check_return(lrandom);
	
	// clear
	lrandom->value = lrandom->seed;
}
static tb_long_t tb_random_linear_range(tb_random_t* random, tb_long_t beg, tb_long_t end)
{
	// check
	tb_random_linear_t* lrandom = tb_random_linear_cast(random);
	tb_assert_and_check_return_val(lrandom && beg < end, beg);

	// generate the next value
	lrandom->value = (lrandom->value * 10807 + 1) & 0xffffffff;

	// make the value with range
	return (beg + (tb_long_t)(lrandom->value % (end - beg)));
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_handle_t tb_random_linear_init(tb_size_t seed)
{
	// done
	tb_bool_t 			ok = tb_false;
	tb_random_linear_t* random = tb_null;
	do
	{
		// make random
		random = (tb_random_linear_t*)tb_malloc0(sizeof(tb_random_linear_t));
		tb_assert_and_check_break(random);

		// init random
		random->base.type 	= TB_RANDOM_GENERATOR_TYPE_LINEAR;
		random->base.exit 	= tb_random_linear_exit;
		random->base.seed 	= tb_random_linear_seed;
		random->base.clear 	= tb_random_linear_clear;
		random->base.range 	= tb_random_linear_range;
		random->seed 		= seed;
		random->value 		= seed;

		// ok
		ok = tb_true;

	} while (0);

	// failed?
	if (!ok)
	{
		// exit it
		if (random) tb_random_exit((tb_handle_t)random);
		random = tb_null;
	}

	// ok?
	return (tb_handle_t)random;
}
