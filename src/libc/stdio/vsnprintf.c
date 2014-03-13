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
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * @author		ruki
 * @file		vsnprintf.c
 * @ingroup 	libc
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "stdio.h"
#include "../../math/math.h"
#include "../../libm/libm.h"
#include "../../utils/utils.h"
#include "../string/string.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// printf format type
typedef enum __tb_printf_type_t
{
	TB_PRINTF_TYPE_NONE
, 	TB_PRINTF_TYPE_INT
, 	TB_PRINTF_TYPE_CHAR
, 	TB_PRINTF_TYPE_CHAR_PERCENT
, 	TB_PRINTF_TYPE_FLOAT
, 	TB_PRINTF_TYPE_DOUBLE
, 	TB_PRINTF_TYPE_STRING
, 	TB_PRINTF_TYPE_WIDTH
, 	TB_PRINTF_TYPE_PRECISION
, 	TB_PRINTF_TYPE_INVALID

}tb_printf_type_t;

// printf format extra info
typedef enum __tb_printf_extra_t
{
	TB_PRINTF_EXTRA_NONE 	= 0
, 	TB_PRINTF_EXTRA_SIGNED 	= 1 	// signed integer for %d %i
, 	TB_PRINTF_EXTRA_UPPER 	= 2 	// upper case for %X %B
, 	TB_PRINTF_EXTRA_PERCENT = 4 	// percent char: %
, 	TB_PRINTF_EXTRA_EXP 	= 8 	// exponent form: [-]d.ddd e[+/-]ddd

}tb_printf_extra_t;

// printf length qualifier
typedef enum __tb_printf_qual_t
{
	TB_PRINTF_QUAL_NONE
, 	TB_PRINTF_QUAL_H
, 	TB_PRINTF_QUAL_L
, 	TB_PRINTF_QUAL_LL
, 	TB_PRINTF_QUAL_I8
, 	TB_PRINTF_QUAL_I16
, 	TB_PRINTF_QUAL_I32
, 	TB_PRINTF_QUAL_I64

}tb_printf_qual_t;

// printf flag type
typedef enum __tb_printf_flag_t
{
	TB_PRINTF_FLAG_NONE		= 0
, 	TB_PRINTF_FLAG_PLUS 	= 1 	// +: denote the sign '+' or '-' of a number
, 	TB_PRINTF_FLAG_LEFT 	= 2 	// -: left-justified
, 	TB_PRINTF_FLAG_ZERO 	= 4 	// 0: fill 0 instead of spaces
, 	TB_PRINTF_FLAG_PFIX 	= 8 	// #: add prefix 

}tb_printf_flag_t;

// printf entry
typedef struct __tb_printf_entry_t
{
	// format type
	tb_printf_type_t 	type;

	// extra info 
	tb_printf_extra_t 	extra;

	// flag
	tb_printf_flag_t 	flags;

	// field width
	tb_int_t 			width;

	// precision
	tb_int_t 			precision;

	// qualifier
	tb_printf_qual_t 	qual;

	// base: 2 8 10 16 
	tb_int_t 			base;


}tb_printf_entry_t;

/* ///////////////////////////////////////////////////////////////////////
 * details
 */

