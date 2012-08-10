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
 * @file		encoding.c
 * @ingroup 	encoding
 *
 */
/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "encoding.h"

/* ///////////////////////////////////////////////////////////////////////
 * ascii
 */
static tb_size_t tb_encoding_ascii_get_unicode(tb_uint32_t* ch, tb_byte_t const** data, tb_size_t size)
{
	tb_check_return_val(size, 0);
	*ch = *(*data)++;
	return 1;
}

static tb_size_t tb_encoding_ascii_set_unicode(tb_uint32_t ch, tb_byte_t** data, tb_size_t size)
{
	tb_check_return_val(size, 0);
	if (ch >= 0x0 && ch <= 0xff)
		*((*data)++) = ch;

	return 1;
}
static tb_uint32_t tb_encoding_ascii_to_unicode(tb_uint32_t ch)
{
	return ch;
}
static tb_uint32_t tb_encoding_ascii_from_unicode(tb_uint32_t ch)
{
	if (ch <= 0xff) return ch;
	else return 0;
}

/* ///////////////////////////////////////////////////////////////////////
 * gb2312
 */

// the encoder table of gb2312 & unicode
extern tb_uint16_t 	g_encoding_unicode_to_gb2312_table_data[][2];
extern tb_uint16_t 	g_encoding_unicode_to_gb2312_table_size;

extern tb_uint16_t 	g_encoding_gb2312_to_unicode_table_data[];
extern tb_size_t 	g_encoding_gb2312_to_unicode_table_size;

