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

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "spinlock.h"
#include "atomic64.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the atomic64 lock mac count
#define TB_ATOMIC64_LOCK_MAXN 		(16)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the atomic64 lock type
typedef struct __tb_atomic64_lock_t
{
	// the lock
	tb_spinlock_t 			lock;

	// the padding
	tb_byte_t 				padding[TB_L1_CACHE_BYTES];

}tb_atomic64_lock_t __tb_cacheline_aligned__;

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the locks
static tb_atomic64_lock_t 	g_locks[TB_ATOMIC64_LOCK_MAXN] = {TB_SPINLOCK_INIT};

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

static __tb_inline_force__ tb_spinlock_t* tb_atomic64_lock(tb_atomic64_t* a)
{
	// trace
	tb_trace1_w("using generic atomic64, maybe slower!");

	// the addr
	tb_hize_t addr = (tb_hong_t)a;

	// compile the hash value
	addr >>= TB_L1_CACHE_SHIFT;
	addr ^= (addr >> 8) ^ (addr >> 16);

	// the lock
	return &g_locks[addr & (TB_ATOMIC64_LOCK_MAXN - 1)].lock;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_hong_t tb_atomic64_get_generic(tb_atomic64_t* a)
{
	// check
	tb_assert(a);

	// the lock
	tb_spinlock_t* lock = tb_atomic64_lock(a);

	// enter
	tb_spinlock_enter(lock);

	// get value
	tb_hong_t v = (tb_hong_t)*a;

	// leave
	tb_spinlock_leave(lock);

	// ok?
	return v;
}
tb_void_t tb_atomic64_set_generic(tb_atomic64_t* a, tb_hong_t v)
{
	// check
	tb_assert(a);

	// the lock
	tb_spinlock_t* lock = tb_atomic64_lock(a);

	// enter
	tb_spinlock_enter(lock);

	// set value
	*a = (tb_atomic64_t)v;

	// leave
	tb_spinlock_leave(lock);
}
tb_void_t tb_atomic64_set0_generic(tb_atomic64_t* a)
{
	tb_atomic64_set_generic(a, 0);
}
tb_void_t tb_atomic64_pset_generic(tb_atomic64_t* a, tb_hong_t p, tb_hong_t v)
{
	// check
	tb_assert(a);

	// the lock
	tb_spinlock_t* lock = tb_atomic64_lock(a);

	// enter
	tb_spinlock_enter(lock);

	// set value
	if ((tb_hong_t)*a == p) *a = (tb_atomic64_t)v;

	// leave
	tb_spinlock_leave(lock);
}
tb_hong_t tb_atomic64_fetch_and_set0_generic(tb_atomic64_t* a)
{
	return tb_atomic64_fetch_and_set_generic(a, 0);
}
tb_hong_t tb_atomic64_fetch_and_set_generic(tb_atomic64_t* a, tb_hong_t v)
{
	// check
	tb_assert(a);

	// the lock
	tb_spinlock_t* lock = tb_atomic64_lock(a);

	// enter
	tb_spinlock_enter(lock);

	// set value
	tb_hong_t o = (tb_hong_t)*a; *a = (tb_atomic64_t)v;

	// leave
	tb_spinlock_leave(lock);

	// ok?
	return o;
}
tb_hong_t tb_atomic64_fetch_and_pset_generic(tb_atomic64_t* a, tb_hong_t p, tb_hong_t v)
{
	// check
	tb_assert(a);

	// the lock
	tb_spinlock_t* lock = tb_atomic64_lock(a);

	// enter
	tb_spinlock_enter(lock);

	// set value
	tb_hong_t o = (tb_hong_t)*a; if (o == p) *a = (tb_atomic64_t)v;

	// leave
	tb_spinlock_leave(lock);

	// ok?
	return o;
}
tb_hong_t tb_atomic64_fetch_and_inc_generic(tb_atomic64_t* a)
{
	// check
	tb_assert(a);

	// the lock
	tb_spinlock_t* lock = tb_atomic64_lock(a);

	// enter
	tb_spinlock_enter(lock);

	// set value
	tb_hong_t o = (tb_hong_t)(*a)++;

	// leave
	tb_spinlock_leave(lock);

	// ok?
	return o;
}
tb_hong_t tb_atomic64_fetch_and_dec_generic(tb_atomic64_t* a)
{
	// check
	tb_assert(a);

	// the lock
	tb_spinlock_t* lock = tb_atomic64_lock(a);

	// enter
	tb_spinlock_enter(lock);

	// set value
	tb_hong_t o = (tb_hong_t)(*a)--;

	// leave
	tb_spinlock_leave(lock);

	// ok?
	return o;
}
tb_hong_t tb_atomic64_fetch_and_add_generic(tb_atomic64_t* a, tb_hong_t v)
{
	// check
	tb_assert(a);

	// the lock
	tb_spinlock_t* lock = tb_atomic64_lock(a);

	// enter
	tb_spinlock_enter(lock);

	// set value
	tb_hong_t o = (tb_hong_t)*a; *a += (tb_atomic64_t)v;

	// leave
	tb_spinlock_leave(lock);

	// ok?
	return o;
}
tb_hong_t tb_atomic64_fetch_and_sub_generic(tb_atomic64_t* a, tb_hong_t v)
{
	return tb_atomic64_fetch_and_add_generic(a, -v);
}
tb_hong_t tb_atomic64_fetch_and_xor_generic(tb_atomic64_t* a, tb_hong_t v)
{
	// check
	tb_assert(a);

	// the lock
	tb_spinlock_t* lock = tb_atomic64_lock(a);

	// enter
	tb_spinlock_enter(lock);

	// set value
	tb_hong_t o = (tb_hong_t)*a; *a ^= (tb_atomic64_t)v;

	// leave
	tb_spinlock_leave(lock);

	// ok?
	return o;
}
tb_hong_t tb_atomic64_fetch_and_and_generic(tb_atomic64_t* a, tb_hong_t v)
{
	// check
	tb_assert(a);

	// the lock
	tb_spinlock_t* lock = tb_atomic64_lock(a);

	// enter
	tb_spinlock_enter(lock);

	// set value
	tb_hong_t o = (tb_hong_t)*a; *a &= (tb_atomic64_t)v;

	// leave
	tb_spinlock_leave(lock);

	// ok?
	return o;
}
tb_hong_t tb_atomic64_fetch_and_or_generic(tb_atomic64_t* a, tb_hong_t v)
{
	// check
	tb_assert(a);

	// the lock
	tb_spinlock_t* lock = tb_atomic64_lock(a);

	// enter
	tb_spinlock_enter(lock);

	// set value
	tb_hong_t o = (tb_hong_t)*a; *a |= (tb_atomic64_t)v;

	// leave
	tb_spinlock_leave(lock);

	// ok?
	return o;
}
tb_hong_t tb_atomic64_inc_and_fetch_generic(tb_atomic64_t* a)
{
	// check
	tb_assert(a);

	// the lock
	tb_spinlock_t* lock = tb_atomic64_lock(a);

	// enter
	tb_spinlock_enter(lock);

	// set value
	tb_hong_t o = (tb_hong_t)++(*a);

	// leave
	tb_spinlock_leave(lock);

	// ok?
	return o;
}
tb_hong_t tb_atomic64_dec_and_fetch_generic(tb_atomic64_t* a)
{
	// check
	tb_assert(a);

	// the lock
	tb_spinlock_t* lock = tb_atomic64_lock(a);

	// enter
	tb_spinlock_enter(lock);

	// set value
	tb_hong_t o = (tb_hong_t)--(*a);

	// leave
	tb_spinlock_leave(lock);

	// ok?
	return o;
}
tb_hong_t tb_atomic64_add_and_fetch_generic(tb_atomic64_t* a, tb_hong_t v)
{
	// check
	tb_assert(a);

	// the lock
	tb_spinlock_t* lock = tb_atomic64_lock(a);

	// enter
	tb_spinlock_enter(lock);

	// set value
	*a += (tb_atomic64_t)v; tb_hong_t o = (tb_hong_t)(*a);

	// leave
	tb_spinlock_leave(lock);

	// ok?
	return o;
}
tb_hong_t tb_atomic64_sub_and_fetch_generic(tb_atomic64_t* a, tb_hong_t v)
{
	return tb_atomic64_add_and_fetch_generic(a, -v);
}
tb_hong_t tb_atomic64_xor_and_fetch_generic(tb_atomic64_t* a, tb_hong_t v)
{
	// check
	tb_assert(a);

	// the lock
	tb_spinlock_t* lock = tb_atomic64_lock(a);

	// enter
	tb_spinlock_enter(lock);

	// set value
	*a ^= (tb_atomic64_t)v; tb_hong_t o = (tb_hong_t)(*a);

	// leave
	tb_spinlock_leave(lock);

	// ok?
	return o;
}
tb_hong_t tb_atomic64_and_and_fetch_generic(tb_atomic64_t* a, tb_hong_t v)
{
	// check
	tb_assert(a);

	// the lock
	tb_spinlock_t* lock = tb_atomic64_lock(a);

	// enter
	tb_spinlock_enter(lock);

	// set value
	*a &= (tb_atomic64_t)v; tb_hong_t o = (tb_hong_t)(*a);

	// leave
	tb_spinlock_leave(lock);

	// ok?
	return o;
}
tb_hong_t tb_atomic64_or_and_fetch_generic(tb_atomic64_t* a, tb_hong_t v)
{
	// check
	tb_assert(a);

	// the lock
	tb_spinlock_t* lock = tb_atomic64_lock(a);

	// enter
	tb_spinlock_enter(lock);

	// set value
	*a |= (tb_atomic64_t)v; tb_hong_t o = (tb_hong_t)(*a);

	// leave
	tb_spinlock_leave(lock);

	// ok?
	return o;
}

