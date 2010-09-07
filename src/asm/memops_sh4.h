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
 * along with TGraphic; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		memops_sh4.h
 *
 */
#ifndef TB_MEMOPS_SH4_H
#define TB_MEMOPS_SH4_H

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

#ifdef TPLAT_ASSEMBLER_GAS

// memset_u16
# 	if 0
# 		define TB_MEMOPS_ASM_MEMSET_U16(dst, src, size) \
		do \
		{ \
			dst += size << 1; \
			__tplat_asm__ __tplat_volatile__ \
			(
				"1:\n\t" \
				"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */ \
				"mov.w %1,@-%2\n\t" /* *--dst = src */ \
				"bf 1b\n\t"  		/* if T == 0 goto label 1: */ \
				: 					/* no output registers */ \
				: "r" (size), "r" (src), "r" (dst) /* constraint: register */ \
			); \
\
		} while (0)

# 	else
# 		define TB_MEMOPS_ASM_MEMSET_U16(dst, src, size) \
		do \
		{ \
			tb_size_t left = size & 0x3; \
			dst += (size << 1); \
			if (!left) \
			{ \
				size >>= 2; \
				__tplat_asm__ __tplat_volatile__ \
				( \
					"1:\n\t" \
					"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */ \
					"mov.w %1,@-%2\n\t" /* *--dst = src */ \
					"mov.w %1,@-%2\n\t" /* *--dst = src */ \
					"mov.w %1,@-%2\n\t" /* *--dst = src */ \
					"mov.w %1,@-%2\n\t" /* *--dst = src */ \
					"bf 1b\n\t"  		/* if T == 0 goto label 1: */ \
					: 					/* no output registers */  \
					: "r" (size), "r" (src), "r" (dst) /* constraint: register */ \
				); \
			} \
			else if (size >= 4) \
			{ \
				size >>= 2; \
				__tplat_asm__ __tplat_volatile__ \
				( \
					"1:\n\t" 			/* fill left data */ \
					"dt %3\n\t" \
					"mov.w %1,@-%2\n\t" \
					"bf 1b\n\t" \
					"2:\n\t"  			/* fill aligned data by 4 */ \
					"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */ \
					"mov.w %1,@-%2\n\t" /* *--dst = src */ \
					"mov.w %1,@-%2\n\t" /* *--dst = src */ \
					"mov.w %1,@-%2\n\t" /* *--dst = src */ \
					"mov.w %1,@-%2\n\t" /* *--dst = src */ \
					"bf 2b\n\t"  		/* if T == 0 goto label 1: */ \
					: 					/* no output registers */  \
					: "r" (size), "r" (src), "r" (dst), "r" (left) /* constraint: register */ \
				);  \
			} \
			else \
			{ \
				__tplat_asm__ __tplat_volatile__ \
				( \
					"1:\n\t" \
					"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */ \
					"mov.w %1,@-%2\n\t" /* *--dst = src */ \
					"bf 1b\n\t"  		/* if T == 0 goto label 1: */ \
					: 					/* no output registers */ \
					: "r" (size), "r" (src), "r" (dst) /* constraint: register */ \
				); \
			} \
\
 		} while (0)
# 	endif

// memset_u32
# 	if 0
# 		define TB_MEMOPS_ASM_MEMSET_U32(dst, src, size) \
		do \
		{ \
			dst += size << 2; \
			__tplat_asm__ __tplat_volatile__\
			( \
				"1:\n\t" \
				"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */ \
				"mov.l %1,@-%2\n\t" /* *--dst = src */ \
				"bf 1b\n\t"  		/* if T == 0 goto label 1: */ \
				: 					/* no output registers */  \
				: "r" (size), "r" (src), "r" (dst) /* constraint: register */ \
			); \
\
		} while (0)

# 	elif 0
# 		define TB_MEMOPS_ASM_MEMSET_U32(dst, src, size) \
		do \
		{ \
			tb_size_t left = size & 0x3; \
			dst += (size << 2); \
			if (!left) \
			{ \
				size >>= 2; \
				__tplat_asm__ __tplat_volatile__ \
				( \
					"1:\n\t"  \
					"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */ \
					"mov.l %1,@-%2\n\t" /* *--dst = src */ \
					"mov.l %1,@-%2\n\t" /* *--dst = src */ \
					"mov.l %1,@-%2\n\t" /* *--dst = src */ \
					"mov.l %1,@-%2\n\t" /* *--dst = src */ \
					"bf 1b\n\t"  		/* if T == 0 goto label 1: */ \
					: 					/* no output registers */  \
					: "r" (size), "r" (src), "r" (dst) /* constraint: register */ \
				);  \
			} \
			else if (size >= 4) /* fixme */ \
			{ \
				size >>= 2; \
				__tplat_asm__ __tplat_volatile__ \
				( \
					"1:\n\t" 			/* fill the left data */ \
					"dt %3\n\t" \
					"mov.l %1,@-%2\n\t" \
					"bf 1b\n\t" \
					"2:\n\t" 			/* fill aligned data by 4 */ \
					"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */ \
					"mov.l %1,@-%2\n\t" /* *--dst = src */ \
					"mov.l %1,@-%2\n\t" /* *--dst = src */ \
					"mov.l %1,@-%2\n\t" /* *--dst = src */ \
					"mov.l %1,@-%2\n\t" /* *--dst = src */ \
					"bf 2b\n\t"  		/* if T == 0 goto label 1: */ \
					: 					/* no output registers */  \
					: "r" (size), "r" (src), "r" (dst), "r" (left) /* constraint: register */ \
				);  \
			} \
			else \
			{ \
				__tplat_asm__ __tplat_volatile__ \
				( \
					"1:\n\t"  \
					"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */ \
					"mov.l %1,@-%2\n\t" /* *--dst = src */ \
					"bf 1b\n\t"  		/* if T == 0 goto label 1: */ \
					: 					/* no output registers */  \
					: "r" (size), "r" (src), "r" (dst) /* constraint: register */ \
				);  \
			} \
\
 		} while (0)
# 	else
# 		define TB_MEMOPS_ASM_MEMSET_U32(dst, src, size) \
		do \
		{ \
			tb_size_t left = size & 0x3; \
			dst += (size << 2); \
			if (!left) \
			{ \
				size >>= 2; \
				__tplat_asm__ __tplat_volatile__ \
				( \
					"1:\n\t"  \
					"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */ \
					"mov.l %1,@-%2\n\t" /* *--dst = src */ \
					"mov.l %1,@-%2\n\t" /* *--dst = src */ \
					"mov.l %1,@-%2\n\t" /* *--dst = src */ \
					"mov.l %1,@-%2\n\t" /* *--dst = src */ \
					"bf 1b\n\t"  		/* if T == 0 goto label 1: */ \
					: 					/* no output registers */  \
					: "r" (size), "r" (src), "r" (dst) /* constraint: register */ \
				);  \
			} \
			else \
			{ \
				__tplat_asm__ __tplat_volatile__ \
				( \
					"1:\n\t"  \
					"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */ \
					"mov.l %1,@-%2\n\t" /* *--dst = src */ \
					"bf 1b\n\t"  		/* if T == 0 goto label 1: */ \
					: 					/* no output registers */  \
					: "r" (size), "r" (src), "r" (dst) /* constraint: register */ \
				);  \
			} \
\
 		} while (0)

# 	endif
#else
# 	undef TB_MEMOPS_ASM_MEMSET_U16
# 	undef TB_MEMOPS_ASM_MEMSET_U32
#endif

// c plus plus
#ifdef __cplusplus
}
#endif

#endif

