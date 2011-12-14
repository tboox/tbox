/*!The Tiny Platform Library
 * 
 * TPlat is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TPlat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TPlat; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		atomic.h
 *
 */
#ifndef TB_PLATFORM_STUB_ATOMIC_H
#define TB_PLATFORM_STUB_ATOMIC_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif


/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* /////////////////////////////////////////////////////////
 * get & set
 */
static __tb_inline__ tb_size_t tb_atomic_get(tb_atomic_t* a)
{
	tb_trace_noimpl();
	tb_assert(a);
	return *a;
}
static __tb_inline__ tb_void_t tb_atomic_set(tb_atomic_t* a, tb_size_t v)
{
	tb_trace_noimpl();
	tb_assert(a);
	*a = v;
}
static __tb_inline__ tb_void_t tb_atomic_set0(tb_atomic_t* a)
{
	tb_trace_noimpl();
	tb_assert(a);
	*a = 0;
}
static __tb_inline__ tb_void_t tb_atomic_pset(tb_atomic_t* a, tb_size_t p, tb_size_t v)
{
	tb_trace_noimpl();
	tb_assert(a);
	if (*a == p) *a = v;
}
static __tb_inline__ tb_size_t tb_atomic_fetch_and_set0(tb_atomic_t* a)
{
	tb_trace_noimpl();
	tb_assert(a);

	tb_size_t o = *a;
	*a = 0;
	return o;
}
static __tb_inline__ tb_size_t tb_atomic_fetch_and_set(tb_atomic_t* a, tb_size_t v)
{
	tb_trace_noimpl();
	tb_assert(a);

	tb_size_t o = *a;
	*a = v;
	return o;
}
static __tb_inline__ tb_size_t tb_atomic_fetch_and_pset(tb_atomic_t* a, tb_size_t p, tb_size_t v)
{
	tb_trace_noimpl();
	tb_assert(a);

	tb_size_t o = *a;
	if (o == p) *a = v;
	return o;
}

/* /////////////////////////////////////////////////////////
 * fetch and ...
 */
static __tb_inline__ tb_long_t tb_atomic_fetch_and_inc(tb_atomic_t* a)
{
	tb_trace_noimpl();
	tb_assert(a);

	tb_long_t __tb_volatile__* pa = (tb_long_t __tb_volatile__*)a;
	return *pa++;
}
static __tb_inline__ tb_long_t tb_atomic_fetch_and_dec(tb_atomic_t* a)
{
	tb_trace_noimpl();
	tb_assert(a);

	tb_long_t __tb_volatile__* pa = (tb_long_t __tb_volatile__*)a;
	return *pa--;
}
static __tb_inline__ tb_long_t tb_atomic_fetch_and_add(tb_atomic_t* a, tb_long_t v)
{
	tb_trace_noimpl();
	tb_assert(a);

	tb_long_t o = *((tb_long_t*)a);
	*((tb_long_t*)a) += v;
	return o;
}
static __tb_inline__ tb_long_t tb_atomic_fetch_and_sub(tb_atomic_t* a, tb_long_t v)
{
	tb_trace_noimpl();
	tb_assert(a);

	tb_long_t o = *((tb_long_t*)a);
	*((tb_long_t*)a) -= v;
	return o;
}
static __tb_inline__ tb_size_t tb_atomic_fetch_and_xor(tb_atomic_t* a, tb_size_t v)
{
	tb_trace_noimpl();
	tb_assert(a);

	tb_size_t o = *a;
	*a ^= v;
	return o;
}
static __tb_inline__ tb_size_t tb_atomic_fetch_and_and(tb_atomic_t* a, tb_size_t v)
{
	tb_trace_noimpl();
	tb_assert(a);

	tb_size_t o = *a;
	*a &= v;
	return o;
}
static __tb_inline__ tb_size_t tb_atomic_fetch_and_or(tb_atomic_t* a, tb_size_t v)
{
	tb_trace_noimpl();
	tb_assert(a);

	tb_size_t o = *a;
	*a |= v;
	return o;
}
static __tb_inline__ tb_size_t tb_atomic_fetch_and_nand(tb_atomic_t* a, tb_size_t v)
{
	tb_trace_noimpl();
	tb_assert(a);

	tb_size_t o = *a;
	*a = ~o & v;
	return o;
}

/* /////////////////////////////////////////////////////////
 * ... and fetch
 */
static __tb_inline__ tb_long_t tb_atomic_inc_and_fetch(tb_atomic_t* a)
{
	tb_trace_noimpl();
	tb_assert(a);

	tb_long_t __tb_volatile__* pa = (tb_long_t __tb_volatile__*)a;
	return ++*pa;
}
static __tb_inline__ tb_long_t tb_atomic_dec_and_fetch(tb_atomic_t* a)
{
	tb_trace_noimpl();
	tb_assert(a);

	tb_long_t __tb_volatile__* pa = (tb_long_t __tb_volatile__*)a;
	return --*pa;
}
static __tb_inline__ tb_long_t tb_atomic_add_and_fetch(tb_atomic_t* a, tb_long_t v)
{
	tb_trace_noimpl();
	tb_assert(a);

	*((tb_long_t*)a) += v;
	return *((tb_long_t*)a);
}
static __tb_inline__ tb_long_t tb_atomic_sub_and_fetch(tb_atomic_t* a, tb_long_t v)
{
	tb_trace_noimpl();
	tb_assert(a);

	*((tb_long_t*)a) -= v;
	return *((tb_long_t*)a);
}
static __tb_inline__ tb_size_t tb_atomic_xor_and_fetch(tb_atomic_t* a, tb_size_t v)
{
	tb_trace_noimpl();
	tb_assert(a);

	*((tb_long_t*)a) ^= v;
	return *((tb_long_t*)a);
}
static __tb_inline__ tb_size_t tb_atomic_and_and_fetch(tb_atomic_t* a, tb_size_t v)
{
	tb_trace_noimpl();
	tb_assert(a);

	*((tb_long_t*)a) &= v;
	return *((tb_long_t*)a);
}
static __tb_inline__ tb_size_t tb_atomic_or_and_fetch(tb_atomic_t* a, tb_size_t v)
{
	tb_trace_noimpl();
	tb_assert(a);

	*((tb_long_t*)a) |= v;
	return *((tb_long_t*)a);
}
static __tb_inline__ tb_size_t tb_atomic_nand_and_fetch(tb_atomic_t* a, tb_size_t v)
{
	tb_trace_noimpl();
	tb_assert(a);

	*((tb_long_t*)a) = ~*((tb_long_t*)a) & v;
	return *((tb_long_t*)a);
}

// c plus plus
#ifdef __cplusplus
}
#endif

#endif
