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
 * \file		stdlib.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * implemention
 */

tb_uint32_t tb_s2tou32(tb_char_t const* s)
{
	tb_assert(s);
	if (!s) return 0;

	// skip space
	while (tb_isspace(*s)) s++;

	// has sign?
	tb_int_t sign = 0;
	if (*s == '-') 
	{
		sign = 1;
		s++;
	}

	// skip "0b"
	if (s[0] == '0' && (s[1] == 'b' || s[1] == 'B'))
		s += 2;

	// skip '0'
	while ((*s) == '0') s++;

	// compute number
	tb_uint32_t val = 0;
	while (*s)
	{
		tb_char_t ch = *s;
		if (tb_isdigit2(ch))
			val = (val << 1) + (ch - '0');
		else break;
	
		s++;
	}

	if (sign) val = ~val + 1;;
	return val;
}
tb_uint32_t tb_s8tou32(tb_char_t const* s)
{
	tb_assert(s);
	if (!s) return 0;

	// skip space
	while (tb_isspace(*s)) s++;

	// has sign?
	tb_int_t sign = 0;
	if (*s == '-') 
	{
		sign = 1;
		s++;
	}

	// skip '0'
	while ((*s) == '0') s++;

	// compute number
	tb_uint32_t val = 0;
	while (*s)
	{
		tb_char_t ch = *s;
		if (tb_isdigit8(ch))
			val = (val << 3) + (ch - '0');
		else break;
	
		s++;
	}

	if (sign) val = ~val + 1;;
	return val;
}

