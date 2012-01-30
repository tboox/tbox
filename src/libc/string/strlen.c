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
 * \file		strlen.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

#ifndef TB_CONFIG_LIBC_HAVE_STRLEN
# 	if defined(TB_CONFIG_ARCH_x86)
# 		include "opt/x86/strlen.c"
# 	elif defined(TB_CONFIG_ARCH_ARM)
# 		include "opt/arm/strlen.c"
# 	elif defined(TB_CONFIG_ARCH_SH4)
# 		include "opt/sh4/strlen.c"
# 	endif
#else
# 	include <string.h>
#endif

/* ///////////////////////////////////////////////////////////////////////
 * interfaces 
 */

#if defined(TB_CONFIG_LIBC_HAVE_STRLEN)
tb_size_t tb_strlen(tb_char_t const* s)
{
	tb_assert_and_check_return_val(s, 0);
	return strlen(s);
}
#elif !defined(TB_LIBC_STRING_OPT_STRLEN)
tb_size_t tb_strlen(tb_char_t const* s)
{
	tb_assert_and_check_return_val(s, 0);

	__tb_register__ tb_char_t const* p = s;

#ifdef TB_CONFIG_BINARY_SMALL
	while (*p) p++;
	return (p - s);
#else
	while (1) 
	{
		if (!p[0]) return (p - s + 0);
		if (!p[1]) return (p - s + 1);
		if (!p[2]) return (p - s + 2);
		if (!p[3]) return (p - s + 3);
		p += 4;
	}
	return 0;
#endif
}
#endif
