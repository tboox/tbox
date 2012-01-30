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
 * \file		strcpy.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "string.h"

#ifndef TB_CONFIG_LIBC_HAVE_STRCPY
# 	if defined(TB_CONFIG_ARCH_x86)
# 		include "opt/x86/strcpy.c"
# 	elif defined(TB_CONFIG_ARCH_ARM)
# 		include "opt/arm/strcpy.c"
# 	elif defined(TB_CONFIG_ARCH_SH4)
# 		include "opt/sh4/strcpy.c"
# 	endif
#else
# 	include <string.h>
#endif

/* ///////////////////////////////////////////////////////////////////////
 * interfaces 
 */

#if defined(TB_CONFIG_LIBC_HAVE_STRCPY)
tb_char_t* tb_strcpy(tb_char_t* s1, tb_char_t const* s2)
{
	tb_assert_and_check_return_val(s1 && s2, TB_NULL);
	return strcpy(s1, s2);
}
#elif !defined(TB_LIBC_STRING_OPT_STRCPY)
tb_char_t* tb_strcpy(tb_char_t* s1, tb_char_t const* s2)
{
	tb_assert_and_check_return_val(s1 && s2, TB_NULL);

	__tb_register__ tb_char_t* s = s1;
	if (s1 == s2) return s;

#if 1
	tb_memcpy(s1, s2, tb_strlen(s2) + 1);
#elif defined(TB_CONFIG_BINARY_SMALL)
	while ((*s++ = *s2++)) ;
#else
	while (1) 
	{
		if (!(s1[0] = s2[0])) break;
		if (!(s1[1] = s2[1])) break;
		if (!(s1[2] = s2[2])) break;
		if (!(s1[3] = s2[3])) break;
		s1 += 4;
		s2 += 4;
	}
#endif

	return s;
}
#endif
