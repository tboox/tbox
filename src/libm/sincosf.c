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
 * @file		sincosf.c
 * @ingroup 	libm
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "math.h"
#include <math.h>

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_void_t tb_sincosf(tb_float_t x, tb_float_t* s, tb_float_t* c)
{
#ifdef TB_CONFIG_LIBM_HAVE_SINCOSF
	sincosf(x, s, c);
#else
	if (s) *s = sinf(x);
	if (c) *c = cosf(x);
#endif
}
