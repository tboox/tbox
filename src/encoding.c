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
 * \file		encoding.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include "encoding.h"

/* /////////////////////////////////////////////////////////
 * ascii
 */
static tb_uint16_t tb_encoding_ascii_get_unicode(tb_char_t const** pc)
{
	return (tb_uint16_t)(*(*pc)++);
}

static void tb_encoding_ascii_set_unicode(tb_uint16_t ch, tb_char_t** pc)
{
	if (ch >= 0x0 && ch <= 0xff)
		*((*pc)++) = ch;
}
static tb_uint16_t tb_encoding_ascii_to_unicode(tb_uint16_t ch)
{
	return ch;
}
static tb_uint16_t tb_encoding_ascii_from_unicode(tb_uint16_t ch)
{
	if (ch <= 0xff) return ch;
	else return 0;
}

/* /////////////////////////////////////////////////////////
 * gb2312
 */

// the converter table of gb2312 & unicode
extern tb_uint16_t 	g_encoding_unicode_to_gb2312_table_data[][2];
extern tb_size_t 	g_encoding_unicode_to_gb2312_table_size;

extern tb_uint16_t 	g_encoding_gb2312_to_unicode_table_data[];
extern tb_size_t 	g_encoding_gb2312_to_unicode_table_size;

static tb_uint16_t tb_encoding_gb2312_from_unicode(tb_uint16_t ch)
{
	// is ascii?
	if (ch <= 0x7f) return ch;

	// find the gb2312 character
	tb_int_t left = 0;
	tb_int_t right = (g_encoding_unicode_to_gb2312_table_size / sizeof(g_encoding_unicode_to_gb2312_table_data[0])) - 1;
	while (left <= right)
	{
		// the middle character
		tb_int_t 	mid = (left + right) >> 1;
		tb_uint16_t mid_unicode = g_encoding_unicode_to_gb2312_table_data[mid][0];

		// find it?
		if (mid_unicode == ch)
			return g_encoding_unicode_to_gb2312_table_data[mid][1];

		if (ch > mid_unicode) left = mid + 1;
		else right = mid - 1;
	}

	return 0;
}
static tb_uint16_t tb_encoding_gb2312_to_unicode(tb_uint16_t ch)
{
	if (ch >= 0xa1a1 && ch <= 0xf7fe)
		return g_encoding_gb2312_to_unicode_table_data[ch - 0xa1a1];
	else return 0;
}
static tb_uint16_t tb_encoding_gb2312_get_unicode(tb_char_t const** pc)
{
	tb_uint16_t ch = *((tb_uint16_t*)(*pc));
	if (ch <= 0x7f) *pc++;
	else *pc += 2;
	return tb_encoding_gb2312_to_unicode(ch);
}
static void tb_encoding_gb2312_set_unicode(tb_uint16_t ch, tb_char_t** pc)
{
	ch = tb_encoding_gb2312_from_unicode(ch);
	if (ch <= 0x7f) *(*pc)++ = ch & 0xff;
	else
	{
		*(*pc)++ = (ch >> 8) & 0xff;
		*(*pc)++ = ch & 0xff;
	}
}
/* /////////////////////////////////////////////////////////
 * utf8
 */

/*
 * 0x00000000 - 0x0000007f:  0xxxxxxx
 * 0x00000080 - 0x000007ff:  110xxxxx 10xxxxxx
 * 0x00000800 - 0x0000ffff:  1110xxxx 10xxxxxx 10xxxxxx
 * 0x00010000 - 0x001fffff:  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
 * 0x00200000 - 0x03ffffff:  111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
 * 0x04000000 - 0x7fffffff:  1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
 *
 * [fixme]: for bigendian
 */
static tb_uint16_t tb_encoding_utf8_get_unicode(tb_char_t const** pc)
{
	tb_byte_t const* p = *pc;

	// 0x00000000 - 0x0000007f
	if (!(*p & 0x80))
	{
		(*pc)++;
		return *p;
	}
	// 0x00000080 - 0x000007ff
	else if ((*p & 0xe0) == 0xc0)
	{
		(*pc) += 2;
		return ((((tb_uint16_t)(p[0] & 0x1f)) << 6) | (p[1] & 0x3f));
	}
	// 0x00000800 - 0x0000ffff
	else if ((*p & 0xf0) == 0xe0)
	{
		(*pc) += 3;
		return ((((tb_uint16_t)(p[0] & 0x0f)) << 12) | (((tb_uint16_t)(p[1] & 0x3f)) << 6) | (p[2] & 0x3f));
	}
	// 0x00010000 - 0x001fffff
	else if ((*p & 0xf8) == 0xf0)
	{
		(*pc) += 4;
		return ((((tb_uint16_t)(p[0] & 0x07)) << 18) | (((tb_uint16_t)(p[1] & 0x3f)) << 12) | (((tb_uint16_t)(p[2] & 0x3f)) << 6) | (p[3] & 0x3f));
	}
	// 0x00200000 - 0x03ffffff
	else if ((*p & 0xfc) == 0xf8)
	{
		(*pc) += 5;
		return ((((tb_uint16_t)(p[0] & 0x03)) << 24) | (((tb_uint16_t)(p[1] & 0x3f)) << 18) | (((tb_uint16_t)(p[2] & 0x3f)) << 12) | (((tb_uint16_t)(p[3] & 0x3f)) << 6) | (p[4] & 0x3f));
	}
	// 0x04000000 - 0x7fffffff
	else if ((*p & 0xfe) == 0xfc)
	{
		(*pc) += 6;
		return ((((tb_uint16_t)(p[0] & 0x01)) << 30) | (((tb_uint16_t)(p[1] & 0x3f)) << 24) | (((tb_uint16_t)(p[2] & 0x3f)) << 18) | (((tb_uint16_t)(p[3] & 0x3f)) << 12) | (((tb_uint16_t)(p[4] & 0x3f)) << 6) | (p[5] & 0x3f));
	}

	// invalid character
	TB_DBG("invalid utf8 character: %x", *p);

	// skip it
	(*pc)++;

	return 0;
}
static void tb_encoding_utf8_set_unicode(tb_uint16_t ch, tb_char_t** pc)
{
	TB_DBG("[not_impl]: unicode to utf8 string");
}
static tb_uint16_t tb_encoding_utf8_from_unicode(tb_uint16_t ch)
{
	if (ch <= 0x7f) return ch;
	else if (ch <= 0x7ff)
	{
		TB_DBG("[not_impl]: unicode to utf8 character");
		return 0;
	}

	// this unicode character is too large
	TB_DBG("only support 2-bytes utf8 character, the character(%x) is too large", ch);

	return 0;
}
static tb_uint16_t tb_encoding_utf8_to_unicode(tb_uint16_t ch)
{
	// 0x00000000 - 0x0000007f
	if (ch <= 0x7f) return ch; 
	// 0x00000080 - 0x000007ff
	else
	{
		TB_DBG("[not_impl]: utf8 to unicode character");
		return 0;
	}
}

