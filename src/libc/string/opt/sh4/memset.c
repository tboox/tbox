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
 * @file		memset.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <string.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_LIBC_STRING_OPT_MEMSET_U8

#if defined(TB_CONFIG_ASSEMBLER_GAS)
# 	define TB_LIBC_STRING_OPT_MEMSET_U16
# 	define TB_LIBC_STRING_OPT_MEMSET_U32
#endif
/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

#ifdef TB_CONFIG_ASSEMBLER_GAS
static __tb_inline__ tb_void_t tb_memset_impl_u8_opt_v1(tb_byte_t* s, tb_byte_t c, tb_size_t n)
{

}
#endif

#ifdef TB_LIBC_STRING_OPT_MEMSET_U8
static tb_pointer_t tb_memset_impl(tb_pointer_t s, tb_byte_t c, tb_size_t n)
{
	// check
	tb_assert_and_check_return_val(s, tb_null);
	if (!n) return s;

# 	if 1
	memset(s, c, n);
# 	elif defined(TB_CONFIG_ASSEMBLER_GAS)
	tb_memset_impl_u8_opt_v1(s, (tb_byte_t)c, n);
# 	else
# 		error
# 	endif

	return s;
}
#endif


#ifdef TB_CONFIG_ASSEMBLER_GAS
static __tb_inline__ tb_void_t tb_memset_u16_impl_opt_v1(tb_uint16_t* s, tb_uint16_t c, tb_size_t n)
{
	/* align by 4-bytes */
	if (((tb_size_t)s) & 0x3)
	{
		*((tb_uint16_t*)s) = c;
		s += 2;
		n--;
	}
	tb_size_t l = n & 0x3;
	s += (n << 1);
	if (!l)
	{
		n >>= 2;
		__tb_asm__ __tb_volatile__
		(
			"1:\n\t"
			"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */
			"mov.w %1,@-%2\n\t" /* *--s = c */
			"mov.w %1,@-%2\n\t" /* *--s = c */
			"mov.w %1,@-%2\n\t" /* *--s = c */
			"mov.w %1,@-%2\n\t" /* *--s = c */
			"bf 1b\n\t"  		/* if T == 0 goto label 1: */
			: 					/* no output registers */ 
			: "r" (n), "r" (c), "r" (s) /* constraint: register */
		);
	}
	else if (n >= 4)
	{
		n >>= 2;
		__tb_asm__ __tb_volatile__
		(
			"1:\n\t" 			/* fill left data */
			"dt %3\n\t"
			"mov.w %1,@-%2\n\t"
			"bf 1b\n\t"
			"2:\n\t"  			/* fill aligned data by 4 */
			"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */
			"mov.w %1,@-%2\n\t" /* *--s = c */
			"mov.w %1,@-%2\n\t" /* *--s = c */
			"mov.w %1,@-%2\n\t" /* *--s = c */
			"mov.w %1,@-%2\n\t" /* *--s = c */
			"bf 2b\n\t"  		/* if T == 0 goto label 1: */
			: 					/* no output registers */ 
			: "r" (n), "r" (c), "r" (s), "r" (l) /* constraint: register */
		); 
	}
	else
	{
		__tb_asm__ __tb_volatile__
		(
			"1:\n\t"
			"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */
			"mov.w %1,@-%2\n\t" /* *--s = c */
			"bf 1b\n\t"  		/* if T == 0 goto label 1: */
			: 					/* no output registers */
			: "r" (n), "r" (c), "r" (s) /* constraint: register */
		);
	}
}
static __tb_inline__ tb_void_t tb_memset_u16_impl_opt_v2(tb_uint16_t* s, tb_uint16_t c, tb_size_t n)
{
	/* align by 4-bytes */
	if (((tb_size_t)s) & 0x3)
	{
		*((tb_uint16_t*)s) = c;
		s += 2;
		n--;
	}
	s += n << 1;
	__tb_asm__ __tb_volatile__
	(
		"1:\n\t"
		"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */
		"mov.w %1,@-%2\n\t" /* *--s = c */
		"bf 1b\n\t"  		/* if T == 0 goto label 1: */
		: 					/* no output registers */
		: "r" (n), "r" (c), "r" (s) /* constraint: register */
	);
}
#endif

#ifdef TB_LIBC_STRING_OPT_MEMSET_U16
static tb_pointer_t tb_memset_u16_impl(tb_pointer_t s, tb_uint16_t c, tb_size_t n)
{
	// check
	tb_assert_and_check_return_val(s, tb_null);

	// align by 2-bytes 
	tb_assert(!(((tb_size_t)s) & 0x1));
	if (!n) return s;

# 	if defined(TB_CONFIG_ASSEMBLER_GAS)
	tb_memset_u16_impl_opt_v1(s, c, n);
# 	else
# 		error
# 	endif

	return s;
}
#endif

