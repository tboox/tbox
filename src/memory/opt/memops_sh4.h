/*!The Tiny Box Library
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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		memops_sh4.h
 *
 */
#ifndef TB_MEMORY_OPT_MEMOPS_SH4_H
#define TB_MEMORY_OPT_MEMOPS_SH4_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* /////////////////////////////////////////////////////////
 * macros
 */

#undef TB_MEMOPS_OPT_MEMSET_U8
#undef TB_MEMOPS_OPT_MEMSET_U16
#undef TB_MEMOPS_OPT_MEMSET_U32

#ifdef TB_CONFIG_ASSEMBLER_GAS
# 	define TB_MEMOPS_OPT_MEMSET_U16
# 	define TB_MEMOPS_OPT_MEMSET_U32
#endif

/* /////////////////////////////////////////////////////////
 * implemention
 */

#ifdef TB_CONFIG_ASSEMBLER_GAS
static __tb_inline__ tb_void_t tb_memset_u16_opt_v1(tb_uint16_t* dst, tb_uint16_t src, tb_size_t size)
{
	/* align by 4-bytes */
	if (((tb_size_t)dst) & 0x3)
	{
		*((tb_uint16_t*)dst) = src;
		dst += 2;
		size--;
	}
	tb_size_t left = size & 0x3;
	dst += (size << 1);
	if (!left)
	{
		size >>= 2;
		__tb_asm__ __tb_volatile__
		(
			"1:\n\t"
			"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */
			"mov.w %1,@-%2\n\t" /* *--dst = src */
			"mov.w %1,@-%2\n\t" /* *--dst = src */
			"mov.w %1,@-%2\n\t" /* *--dst = src */
			"mov.w %1,@-%2\n\t" /* *--dst = src */
			"bf 1b\n\t"  		/* if T == 0 goto label 1: */
			: 					/* no output registers */ 
			: "r" (size), "r" (src), "r" (dst) /* constraint: register */
		);
	}
	else if (size >= 4)
	{
		size >>= 2;
		__tb_asm__ __tb_volatile__
		(
			"1:\n\t" 			/* fill left data */
			"dt %3\n\t"
			"mov.w %1,@-%2\n\t"
			"bf 1b\n\t"
			"2:\n\t"  			/* fill aligned data by 4 */
			"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */
			"mov.w %1,@-%2\n\t" /* *--dst = src */
			"mov.w %1,@-%2\n\t" /* *--dst = src */
			"mov.w %1,@-%2\n\t" /* *--dst = src */
			"mov.w %1,@-%2\n\t" /* *--dst = src */
			"bf 2b\n\t"  		/* if T == 0 goto label 1: */
			: 					/* no output registers */ 
			: "r" (size), "r" (src), "r" (dst), "r" (left) /* constraint: register */
		); 
	}
	else
	{
		__tb_asm__ __tb_volatile__
		(
			"1:\n\t"
			"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */
			"mov.w %1,@-%2\n\t" /* *--dst = src */
			"bf 1b\n\t"  		/* if T == 0 goto label 1: */
			: 					/* no output registers */
			: "r" (size), "r" (src), "r" (dst) /* constraint: register */
		);
	}
}
static __tb_inline__ tb_void_t tb_memset_u16_opt_v2(tb_uint16_t* dst, tb_uint16_t src, tb_size_t size)
{
	/* align by 4-bytes */
	if (((tb_size_t)dst) & 0x3)
	{
		*((tb_uint16_t*)dst) = src;
		dst += 2;
		size--;
	}
	dst += size << 1;
	__tb_asm__ __tb_volatile__
	(
		"1:\n\t"
		"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */
		"mov.w %1,@-%2\n\t" /* *--dst = src */
		"bf 1b\n\t"  		/* if T == 0 goto label 1: */
		: 					/* no output registers */
		: "r" (size), "r" (src), "r" (dst) /* constraint: register */
	);
}
tb_void_t tb_memset_u16(tb_byte_t* dst, tb_uint16_t src, tb_size_t size)
{
	if (!dst) return ;

	if (size > 1) tb_memset_u16_opt_v1((tb_uint16_t*)dst, src, size);
	else if (size == 1) *dst = src;
}
static __tb_inline__ tb_void_t tb_memset_u32_opt_v1(tb_uint32_t* dst, tb_uint32_t src, tb_size_t size)
{
	tb_size_t left = size & 0x3;
	dst += (size << 2);
	if (!left)
	{
		size >>= 2;
		__tb_asm__ __tb_volatile__
		(
			"1:\n\t" 
			"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */
			"mov.l %1,@-%2\n\t" /* *--dst = src */
			"mov.l %1,@-%2\n\t" /* *--dst = src */
			"mov.l %1,@-%2\n\t" /* *--dst = src */
			"mov.l %1,@-%2\n\t" /* *--dst = src */
			"bf 1b\n\t"  		/* if T == 0 goto label 1: */
			: 					/* no output registers */ 
			: "r" (size), "r" (src), "r" (dst) /* constraint: register */
		); 
	}
	else
	{
		__tb_asm__ __tb_volatile__
		(
			"1:\n\t" 
			"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */
			"mov.l %1,@-%2\n\t" /* *--dst = src */
			"bf 1b\n\t"  		/* if T == 0 goto label 1: */
			: 					/* no output registers */ 
			: "r" (size), "r" (src), "r" (dst) /* constraint: register */
		); 
	}
}
static __tb_inline__ tb_void_t tb_memset_u32_opt_v2(tb_uint32_t* dst, tb_uint32_t src, tb_size_t size)
{
	tb_size_t left = size & 0x3;
	dst += (size << 2);
	if (!left)
	{
		size >>= 2;
		__tb_asm__ __tb_volatile__
		(
			"1:\n\t" 
			"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */
			"mov.l %1,@-%2\n\t" /* *--dst = src */
			"mov.l %1,@-%2\n\t" /* *--dst = src */
			"mov.l %1,@-%2\n\t" /* *--dst = src */
			"mov.l %1,@-%2\n\t" /* *--dst = src */
			"bf 1b\n\t"  		/* if T == 0 goto label 1: */
			: 					/* no output registers */ 
			: "r" (size), "r" (src), "r" (dst) /* constraint: register */
		); 
	}
	else if (size >= 4) /* fixme */
	{
		size >>= 2;
		__tb_asm__ __tb_volatile__
		(
			"1:\n\t" 			/* fill the left data */
			"dt %3\n\t"
			"mov.l %1,@-%2\n\t"
			"bf 1b\n\t"
			"2:\n\t" 			/* fill aligned data by 4 */
			"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */
			"mov.l %1,@-%2\n\t" /* *--dst = src */
			"mov.l %1,@-%2\n\t" /* *--dst = src */
			"mov.l %1,@-%2\n\t" /* *--dst = src */
			"mov.l %1,@-%2\n\t" /* *--dst = src */
			"bf 2b\n\t"  		/* if T == 0 goto label 1: */
			: 					/* no output registers */ 
			: "r" (size), "r" (src), "r" (dst), "r" (left) /* constraint: register */
		); 
	}
	else
	{
		__tb_asm__ __tb_volatile__
		(
			"1:\n\t" 
			"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */
			"mov.l %1,@-%2\n\t" /* *--dst = src */
			"bf 1b\n\t"  		/* if T == 0 goto label 1: */
			: 					/* no output registers */ 
			: "r" (size), "r" (src), "r" (dst) /* constraint: register */
		); 
	}
}
static __tb_inline__ tb_void_t tb_memset_u32_opt_v3(tb_uint32_t* dst, tb_uint32_t src, tb_size_t size)
{
	dst += size << 2;
	__tb_asm__ __tb_volatile__
	(
		"1:\n\t"
		"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */
		"mov.l %1,@-%2\n\t" /* *--dst = src */
		"bf 1b\n\t"  		/* if T == 0 goto label 1: */
		: 					/* no output registers */ 
		: "r" (size), "r" (src), "r" (dst) /* constraint: register */
	);
}
tb_void_t tb_memset_u32(tb_byte_t* dst, tb_uint32_t src, tb_size_t size)
{
	if (!dst) return ;

	if (size > 1)  tb_memset_u32_opt_v1((tb_uint32_t*)dst, src, size);
	else if (size == 1) *dst = src;
}
#endif

// c plus plus
#ifdef __cplusplus
}
#endif

#endif

