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
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * @author		ruki
 * @file		rand.c
 *
 */
/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "rand.h"
#include "../memory/memory.h"
#include "../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_RAND_LINEAR_SEED 		(1)

/* ///////////////////////////////////////////////////////////////////////
 * globals
 */
static tb_rand_t* 		g_rand = tb_null;
static tb_spinlock_t 	g_lock = TB_SPINLOCK_INIT;

/* ///////////////////////////////////////////////////////////////////////
 * details
 */
static __tb_inline__ tb_uint32_t tb_rand_linear_next_uint32(tb_rand_linear_t* rand)
{
	rand->data = (rand->data * 10807 + 1) & 0xffffffff;
	return rand->data;
}

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */

// the global rand
tb_bool_t tb_rand_init()
{
	tb_spinlock_enter(&g_lock);
	if (!g_rand) g_rand = tb_rand_linear_init(TB_RAND_LINEAR_SEED);
	tb_spinlock_leave(&g_lock);

	return g_rand? tb_true : tb_false;
}
tb_void_t tb_rand_exit()
{
	tb_spinlock_enter(&g_lock);
	if (g_rand)
	{
		tb_rand_linear_exit(g_rand);
		g_rand = tb_null;
	}
	tb_spinlock_leave(&g_lock);
}
tb_void_t tb_rand_seed(tb_uint32_t seed)
{
	// check 
	tb_assert_and_check_return(g_rand);

	// enter
	tb_spinlock_enter(&g_lock);

	// seed
	tb_rand_linear_seed(g_rand, seed);

	// leave
	tb_spinlock_leave(&g_lock);
}
tb_void_t tb_rand_clear()
{
	// check 
	tb_assert_and_check_return(g_rand);

	// enter
	tb_spinlock_enter(&g_lock);
	
	// clear
	tb_rand_linear_clear(g_rand);

	// leave
	tb_spinlock_leave(&g_lock);
}
tb_uint32_t tb_rand_uint32(tb_uint32_t b, tb_uint32_t e)
{
	// check 
	tb_uint32_t rand = b;
	tb_assert_and_check_return_val(g_rand, rand);

	// enter
	tb_spinlock_enter(&g_lock);
	
	// rand
	rand = tb_rand_linear_uint32(g_rand, b, e);

	// leave
	tb_spinlock_leave(&g_lock);

	// ok
	return rand;
}
tb_sint32_t tb_rand_sint32(tb_sint32_t b, tb_sint32_t e)
{
	// check 
	tb_sint32_t rand = b;
	tb_assert_and_check_return_val(g_rand, rand);

	// enter
	tb_spinlock_enter(&g_lock);
	
	// rand
	rand = tb_rand_linear_sint32(g_rand, b, e);

	// leave
	tb_spinlock_leave(&g_lock);

	// ok
	return rand;
}

#ifdef TB_CONFIG_TYPE_FLOAT
tb_float_t tb_rand_float(tb_float_t b, tb_float_t e)
{
	// check 
	tb_float_t rand = b;
	tb_assert_and_check_return_val(g_rand, rand);

	// enter
	tb_spinlock_enter(&g_lock);
	
	// rand
	rand = tb_rand_linear_float(g_rand, b, e);

	// leave
	tb_spinlock_leave(&g_lock);

	// ok
	return rand;
}
#endif

// the linear rand
tb_rand_linear_t* tb_rand_linear_init(tb_uint32_t seed)
{
	// alloc rand
	tb_rand_linear_t* rand = tb_malloc0(sizeof(tb_rand_linear_t));
	tb_assert_and_check_return_val(rand, tb_null);

	// init rand
	rand->seed = seed;
	rand->data = seed;

	return rand;
}
tb_void_t tb_rand_linear_exit(tb_rand_linear_t* rand)
{
	if (rand) tb_free(rand);
}
tb_void_t tb_rand_linear_seed(tb_rand_linear_t* rand, tb_uint32_t seed)
{
	tb_assert_and_check_return(rand);
	rand->seed = seed;
	rand->data = seed;
}
tb_void_t tb_rand_linear_clear(tb_rand_linear_t* rand)
{
	tb_assert_and_check_return(rand);

	// init rand
	rand->data = rand->seed;
}
#ifdef TB_CONFIG_TYPE_FLOAT
tb_float_t tb_rand_linear_float(tb_rand_linear_t* rand, tb_float_t b, tb_float_t e)
{
	tb_assert_and_check_return_val(rand && e > b, 0);
	return (b + (((tb_float_t)tb_rand_linear_next_uint32(rand) * (e - b)) / TB_MAXU32));
}
tb_uint32_t tb_rand_linear_uint32(tb_rand_linear_t* rand, tb_uint32_t b, tb_uint32_t e)
{
	return (tb_uint32_t)tb_rand_linear_float(rand, (tb_float_t)b, (tb_float_t)e);
}
tb_sint32_t tb_rand_linear_sint32(tb_rand_linear_t* rand, tb_sint32_t b, tb_sint32_t e)
{
	return (tb_sint32_t)tb_rand_linear_float(rand, (tb_float_t)b, (tb_float_t)e);
}
#else
tb_uint32_t tb_rand_linear_uint32(tb_rand_linear_t* rand, tb_uint32_t b, tb_uint32_t e)
{
	// FIXME: for small range
	tb_assert_and_check_return_val(rand && e > b, 0);
	return (b + (tb_rand_linear_next_uint32(rand) % (e - b)));
}
tb_sint32_t tb_rand_linear_sint32(tb_rand_linear_t* rand, tb_sint32_t b, tb_sint32_t e)
{
	tb_assert_and_check_return_val(rand && e > b, 0);
	return (b + (tb_sint32_t)(tb_rand_linear_next_uint32(rand) % (e - b)));
}
#endif

