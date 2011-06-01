/*!The Tiny Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		memops_x86.h
 *
 */
#ifndef TB_MEMORY_OPT_MEMOPS_x86_H
#define TB_MEMORY_OPT_MEMOPS_x86_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#ifdef TB_CONFIG_OPTI_SSE2_ENABLE
# 	include <emmintrin.h>
#endif

/* /////////////////////////////////////////////////////////
 * macros
 */

#ifdef TB_CONFIG_ASSEMBLER_GAS

// memset_u16
# 	if 1
# 		define TB_MEMOPS_OPT_MEMSET_U16(dst, src, size) \
		do \
		{ \
			/* align by 4-bytes */ \
			if (((tb_size_t)dst) & 0x3) \
			{ \
				*((tb_uint16_t*)dst) = src; \
				dst += 2; \
				size--; \
			} \
			__tb_asm__ __tb_volatile__ \
			( \
				"cld\n\t" 		/* clear the direction bit, dst++, not dst-- */ \
				"rep stosw" 	/* *dst++ = ax */ \
				: 				/* no output registers */ \
				: "c" (size), "a" (src), "D" (dst) /* ecx = size, eax = src, edi = dst */ \
			); \
\
		} while (0)

# 	elif 0
# 		define TB_MEMOPS_OPT_MEMSET_U16(dst, src, size) \
		do \
		{ \
			/* align by 4-bytes */ \
			if (((tb_size_t)dst) & 0x3) \
			{ \
				*((tb_uint16_t*)dst) = src; \
				dst += 2; \
				size--; \
			} \
			tb_size_t left = size & 0x3; \
			if (!left) \
			{ \
				size >>= 2; \
				__tb_asm__ __tb_volatile__ \
				( \
					"cld\n\t" \
					"1:\n\t" \
					"dec %%ecx\n\t" /* i--, i > 0? T = 0 : 1 */ \
					"stosw\n\t" 	/* *--dst = src */ \
					"stosw\n\t" 	/* *--dst = src */ \
					"stosw\n\t" 	/* *--dst = src */ \
					"stosw\n\t" 	/* *--dst = src */ \
					"jnz 1b\n\t"  	/* if T == 0 goto label 1: */ \
					: 				/* no output registers */  \
					: "c" (size), "a" (src), "D" (dst) /* constraint: register */ \
				); \
			} \
			else if (size >= 4) \
			{ \
				size >>= 2; \
				__tb_asm__ __tb_volatile__ \
				( \
				  	"cld\n\t" \
					"1:\n\t" 		/* fill left data */ \
					"dec %3\n\t" \
					"stosw\n\t" \
					"jnz 1b\n\t" \
					"2:\n\t"  		/* fill aligned data by 4 */ \
					"dec %%ecx\n\t" /* i--, i > 0? T = 0 : 1 */ \
					"stosw\n\t" 	/* *--dst = src */ \
					"stosw\n\t" 	/* *--dst = src */ \
					"stosw\n\t" 	/* *--dst = src */ \
					"stosw\n\t" 	/* *--dst = src */ \
					"jnz 2b\n\t"  	/* if T == 0 goto label 1: */ \
					: 				/* no output registers */  \
					: "c" (size), "a" (src), "D" (dst), "r" (left) /* constraint: register */ \
				);  \
			} \
			else \
			{ \
				__tb_asm__ __tb_volatile__ \
				( \
				  	"cld\n\t" \
					"1:\n\t" \
					"dec %%ecx\n\t" 		/* i--, i > 0? T = 0 : 1 */ \
					"stosw\n\t" /* *--dst = src */ \
					"jnz 1b\n\t"  		/* if T == 0 goto label 1: */ \
					: 					/* no output registers */ \
					: "c" (size), "a" (src), "D" (dst) /* constraint: register */ \
				); \
			} \
\
 		} while (0)

# 	else
# 		define TB_MEMOPS_OPT_MEMSET_U16(dst, src, size) \
		do \
		{ \
			/* align by 4-bytes */ \
			if (((tb_size_t)dst) & 0x3) \
			{ \
				*((tb_uint16_t*)dst) = src; \
				dst += 2; \
				size--; \
			} \
			__tb_asm__ __tb_volatile__ \
			( \
				"cld\n\t" \
				"mov %0, %%ecx\n\t" \
				"mov %1, %%eax\n\t" \
				"mov %2, %%edi\n\t" \
				"rep stosw" \
				: /* no output registers */ \
				: "m" (size), "m" (src), "m" (dst) \
				: "%ecx", "%eax", "%edi" \
			); \
\
 		} while (0)
# 	endif

// memset_u32
# 	if 1
# 		define TB_MEMOPS_OPT_MEMSET_U32(dst, src, size) \
		do \
		{ \
			__tb_asm__ __tb_volatile__ \
			( \
				"cld\n\t" 		/* clear the direction bit, dst++, not dst-- */ \
				"rep stosl" 	/* *dst++ = eax */ \
				: 				/* no output registers */ \
				: "c" (size), "a" (src), "D" (dst) /* ecx = size, eax = src, edi = dst */ \
			); \
\
		} while (0)

# 	else
# 		define TB_MEMOPS_OPT_MEMSET_U32(dst, src, size) \
		do \
		{ \
			__tb_asm__ __tb_volatile__ \
			( \
				"cld\n\t" \
				"mov %0, %%ecx\n\t" \
				"mov %1, %%eax\n\t" \
				"mov %2, %%edi\n\t" \
				"rep stosl" \
				: /* no output registers */ \
				: "m" (size), "m" (src), "m" (dst) \
				: "%ecx", "%eax", "%edi" \
			); \
\
 		} while (0)

# 	endif
#else
# 	undef TB_MEMOPS_OPT_MEMSET_U16
# 	undef TB_MEMOPS_OPT_MEMSET_U32
#endif

// c plus plus
#ifdef __cplusplus
}
#endif

#endif

