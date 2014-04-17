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
#ifndef TB_PLATFORM_ARCH_x86_ATOMIC_H
#define TB_PLATFORM_ARCH_x86_ATOMIC_H


/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#ifdef TB_CONFIG_ASSEMBLER_GAS

#ifndef tb_atomic_get
# 	define tb_atomic_get(a) 					tb_atomic_fetch_and_pset_x86(a, 0, 0)
#endif

#ifndef tb_atomic_set
# 	define tb_atomic_set(a, v) 					tb_atomic_set_x86(a, v)
#endif

#ifndef tb_atomic_set0
# 	define tb_atomic_set0(a) 					tb_atomic_set_x86(a, 0)
#endif

#ifndef tb_atomic_pset
# 	define tb_atomic_pset(a, p, v) 				tb_atomic_pset_x86(a, p, v)
#endif

#ifndef tb_atomic_fetch_and_set0
# 	define tb_atomic_fetch_and_set0(a) 			tb_atomic_fetch_and_set_x86(a, 0)
#endif

#ifndef tb_atomic_fetch_and_set
# 	define tb_atomic_fetch_and_set(a, v) 		tb_atomic_fetch_and_set_x86(a, v)
#endif

#ifndef tb_atomic_fetch_and_pset
# 	define tb_atomic_fetch_and_pset(a, p, v) 	tb_atomic_fetch_and_pset_x86(a, p, v)
#endif

#ifndef tb_atomic_fetch_and_inc
# 	define tb_atomic_fetch_and_inc(a) 			tb_atomic_fetch_and_add_x86(a, 1)
#endif

#ifndef tb_atomic_fetch_and_dec
# 	define tb_atomic_fetch_and_dec(a) 			tb_atomic_fetch_and_add_x86(a, -1)
#endif

#ifndef tb_atomic_fetch_and_add
# 	define tb_atomic_fetch_and_add(a, v) 		tb_atomic_fetch_and_add_x86(a, v)
#endif

#ifndef tb_atomic_fetch_and_sub
# 	define tb_atomic_fetch_and_sub(a, v) 		tb_atomic_fetch_and_add_x86(a, -(v))
#endif

#ifndef tb_atomic_fetch_and_or
# 	define tb_atomic_fetch_and_or(a, v) 		tb_atomic_fetch_and_or_x86(a, v)
#endif

#ifndef tb_atomic_fetch_and_xor
# 	define tb_atomic_fetch_and_xor(a, v) 		tb_atomic_fetch_and_xor_x86(a, v)
#endif

#ifndef tb_atomic_fetch_and_and
# 	define tb_atomic_fetch_and_and(a, v) 		tb_atomic_fetch_and_and_x86(a, v)
#endif

#ifndef tb_atomic_add_and_fetch
# 	define tb_atomic_add_and_fetch(a, v) 		(tb_atomic_fetch_and_add_x86(a, v) + (v))
#endif

#ifndef tb_atomic_sub_and_fetch
# 	define tb_atomic_sub_and_fetch(a, v) 		tb_atomic_add_and_fetch(a, -(v))
#endif

#ifndef tb_atomic_inc_and_fetch
# 	define tb_atomic_inc_and_fetch(a) 			tb_atomic_add_and_fetch(a, 1)
#endif

#ifndef tb_atomic_dec_and_fetch
# 	define tb_atomic_dec_and_fetch(a) 			tb_atomic_sub_and_fetch(a, 1)
#endif

#ifndef tb_atomic_or_and_fetch
# 	define tb_atomic_or_and_fetch(a, v) 		tb_atomic_or_and_fetch_x86(a, v)
#endif

#ifndef tb_atomic_xor_and_fetch
# 	define tb_atomic_xor_and_fetch(a, v) 		tb_atomic_xor_and_fetch_x86(a, v)
#endif

#ifndef tb_atomic_and_and_fetch
# 	define tb_atomic_and_and_fetch(a, v) 		tb_atomic_and_and_fetch_x86(a, v)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * get & set
 */

static __tb_inline__ tb_void_t tb_atomic_set_x86(tb_atomic_t* a, tb_size_t v)
{
	tb_assert(a);

	__tb_asm__ __tb_volatile__ 
	(
#if TB_CPU_BITSIZE == 64
		"lock xchgq %1, %0\n" 	//!< xchgq v, [a]
#else
		"lock xchgl %1, %0\n" 	//!< xchgl v, [a]
#endif
		:
		: "m" (*a), "r"(v) 
		: "memory"
	);
}
static __tb_inline__ tb_size_t tb_atomic_fetch_and_set_x86(tb_atomic_t* a, tb_size_t v)
{
	tb_assert(a);

	__tb_asm__ __tb_volatile__ 
	(
#if TB_CPU_BITSIZE == 64
		"lock xchgq %0, %1\n" 	//!< xchgq v, [a]
#else
		"lock xchgl %0, %1\n" 	//!< xchgl v, [a]
#endif

		: "+r" (v) 
		: "m" (*a)
		: "memory"
	);

    return v;
}
static __tb_inline__ tb_void_t tb_atomic_pset_x86(tb_atomic_t* a, tb_size_t p, tb_size_t v)
{
	tb_assert(a);

	/*
	 * cmpxchgl v, [a]:
	 *
	 * if (eax == [a]) 
	 * {
	 * 		zf = 1;
	 * 		[a] = v;
	 * } 
	 * else 
	 * {
	 * 		zf = 0;
	 * 		eax = [a];
	 * }
	 *
	 */
	__tb_asm__ __tb_volatile__ 
	(
#if TB_CPU_BITSIZE == 64
		"lock cmpxchgq 	%2, %0 	\n" 	//!< cmpxchgq v, [a]
#else
		"lock cmpxchgl 	%2, %0 	\n" 	//!< cmpxchgl v, [a]
#endif
		:
		: "m" (*a), "a" (p), "r" (v) 
		: "cc", "memory" 				//!< "cc" means that flags were changed.
	);
}
static __tb_inline__ tb_size_t tb_atomic_fetch_and_pset_x86(tb_atomic_t* a, tb_size_t p, tb_size_t v)
{
	tb_assert(a);

	/*
	 * cmpxchgl v, [a]:
	 *
	 * if (eax == [a]) 
	 * {
	 * 		zf = 1;
	 * 		[a] = v;
	 * } 
	 * else 
	 * {
	 * 		zf = 0;
	 * 		eax = [a];
	 * }
	 *
	 */
	tb_size_t o;
	__tb_asm__ __tb_volatile__ 
	(
#if TB_CPU_BITSIZE == 64
		"lock cmpxchgq 	%3, %1 	\n" 	//!< cmpxchgl v, [a]
#else
		"lock cmpxchgl 	%3, %1 	\n" 	//!< cmpxchgq v, [a]
#endif

		: "=a" (o) 
		: "m" (*a), "a" (p), "r" (v) 
		: "cc", "memory" 				//!< "cc" means that flags were changed.
	);

    return o;
}
/* //////////////////////////////////////////////////////////////////////////////////////
 * fetch and ...
 */

