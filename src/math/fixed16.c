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
 * \file		fixed16.c
 *
 */
/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "fixed16.h"
#include "int32.h"

#ifdef TB_CONFIG_TYPE_FLOAT
# 	include "float.h"
#endif

/* ////////////////////////////////////////////////////////////////////////
 * implemention
 */

tb_fixed16_t tb_fixed16_mul_generic(tb_fixed16_t x, tb_fixed16_t y)
{
#ifdef TB_CONFIG_TYPE_FLOAT
	return tb_float_to_fixed16(tb_float_mul(tb_fixed16_to_float(x), tb_fixed16_to_float(y)));
#else
	TB_NOT_IMPLEMENT();
	return 0;
#endif
}

tb_fixed16_t tb_fixed16_div_generic(tb_fixed16_t x, tb_fixed16_t y)
{
#ifdef TB_CONFIG_TYPE_FLOAT
	return tb_float_to_fixed16(tb_float_div(tb_fixed16_to_float(x), tb_fixed16_to_float(y)));
#else
	TB_NOT_IMPLEMENT();
	return 0;
#endif
}
tb_fixed16_t tb_fixed16_sqre_generic(tb_fixed16_t x)
{
#ifdef TB_CONFIG_TYPE_FLOAT
	return tb_float_to_fixed16(tb_float_sqre(tb_fixed16_to_float(x)));
#else
	TB_NOT_IMPLEMENT();
	return 0;
#endif
}
tb_fixed16_t tb_fixed16_sqrt_generic(tb_fixed16_t x)
{
	TB_ASSERT(x > 0);
	return (x > 0? (tb_int32_sqrt(x) << 8) : 0);
}
tb_fixed16_t tb_fixed16_sin_generic(tb_fixed16_t x)
{
#ifdef TB_CONFIG_TYPE_FLOAT
	return tb_float_to_fixed16(tb_float_sin(tb_fixed16_to_float(x)));
#else
	TB_NOT_IMPLEMENT();
	return 0;
#endif
}
tb_fixed16_t tb_fixed16_cos_generic(tb_fixed16_t x)
{
#ifdef TB_CONFIG_TYPE_FLOAT
	return tb_float_to_fixed16(tb_float_cos(tb_fixed16_to_float(x)));
#else
	TB_NOT_IMPLEMENT();
	return 0;
#endif
}
tb_fixed16_t tb_fixed16_tan_generic(tb_fixed16_t x)
{
#ifdef TB_CONFIG_TYPE_FLOAT
	return tb_float_to_fixed16(tb_float_tan(tb_fixed16_to_float(x)));
#else
	TB_NOT_IMPLEMENT();
	return 0;
#endif
}
tb_fixed16_t tb_fixed16_asin_generic(tb_fixed16_t x)
{
#ifdef TB_CONFIG_TYPE_FLOAT
	return tb_float_to_fixed16(tb_float_asin(tb_fixed16_to_float(x)));
#else
	TB_NOT_IMPLEMENT();
	return 0;
#endif
}
tb_fixed16_t tb_fixed16_acos_generic(tb_fixed16_t x)
{
#ifdef TB_CONFIG_TYPE_FLOAT
	return tb_float_to_fixed16(tb_float_acos(tb_fixed16_to_float(x)));
#else
	TB_NOT_IMPLEMENT();
	return 0;
#endif
}
tb_fixed16_t tb_fixed16_atan_generic(tb_fixed16_t x)
{
#ifdef TB_CONFIG_TYPE_FLOAT
	return tb_float_to_fixed16(tb_float_atan(tb_fixed16_to_float(x)));
#else
	TB_NOT_IMPLEMENT();
	return 0;
#endif
}
tb_fixed16_t tb_fixed16_exp_generic(tb_fixed16_t x)
{
#ifdef TB_CONFIG_TYPE_FLOAT
	return tb_float_to_fixed16(tb_float_exp(tb_fixed16_to_float(x)));
#else
	TB_NOT_IMPLEMENT();
	return 0;
#endif
}
tb_fixed16_t tb_fixed16_exp1_generic(tb_fixed16_t x)
{
#ifdef TB_CONFIG_TYPE_FLOAT
	return tb_float_to_fixed16(tb_float_exp1(tb_fixed16_to_float(x)));
#else
	TB_NOT_IMPLEMENT();
	return 0;
#endif
}
tb_fixed16_t tb_fixed16_expi_generic(tb_uint16_t x)
{
#ifdef TB_CONFIG_TYPE_FLOAT
	return tb_float_to_fixed16(tb_float_expi(x));
#else
	TB_NOT_IMPLEMENT();
	return 0;
#endif
}
tb_uint32_t tb_fixed16_ilog2_generic(tb_fixed16_t x)
{
	TB_ASSERT(x > 0);
	tb_uint32_t lg = tb_int32_log2(x);
	return (lg > 16? (lg - 16) : 0);
}
tb_uint32_t tb_fixed16_iclog2_generic(tb_fixed16_t x)
{
	TB_ASSERT(x > 0);
	tb_uint32_t lg = tb_int32_clog2(x);
	return (lg > 16? (lg - 16) : 0);
}
tb_uint32_t tb_fixed16_irlog2_generic(tb_fixed16_t x)
{
	TB_ASSERT(x > 0);
	tb_uint32_t lg = tb_int32_rlog2(x);
	return (lg > 16? (lg - 16) : 0);
}