static tb_int_t tb_skip_atoi(tb_char_t const** s)
{
	tb_int_t i = 0;
	while (tb_isdigit(**s)) 
		i = i * 10 + *((*s)++) - '0';
	return i;
}
static tb_char_t* tb_printf_string(tb_char_t* pb, tb_char_t* pe, tb_printf_entry_t e, tb_char_t const* s)
{
	if (s)
	{
		tb_int_t n = tb_strnlen(s, e.precision);

		// fill space at left side, e.g. "   abcd"
		if (!(e.flags & TB_PRINTF_FLAG_LEFT)) 
		{
			while (n < e.width--) 
				if (pb < pe) *pb++ = ' ';
		}

		// copy string
		tb_size_t i = 0;
		for (i = 0; i < n; ++i)
			if (pb < pe) *pb++ = *s++;

		// fill space at right side, e.g. "abcd    "
		while (n < e.width--) 
			if (pb < pe) *pb++ = ' ';
	}
	else 
	{
		if (pb < pe) *pb++ = 'n';
		if (pb < pe) *pb++ = 'u';
		if (pb < pe) *pb++ = 'l';
		if (pb < pe) *pb++ = 'l';
	}

	return pb;
}
static tb_char_t* tb_printf_int64(tb_char_t* pb, tb_char_t* pe, tb_printf_entry_t e, tb_uint64_t num)
{
	// digits table
	static tb_char_t const digits_table[16] = "0123456789ABCDEF";

	// max: 64-bit binary decimal
	tb_char_t 	digits[64] = {0};
	tb_int_t 	digit_i = 0;

	// lowercase mask, e.g. 'F' | 0x20 => 'f'
	tb_int_t lomask = (e.extra & TB_PRINTF_EXTRA_UPPER)? 0x0 : 0x20;

	// sign: + -
	tb_char_t sign = 0;
	if (e.extra & TB_PRINTF_EXTRA_SIGNED)
	{
		if ((tb_int64_t)num < 0) 
		{
			sign = '-';
			--e.width;
	
			num = (tb_uint64_t)(-(tb_int64_t)num); 
		}
		else if (e.flags & TB_PRINTF_FLAG_PLUS)
		{
			sign = '+';
			--e.width;
		}
	}

	// convert num => digits string in reverse order
	if (num == 0) digits[digit_i++] = '0';
	else 
	{
#if 0
		do 
		{
			digits[digit_i++] = digits_table[num % e.base] | lomask;
			num /= e.base;
		}
		while (num);
#else
		if (e.base != 10)
		{
			tb_int_t shift_bits = 4;
			if (e.base == 8) shift_bits--;
			else if (e.base == 2) shift_bits -= 3;
			do 
			{
				digits[digit_i++] = digits_table[(tb_uint8_t)num & (e.base - 1)] | lomask;
				num >>= shift_bits;
			}
			while (num);
		}
		else
		{
			do 
			{
				digits[digit_i++] = digits_table[num % e.base] | lomask;
				num /= e.base;
			}
			while (num);
		}
#endif
	}

	// adjust precision
	if (digit_i > e.precision) 
		e.precision = digit_i;

	// fill spaces at left side, e.g. "   0x0"
	e.width -= e.precision;
	if (!(e.flags & (TB_PRINTF_FLAG_LEFT + TB_PRINTF_FLAG_ZERO)))
	{
		while (--e.width >= 0)
			if (pb < pe) *pb++ = ' ';
	}

	// append sign: + / -
	if (sign && (pb < pe)) *pb++ = sign;

	// append prefix: 0x..., 0X..., 0b..., 0B...
	if (e.flags & TB_PRINTF_FLAG_PFIX)
	{
		switch (e.base)
		{
		case 16:
			{
				if (pb + 1 < pe) 
				{
					*pb++ = '0';
					*pb++ = 'X' | lomask;
					e.width -= 2;
				}
				break;
			}
		case 8:
			{
				if (pb < pe) 
				{
					*pb++ = '0';
					--e.width;
				}
				break;
			}
		case 2:
			{
				if (pb + 1 < pe) 
				{
					*pb++ = '0';
					*pb++ = 'B' | lomask;
					e.width -= 2;
				}
				break;
			}
		default:
			break;
		}
	}

	// fill 0 or spaces, e.g. "0x   ff"
	if (!(e.flags & TB_PRINTF_FLAG_LEFT))
	{
		tb_char_t c = (e.flags & TB_PRINTF_FLAG_ZERO)? '0' : ' ';
		while (--e.width >= 0)
			if (pb < pe) *pb++ = c;
	}

	// fill 0 if precision is larger, e.g. "0x000ff"
	while (digit_i <= --e.precision) 
		if (pb < pe) *pb++ = '0';

	// append digits
	while (--digit_i >= 0) 
		if (pb < pe) *pb++ = digits[digit_i];

	// trailing space padding for left-justified flags, e.g. "0xff   "
	while (--e.width >= 0)
		if (pb < pe) *pb++ = ' ';

	return pb;
}
static tb_char_t* tb_printf_int32(tb_char_t* pb, tb_char_t* pe, tb_printf_entry_t e, tb_uint32_t num)
{
	// digits table
	static tb_char_t const digits_table[16] = "0123456789ABCDEF";

	// max: 64-bit binary decimal
	tb_char_t 	digits[64] = {0};
	tb_int_t 	digit_i = 0;

	// lowercase mask, e.g. 'F' | 0x20 => 'f'
	tb_int_t lomask = (e.extra & TB_PRINTF_EXTRA_UPPER)? 0x0 : 0x20;

	// sign: + -
	tb_char_t sign = 0;
	if (e.extra & TB_PRINTF_EXTRA_SIGNED)
	{
		if ((tb_int32_t)num < 0) 
		{
			sign = '-';
			--e.width;
			num = (tb_uint32_t)(-(tb_int32_t)num); 
		}
		else if (e.flags & TB_PRINTF_FLAG_PLUS)
		{
			sign = '+';
			--e.width;
		}
	}

	// convert num => digits string in reverse order
	if (num == 0) digits[digit_i++] = '0';
	else 
	{
#if 0
		do 
		{
			digits[digit_i++] = digits_table[num % e.base] | lomask;
			num /= e.base;
		}
		while (num);
#else
		if (e.base != 10)
		{
			tb_int_t shift_bits = 4;
			if (e.base == 8) shift_bits--;
			else if (e.base == 2) shift_bits -= 3;
			do 
			{
				digits[digit_i++] = digits_table[(tb_uint8_t)num & (e.base - 1)] | lomask;
				num >>= shift_bits;
			}
			while (num);
		}
		else
		{
			do 
			{
				digits[digit_i++] = digits_table[num % e.base] | lomask;
				num /= e.base;
			}
			while (num);
		}
#endif
	}

	// adjust precision
	if (digit_i > e.precision) 
		e.precision = digit_i;

	// fill spaces at left side, e.g. "   0x0"
	e.width -= e.precision;
	if (!(e.flags & (TB_PRINTF_FLAG_LEFT + TB_PRINTF_FLAG_ZERO)))
	{
		while (--e.width >= 0)
			if (pb < pe) *pb++ = ' ';
	}

	// append sign: + / -
	if (sign && (pb < pe)) *pb++ = sign;

	// append prefix: 0x..., 0X..., 0b..., 0B...
	if (e.flags & TB_PRINTF_FLAG_PFIX)
	{
		switch (e.base)
		{
		case 16:
			{
				if (pb + 1 < pe) 
				{
					*pb++ = '0';
					*pb++ = 'X' | lomask;
					e.width -= 2;
				}
				break;
			}
		case 8:
			{
				if (pb < pe) 
				{
					*pb++ = '0';
					--e.width;
				}
				break;
			}
		case 2:
			{
				if (pb + 1 < pe) 
				{
					*pb++ = '0';
					*pb++ = 'B' | lomask;
					e.width -= 2;
				}
				break;
			}
		default:
			break;
		}
	}

	// fill 0 or spaces, e.g. "0x   ff"
	if (!(e.flags & TB_PRINTF_FLAG_LEFT))
	{
		tb_char_t c = (e.flags & TB_PRINTF_FLAG_ZERO)? '0' : ' ';
		while (--e.width >= 0)
			if (pb < pe) *pb++ = c;
	}

	// fill 0 if precision is larger, e.g. "0x000ff"
	while (digit_i <= --e.precision) 
		if (pb < pe) *pb++ = '0';

	// append digits
	while (--digit_i >= 0) 
		if (pb < pe) *pb++ = digits[digit_i];

	// trailing space padding for left-justified flags, e.g. "0xff   "
	while (--e.width >= 0)
		if (pb < pe) *pb++ = ' ';

	return pb;
}
#ifdef TB_CONFIG_TYPE_FLOAT
static tb_char_t* tb_printf_float(tb_char_t* pb, tb_char_t* pe, tb_printf_entry_t e, tb_float_t num)
{
	// digits
	tb_char_t 	ints[32] = {0};
	tb_char_t 	decs[32] = {0};
	tb_long_t 	ints_i = 0, decs_i = 0;

	// for inf nan
	if (tb_isinf(num))
	{
		if (pb < pe && num < 0) *pb++ = '-';
		if (pb < pe) *pb++ = 'i';
		if (pb < pe) *pb++ = 'n';
		if (pb < pe) *pb++ = 'f';
		return pb;
	}
	else if (tb_isnanf(num))
	{
		if (pb < pe) *pb++ = 'n';
		if (pb < pe) *pb++ = 'a';
		if (pb < pe) *pb++ = 'n';
		return pb;
	}

	// sign: + -
	tb_char_t sign = 0;
	if (e.extra & TB_PRINTF_EXTRA_SIGNED)
	{
		if (num < 0) 
		{
			sign = '-';
			--e.width;
		}
		else if (e.flags & TB_PRINTF_FLAG_PLUS)
		{
			sign = '+';
			--e.width;
		}
	}

	// adjust sign
	if (num < 0) num = -num;

	// default precision: 6
	if (e.precision <= 0) e.precision = 6;

	// round? i.dddddddd5 => i.ddddddde
	tb_uint32_t p = 1;
	tb_size_t 	n = e.precision;
	while (n--) p *= 10;
	if (((tb_uint32_t)(num * p * 10) % 10) > 4) 
		num += 1. / (tb_float_t)p;

	// get integer & decimal
	tb_int32_t integer = (tb_int32_t)num;
	tb_float_t decimal = num - integer;

	// convert integer => digits string in reverse order
	if (integer == 0) ints[ints_i++] = '0';
	else 
	{
		if (integer < 0) integer = -integer; 
		do 
		{
			ints[ints_i++] = (integer % 10) + '0';
			integer /= 10;
		}
		while (integer);
	}

	// convert decimal => digits string in positive order
	if (decimal == 0) decs[decs_i++] = '0';
	else 
	{
		tb_long_t d = (tb_long_t)(decimal * 10);
		do 
		{
			decs[decs_i++] = d + '0';
			decimal = decimal * 10 - d;
			d = (tb_long_t)(decimal * 10);
		}
		while (decs_i < e.precision);
	}

	// adjust precision
	if (decs_i > e.precision) 
		decs_i = e.precision;

	// fill spaces at left side, e.g. "   0.31415926"
	e.width -= ints_i + 1 + e.precision;
	if (!(e.flags & (TB_PRINTF_FLAG_LEFT + TB_PRINTF_FLAG_ZERO)))
	{
		while (--e.width >= 0)
			if (pb < pe) *pb++ = ' ';
	}

	// append sign: + / -
	if (sign && (pb < pe)) *pb++ = sign;

	// fill 0 or spaces, e.g. "00003.1415926"
	if (!(e.flags & TB_PRINTF_FLAG_LEFT))
	{
		tb_char_t c = (e.flags & TB_PRINTF_FLAG_ZERO)? '0' : ' ';
		while (--e.width >= 0)
			if (pb < pe) *pb++ = c;
	}

	// append integer
	while (--ints_i >= 0) 
		if (pb < pe) *pb++ = ints[ints_i];

	// append .
	if (pb < pe) *pb++ = '.';

	// append decimal
	tb_int_t decs_n = decs_i;
	while (--decs_i >= 0) 
		if (pb < pe) *pb++ = decs[decs_n - decs_i - 1];

	// fill 0 if precision is larger, e.g. "0.3140000"
	while (decs_n <= --e.precision) 
		if (pb < pe) *pb++ = '0';

	// trailing space padding for left-justified flags, e.g. "0.31415926   "
	while (--e.width >= 0)
		if (pb < pe) *pb++ = ' ';

	return pb;
}
static tb_char_t* tb_printf_double(tb_char_t* pb, tb_char_t* pe, tb_printf_entry_t e, tb_double_t num)
{
	// digits
	tb_char_t 	ints[64] = {0};
	tb_char_t 	decs[64] = {0};
	tb_long_t 	ints_i = 0, decs_i = 0;

	// for inf nan
	if (tb_isin(num))
	{
		if (pb < pe && num < 0) *pb++ = '-';
		if (pb < pe) *pb++ = 'i';
		if (pb < pe) *pb++ = 'n';
		if (pb < pe) *pb++ = 'f';
		return pb;
	}
	else if (tb_isnan(num))
	{
		if (pb < pe) *pb++ = 'n';
		if (pb < pe) *pb++ = 'a';
		if (pb < pe) *pb++ = 'n';
		return pb;
	}


	// sign: + -
	tb_char_t sign = 0;
	if (e.extra & TB_PRINTF_EXTRA_SIGNED)
	{
		if (num < 0) 
		{
			sign = '-';
			--e.width;
		}
		else if (e.flags & TB_PRINTF_FLAG_PLUS)
		{
			sign = '+';
			--e.width;
		}
	}

	// adjust sign
	if (num < 0) num = -num;

	// default precision: 6
	if (e.precision <= 0) e.precision = 6;

	// round? i.dddddddd5 => i.ddddddde
	tb_uint64_t p = 1;
	tb_size_t 	n = e.precision;
	while (n--) p *= 10;
	if (((tb_uint64_t)(num * p * 10) % 10) > 4) 
		num += 1. / (tb_double_t)p;

	// get integer & decimal
	tb_int64_t integer = (tb_int64_t)num;
	tb_double_t decimal = num - integer;

	// convert integer => digits string in reverse order
	if (integer == 0) ints[ints_i++] = '0';
	else 
	{
		if (integer < 0) integer = -integer; 
		do 
		{
			ints[ints_i++] = (integer % 10) + '0';
			integer /= 10;
		}
		while (integer);
	}

	// convert decimal => digits string in positive order
	if (decimal == 0) decs[decs_i++] = '0';
	else 
	{
		tb_long_t d = (tb_long_t)(decimal * 10);
		do 
		{
			decs[decs_i++] = d + '0';
			decimal = decimal * 10 - d;
			d = (tb_long_t)(decimal * 10);
		}
		while (decs_i < e.precision);
	}

	// adjust precision
	if (decs_i > e.precision) 
		decs_i = e.precision;

	// fill spaces at left side, e.g. "   0.31415926"
	e.width -= ints_i + 1 + e.precision;
	if (!(e.flags & (TB_PRINTF_FLAG_LEFT + TB_PRINTF_FLAG_ZERO)))
	{
		while (--e.width >= 0)
			if (pb < pe) *pb++ = ' ';
	}

	// append sign: + / -
	if (sign && (pb < pe)) *pb++ = sign;

	// fill 0 or spaces, e.g. "00003.1415926"
	if (!(e.flags & TB_PRINTF_FLAG_LEFT))
	{
		tb_char_t c = (e.flags & TB_PRINTF_FLAG_ZERO)? '0' : ' ';
		while (--e.width >= 0)
			if (pb < pe) *pb++ = c;
	}

	// append integer
	while (--ints_i >= 0) 
		if (pb < pe) *pb++ = ints[ints_i];

	// append .
	if (pb < pe) *pb++ = '.';

	// append decimal
	tb_int_t decs_n = decs_i;
	while (--decs_i >= 0) 
		if (pb < pe) *pb++ = decs[decs_n - decs_i - 1];

	// fill 0 if precision is larger, e.g. "0.3140000"
	while (decs_n <= --e.precision) 
		if (pb < pe) *pb++ = '0';

	// trailing space padding for left-justified flags, e.g. "0.31415926   "
	while (--e.width >= 0)
		if (pb < pe) *pb++ = ' ';

	return pb;
}
#endif
// get a printf format entry
static tb_int_t tb_printf_entry(tb_char_t const* fmt, tb_printf_entry_t* e)
{
	tb_char_t const* p = fmt;

	// get field width for *
	if (e->type == TB_PRINTF_TYPE_WIDTH)
	{
		if (e->width < 0) 
		{
			e->width = -e->width;
			e->flags |= TB_PRINTF_FLAG_LEFT;
		}
		e->type = TB_PRINTF_TYPE_NONE;
		goto get_precision;
	}

	// get precision for *
	if (e->type == TB_PRINTF_TYPE_PRECISION)
	{
		if (e->precision < 0) e->precision = 0;
		e->type = TB_PRINTF_TYPE_NONE;
		goto get_qualifier;
	}

	// default type
	e->type = TB_PRINTF_TYPE_NONE;

	// goto %
	for (; *p; ++p) 
		if (*p == '%') break;

	// return non-format string
	if (p != fmt || !*p)
		return (p - fmt);

	// skip %
	++p;

	// get flags
	e->flags = TB_PRINTF_FLAG_NONE;
	while (1)
	{
		tb_bool_t is_found = tb_true;
		switch (*p)
		{
		case '+': e->flags |= TB_PRINTF_FLAG_PLUS; break;
		case '-': e->flags |= TB_PRINTF_FLAG_LEFT; break;
		case '0': e->flags |= TB_PRINTF_FLAG_ZERO; break;
		case '#': e->flags |= TB_PRINTF_FLAG_PFIX; break;
		default: is_found = tb_false; break;
		}
		if (is_found == tb_false) break;
		else ++p;
	}

	// get field width
	e->width = -1;
	if (tb_isdigit(*p)) e->width = tb_skip_atoi(&p);
	else if (*p == '*') 
	{
		// it's the next argument
		e->type = TB_PRINTF_TYPE_WIDTH;
		return ++p - fmt;
	}

get_precision:
	// get precision
	e->precision = -1;
	if (*p == '.')
	{
		++p;
		if (tb_isdigit(*p)) 
		{
			e->precision = tb_skip_atoi(&p);
			if (e->precision < 0) e->precision = 0;
		}
		else if (*p == '*') 
		{
			// it's the next argument
			e->type = TB_PRINTF_TYPE_PRECISION;
			return ++p - fmt;
		}
	}

get_qualifier:
	// get length qualifier
	e->qual = TB_PRINTF_QUAL_NONE;
	switch (*p)
	{
		// short & long => int
	case 'h':
		e->qual = TB_PRINTF_QUAL_H;
		++p;
		break;
	case 'l':
		e->qual = TB_PRINTF_QUAL_L;
		++p;
		if (*p == 'l') 
		{
			e->qual = TB_PRINTF_QUAL_LL;
			++p;
		}
		break;
	case 'I':
		{
			++p;
			tb_int_t n = tb_skip_atoi(&p);
			switch (n)
			{
			case 8: e->qual = TB_PRINTF_QUAL_I8; break;
			case 16: e->qual = TB_PRINTF_QUAL_I16; break;
			case 32: e->qual = TB_PRINTF_QUAL_I32; break;
			case 64: e->qual = TB_PRINTF_QUAL_I64; break;
			default: break;
			}
			break;
		}
	default:
		e->qual = TB_PRINTF_QUAL_NONE;
		break;
	}

	// get base & type
	e->base = -1;
	e->type = TB_PRINTF_TYPE_INVALID;
	e->extra = TB_PRINTF_EXTRA_NONE;
	switch (*p)
	{
	case 's':
		e->type = TB_PRINTF_TYPE_STRING;
		return (++p - fmt);
	case '%':
		e->extra |= TB_PRINTF_EXTRA_PERCENT;
	case 'c':
		e->type = TB_PRINTF_TYPE_CHAR;
		return (++p - fmt);
	case 'd':
	case 'i':
		e->extra |= TB_PRINTF_EXTRA_SIGNED;
	case 'u':
		e->base = 10;
		e->type = TB_PRINTF_TYPE_INT;
		break;
	case 'X':
		e->extra |= TB_PRINTF_EXTRA_UPPER;
	case 'x':
		e->base = 16;
		e->type = TB_PRINTF_TYPE_INT;
		break;
	case 'P':
		e->extra |= TB_PRINTF_EXTRA_UPPER;
	case 'p':
		e->base = 16;
		e->type = TB_PRINTF_TYPE_INT;
		e->flags |= TB_PRINTF_FLAG_PFIX;
#if TB_CPU_BITSIZE == 64
		e->qual = TB_PRINTF_QUAL_I64;
#endif
		break;
	case 'o':
		e->base = 8;
		e->type = TB_PRINTF_TYPE_INT;
		break;
	case 'B':
		e->extra |= TB_PRINTF_EXTRA_UPPER;
	case 'b':
		e->base = 2;
		e->type = TB_PRINTF_TYPE_INT;
		break;
#ifdef TB_CONFIG_TYPE_FLOAT
	case 'F':
		e->extra |= TB_PRINTF_EXTRA_UPPER;
	case 'f':
		e->type = TB_PRINTF_TYPE_FLOAT;
		e->extra |= TB_PRINTF_EXTRA_SIGNED;
		break;
	case 'E':
		e->extra |= TB_PRINTF_EXTRA_UPPER;
	case 'e':
		e->type = TB_PRINTF_TYPE_FLOAT;
		e->extra |= TB_PRINTF_EXTRA_SIGNED;
		e->extra |= TB_PRINTF_EXTRA_EXP;
		break;
#endif
	default:
		e->type = TB_PRINTF_TYPE_INVALID;
		return (p - fmt);
	}

	return (++p - fmt);
}

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */

