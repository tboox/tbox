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
 * \file		atomic_sync.h
 *
 */
#ifndef TB_PLATFORM_COMPILER_GCC_ATOMIC_SYNC_H
#define TB_PLATFORM_COMPILER_GCC_ATOMIC_SYNC_H

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
	tb_assert(a);
	return __sync_val_compare_and_swap(a, 0, 0);
}
static __tb_inline__ tb_void_t tb_atomic_set(tb_atomic_t* a, tb_size_t v)
{
	tb_assert(a);
	__sync_lock_test_and_set(a, v);
}
static __tb_inline__ tb_void_t tb_atomic_set0(tb_atomic_t* a)
{
	tb_assert(a);
	 __sync_lock_test_and_set(a, 0);
}
static __tb_inline__ tb_void_t tb_atomic_pset(tb_atomic_t* a, tb_size_t p, tb_size_t v)
{
	tb_assert(a);
	__sync_val_compare_and_swap(a, p, v);
}
static __tb_inline__ tb_size_t tb_atomic_fetch_and_set0(tb_atomic_t* a)
{
	tb_assert(a);
	return __sync_lock_test_and_set(a, 0);
}
static __tb_inline__ tb_size_t tb_atomic_fetch_and_set(tb_atomic_t* a, tb_size_t v)
{
	tb_assert(a);
	return __sync_lock_test_and_set(a, v);
}
static __tb_inline__ tb_size_t tb_atomic_fetch_and_pset(tb_atomic_t* a, tb_size_t p, tb_size_t v)
{
	tb_assert(a);
	return __sync_val_compare_and_swap(a, p, v);
}

/* /////////////////////////////////////////////////////////
 * fetch and ...
 */
static __tb_inline__ tb_long_t tb_atomic_fetch_and_inc(tb_atomic_t* a)
{
	tb_assert(a);
	return __sync_fetch_and_add(a, 1);
}
static __tb_inline__ tb_long_t tb_atomic_fetch_and_dec(tb_atomic_t* a)
{
	tb_assert(a);
	return __sync_fetch_and_sub(a, 1);
}
static __tb_inline__ tb_long_t tb_atomic_fetch_and_add(tb_atomic_t* a, tb_long_t v)
{
	tb_assert(a);
	return __sync_fetch_and_add(a, v);
}
static __tb_inline__ tb_long_t tb_atomic_fetch_and_sub(tb_atomic_t* a, tb_long_t v)
{
	tb_assert(a);
	return __sync_fetch_and_sub(a, v);
}
static __tb_inline__ tb_size_t tb_atomic_fetch_and_xor(tb_atomic_t* a, tb_size_t v)
{
	tb_assert(a);
	return __sync_fetch_and_xor(a, v);
}
static __tb_inline__ tb_size_t tb_atomic_fetch_and_and(tb_atomic_t* a, tb_size_t v)
{
	tb_assert(a);
	return __sync_fetch_and_and(a, v);
}
static __tb_inline__ tb_size_t tb_atomic_fetch_and_or(tb_atomic_t* a, tb_size_t v)
{
	tb_assert(a);
	return __sync_fetch_and_or(a, v);
}
static __tb_inline__ tb_size_t tb_atomic_fetch_and_nand(tb_atomic_t* a, tb_size_t v)
{
	tb_assert(a);
	return __sync_fetch_and_nand(a, v);
}

/* /////////////////////////////////////////////////////////
 * ... and fetch
 */
static __tb_inline__ tb_long_t tb_atomic_inc_and_fetch(tb_atomic_t* a)
{
	tb_assert(a);
	return __sync_add_and_fetch(a, 1);
}
static __tb_inline__ tb_long_t tb_atomic_dec_and_fetch(tb_atomic_t* a)
{
	tb_assert(a);
	return __sync_sub_and_fetch(a, 1);
}
static __tb_inline__ tb_long_t tb_atomic_add_and_fetch(tb_atomic_t* a, tb_long_t v)
{
	tb_assert(a);
	return __sync_add_and_fetch(a, v);
}
static __tb_inline__ tb_long_t tb_atomic_sub_and_fetch(tb_atomic_t* a, tb_long_t v)
{
	tb_assert(a);
	return __sync_sub_and_fetch(a, v);
}
static __tb_inline__ tb_size_t tb_atomic_xor_and_fetch(tb_atomic_t* a, tb_size_t v)
{
	tb_assert(a);
	return __sync_xor_and_fetch(a, v);
}
static __tb_inline__ tb_size_t tb_atomic_and_and_fetch(tb_atomic_t* a, tb_size_t v)
{
	tb_assert(a);
	return __sync_xor_and_fetch(a, v);
}
static __tb_inline__ tb_size_t tb_atomic_or_and_fetch(tb_atomic_t* a, tb_size_t v)
{
	tb_assert(a);
	return __sync_or_and_fetch(a, v);
}
static __tb_inline__ tb_size_t tb_atomic_nand_and_fetch(tb_atomic_t* a, tb_size_t v)
{
	tb_assert(a);
	return __sync_nand_and_fetch(a, v);
}

// c plus plus
#ifdef __cplusplus
}
#endif

#endif
