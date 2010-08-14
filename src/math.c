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
 * along with TGraphic; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		math.c
 *
 */

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "math.h"

/* ////////////////////////////////////////////////////////////////////////
 * macros
 */
#ifdef TB_WORDS_BIGENDIAN

typedef union
{
	double value;
	struct
	{
		tb_uint32_t msw;
		tb_uint32_t lsw;

	} parts;
} ieee_double_shape_type;

#else

typedef union
{
	double value;
	struct
	{
		tb_uint32_t lsw;
		tb_uint32_t msw;

	} parts;
} ieee_double_shape_type;

#endif

/* Get two 32 bit ints from a double.  */

#define EXTRACT_WORDS(ix0,ix1,d)				\
	do {								\
		ieee_double_shape_type ew_u;					\
		ew_u.value = (d);						\
		(ix0) = ew_u.parts.msw;					\
		(ix1) = ew_u.parts.lsw;					\
	} while (0)

/* Get the more significant 32 bit int from a double.  */

#define GET_HIGH_WORD(i,d)					\
	do {								\
		ieee_double_shape_type gh_u;					\
		gh_u.value = (d);						\
		(i) = gh_u.parts.msw;						\
	} while (0)

/* Get the less significant 32 bit int from a double.  */

#define GET_LOW_WORD(i,d)					\
	do {								\
		ieee_double_shape_type gl_u;					\
		gl_u.value = (d);						\
		(i) = gl_u.parts.lsw;						\
	} while (0)

/* Set a double from two 32 bit ints.  */

#define INSERT_WORDS(d,ix0,ix1)					\
	do {								\
		ieee_double_shape_type iw_u;					\
		iw_u.parts.msw = (ix0);					\
		iw_u.parts.lsw = (ix1);					\
		(d) = iw_u.value;						\
	} while (0)

/* Set the more significant 32 bits of a double from an int.  */

#define SET_HIGH_WORD(d,v)					\
	do {								\
		ieee_double_shape_type sh_u;					\
		sh_u.value = (d);						\
		sh_u.parts.msw = (v);						\
		(d) = sh_u.value;						\
	} while (0)

/* Set the less significant 32 bits of a double from an int.  */

#define SET_LOW_WORD(d,v)					\
	do {								\
		ieee_double_shape_type sl_u;					\
		sl_u.value = (d);						\
		sl_u.parts.lsw = (v);						\
		(d) = sl_u.value;						\
	} while (0)

/* A union which permits us to convert between a float and a 32 bit
   int.  */

typedef union
{
	float value;
	tb_uint32_t word;

} ieee_float_shape_type;

/* Get a 32 bit int from a float.  */

#define GET_FLOAT_WORD(i,d)					\
	do {								\
		ieee_float_shape_type gf_u;					\
		gf_u.value = (d);						\
		(i) = gf_u.word;						\
	} while (0)

/* Set a float from a 32 bit int.  */

#define SET_FLOAT_WORD(d,i)					\
	do {								\
		ieee_float_shape_type sf_u;					\
		sf_u.word = (i);						\
		(d) = sf_u.value;						\
	} while (0)

/* ////////////////////////////////////////////////////////////////////////
 * types
 */

/* ////////////////////////////////////////////////////////////////////////
 * details
 */


/* ////////////////////////////////////////////////////////////////////////
 * interfaces
 */

#if 0
tb_float_t tb_sqrt(tf_float_t x)
{
#if 0 // high precison, but slowest
	return sqrt(x);
#elif 0 // better precison, but faster
	union
	{
		int i;
		float f;
	} c1, c2;

	c1.f = x;
	c2.f = x;
	c1.i = 0x1FBCF800 + (c1.i >> 1);
	c2.i = 0x5f3759df - (c2.i >> 1);
	return (0.5f * (c1.f + (x * c2.f)));
#elif 0 // general precison, but fastest
	union
	{
		int i;
		float f;
	} c;

	c.f = x;
	c.i-= 0x3f800000;
	c.i >>= 1;
	c.i += 0x3f800000;
	return c.f;
#endif
}
#endif

#if !TB_MATH_HAS_ROUND
static const double huge = 1.0e300;
double round (double x)
{
	tb_int32_t i0, j0;
	tb_uint32_t i1;

	EXTRACT_WORDS (i0, i1, x);
	j0 = ((i0 >> 20) & 0x7ff) - 0x3ff;
	if (j0 < 20)
	{
		if (j0 < 0)
		{
			if (huge + x > 0.0)
			{
				i0 &= 0x80000000;
				if (j0 == -1)
					i0 |= 0x3ff00000;
				i1 = 0;
			}
		}
		else
		{
			tb_uint32_t i = 0x000fffff >> j0;
			if (((i0 & i) | i1) == 0)
				/* X is integral.  */
				return x;
			if (huge + x > 0.0)
			{
				/* Raise inexact if x != 0.  */
				i0 += 0x00080000 >> j0;
				i0 &= ~i;
				i1 = 0;
			}
		}
	}
	else if (j0 > 51)
	{
		if (j0 == 0x400)
			/* Inf or NaN.  */
			return x + x;
		else
			return x;
	}
	else
	{
		tb_uint32_t i = 0xffffffff >> (j0 - 20);
		if ((i1 & i) == 0)
			/* X is integral.  */
			return x;

		if (huge + x > 0.0)
		{
			/* Raise inexact if x != 0.  */
			tb_uint32_t j = i1 + (1 << (51 - j0));
			if (j < i1)
				i0 += 1;
			i1 = j;
		}
		i1 &= ~i;
	}

	INSERT_WORDS (x, i0, i1);
	return x;
}
#endif