/* /////////////////////////////////////////////////////////
 * unicode
 */
static tb_uint16_t tb_encoding_unicode_get_unicode(tb_char_t const** pc)
{
	tb_uint16_t ch = *((tb_uint16_t*)(*pc));
	*pc += 2;
	return ch;
}
static void tb_encoding_unicode_set_unicode(tb_uint16_t ch, tb_char_t** pc)
{
	*((tb_uint16_t*)(*pc)) = ch;
	*pc += 2;
}
static tb_uint16_t tb_encoding_unicode_from_unicode(tb_uint16_t ch)
{
	return ch;
}
static tb_uint16_t tb_encoding_unicode_to_unicode(tb_uint16_t ch)
{
	return ch;
}
/* /////////////////////////////////////////////////////////
 * globals
 */

// the encoding converters
static tb_encoding_converter_t g_encoding_converters[] =
{
	{TB_ENCODING_ASCII, 	0, 		0xff, 	tb_encoding_ascii_get_unicode, 		tb_encoding_ascii_set_unicode, 		tb_encoding_ascii_from_unicode, 	tb_encoding_ascii_to_unicode}
,	{TB_ENCODING_GB2312, 	0x3000, 0x9f44, tb_encoding_gb2312_get_unicode, 	tb_encoding_gb2312_set_unicode, 	tb_encoding_gb2312_from_unicode, 	tb_encoding_gb2312_to_unicode}
,	{TB_ENCODING_GBK, 		0/*0x8040*/, 0/*0xfefe*/, TB_NULL, 					TB_NULL, 							TB_NULL, 							TB_NULL}
,	{TB_ENCODING_UTF8, 		0, 		0xffff,	tb_encoding_utf8_get_unicode, 		tb_encoding_utf8_set_unicode,		tb_encoding_utf8_from_unicode, 		tb_encoding_utf8_to_unicode}
,	{TB_ENCODING_UNICODE, 	0, 		0xffff, tb_encoding_unicode_get_unicode, 	tb_encoding_unicode_set_unicode, 	tb_encoding_unicode_from_unicode, 	tb_encoding_unicode_to_unicode}
};

/* /////////////////////////////////////////////////////////
 * interfaces 
 */
tb_encoding_converter_t const* tb_encoding_get_converter(tb_encoding_t encoding)
{
	tb_uint8_t idx = (tb_uint8_t)encoding;
	if (idx < TB_STATIC_ARRAY_SIZE(g_encoding_converters))
	{
		tb_encoding_converter_t const* converter = &g_encoding_converters[idx];
		TB_ASSERT(converter->encoding == idx);
		return converter;
	}
	return TB_NULL;
}
tb_size_t tb_encoding_convert_string(tb_encoding_t src_e, tb_encoding_t dst_e, tb_char_t const* src_s, tb_size_t src_n, tb_char_t* dst_s, tb_size_t dst_n)
{
	// get the encoding converters
	tb_encoding_converter_t const* src_c = tb_encoding_get_converter(src_e);
	tb_encoding_converter_t const* dst_c = tb_encoding_get_converter(dst_e);

	// check converters
	TB_ASSERT(src_c && dst_c && src_c->get && dst_c->set);
	if (!src_c || !dst_c || !src_c->get || !dst_c->set) return 0;

	// check string
	TB_ASSERT(src_s && dst_s);
	if (!src_s || !dst_s) return 0;

	// get string
	tb_char_t* sb = src_s;
	tb_char_t* se = sb + src_n;

	tb_char_t* db = dst_s;
	tb_char_t* de = db + dst_n;

	// convert
	while (sb < se && db < de) dst_c->set(src_c->get(&sb), &db);

	return (tb_size_t)(db - dst_s);
}

