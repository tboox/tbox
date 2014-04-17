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
 * @file		atomic64.h
 *
 */
#ifndef TB_PLATFORM_WINDOWS_ATOMIC64_H
#define TB_PLATFORM_WINDOWS_ATOMIC64_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <windows.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

#if !defined(tb_atomic64_get) && defined(InterlockedCompareExchange64)
# 	define tb_atomic64_get(a) 					tb_atomic64_fetch_and_pset_windows(a, 0, 0)
#endif

#if !defined(tb_atomic64_set) && defined(InterlockedExchange64)
# 	define tb_atomic64_set(a, v) 				tb_atomic64_fetch_and_set_windows(a, v)
#endif

#if !defined(tb_atomic64_set0) && defined(InterlockedExchange64)
# 	define tb_atomic64_set0(a) 					tb_atomic64_fetch_and_set_windows(a, 0)
#endif

#if !defined(tb_atomic64_pset) && defined(InterlockedCompareExchange64)
# 	define tb_atomic64_pset(a, p, v) 			tb_atomic64_fetch_and_pset_windows(a, p, v)
#endif

#if !defined(tb_atomic64_fetch_and_set0) && defined(InterlockedExchange64)
# 	define tb_atomic64_fetch_and_set0(a) 		tb_atomic64_fetch_and_set_windows(a, 0)
#endif

#if !defined(tb_atomic64_fetch_and_set) && defined(InterlockedExchange64)
# 	define tb_atomic64_fetch_and_set(a, v) 		tb_atomic64_fetch_and_set_windows(a, v)
#endif

#if !defined(tb_atomic64_fetch_and_pset) && defined(InterlockedCompareExchange64)
# 	define tb_atomic64_fetch_and_pset(a, p, v) 	tb_atomic64_fetch_and_pset_windows(a, p, v)
#endif

#if !defined(tb_atomic64_fetch_and_inc) && defined(InterlockedIncrement64)
# 	define tb_atomic64_fetch_and_inc(a) 		tb_atomic64_fetch_and_inc_windows(a)
#endif

#if !defined(tb_atomic64_fetch_and_dec) && defined(InterlockedDecrement64)
# 	define tb_atomic64_fetch_and_dec(a) 		tb_atomic64_fetch_and_dec_windows(a)
#endif

#if !defined(tb_atomic64_fetch_and_add) && defined(InterlockedExchangeAdd64)
# 	define tb_atomic64_fetch_and_add(a, v) 		tb_atomic64_fetch_and_add_windows(a, v)
#endif

#if !defined(tb_atomic64_fetch_and_sub) && defined(InterlockedExchangeAdd64)
# 	define tb_atomic64_fetch_and_sub(a, v) 		tb_atomic64_fetch_and_add_windows(a, -(v))
#endif

#if !defined(tb_atomic64_inc_and_fetch) && defined(InterlockedIncrement64)
# 	define tb_atomic64_inc_and_fetch(a) 		tb_atomic64_inc_and_fetch_windows(a)
#endif

#if !defined(tb_atomic64_dec_and_fetch) && defined(InterlockedDecrement64)
# 	define tb_atomic64_dec_and_fetch(a) 		tb_atomic64_dec_and_fetch_windows(a)
#endif

#if !defined(tb_atomic64_add_and_fetch) && defined(InterlockedExchangeAdd64)
# 	define tb_atomic64_add_and_fetch(a, v) 		tb_atomic64_add_and_fetch_windows(a, v)
#endif

#if !defined(tb_atomic64_sub_and_fetch) && defined(InterlockedExchangeAdd64)
# 	define tb_atomic64_sub_and_fetch(a, v) 		tb_atomic64_add_and_fetch_windows(a, -(v))
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * get & set
 */
#ifdef InterlockedExchange64
static __tb_inline__ tb_hize_t tb_atomic64_fetch_and_set_windows(tb_atomic64_t* a, tb_hize_t v)
{
	tb_assert(a);
	return (tb_hize_t)InterlockedExchange64((LONGLONG __tb_volatile__*)a, v);
}
#endif

#ifdef InterlockedCompareExchange64
static __tb_inline__ tb_hize_t tb_atomic64_fetch_and_pset_windows(tb_atomic64_t* a, tb_hize_t p, tb_hize_t v)
{
	tb_assert(a);
	return (tb_hize_t)InterlockedCompareExchange64((LONGLONG __tb_volatile__*)a, v, p);
}
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * fetch and ...
 */
#ifdef InterlockedIncrement64
static __tb_inline__ tb_hong_t tb_atomic64_fetch_and_inc_windows(tb_atomic64_t* a)
{
	tb_assert(a);

	tb_hong_t o = tb_atomic64_get(a);
	InterlockedIncrement64((LONGLONG __tb_volatile__*)a);
	return o;
}
#endif

#ifdef InterlockedDecrement64
static __tb_inline__ tb_hong_t tb_atomic64_fetch_and_dec_windows(tb_atomic64_t* a)
{
	tb_assert(a);

	tb_hong_t o = tb_atomic64_get(a);
	InterlockedDecrement64((LONGLONG __tb_volatile__*)a);
	return o;
}
#endif

#ifdef InterlockedExchangeAdd64
static __tb_inline__ tb_hong_t tb_atomic64_fetch_and_add_windows(tb_atomic64_t* a, tb_hong_t v)
{
	tb_assert(a);

	return (tb_hong_t)InterlockedExchangeAdd64((LONGLONG __tb_volatile__*)a, v);
}
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * ... and fetch
 */
#ifdef InterlockedIncrement64
static __tb_inline__ tb_hong_t tb_atomic64_inc_and_fetch_windows(tb_atomic64_t* a)
{
	tb_assert(a);
	return (tb_hong_t)InterlockedIncrement64((LONGLONG __tb_volatile__*)a);
}
#endif

#ifdef InterlockedDecrement64
static __tb_inline__ tb_hong_t tb_atomic64_dec_and_fetch_windows(tb_atomic64_t* a)
{
	tb_assert(a);
	return (tb_hong_t)InterlockedDecrement64((LONGLONG __tb_volatile__*)a);
}
#endif

#ifdef InterlockedExchangeAdd64
static __tb_inline__ tb_hong_t tb_atomic64_add_and_fetch_windows(tb_atomic64_t* a, tb_hong_t v)
{
	tb_assert(a);
	return InterlockedExchangeAdd64((LONGLONG __tb_volatile__*)a, v) + v;
}
#endif

#endif