static __tb_inline__ tb_long_t tb_atomic_fetch_and_add_x86(tb_atomic_t* a, tb_long_t v)
{
	tb_assert(a);
	/*
	 * xaddl v, [a]:
	 *
	 * o = [a]
	 * [a] += v;
	 * v = o;
	 *
	 * cf, ef, of, sf, zf, pf... maybe changed
	 */
	__tb_asm__ __tb_volatile__ 
	(
#if TB_CPU_BITSIZE == 64
		"lock xaddq %0, %1 \n" 			//!< xaddq v, [a]
#else
		"lock xaddl %0, %1 \n" 			//!< xaddl v, [a]
#endif

		: "+r" (v) 
		: "m" (*a) 
		: "cc", "memory"
	);

    return v;
}
static __tb_inline__ tb_long_t tb_atomic_fetch_and_and_x86(tb_atomic_t* a, tb_long_t v)
{
	tb_assert(a);
	tb_long_t o = tb_atomic_get(a);

	__tb_asm__ __tb_volatile__ 
	(
#if TB_CPU_BITSIZE == 64
		"lock andq %0, %1 \n"
#else
		"lock andl %0, %1 \n"
#endif
		:
		: "r" (v), "m" (*a) 
		: "cc", "memory"
	);

    return o;
}
static __tb_inline__ tb_long_t tb_atomic_fetch_and_xor_x86(tb_atomic_t* a, tb_long_t v)
{
	tb_assert(a);
	tb_long_t o = tb_atomic_get(a);

	__tb_asm__ __tb_volatile__ 
	(
#if TB_CPU_BITSIZE == 64
		"lock xorq %0, %1 \n"
#else
		"lock xorl %0, %1 \n"
#endif
		:
		: "r" (v), "m" (*a) 
		: "cc", "memory"
	);

    return o;
}
static __tb_inline__ tb_long_t tb_atomic_fetch_and_or_x86(tb_atomic_t* a, tb_long_t v)
{
	tb_assert(a);
	tb_long_t o = tb_atomic_get(a);

	__tb_asm__ __tb_volatile__ 
	(
#if TB_CPU_BITSIZE == 64
		"lock orq %0, %1 \n"
#else
		"lock orl %0, %1 \n"
#endif
		:
		: "r" (v), "m" (*a) 
		: "cc", "memory"
	);

    return o;
}
/* //////////////////////////////////////////////////////////////////////////////////////
 * ... and fetch
 */

static __tb_inline__ tb_long_t tb_atomic_and_and_fetch_x86(tb_atomic_t* a, tb_long_t v)
{
	tb_assert(a);
	__tb_asm__ __tb_volatile__ 
	(
#if TB_CPU_BITSIZE == 64
		"lock andq %0, %1 \n"
#else
		"lock andl %0, %1 \n"
#endif

		:
		: "r" (v), "m" (*a) 
		: "cc", "memory"
	);

    return tb_atomic_get(a);
}

static __tb_inline__ tb_long_t tb_atomic_xor_and_fetch_x86(tb_atomic_t* a, tb_long_t v)
{
	tb_assert(a);
	__tb_asm__ __tb_volatile__ 
	(
#if TB_CPU_BITSIZE == 64
		"lock xorq %0, %1 \n"
#else
		"lock xorl %0, %1 \n"
#endif

		:
		: "r" (v), "m" (*a) 
		: "cc", "memory"
	);

    return tb_atomic_get(a);
}

static __tb_inline__ tb_long_t tb_atomic_or_and_fetch_x86(tb_atomic_t* a, tb_long_t v)
{
	tb_assert(a);
	__tb_asm__ __tb_volatile__ 
	(
#if TB_CPU_BITSIZE == 64
		"lock orq %0, %1 \n"
#else
		"lock orl %0, %1 \n"
#endif

		:
		: "r" (v), "m" (*a) 
		: "cc", "memory"
	);

    return tb_atomic_get(a);
}


#endif // TB_CONFIG_ASSEMBLER_GAS


#endif