static tb_uint32_t tb_encoding_gb2312_from_unicode(tb_uint32_t ch)
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
static tb_uint32_t tb_encoding_gb2312_to_unicode(tb_uint32_t ch)
{
	// is ascii?
	if (ch <= 0x7f) return ch;

	// is gb2312?
	if (ch >= 0xa1a1 && ch <= 0xf7fe)
		return g_encoding_gb2312_to_unicode_table_data[ch - 0xa1a1];
	else return 0;
}
static tb_size_t tb_encoding_gb2312_get_unicode(tb_uint32_t* ch, tb_byte_t const** data, tb_size_t size)
{
	if (**data <= 0x7f) 
	{
		tb_check_return_val(size, 0);
		*ch = *(*data)++;
		return 1;
	}
	else
	{
		tb_check_return_val(size > 1, 0);
		*ch = tb_encoding_gb2312_to_unicode((tb_uint16_t)((((tb_uint16_t)(*data)[0]) << 8) | (*data)[1]));
		(*data) += 2;
		return 2;
	}
}
static tb_size_t tb_encoding_gb2312_set_unicode(tb_uint32_t ch, tb_byte_t** data, tb_size_t size)
{
	ch = tb_encoding_gb2312_from_unicode(ch);
	if (ch <= 0x7f) 
	{
		tb_check_return_val(size, 0);
		*(*data)++ = ch & 0xff;
		return 1;
	}
	else
	{
		tb_check_return_val(size > 1, 0);
		*(*data)++ = (ch >> 8) & 0xff;
		*(*data)++ = ch & 0xff;
		return 2;
	}
}
/* ///////////////////////////////////////////////////////////////////////
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
static tb_size_t tb_encoding_utf8_get_unicode(tb_uint32_t* ch, tb_byte_t const** data, tb_size_t size)
{
	tb_byte_t const* p = *data;
	tb_byte_t const* q = *data;

	// 0x00000000 - 0x0000007f
	if (!(*p & 0x80))
	{
		tb_check_return_val(size, 0);
		*ch = *p++;
	}
	// 0x00000080 - 0x000007ff
	else if ((*p & 0xe0) == 0xc0)
	{
		tb_check_return_val(size > 1, 0);
		*ch = ((((tb_uint32_t)(p[0] & 0x1f)) << 6) | (p[1] & 0x3f));
		p += 2;
	}
	// 0x00000800 - 0x0000ffff
	else if ((*p & 0xf0) == 0xe0)
	{
		tb_check_return_val(size > 2, 0);
		*ch = ((((tb_uint32_t)(p[0] & 0x0f)) << 12) | (((tb_uint32_t)(p[1] & 0x3f)) << 6) | (p[2] & 0x3f));
		p += 3;
	}
	// 0x00010000 - 0x001fffff
	else if ((*p & 0xf8) == 0xf0)
	{
		tb_check_return_val(size > 3, 0);
		*ch = ((((tb_uint32_t)(p[0] & 0x07)) << 18) | (((tb_uint32_t)(p[1] & 0x3f)) << 12) | (((tb_uint32_t)(p[2] & 0x3f)) << 6) | (p[3] & 0x3f));
		p += 4;
	}
	// 0x00200000 - 0x03ffffff
	else if ((*p & 0xfc) == 0xf8)
	{
		tb_check_return_val(size > 4, 0);
		*ch = ((((tb_uint32_t)(p[0] & 0x03)) << 24) | (((tb_uint32_t)(p[1] & 0x3f)) << 18) | (((tb_uint32_t)(p[2] & 0x3f)) << 12) | (((tb_uint32_t)(p[3] & 0x3f)) << 6) | (p[4] & 0x3f));
		p += 5;
	}
	// 0x04000000 - 0x7fffffff
	else if ((*p & 0xfe) == 0xfc)
	{
		tb_check_return_val(size > 5, 0);
		*ch = ((((tb_uint32_t)(p[0] & 0x01)) << 30) | (((tb_uint32_t)(p[1] & 0x3f)) << 24) | (((tb_uint32_t)(p[2] & 0x3f)) << 18) | (((tb_uint32_t)(p[3] & 0x3f)) << 12) | (((tb_uint32_t)(p[4] & 0x3f)) << 6) | (p[5] & 0x3f));
		p += 6;
	}
	else
	{
		// invalid character
		tb_trace("invalid utf8 character: %x", *p);

		// skip it
		p++;
	}

	*data = p;
	return (p - q);
}
static tb_byte_t* tb_encoding_utf8_set_unicode(tb_uint32_t ch, tb_byte_t** data, tb_size_t size)
{	
	tb_byte_t* p = *data;
	tb_byte_t* q = *data;

	// 0x00000000 - 0x0000007f
	if (ch <= 0x0000007f) 
	{
		tb_check_return_val(size, 0);
		*p++ = ch;
	}
	// 0x00000080 - 0x000007ff
	else if (ch <= 0x000007ff) 
	{
		tb_check_return_val(size > 1, 0);
		// 110xxxxx 10xxxxxx
		//      xxx xxxxxxxx
		*p++ = ((ch >> 6) & 0x1f) | 0xc0;
		*p++ = (ch & 0x3f) | 0x80;
	}
	// 0x00000800 - 0x0000ffff
	else if (ch <= 0x0000ffff) 
	{
		tb_check_return_val(size > 2, 0);
		// 1110xxxx 10xxxxxx 10xxxxxx
		//          xxxxxxxx xxxxxxxx
		*p++ = ((ch >> 12) & 0x0f) | 0xe0;
		*p++ = ((ch >> 6) & 0x3f) | 0x80;
		*p++ = (ch & 0x3f) | 0x80;
	}
	// 0x00010000 - 0x001fffff
	else if (ch <= 0x001fffff) 
	{
		tb_check_return_val(size > 3, 0);
		// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		//             xxxxx xxxxxxxx xxxxxxxx
		*p++ = ((ch >> 18) & 0x07) | 0xf0;
		*p++ = ((ch >> 12) & 0x3f) | 0x80;
		*p++ = ((ch >> 6) & 0x3f) | 0x80;
		*p++ = (ch & 0x3f) | 0x80;
	}
	// 0x00200000 - 0x03ffffff
	else if (ch <= 0x03ffffff) 
	{
		tb_check_return_val(size > 4, 0);
		// 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
		//                xx xxxxxxxx xxxxxxxx xxxxxxxx
		*p++ = ((ch >> 24) & 0x03) | 0xf8;
		*p++ = ((ch >> 18) & 0x3f) | 0x80;
		*p++ = ((ch >> 12) & 0x3f) | 0x80;
		*p++ = ((ch >> 6) & 0x3f) | 0x80;
		*p++ = (ch & 0x3f) | 0x80;
	}
	// 0x04000000 - 0x7fffffff
	else if (ch <= 0x7fffffff) 
	{
		tb_check_return_val(size > 5, 0);
		// 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
		//                    xxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
		*p++ = ((ch >> 30) & 0x01) | 0xfc;
		*p++ = ((ch >> 24) & 0x3f) | 0x80;
		*p++ = ((ch >> 18) & 0x3f) | 0x80;
		*p++ = ((ch >> 12) & 0x3f) | 0x80;
		*p++ = ((ch >> 6) & 0x3f) | 0x80;
		*p++ = (ch & 0x3f) | 0x80;
	}

	*data = p;
	return (p - q);
}
static tb_uint32_t tb_encoding_utf8_from_unicode(tb_uint32_t ch)
{
	if (ch <= 0x7f) return ch;
	else if (ch <= 0x7ff)
	{
		tb_trace("[not_impl]: unicode to utf8 character");
		return 0;
	}

	// this unicode character is too large
	tb_trace("only support 2-bytes utf8 character, the character(%x) is too large", ch);

	return 0;
}
static tb_uint32_t tb_encoding_utf8_to_unicode(tb_uint32_t ch)
{
	// 0x00000000 - 0x0000007f
	if (ch <= 0x7f) return ch; 
	// 0x00000080 - 0x000007ff
	else
	{
		tb_trace("[not_impl]: utf8 to unicode character");
		return 0;
	}
}

/* ///////////////////////////////////////////////////////////////////////
 * unicode
 */
