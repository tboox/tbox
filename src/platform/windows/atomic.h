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
 * @file		atomic.h
 *
 */
#ifndef TB_PLATFORM_WINDOWS_ATOMIC_H
#define TB_PLATFORM_WINDOWS_ATOMIC_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <windows.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

#ifndef tb_atomic_get
# 	define tb_atomic_get(a) 					tb_atomic_fetch_and_pset_windows(a, 0, 0)
#endif

#ifndef tb_atomic_set
# 	define tb_atomic_set(a, v) 					tb_atomic_fetch_and_set_windows(a, v)
#endif

#ifndef tb_atomic_set0
# 	define tb_atomic_set0(a) 					tb_atomic_fetch_and_set_windows(a, 0)
#endif

#ifndef tb_atomic_pset
# 	define tb_atomic_pset(a, p, v) 				tb_atomic_fetch_and_pset_windows(a, p, v)
#endif

#ifndef tb_atomic_bool_and_pset
# 	define tb_atomic_bool_and_pset(a, p, v) 	tb_atomic_bool_and_pset_windows(a, p, v)
#endif

#ifndef tb_atomic_fetch_and_set0
# 	define tb_atomic_fetch_and_set0(a) 			tb_atomic_fetch_and_set_windows(a, 0)
#endif

#ifndef tb_atomic_fetch_and_set
# 	define tb_atomic_fetch_and_set(a, v) 		tb_atomic_fetch_and_set_windows(a, v)
#endif

#ifndef tb_atomic_fetch_and_pset
# 	define tb_atomic_fetch_and_pset(a, p, v) 	tb_atomic_fetch_and_pset_windows(a, p, v)
#endif

#ifndef tb_atomic_fetch_and_inc
# 	define tb_atomic_fetch_and_inc(a) 			tb_atomic_fetch_and_inc_windows(a)
#endif

#ifndef tb_atomic_fetch_and_dec
# 	define tb_atomic_fetch_and_dec(a) 			tb_atomic_fetch_and_dec_windows(a)
#endif

#ifndef tb_atomic_fetch_and_add
# 	define tb_atomic_fetch_and_add(a, v) 		tb_atomic_fetch_and_add_windows(a, v)
#endif

#ifndef tb_atomic_fetch_and_sub
# 	define tb_atomic_fetch_and_sub(a, v) 		tb_atomic_fetch_and_add_windows(a, -(v))
#endif

#ifndef tb_atomic_inc_and_fetch
# 	define tb_atomic_inc_and_fetch(a) 			tb_atomic_inc_and_fetch_windows(a)
#endif

#ifndef tb_atomic_dec_and_fetch
# 	define tb_atomic_dec_and_fetch(a) 			tb_atomic_dec_and_fetch_windows(a)
#endif

#ifndef tb_atomic_add_and_fetch
# 	define tb_atomic_add_and_fetch(a, v) 		tb_atomic_add_and_fetch_windows(a, v)
#endif

#ifndef tb_atomic_sub_and_fetch
# 	define tb_atomic_sub_and_fetch(a, v) 		tb_atomic_add_and_fetch_windows(a, -(v))
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * get & set
 */

static __tb_inline__ tb_size_t tb_atomic_fetch_and_set_windows(tb_atomic_t* a, tb_size_t v)
{
	tb_assert(a);

#if TB_CPU_BIT64
	return (tb_size_t)InterlockedExchange64((LONGLONG __tb_volatile__*)a, v);
#else
	return (tb_size_t)InterlockedExchange((LONG __tb_volatile__*)a, v);
#endif

}
static __tb_inline__ tb_size_t tb_atomic_fetch_and_pset_windows(tb_atomic_t* a, tb_size_t p, tb_size_t v)
{
	tb_assert(a);

#if TB_CPU_BIT64
	return (tb_size_t)InterlockedCompareExchange64((LONGLONG __tb_volatile__*)a, v, p);
#else
	return (tb_size_t)InterlockedCompareExchange((LONG __tb_volatile__*)a, v, p);
#endif
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * fetch and ...
 */
static __tb_inline__ tb_long_t tb_atomic_fetch_and_inc_windows(tb_atomic_t* a)
{
	tb_assert(a);

	tb_long_t o = tb_atomic_get(a);

#if TB_CPU_BIT64
	InterlockedIncrement64((LONGLONG __tb_volatile__*)a);
#else
	InterlockedIncrement((LONG __tb_volatile__*)a);
#endif

	return o;
}
static __tb_inline__ tb_long_t tb_atomic_fetch_and_dec_windows(tb_atomic_t* a)
{
	tb_assert(a);

	tb_long_t o = tb_atomic_get(a);

#if TB_CPU_BIT64
	InterlockedDecrement64((LONGLONG __tb_volatile__*)a);
#else
	InterlockedDecrement((LONG __tb_volatile__*)a);
#endif
	return o;
}
static __tb_inline__ tb_long_t tb_atomic_fetch_and_add_windows(tb_atomic_t* a, tb_long_t v)
{
	tb_assert(a);

#if TB_CPU_BIT64
	return (tb_long_t)InterlockedExchangeAdd64((LONGLONG __tb_volatile__*)a, v);
#else
	return (tb_long_t)InterlockedExchangeAdd((LONG __tb_volatile__*)a, v);
#endif
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * ... and fetch
 */
static __tb_inline__ tb_long_t tb_atomic_inc_and_fetch_windows(tb_atomic_t* a)
{
	tb_assert(a);

#if TB_CPU_BIT64
	return (tb_long_t)InterlockedIncrement64((LONGLONG __tb_volatile__*)a);
#else
	return (tb_long_t)InterlockedIncrement((LONG __tb_volatile__*)a);
#endif

}
static __tb_inline__ tb_long_t tb_atomic_dec_and_fetch_windows(tb_atomic_t* a)
{
	tb_assert(a);

#if TB_CPU_BIT64
	return (tb_long_t)InterlockedDecrement64((LONGLONG __tb_volatile__*)a);
#else
	return (tb_long_t)InterlockedDecrement((LONG __tb_volatile__*)a);
#endif

}
static __tb_inline__ tb_long_t tb_atomic_add_and_fetch_windows(tb_atomic_t* a, tb_long_t v)
{
	tb_assert(a);

#if TB_CPU_BIT64
	return InterlockedExchangeAdd64((LONGLONG __tb_volatile__*)a, v) + v;
#else
	return InterlockedExchangeAdd((LONG __tb_volatile__*)a, v) + v;
#endif
}

#endif
