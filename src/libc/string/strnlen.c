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
 * \file		strnlen.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

#if defined(TB_CONFIG_ARCH_x86)
# 	include "opt/x86/strlen.c"
#elif defined(TB_CONFIG_ARCH_ARM)
# 	include "opt/arm/strlen.c"
#elif defined(TB_CONFIG_ARCH_SH4)
# 	include "opt/sh4/strlen.c"
#endif

/* /////////////////////////////////////////////////////////
 * interfaces 
 */

#ifndef TB_LIBC_STRING_OPT_STRNLEN
tb_size_t tb_strnlen(tb_char_t const* s, tb_size_t n)
{
	TB_ASSERT_RETURN_VAL(s, 0);

	__tb_register__ tb_char_t const* p = s;

#ifdef TB_CONFIG_BINARY_SMALL
	while (n-- && *p) ++p;
	return p - s;
#else
	tb_size_t l = n & 0x3; n = (n - l) >> 2;
	while (n--)
	{
		if (!p[0]) return (p - s + 0);
		if (!p[1]) return (p - s + 1);
		if (!p[2]) return (p - s + 2);
		if (!p[3]) return (p - s + 3);
		p += 4;
	}

	while (l-- && *p) ++p;
	return p - s;
#endif
}
#endif