/*! format a string and place it in a buffer
 *
 * @param s 	the buffer to place the result into
 * @param n 	the size of the buffer, including the trailing null space
 * @param fmt 	the format string to use
 * @param args 	arguments for the format string
 *
 * @return 		the number of characters which would be generated for the given input, excluding the trailing '\0'.
 *
 * - format: %[flags][width][.precision][qualifier]type
 *
 * - flags:
 *   - default: right-justified, left-pad the output with spaces until the required length of output is attained. 
 *   			If combined with '0' (see below), 
 *   			it will cause the sign to become a space when positive, 
 *   			but the remaining characters will be zero-padded
 *   - -: 		left-justified, e.g. %-d
 *   - +: 		denote the sign '+' or '-' of a number
 *   - 0: 		use 0 instead of spaces to left-fill a fixed-length field
 *   - #: 		add prefix or suffix	
 *     - %#o => add prefix: 0...
 *     - %#x => add prefix: 0x...
 *     - %#X => add prefix: 0X...
 *     - %#b => add prefix: 0b...
 *     - %#B => add prefix: 0B...
 *     - %#f => add prefix: 0f...
 *     - %#F => add prefix: 0F...
 *
 * - width:
 *   - n: 		n = 1, 2, 3, ..., fill spaces
 *   - 0n: 		n = 1, 2, 3, ..., fill 0
 *   - *: 		causes printf to pad the output until it is n characters wide, 
 *   			where n is an integer value stored in the a function argument just preceding 
 *   			that represented by the modified type. 
 *   			e.g. printf("%*d", 5, 10) will result in "10" being printed with a width of 5.
 *
 * - .precision:
 *   - .n: 		for non-integral numeric types, causes the decimal portion of the output to be expressed in at least number digits. 
 *   			for the string type, causes the output to be truncated at number characters. 
 *   			if the precision is zero, nothing is printed for the corresponding argument.
 *   - *: 		same as the above, but uses an integer value in the intaken argument to 
 *   			determine the number of decimal places or maximum string length. 
 *   			e.g. printf("%.*s", 3, "abcdef") will result in "abc" being printed.
 *
 * - qualifier:
 *   - h: 		short integer or single double-point
 *   - l: 		long integer or double double-point
 *   - I8: 		8-bit integer
 *   - I16: 	16-bit integer
 *   - I32: 	32-bit integer
 *   - I64/ll: 	64-bit integer
 *
 * @note support h, l, I8, I16, I32, I64, ll
 *
 * - type(e.g. %d %x %u %% ...):
 *   - d, i: 	print an int as a signed decimal number. 
 *   			'%d' and '%i' are synonymous for output, but are different when used with scanf() for input.
 *   - u: 		print decimal unsigned int.
 *   - o: 		print an unsigned int in octal.
 *   - x/X: 	print an unsigned int as a hexadecimal number. 'x' uses lower-case letters and 'X' uses upper-case.
 *   - b/B: 	print an unsigned binary interger
 *   - e/E: 	print a double value in standard form ([-]d.ddd e[+/-]ddd).
 *   			An E conversion uses the letter E (rather than e) to introduce the exponent. 
 *  			The exponent always contains at least two digits; if the value is zero, the exponent is 00.
 *  			e.g. 3.141593e+00
 *   - f/F: 	Print a double in normal (fixed-point) notation. 
 *   			'f' and 'F' only differs in how the strings for an infinite number or NaN are printed 
 *  			('inf', 'infinity' and 'nan' for 'f', 'INF', 'INFINITY' and 'NAN' for 'F').
 *   - g/G: 	print a double in either normal or exponential notation, whichever is more appropriate for its magnitude. 
 *   			'g' uses lower-case letters, 'G' uses upper-case letters. 
 *   			This type differs slightly from fixed-point notation in 
 *   			that insignificant zeroes to the right of the decimal point are not included. 
 *   			Also, the decimal point is not included on whole numbers.
 *   - c: 		print a char (character).
 *   - s: 		print a character string
 *   - p: 		print a void * (pointer to void) in an implementation-defined format.
 *   - n: 		print nothing, but write number of characters successfully written so far into an integer pointer parameter.
 *   - %: 		%
 *
 * @note support 		d, i, u, o, u, x/X, b/B, f/F, c, s
 * @note not support 	e/E, g/G, p, n
 *
 * e.g.
 * @code
 * tb_printf("|hello world|\n");
 * tb_printf("|%-10s|%%|%10s|\n", "hello", "world");
 * tb_printf("|%#2c|%2.5c|%*c|\n", 'A', 'B', 5, 'C');
 * tb_printf("|%#2d|%#8.3o|%*.*d|\n", -56, 56, 10, 5, 56);
 * tb_printf("|%#-8.5x|%#2.9X|\n", 0x1f, 0x1f);
 * tb_printf("|%#-8.5b|%#2.9B|\n", 0x1f, 0x1f);
 * tb_printf("|%-6Id|%5I8u|%#I64x|%#llx|\n", 256, 255, (tb_int64_t)0x8fffffffffff, (tb_int64_t)0x8fffffffffff);
 * tb_printf("|%lf|\n", -3.1415926535897932384626433832795);
 * tb_printf("|%lf|%lf|%lf|\n", 3.14, 0, -0);
 * tb_printf("|%0.9f|\n", 3.1415926535897932384626433832795);
 * tb_printf("|%16.9f|\n", 3.1415926535897932384626433832795);
 * tb_printf("|%016.9f|\n", 3.14159);
 * tb_printf("|%lf|\n", 1.0 / 6.0);
 * tb_printf("|%lf|\n", 0.0003141596);
 * @endcode
 *
 */
