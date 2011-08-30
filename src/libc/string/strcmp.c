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
 * \file		strcmp.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

#if defined(TB_CONFIG_ARCH_x86)
# 	include "opt/x86/strcmp.c"
#elif defined(TB_CONFIG_ARCH_ARM)
# 	include "opt/arm/strcmp.c"
#elif defined(TB_CONFIG_ARCH_SH4)
# 	include "opt/sh4/strcmp.c"
#endif

/* /////////////////////////////////////////////////////////
 * interfaces 
 */
#ifndef TB_LIBC_STRING_OPT_STRCMP
tb_int_t tb_strcmp(tb_char_t const* s1, tb_char_t const* s2)
{
	TB_ASSERT_RETURN_VAL(s1 && s2, 0);
	if (s1 == s2) return 0;

	tb_int_t r = 0;
	while (((r = ((tb_int_t)(*((tb_byte_t *)s1))) - *((tb_byte_t *)s2++)) == 0) && *s1++);
	return r;
}
#endif