static tb_size_t tb_encoding_unicode_get_unicode(tb_uint32_t* ch, tb_byte_t const** data, tb_size_t size)
{
	tb_check_return_val(size > 3, 0);
	*ch = *((tb_uint32_t*)(*data));
	*data += 4;
	return 4;
}
static tb_byte_t* tb_encoding_unicode_set_unicode(tb_uint32_t ch, tb_byte_t** data, tb_size_t size)
{
	tb_check_return_val(size > 3, 0);
	*((tb_uint32_t*)(*data)) = ch;
	*data += 4;
	return 4;
}
static tb_uint32_t tb_encoding_unicode_from_unicode(tb_uint32_t ch)
{
	return ch;
}
static tb_uint32_t tb_encoding_unicode_to_unicode(tb_uint32_t ch)
{
	return ch;
}
/* ///////////////////////////////////////////////////////////////////////
 * globals
 */

// the encoding encoders
static tb_encoder_t g_encoders[] =
{
	{TB_ENCODING_ASCII, 	0x00000000, 		0x000000ff, tb_encoding_ascii_get_unicode, 		tb_encoding_ascii_set_unicode, 		tb_encoding_ascii_from_unicode, 	tb_encoding_ascii_to_unicode}
,	{TB_ENCODING_GB2312, 	0x00003000, 		0x00009f44, tb_encoding_gb2312_get_unicode, 	tb_encoding_gb2312_set_unicode, 	tb_encoding_gb2312_from_unicode, 	tb_encoding_gb2312_to_unicode}
,	{TB_ENCODING_GBK, 		0x00000000/*0x8040*/, 0x00000000/*0xfefe*/, TB_NULL, 				TB_NULL, 							TB_NULL, 							TB_NULL}
,	{TB_ENCODING_UTF8, 		0x00000000, 		0x0000ffff,	tb_encoding_utf8_get_unicode, 		tb_encoding_utf8_set_unicode,		tb_encoding_utf8_from_unicode, 		tb_encoding_utf8_to_unicode}
,	{TB_ENCODING_UNICODE, 	0x00000000, 		0x0000ffff, tb_encoding_unicode_get_unicode, 	tb_encoding_unicode_set_unicode, 	tb_encoding_unicode_from_unicode, 	tb_encoding_unicode_to_unicode}
};

/* ///////////////////////////////////////////////////////////////////////
 * interfaces 
 */
tb_encoder_t const* tb_encoding_get_encoder(tb_size_t encoding)
{
	tb_uint8_t idx = (tb_uint8_t)encoding;
	if (idx < tb_arrayn(g_encoders))
	{
		tb_encoder_t const* encoder = &g_encoders[idx];
		tb_assert(encoder->encoding == idx);
		return encoder;
	}
	return TB_NULL;
}
tb_size_t tb_encoding_convert_string(tb_size_t src_e, tb_size_t dst_e, tb_byte_t const* src_s, tb_size_t src_n, tb_byte_t* dst_s, tb_size_t dst_n)
{
	// get the encoding encoders
	tb_encoder_t const* src_c = tb_encoding_get_encoder(src_e);
	tb_encoder_t const* dst_c = tb_encoding_get_encoder(dst_e);

	// check encoders
	tb_assert(src_c && dst_c && src_c->get && dst_c->set);
	if (!src_c || !dst_c || !src_c->get || !dst_c->set) return 0;

	// check string
	tb_assert(src_s && dst_s);
	if (!src_s || !dst_s) return 0;

	// { get string
	tb_byte_t const* sb = src_s;
	tb_byte_t const* se = sb + src_n;

	tb_byte_t* db = dst_s;
	tb_byte_t* de = db + dst_n;

	// convert
	tb_uint32_t ch;
	while (sb < se && db < de) 
	{
		if (!src_c->get(&ch, &sb, se - sb)) break;
		if (!dst_c->set(ch, &db, de - db)) break;
	}

	return (tb_size_t)(db - dst_s);
	// }
}

