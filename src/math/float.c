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
 * \file		float.c
 *
 */
/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "float.h"
#include "int32.h"

/* ////////////////////////////////////////////////////////////////////////
 * implemention
 */

tb_float_t tb_float_sqrt_generic(tb_float_t x)
{
	TB_ASSERT(x > 0);
	return sqrt(x);
}
tb_float_t tb_float_sin_generic(tb_float_t x)
{
	TB_NOT_IMPLEMENT();
	return 0;
}
tb_float_t tb_float_cos_generic(tb_float_t x)
{
	TB_NOT_IMPLEMENT();
	return 0;
}
tb_float_t tb_float_tan_generic(tb_float_t x)
{
	TB_NOT_IMPLEMENT();
	return 0;
}
tb_float_t tb_float_asin_generic(tb_float_t x)
{
	TB_NOT_IMPLEMENT();
	return 0;
}
tb_float_t tb_float_acos_generic(tb_float_t x)
{
	TB_NOT_IMPLEMENT();
	return 0;
}
tb_float_t tb_float_atan_generic(tb_float_t x)
{
	TB_NOT_IMPLEMENT();
	return 0;
}
tb_float_t tb_float_exp_generic(tb_float_t x)
{
	TB_NOT_IMPLEMENT();
	return 0;
}
tb_uint32_t tb_float_ilog2_generic(tb_float_t x)
{
	TB_ASSERT(x > 0);
	return tb_int32_log2(tb_float_round(x));
}