tb_long_t tb_vsnprintf(tb_char_t* s, tb_size_t n, tb_char_t const* fmt, tb_va_list_t args)
{
	if (!n || !s || !fmt) return 0;

	tb_char_t* pb = s;
	tb_char_t* pe = s + n;

#if 0
	// pe must be larger than pb
	if (pe < pb) 
	{
		pe = ((tb_char_t*)-1);
		n = (tb_size_t)(pe - pb);
	}
#endif

	// parse format
	tb_printf_entry_t e = {0};
	tb_int_t en = 0;
	while (*fmt)
	{
		tb_char_t const* ofmt = fmt;

		// get an entry
		en = tb_printf_entry(fmt, &e);
		fmt += en;

		switch (e.type)
		{
			// copy it if none type
		case TB_PRINTF_TYPE_NONE:
			{
				tb_int_t copy_n = en;
				if (pb < pe) 
				{
					if (copy_n > pe - pb) copy_n = pe - pb;
					tb_memcpy(pb, ofmt, copy_n);
					pb += copy_n;
				}
				break;
			}
			// get a character for %c
		case TB_PRINTF_TYPE_CHAR:
			{
				// char: %
				if (e.extra & TB_PRINTF_EXTRA_PERCENT)
				{
					if (pb < pe) *pb++ = '%';
				}
				// char: %c
				else
				{
					// fill space at left side, e.g. "   a"
					if (!(e.flags & TB_PRINTF_FLAG_LEFT)) 
					{
						while (--e.width > 0) 
						{
							if (pb < pe) *pb++ = ' ';
						}
					}

					if (pb < pe) *pb++ = (tb_char_t)tb_va_arg(args, tb_int_t);

					// fill space at right side, e.g. "a   "
					while (--e.width > 0) 
					{
						if (pb < pe) *pb++ = ' ';
					}
				}
				break;
			}
			// get field width for *
		case TB_PRINTF_TYPE_WIDTH:
			e.width = tb_va_arg(args, tb_int_t);
			break;
			// get precision for *
		case TB_PRINTF_TYPE_PRECISION:
			e.precision = tb_va_arg(args, tb_int_t);
			break;
			// get string for %s
		case TB_PRINTF_TYPE_STRING:
			{
				pb = tb_printf_string(pb, pe, e, tb_va_arg(args, tb_char_t const*));
				break;
			}
			// get an integer for %d %u %x ...
		case TB_PRINTF_TYPE_INT:
			{
				if ( 	e.qual == TB_PRINTF_QUAL_I64
#if TB_CPU_BIT64
					|| 	e.qual == TB_PRINTF_QUAL_L
#endif
					|| 	e.qual == TB_PRINTF_QUAL_LL)
					pb = tb_printf_int64(pb, pe, e, tb_va_arg(args, tb_uint64_t));
				else
				{
					tb_uint32_t num = 0;
					if (e.extra & TB_PRINTF_EXTRA_SIGNED)
					{
						switch (e.qual)
						{
						case TB_PRINTF_QUAL_I8:		num = (tb_int8_t)tb_va_arg(args, tb_int_t); break;
						case TB_PRINTF_QUAL_I16:	num = (tb_int16_t)tb_va_arg(args, tb_int_t); break;
						case TB_PRINTF_QUAL_I32:	num = tb_va_arg(args, tb_int32_t); break;
						default: 					num = tb_va_arg(args, tb_int_t); break;
						}
					}
					else
					{
						switch (e.qual)
						{
						case TB_PRINTF_QUAL_I8:		num = (tb_uint8_t)tb_va_arg(args, tb_uint_t); break;
						case TB_PRINTF_QUAL_I16:	num = (tb_uint16_t)tb_va_arg(args, tb_uint_t); break;
						case TB_PRINTF_QUAL_I32:	num = tb_va_arg(args, tb_uint32_t); break;
						default: 					num = tb_va_arg(args, tb_uint_t); break;
						}
					}
					pb = tb_printf_int32(pb, pe, e, num);
				}
				break;
			}
#ifdef TB_CONFIG_TYPE_FLOAT
		case TB_PRINTF_TYPE_FLOAT:
			{
				if (e.qual == TB_PRINTF_QUAL_L)
				{
					tb_double_t num = tb_va_arg(args, tb_double_t);
					pb = tb_printf_double(pb, pe, e, num);
				}
				else 
				{
					tb_float_t num = tb_va_arg(args, tb_double_t);
					pb = tb_printf_float(pb, pe, e, num);
				}
				break;
			}
#endif
		case TB_PRINTF_TYPE_INVALID:
			{
				if (pb < pe) *pb++ = '%';
				break;
			}
		default:
			break;
		}
	}

	// the trailing null byte doesn't count towards the total
	return (pb - s);
}

