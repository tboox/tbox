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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		memcpy.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

#ifndef TB_CONFIG_LIBC_HAVE_MEMCPY
# 	if defined(TB_CONFIG_ARCH_x86)
# 		include "opt/x86/memcpy.c"
# 	elif defined(TB_CONFIG_ARCH_ARM)
# 		include "opt/arm/memcpy.c"
# 	elif defined(TB_CONFIG_ARCH_SH4)
# 		include "opt/sh4/memcpy.c"
# 	endif
#else
# 	include <string.h>
#endif

/* ///////////////////////////////////////////////////////////////////////
 * implemention
 */

#if defined(TB_CONFIG_LIBC_HAVE_MEMCPY)
tb_pointer_t tb_memcpy(tb_pointer_t s1, tb_cpointer_t s2, tb_size_t n)
{
	tb_assert_and_check_return_val(s1 && s2, TB_NULL);
	return memcpy(s1, s2, n);
}
#elif !defined(TB_LIBC_STRING_OPT_MEMCPY)
tb_pointer_t tb_memcpy(tb_pointer_t s1, tb_cpointer_t s2, tb_size_t n)
{
	tb_assert_and_check_return_val(s1 && s2, TB_NULL);

#ifdef TB_CONFIG_BINARY_SMALL
	__tb_register__ tb_byte_t* p1 = s1;
	__tb_register__ tb_byte_t* p2 = s2;
	if (p1 == p2 || !n) return s1;
	while (n--) *p1++ = *p2++;
	return s1;
#else
	__tb_register__ tb_byte_t* p1 = s1;
	__tb_register__ tb_byte_t* p2 = s2;
	if (p1 == p2 || !n) return s1;
	
	tb_size_t l = n & 0x3; n = (n - l) >> 2;
	while (n--)
	{
		p1[0] = p2[0];
		p1[1] = p2[1];
		p1[2] = p2[2];
		p1[3] = p2[3];
		p1 += 4;
		p2 += 4;
	}
	while (l--) *p1++ = *p2++;
	return s1;
#endif /* TB_CONFIG_BINARY_SMALL */
}
#endif

