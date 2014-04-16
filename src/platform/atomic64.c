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
 * @file		atomic64.c
 * @ingroup 	platform
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "spinlock.h"
#include "atomic64.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// the atomic64 lock mac count
#ifdef __tb_small__
# 	define TB_ATOMIC64_LOCK_MAXN 		(16)
#else
# 	define TB_ATOMIC64_LOCK_MAXN 		(32)
#endif

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the atomic64 lock type
typedef struct __tb_atomic64_lock_t
{
	// the lock
	tb_spinlock_t 			lock;

	// the padding
	tb_byte_t 				padding[TB_L1_CACHE_BYTES];

}tb_atomic64_lock_t;

/* ///////////////////////////////////////////////////////////////////////
 * globals
 */

// the locks
static tb_atomic64_lock_t 	g_locks[TB_ATOMIC64_LOCK_MAXN] = {0};

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_hize_t tb_atomic64_get_generic(tb_atomic64_t* a)
{
	tb_trace_nosafe();
	tb_assert(a);
	return *a;
}
tb_void_t tb_atomic64_set_generic(tb_atomic64_t* a, tb_hize_t v)
{
	tb_trace_nosafe();
	tb_assert(a);
	*a = v;
}
tb_void_t tb_atomic64_set0_generic(tb_atomic64_t* a)
{
	tb_trace_nosafe();
	tb_assert(a);
	*a = 0;
}
tb_void_t tb_atomic64_pset_generic(tb_atomic64_t* a, tb_hize_t p, tb_hize_t v)
{
	tb_trace_nosafe();
	tb_assert(a);
	if (*a == p) *a = v;
}
tb_hize_t tb_atomic64_fetch_and_set0_generic(tb_atomic64_t* a)
{
	tb_trace_nosafe();
	tb_assert(a);

	tb_hize_t o = *a;
	*a = 0;
	return o;
}
tb_hize_t tb_atomic64_fetch_and_set_generic(tb_atomic64_t* a, tb_hize_t v)
{
	tb_trace_nosafe();
	tb_assert(a);

	tb_hize_t o = *a;
	*a = v;
	return o;
}
tb_hize_t tb_atomic64_fetch_and_pset_generic(tb_atomic64_t* a, tb_hize_t p, tb_hize_t v)
{
	tb_trace_nosafe();
	tb_assert(a);

	tb_hize_t o = *a;
	if (o == p) *a = v;
	return o;
}
tb_hong_t tb_atomic64_fetch_and_inc_generic(tb_atomic64_t* a)
{
	tb_trace_nosafe();
	tb_assert(a);

	tb_hong_t __tb_volatile__* pa = (tb_hong_t __tb_volatile__*)a;
	return *pa++;
}
tb_hong_t tb_atomic64_fetch_and_dec_generic(tb_atomic64_t* a)
{
	tb_trace_nosafe();
	tb_assert(a);

	tb_hong_t __tb_volatile__* pa = (tb_hong_t __tb_volatile__*)a;
	return *pa--;
}
tb_hong_t tb_atomic64_fetch_and_add_generic(tb_atomic64_t* a, tb_hong_t v)
{
	tb_trace_nosafe();
	tb_assert(a);

	tb_hong_t o = *((tb_hong_t*)a);
	*((tb_hong_t*)a) += v;
	return o;
}
tb_hong_t tb_atomic64_fetch_and_sub_generic(tb_atomic64_t* a, tb_hong_t v)
{
	tb_trace_nosafe();
	tb_assert(a);

	tb_hong_t o = *((tb_hong_t*)a);
	*((tb_hong_t*)a) -= v;
	return o;
}
tb_hize_t tb_atomic64_fetch_and_xor_generic(tb_atomic64_t* a, tb_hize_t v)
{
	tb_trace_nosafe();
	tb_assert(a);

	tb_hize_t o = *a;
	*a ^= v;
	return o;
}
tb_hize_t tb_atomic64_fetch_and_and_generic(tb_atomic64_t* a, tb_hize_t v)
{
	tb_trace_nosafe();
	tb_assert(a);

	tb_hize_t o = *a;
	*a &= v;
	return o;
}
tb_hize_t tb_atomic64_fetch_and_or_generic(tb_atomic64_t* a, tb_hize_t v)
{
	tb_trace_nosafe();
	tb_assert(a);

	tb_hize_t o = *a;
	*a |= v;
	return o;
}
tb_hong_t tb_atomic64_inc_and_fetch_generic(tb_atomic64_t* a)
{
	tb_trace_nosafe();
	tb_assert(a);

	tb_hong_t __tb_volatile__* pa = (tb_hong_t __tb_volatile__*)a;
	return ++*pa;
}
tb_hong_t tb_atomic64_dec_and_fetch_generic(tb_atomic64_t* a)
{
	tb_trace_nosafe();
	tb_assert(a);

	tb_hong_t __tb_volatile__* pa = (tb_hong_t __tb_volatile__*)a;
	return --*pa;
}
tb_hong_t tb_atomic64_add_and_fetch_generic(tb_atomic64_t* a, tb_hong_t v)
{
	tb_trace_nosafe();
	tb_assert(a);

	*((tb_hong_t*)a) += v;
	return *((tb_hong_t*)a);
}
tb_hong_t tb_atomic64_sub_and_fetch_generic(tb_atomic64_t* a, tb_hong_t v)
{
	tb_trace_nosafe();
	tb_assert(a);

	*((tb_hong_t*)a) -= v;
	return *((tb_hong_t*)a);
}
tb_hize_t tb_atomic64_xor_and_fetch_generic(tb_atomic64_t* a, tb_hize_t v)
{
	tb_trace_nosafe();
	tb_assert(a);

	*((tb_hong_t*)a) ^= v;
	return *((tb_hong_t*)a);
}
tb_hize_t tb_atomic64_and_and_fetch_generic(tb_atomic64_t* a, tb_hize_t v)
{
	tb_trace_nosafe();
	tb_assert(a);

	*((tb_hong_t*)a) &= v;
	return *((tb_hong_t*)a);
}
tb_hize_t tb_atomic64_or_and_fetch_generic(tb_atomic64_t* a, tb_hize_t v)
{
	tb_trace_nosafe();
	tb_assert(a);

	*((tb_hong_t*)a) |= v;
	return *((tb_hong_t*)a);
}