#ifdef TB_CONFIG_ASSEMBLER_GAS
static __tb_inline__ tb_void_t tb_memset_u32_impl_opt_v1(tb_uint32_t* s, tb_uint32_t c, tb_size_t n)
{
	tb_size_t l = n & 0x3;
	s += (n << 2);
	if (!l)
	{
		n >>= 2;
		__tb_asm__ __tb_volatile__
		(
			"1:\n\t" 
			"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */
			"mov.l %1,@-%2\n\t" /* *--s = c */
			"mov.l %1,@-%2\n\t" /* *--s = c */
			"mov.l %1,@-%2\n\t" /* *--s = c */
			"mov.l %1,@-%2\n\t" /* *--s = c */
			"bf 1b\n\t"  		/* if T == 0 goto label 1: */
			: 					/* no output registers */ 
			: "r" (n), "r" (c), "r" (s) /* constraint: register */
		); 
	}
	else
	{
		__tb_asm__ __tb_volatile__
		(
			"1:\n\t" 
			"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */
			"mov.l %1,@-%2\n\t" /* *--s = c */
			"bf 1b\n\t"  		/* if T == 0 goto label 1: */
			: 					/* no output registers */ 
			: "r" (n), "r" (c), "r" (s) /* constraint: register */
		); 
	}
}
static __tb_inline__ tb_void_t tb_memset_u32_impl_opt_v2(tb_uint32_t* s, tb_uint32_t c, tb_size_t n)
{
	tb_size_t l = n & 0x3;
	s += (n << 2);
	if (!l)
	{
		n >>= 2;
		__tb_asm__ __tb_volatile__
		(
			"1:\n\t" 
			"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */
			"mov.l %1,@-%2\n\t" /* *--s = c */
			"mov.l %1,@-%2\n\t" /* *--s = c */
			"mov.l %1,@-%2\n\t" /* *--s = c */
			"mov.l %1,@-%2\n\t" /* *--s = c */
			"bf 1b\n\t"  		/* if T == 0 goto label 1: */
			: 					/* no output registers */ 
			: "r" (n), "r" (c), "r" (s) /* constraint: register */
		); 
	}
	else if (n >= 4) /* fixme */
	{
		n >>= 2;
		__tb_asm__ __tb_volatile__
		(
			"1:\n\t" 			/* fill the left data */
			"dt %3\n\t"
			"mov.l %1,@-%2\n\t"
			"bf 1b\n\t"
			"2:\n\t" 			/* fill aligned data by 4 */
			"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */
			"mov.l %1,@-%2\n\t" /* *--s = c */
			"mov.l %1,@-%2\n\t" /* *--s = c */
			"mov.l %1,@-%2\n\t" /* *--s = c */
			"mov.l %1,@-%2\n\t" /* *--s = c */
			"bf 2b\n\t"  		/* if T == 0 goto label 1: */
			: 					/* no output registers */ 
			: "r" (n), "r" (c), "r" (s), "r" (l) /* constraint: register */
		); 
	}
	else
	{
		__tb_asm__ __tb_volatile__
		(
			"1:\n\t" 
			"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */
			"mov.l %1,@-%2\n\t" /* *--s = c */
			"bf 1b\n\t"  		/* if T == 0 goto label 1: */
			: 					/* no output registers */ 
			: "r" (n), "r" (c), "r" (s) /* constraint: register */
		); 
	}
}
static __tb_inline__ tb_void_t tb_memset_u32_impl_opt_v3(tb_uint32_t* s, tb_uint32_t c, tb_size_t n)
{	
	s += n << 2;
	__tb_asm__ __tb_volatile__
	(
		"1:\n\t"
		"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */
		"mov.l %1,@-%2\n\t" /* *--s = c */
		"bf 1b\n\t"  		/* if T == 0 goto label 1: */
		: 					/* no output registers */ 
		: "r" (n), "r" (c), "r" (s) /* constraint: register */
	);
}
#endif

#ifdef TB_LIBC_STRING_OPT_MEMSET_U32
static tb_pointer_t tb_memset_u32_impl(tb_pointer_t s, tb_uint32_t c, tb_size_t n)
{
	// check
	tb_assert_and_check_return_val(s, tb_null);

	// align by 4-bytes 
	tb_assert(!(((tb_size_t)s) & 0x3));
	if (!n) return s;

# 	if defined(TB_CONFIG_ASSEMBLER_GAS)
	tb_memset_u32_impl_opt_v1(s, c, n);
# 	else
# 		error
# 	endif

	return s;
}
#endif


