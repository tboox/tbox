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
 * \file		log.h
 *
 */
#ifndef TB_MATH_LOG_H
#define TB_MATH_LOG_H

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ////////////////////////////////////////////////////////////////////////
 * macros
 */

// ilog2i(x) 
#define TB_MATH_ILOG2I(x) 					tb_math_ilog2i(x)

// irlog2i(x) 
#define TB_MATH_IRLOG2I(x) 					tb_math_irlog2i(x)

// iclog2i(x) 
#define TB_MATH_ICLOG2I(x) 					tb_math_iclog2i(x)

// ilog2f(x) 
#define TB_MATH_ILOG2F(x) 					tb_math_ilog2i(TB_MATH_IROUND(x))

// round(ilog2f(x))
#define TB_MATH_IRLOG2F(x) 					tb_math_irlog2i(TB_MATH_IROUND(x))

// ceil(ilog2f(x))
#define TB_MATH_ICLOG2F(x) 					tb_math_iclog2i(TB_MATH_IROUND(x))

// flog10f(x) 
#define TB_MATH_FLOG10F(x) 					log(x)

/* ////////////////////////////////////////////////////////////////////////
 * globals
 */
extern tb_uint32_t g_tb_math_ilog2i_table[32];
extern tb_uint32_t g_tb_math_irlog2i_table[32];
extern tb_uint32_t g_tb_math_iclog2i_table[33];

/* ////////////////////////////////////////////////////////////////////////
 * implements
 */
static __tb_inline__ tb_uint32_t tb_math_ilog2i(tb_uint32_t x)
{
	tb_int_t l = 0;
	tb_int_t m = 15;
	tb_int_t r = 32;
	while ((r - l) > 1)
	{
		tb_uint32_t v = g_tb_math_ilog2i_table[m];
		if (x < v) r = m;
		else l = m;
		m = (l + r) >> 1;
	}
	return m;
}
static __tb_inline__ tb_uint32_t tb_math_irlog2i(tb_uint32_t x)
{
	tb_int_t l = 0;
	tb_int_t m = 15;
	tb_int_t r = 32;
	while ((r - l) > 1)
	{
		tb_uint32_t v = g_tb_math_irlog2i_table[m];
		if (x < v) r = m;
		else l = m;
		m = (l + r) >> 1;
	}
	return m;
}

static __tb_inline__ tb_uint32_t tb_math_iclog2i(tb_uint32_t x)
{
	tb_int_t l = 0;
	tb_int_t m = 16;
	tb_int_t r = 33;
	while ((r - l) > 1)
	{
		tb_uint32_t v = g_tb_math_iclog2i_table[m];
		if (x < v) r = m;
		else l = m;
		m = (l + r) >> 1;
	}
	return m;
}
#endif