tb_uint32_t tb_s10tou32(tb_char_t const* s)
{
	tb_assert(s);
	if (!s) return 0;

	// skip space
	while (tb_isspace(*s)) s++;

	// has sign?
	tb_int_t sign = 0;
	if (*s == '-') 
	{
		sign = 1;
		s++;
	}

	// skip '0'
	while ((*s) == '0') s++;

	// compute number
	tb_uint32_t val = 0;
	while (*s)
	{
		tb_char_t ch = *s;
		if (tb_isdigit10(ch))
			val = val * 10 + (ch - '0');
		else break;
	
		s++;
	}

	if (sign) val = ~val + 1;;
	return val;
}
tb_uint32_t tb_s16tou32(tb_char_t const* s)
{
	tb_assert(s);
	if (!s) return 0;

	// skip space
	while (tb_isspace(*s)) s++;

	// has sign?
	tb_int_t sign = 0;
	if (*s == '-') 
	{
		sign = 1;
		s++;
	}

	// skip "0x"
	if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
		s += 2;

	// skip '0'
	while ((*s) == '0') s++;

	// compute number
	tb_uint32_t val = 0;
	while (*s)
	{
		tb_char_t ch = *s;
		if (tb_isdigit10(ch))
			val = (val << 4) + (ch - '0');
		else if (ch > ('a' - 1) && ch < ('f' + 1))
			val = (val << 4) + (ch - 'a') + 10;
		else if (ch > ('A' - 1) && ch < ('F' + 1))
			val = (val << 4) + (ch - 'A') + 10;
		else break;
	
		s++;
	}

	if (sign) val = ~val + 1;;
	return val;
}
tb_uint32_t tb_stou32(tb_char_t const* s)
{
	tb_assert(s);
	if (!s) return 0;

	// skip space
	tb_char_t const* p = s;
	while (tb_isspace(*p)) p++;

	// has sign?
	if (*p == '-') p++;

	// is hex?
	if (*p++ == '0')
	{
		if (*p == 'x' || *p == 'X')
			return tb_s16tou32(s);
		else if (*p == 'b' || *p == 'B')
			return tb_s2tou32(s);
		else return tb_s8tou32(s);
	}
	else return tb_s10tou32(s);
}
tb_uint32_t tb_sbtou32(tb_char_t const* s, tb_int_t base)
{
	typedef tb_uint32_t (*tb_t)(tb_char_t const*);
	tb_t convs[] =
	{
		TB_NULL
	, 	TB_NULL
	,	tb_s2tou32
	, 	TB_NULL
	, 	TB_NULL
	, 	TB_NULL
	, 	TB_NULL
	, 	TB_NULL
	, 	tb_s8tou32
	, 	TB_NULL
	, 	tb_s10tou32
	, 	TB_NULL
	, 	TB_NULL
	, 	TB_NULL
	, 	TB_NULL
	, 	TB_NULL
	, 	tb_s16tou32
	};
	tb_assert(base < tb_arrayn(convs));
	if (convs[base]) return convs[base](s);
	else return 0;
}
#ifdef TB_CONFIG_TYPE_FLOAT
tb_float_t tb_s2tof(tb_char_t const* s)
{
	tb_assert(s);
	if (!s) return 0.;

	// skip space
	while (tb_isspace(*s)) s++;

	// has sign?
	tb_int_t sign = 0;
	if (*s == '-') 
	{
		sign = 1;
		s++;
	}

	// skip "0b"
	if (s[0] == '0' && (s[1] == 'b' || s[1] == 'B'))
		s += 2;

	// compute float: lhs.rhs
	tb_int_t 	dec = 0;
	tb_uint32_t lhs = 0;
	tb_float_t 	rhs = 0.;
	tb_float_t 	val = 0;
	tb_int_t 	zeros = 0;
	tb_int8_t 	decimals[256];
	tb_int8_t* 	d = decimals;
	tb_int8_t* 	e = decimals + 256;
	while (*s)
	{
		tb_char_t ch = *s;

		// is the part of decimal?
		if (ch == '.')
		{
			if (!dec) 
			{
				dec = 1;
				s++;
				continue ;
			}
			else break;
		}

		// parse integer & decimal
		if (tb_isdigit2(ch))
		{
			// save decimals
			if (dec) 
			{
				if (d < e)
				{
					if (ch != '0')
					{
						// fill '0'
						while (zeros--) *d++ = 0;
						zeros = 0;

						// save decimal
						*d++ = ch - '0';
					}
					else zeros++;
				}
			}
			else lhs = (lhs << 1) + (ch - '0');
		}
		else break;
	
		s++;
	}

	tb_assert(d <= decimals + 256);

	// compute decimal
	while (d-- > decimals) rhs = (rhs + *d) / 2;

	// merge 
	return (sign? ((tb_float_t)lhs + rhs) * -1. : ((tb_float_t)lhs + rhs));
}
tb_float_t tb_s8tof(tb_char_t const* s)
{
	tb_assert(s);
	if (!s) return 0.;

	// skip space
	while (tb_isspace(*s)) s++;

	// has sign?
	tb_int_t sign = 0;
	if (*s == '-') 
	{
		sign = 1;
		s++;
	}

	// skip '0'
	while ((*s) == '0') s++;

	// compute float: lhs.rhs
	tb_int_t 	dec = 0;
	tb_uint32_t lhs = 0;
	tb_float_t 	rhs = 0.;
	tb_float_t 	val = 0;
	tb_int_t 	zeros = 0;
	tb_int8_t 	decimals[256];
	tb_int8_t* 	d = decimals;
	tb_int8_t* 	e = decimals + 256;
	while (*s)
	{
		tb_char_t ch = *s;

		// is the part of decimal?
		if (ch == '.')
		{
			if (!dec) 
			{
				dec = 1;
				s++;
				continue ;
			}
			else break;
		}

		// parse integer & decimal
		if (tb_isdigit8(ch))
		{
			// save decimals
			if (dec) 
			{
				if (d < e)
				{
					if (ch != '0')
					{
						// fill '0'
						while (zeros--) *d++ = 0;
						zeros = 0;

						// save decimal
						*d++ = ch - '0';
					}
					else zeros++;
				}
			}
			else lhs = (lhs << 3) + (ch - '0');
		}
		else break;
	
		s++;
	}

	tb_assert(d <= decimals + 256);

	// compute decimal
	while (d-- > decimals) rhs = (rhs + *d) / 8;

	// merge 
	return (sign? ((tb_float_t)lhs + rhs) * -1. : ((tb_float_t)lhs + rhs));
}
tb_float_t tb_s10tof(tb_char_t const* s)
{
	tb_assert(s);
	if (!s) return 0.;

	// skip space
	while (tb_isspace(*s)) s++;

	// has sign?
	tb_int_t sign = 0;
	if (*s == '-') 
	{
		sign = 1;
		s++;
	}

	// skip '0'
	while ((*s) == '0') s++;

	// compute float: lhs.rhs
	tb_int_t 	dec = 0;
	tb_uint32_t lhs = 0;
	tb_float_t 	rhs = 0.;
	tb_float_t 	val = 0;
	tb_int_t 	zeros = 0;
	tb_int8_t 	decimals[256];
	tb_int8_t* 	d = decimals;
	tb_int8_t* 	e = decimals + 256;
	while (*s)
	{
		tb_char_t ch = *s;

		// is the part of decimal?
		if (ch == '.')
		{
			if (!dec) 
			{
				dec = 1;
				s++;
				continue ;
			}
			else break;
		}

		// parse integer & decimal
		if (tb_isdigit10(ch))
		{
			// save decimals
			if (dec) 
			{
				if (d < e)
				{
					if (ch != '0')
					{
						// fill '0'
						while (zeros--) *d++ = 0;
						zeros = 0;

						// save decimal
						*d++ = ch - '0';
					}
					else zeros++;
				}
			}
			else lhs = lhs * 10 + (ch - '0');
		}
		else break;
	
		s++;
	}

	tb_assert(d <= decimals + 256);

	// compute decimal
	while (d-- > decimals) rhs = (rhs + *d) / 10;

	// merge 
	return (sign? ((tb_float_t)lhs + rhs) * -1. : ((tb_float_t)lhs + rhs));
}
tb_float_t tb_s16tof(tb_char_t const* s)
{
	tb_assert(s);
	if (!s) return 0.;

	// skip space
	while (tb_isspace(*s)) s++;

	// has sign?
	tb_int_t sign = 0;
	if (*s == '-') 
	{
		sign = 1;
		s++;
	}

	// skip "0x"
	if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
		s += 2;

	// compute float: lhs.rhs
	tb_int_t 	dec = 0;
	tb_uint32_t lhs = 0;
	tb_float_t 	rhs = 0.;
	tb_float_t 	val = 0;
	tb_int_t 	zeros = 0;
	tb_int8_t 	decimals[256];
	tb_int8_t* 	d = decimals;
	tb_int8_t* 	e = decimals + 256;
	while (*s)
	{
		tb_char_t ch = *s;

		// is the part of decimal?
		if (ch == '.')
		{
			if (!dec) 
			{
				dec = 1;
				s++;
				continue ;
			}
			else break;
		}

		// parse integer & decimal
		if (tb_isdigit10(ch))
		{
			// save decimals
			if (dec) 
			{
				if (d < e)
				{
					if (ch != '0')
					{
						// fill '0'
						while (zeros--) *d++ = 0;
						zeros = 0;

						// save decimal
						*d++ = ch - '0';
					}
					else zeros++;
				}
			}
			else lhs = (lhs << 4) + (ch - '0');
		}
		else if (ch > ('a' - 1) && ch < ('f' + 1))
		{
			// save decimals
			if (dec) 
			{
				if (d < e)
				{
					if (ch != '0')
					{
						// fill '0'
						while (zeros--) *d++ = 0;
						zeros = 0;

						// save decimal
						*d++ = (ch - 'a') + 10;
					}
					else zeros++;
				}
			}
			else lhs = (lhs << 4) + (ch - 'a') + 10;
		}
		else if (ch > ('A' - 1) && ch < ('F' + 1))
		{
			// save decimals
			if (dec) 
			{
				if (d < e)
				{
					if (ch != '0')
					{
						// fill '0'
						while (zeros--) *d++ = 0;
						zeros = 0;

						// save decimal
						*d++ = (ch - 'A') + 10;
					}
					else zeros++;
				}
			}
			else lhs = (lhs << 4) + (ch - 'A') + 10;
		}
		else break;
	
		s++;
	}

	tb_assert(d <= decimals + 256);

	// compute decimal
	while (d-- > decimals) rhs = (rhs + *d) / 16;

	// merge 
	return (sign? ((tb_float_t)lhs + rhs) * -1. : ((tb_float_t)lhs + rhs));
}
tb_float_t tb_stof(tb_char_t const* s)
{
	tb_assert(s);
	if (!s) return 0;

	// skip space
	tb_char_t const* p = s;
	while (tb_isspace(*p)) p++;

	// has sign?
	if (*p == '-') p++;

	// is hex?
	if (*p++ == '0')
	{
		if (*p == 'x' || *p == 'X')
			return tb_s16tof(s);
		else if (*p == 'b' || *p == 'B')
			return tb_s2tof(s);
		else return tb_s8tof(s);
	}
	else return tb_s10tof(s);
}
tb_float_t tb_sbtof(tb_char_t const* s, tb_int_t base)
{
	typedef tb_float_t (*tb_t)(tb_char_t const*);
	tb_t convs[] =
	{
		TB_NULL
	, 	TB_NULL
	,	tb_s2tof
	, 	TB_NULL
	, 	TB_NULL
	, 	TB_NULL
	, 	TB_NULL
	, 	TB_NULL
	, 	tb_s8tof
	, 	TB_NULL
	, 	tb_s10tof
	, 	TB_NULL
	, 	TB_NULL
	, 	TB_NULL
	, 	TB_NULL
	, 	TB_NULL
	, 	tb_s16tof
	};
	tb_assert(base < tb_arrayn(convs));
	if (convs[base]) return convs[base](s);
	else return 0.;
}
#endif

