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
 * \file		bstream.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include "bstream.h"
#include "gstream.h"

/* /////////////////////////////////////////////////////////
 * macros
 */

/* /////////////////////////////////////////////////////////
 * attach 
 */
tb_bstream_t* tb_bstream_attach(tb_bstream_t* bst, tb_byte_t* data, tb_size_t size)
{
	TB_ASSERT(bst && data && size);

	bst->p 	= data;
	bst->b 	= 0;
	bst->n 	= size;
	bst->e 	= data + size;

	return bst;
}

/* /////////////////////////////////////////////////////////
 * load & save 
 */
tb_size_t tb_bstream_load(tb_bstream_t* bst, void* gst)
{
	TB_ASSERT(bst && gst);
	if (!bst || !gst) return 0;

	// sync it first
	tb_bstream_sync(bst);

	// load
	tb_byte_t 		data[TB_GSTREAM_DATA_MAX];
	tb_size_t 		load = 0;
	tb_size_t 		base = (tb_size_t)tplat_clock();
	tb_size_t 		time = (tb_size_t)tplat_clock();
	tb_size_t 		left = tb_gstream_left(gst);

	while(1)
	{
		tb_int_t ret = tb_gstream_read(gst, data, TB_GSTREAM_DATA_MAX);
		//TB_DBG("ret: %d", ret);
		if (ret < 0) break;
		else if (!ret) 
		{
			// > 10s?
			tb_size_t timeout = ((tb_size_t)tplat_clock()) - time;
			if (timeout > 10000) break;
		}
		else
		{
			load += ret;
			if (tb_bstream_set_data(bst, data, ret) != ret) break;
			time = (tb_size_t)tplat_clock();
		}

		// is end?
		if (left && load >= left) break;

	}

	return load;
}
tb_size_t tb_bstream_save(tb_bstream_t* bst, void* gst)
{
	TB_ASSERT(bst && gst);
	if (!bst || !gst) return 0;

	// sync it first
	tb_bstream_sync(bst);

	// load
	tb_byte_t 		data[TB_GSTREAM_DATA_MAX];
	tb_size_t 		save = 0;
	tb_size_t 		base = (tb_size_t)tplat_clock();
	tb_size_t 		time = (tb_size_t)tplat_clock();
	while(1)
	{
		// get data
		tb_int_t size = tb_bstream_get_data(bst, data, TB_GSTREAM_DATA_MAX);
		//TB_DBG("ret: %d", ret);

		// is end?
		if (size)
		{
			// write it
			tb_int_t write = 0;
			while (write < size)
			{
				tb_int_t ret = tb_gstream_write(gst, data + write, size - write);
				if (ret < 0) break;
				else if (!ret)
				{
					// > 10s?
					tb_size_t timeout = ((tb_size_t)tplat_clock()) - time;
					if (timeout > 10000) break;
				}
				else
				{
					write += ret;
					time = (tb_size_t)tplat_clock();
				}
			}

			// update size
			save += write;
			if (write != size) break;
		}
		else break;
	}

	return save;
}
/* /////////////////////////////////////////////////////////
 * modifiors
 */
void tb_bstream_goto(tb_bstream_t* bst, tb_byte_t* data)
{
	TB_ASSERT(bst && data && data <= bst->e);
	bst->b = 0;
	if (data <= bst->e) bst->p = data;
}
void tb_bstream_sync(tb_bstream_t* bst)
{
	if (bst->b) 
	{
		bst->p++;
		bst->b = 0;
	}
}
/* /////////////////////////////////////////////////////////
 * position
 */
tb_byte_t const* tb_bstream_beg(tb_bstream_t* bst)
{
	TB_ASSERT(bst && bst->e);
	return (bst->e - bst->n);
}
tb_byte_t const* tb_bstream_pos(tb_bstream_t* bst)
{
	TB_ASSERT(bst && bst->p <= bst->e);
	tb_bstream_sync(bst);
	return bst->p;
}

tb_byte_t const* tb_bstream_end(tb_bstream_t* bst)
{
	TB_ASSERT(bst && bst->e);
	return bst->e;
}

/* /////////////////////////////////////////////////////////
 * size
 */
tb_size_t tb_bstream_size(tb_bstream_t* bst)
{
	TB_ASSERT(bst);
	return bst->n;
}
tb_size_t tb_bstream_offset(tb_bstream_t* bst)
{
	TB_ASSERT(bst);
	return (((bst->p + bst->n) > bst->e)? (bst->p + bst->n - bst->e) : 0);
}
tb_size_t tb_bstream_left(tb_bstream_t* bst)
{
	TB_ASSERT(bst && bst->p <= bst->e);
	tb_bstream_sync(bst);
	return (bst->e - bst->p);
}
tb_size_t tb_bstream_left_bits(tb_bstream_t* bst)
{
	TB_ASSERT(bst);
	return ((bst->p < bst->e)? (((bst->e - bst->p) << 3) - bst->b) : 0);
}
/* /////////////////////////////////////////////////////////
 * skip
 */
void tb_bstream_skip(tb_bstream_t* bst, tb_size_t size)
{
	TB_ASSERT(bst && bst->p <= bst->e);
	tb_bstream_sync(bst);
	bst->p += size;
}
void tb_bstream_skip_bits(tb_bstream_t* bst, tb_size_t nbits)
{
	TB_ASSERT(bst && bst->p <= bst->e);
	bst->b += nbits;
	while (bst->b > 7) 
	{
		bst->p++;
		bst->b -= 8;
	}
}
tb_char_t const* tb_bstream_skip_string(tb_bstream_t* bst)
{
	return tb_bstream_get_string(bst);
}

/* /////////////////////////////////////////////////////////
 * get
 */
tb_uint8_t tb_bstream_get_u1(tb_bstream_t* bst)
{
	tb_uint8_t val = ((*bst->p) >> (7 - bst->b)) & 1;
	bst->b++;
	if (bst->b >= 8) 
	{
		bst->p++;
		bst->b = 0;
	}
	return val;
}
tb_uint16_t tb_bstream_get_u16_be(tb_bstream_t* bst)
{
	TB_ASSERT(!bst->b);
	// {
	tb_uint16_t val = tb_bits_get_u16_be(bst->p);
	bst->p += 2;
	return val;
	// }
}
tb_sint16_t tb_bstream_get_s16_be(tb_bstream_t* bst)
{
	TB_ASSERT(!bst->b);
	// {
	tb_sint16_t val = tb_bits_get_s16_be(bst->p);
	bst->p += 2;
	return val;
	// }
}
tb_uint16_t tb_bstream_get_u16_le(tb_bstream_t* bst)
{
	TB_ASSERT(!bst->b);
	// {
	tb_uint16_t val = tb_bits_get_u16_le(bst->p);
	bst->p += 2;
	return val;
	// }
}
tb_sint16_t tb_bstream_get_s16_le(tb_bstream_t* bst)
{
	TB_ASSERT(!bst->b);
	// {
	tb_sint16_t val = tb_bits_get_s16_le(bst->p);
	bst->p += 2;
	return val;
	// }
}
tb_uint32_t tb_bstream_get_u24_be(tb_bstream_t* bst)
{
	TB_ASSERT(!bst->b);
	// {
	tb_uint32_t val = tb_bits_get_u24_be(bst->p);
	bst->p += 3;
	return val;
	// }
}
tb_sint32_t tb_bstream_get_s24_be(tb_bstream_t* bst)
{
	TB_ASSERT(!bst->b);
	// {
	tb_sint32_t val = tb_bits_get_s24_be(bst->p);
	bst->p += 3;
	return val;
	// }
}
tb_uint32_t tb_bstream_get_u32_be(tb_bstream_t* bst)
{
	TB_ASSERT(!bst->b);
	// {
	tb_uint32_t val = tb_bits_get_u32_be(bst->p);;
	bst->p += 4;
	return val;
	// }
}
tb_sint32_t tb_bstream_get_s32_be(tb_bstream_t* bst)
{
	TB_ASSERT(!bst->b);
	// {
	tb_sint32_t val = tb_bits_get_s32_be(bst->p);
	bst->p += 4;
	return val;
	// }
}
tb_uint32_t tb_bstream_get_u24_le(tb_bstream_t* bst)
{
	TB_ASSERT(!bst->b);
	// {
	tb_uint32_t val = tb_bits_get_u24_le(bst->p);
	bst->p += 3;
	return val;
	// }
}
tb_sint32_t tb_bstream_get_s24_le(tb_bstream_t* bst)
{
	TB_ASSERT(!bst->b);
	// {
	tb_sint32_t val = tb_bits_get_s24_le(bst->p);
	bst->p += 3;
	return val;
	// }
}
tb_uint32_t tb_bstream_get_u32_le(tb_bstream_t* bst)
{
	TB_ASSERT(!bst->b);
	// {
	tb_uint32_t val = tb_bits_get_u32_le(bst->p);
	bst->p += 4;
	return val;
	// }
}
tb_sint32_t tb_bstream_get_s32_le(tb_bstream_t* bst)
{
	TB_ASSERT(!bst->b);
	// {
	tb_sint32_t val = tb_bits_get_s32_le(bst->p);
	bst->p += 4;
	return val;
	// }
}
tb_float_t tb_bstream_get_float_le(tb_bstream_t* bst)
{
	union 
	{
		tb_uint32_t i;
		float 		f;

	} conv;

	conv.i = tb_bstream_get_u32_le(bst);
	return (tb_float_t)conv.f;
}
tb_float_t tb_bstream_get_float_be(tb_bstream_t* bst)
{
	union 
	{
		tb_uint32_t i;
		float 		f;

	} conv;

	conv.i = tb_bstream_get_u32_be(bst);
	return (tb_float_t)conv.f;
}
tb_float_t tb_bstream_get_float_ne(tb_bstream_t* bst)
{
	union 
	{
		tb_uint32_t i;
		float 		f;

	} conv;

	conv.i = tb_bstream_get_u32_ne(bst);
	return (tb_float_t)conv.f;
}

tb_float_t tb_bstream_get_double_le(tb_bstream_t* bst)
{
	union 
	{
		tb_uint32_t i[2];
		double 		f;

	} conv;

#ifdef TPLAT_FLOAT_BIGENDIAN
	conv.i[0] = tb_bstream_get_u32_le(bst);
	conv.i[1] = tb_bstream_get_u32_le(bst);
#else
	conv.i[1] = tb_bstream_get_u32_le(bst);
	conv.i[0] = tb_bstream_get_u32_le(bst);
#endif

	return (tb_float_t)conv.f;
}
tb_float_t tb_bstream_get_double_be(tb_bstream_t* bst)
{
	union 
	{
		tb_uint32_t i[2];
		double 		f;

	} conv;

#ifdef TPLAT_FLOAT_BIGENDIAN
	conv.i[0] = tb_bstream_get_u32_be(bst);
	conv.i[1] = tb_bstream_get_u32_be(bst);
#else
	conv.i[1] = tb_bstream_get_u32_be(bst);
	conv.i[0] = tb_bstream_get_u32_be(bst);
#endif

	return (tb_float_t)conv.f;
}
tb_float_t tb_bstream_get_double_ne(tb_bstream_t* bst)
{
	union 
	{
		tb_uint32_t i[2];
		double 		f;

	} conv;

#ifdef TPLAT_FLOAT_BIGENDIAN
	conv.i[0] = tb_bstream_get_u32_ne(bst);
	conv.i[1] = tb_bstream_get_u32_ne(bst);
#else
	conv.i[1] = tb_bstream_get_u32_ne(bst);
	conv.i[0] = tb_bstream_get_u32_ne(bst);
#endif

	return (tb_float_t)conv.f;
}

tb_uint32_t tb_bstream_get_ubits(tb_bstream_t* bst, tb_size_t nbits)
{
	if (!nbits || !bst) return 0;

	// {
	tb_uint32_t val = 0;
	tb_uint8_t i = bst->b; 
	tb_uint8_t j = 24;

	bst->b += nbits;
	while (bst->b > 7) 
	{
		val |= *(bst->p++) << (i + j);
		j -= 8;
		bst->b -= 8;
	}
	if (bst->b > 0) val |= *(bst->p) << (i + j);
	val >>= 1;
	if (val & 0x80000000) val &= 0x7fffffff;
	val >>= (31 - nbits);

	return val;
	// }
}
tb_sint32_t tb_bstream_get_sbits(tb_bstream_t* bst, tb_size_t nbits)
{
	if (!nbits || !bst) return 0;

	// {
#if 1
	tb_sint32_t val = 0;
	tb_uint8_t i = bst->b; 
	tb_uint8_t j = 24;

	bst->b += nbits;
	while (bst->b > 7) 
	{
		val |= *(bst->p++) << (i + j);
		j -= 8;
		bst->b -= 8;
	}

	if (bst->b > 0) val |= *(bst->p) << (i + j);
	val >>= (32 - nbits);
	return val;
#else
	tb_sint32_t val = 0;
	val = -tb_bstream_get_u1(bst);
	val = (val << (nbits - 1)) | tb_bstream_get_ubits(bst, nbits - 1);
	return val;
#endif
	// }
}
tb_char_t const* tb_bstream_get_string(tb_bstream_t* bst)
{
	TB_ASSERT(bst && bst->p <= bst->e);
	tb_bstream_sync(bst);

	// { find '\0'
	tb_byte_t const* p = bst->p;
	while (*p && p < bst->e) p++;

	// is string with '\0' ?
	if ((*p)) return TB_NULL;

	// {
	tb_char_t const* s = (tb_char_t const*)bst->p;
	bst->p += p - bst->p + 1;

	return s;
	// }}
}
tb_size_t tb_bstream_get_data(tb_bstream_t* bst, tb_byte_t* data, tb_size_t size)
{
	TB_ASSERT(bst->e >= bst->p);
	tb_bstream_sync(bst);
	
	// {
	tb_int_t get_n = size;
	if (bst->e - bst->p < get_n) get_n = bst->e - bst->p;
	if (get_n)
	{
		memcpy(data, bst->p, get_n);
		bst->p += get_n;
	}
	return (get_n > 0? get_n : 0);
	// }
}
/* /////////////////////////////////////////////////////////
 * set
 */
void tb_bstream_set_u1(tb_bstream_t* bst, tb_uint8_t val)
{
	// set
	*(bst->p) &= ~(0x1 << (7 - bst->b));
	*(bst->p) |= ((val & 0x1) << (7 - bst->b));

	// next
	bst->b++;
	if (bst->b >= 8) 
	{
		bst->p++;
		bst->b = 0;
	}
}
void tb_bstream_set_u16_le(tb_bstream_t* bst, tb_uint16_t val)
{
	TB_ASSERT(!bst->b);
	*(bst->p++) = (val) & 0xff;
	*(bst->p++) = (val >> 8) & 0xff;
}
void tb_bstream_set_s16_le(tb_bstream_t* bst, tb_sint16_t val)
{
	TB_ASSERT(!bst->b);
	*(bst->p++) = (val) & 0xff;
	*(bst->p++) = (val >> 8) & 0xff;
}
void tb_bstream_set_u32_le(tb_bstream_t* bst, tb_uint32_t val)
{
	TB_ASSERT(!bst->b);

	*(bst->p++) = (val) & 0xff;
	*(bst->p++) = (val >> 8) & 0xff;
	*(bst->p++) = (val >> 16) & 0xff;
	*(bst->p++) = (val >> 24) & 0xff;
}
void tb_bstream_set_s32_le(tb_bstream_t* bst, tb_sint32_t val)
{
	TB_ASSERT(!bst->b);
	*(bst->p++) = (val) & 0xff;
	*(bst->p++) = (val >> 8) & 0xff;
	*(bst->p++) = (val >> 16) & 0xff;
	*(bst->p++) = (val >> 24) & 0xff;
}
void tb_bstream_set_u16_be(tb_bstream_t* bst, tb_uint16_t val)
{
	TB_ASSERT(!bst->b);
	*(bst->p++) = (val >> 8) & 0xff;
	*(bst->p++) = (val) & 0xff;
}
void tb_bstream_set_s16_be(tb_bstream_t* bst, tb_sint16_t val)
{
	TB_ASSERT(!bst->b);
	*(bst->p++) = (val >> 8) & 0xff;
	*(bst->p++) = (val) & 0xff;
}
void tb_bstream_set_u32_be(tb_bstream_t* bst, tb_uint32_t val)
{
	TB_ASSERT(!bst->b);

	*(bst->p++) = (val >> 24) & 0xff;
	*(bst->p++) = (val >> 16) & 0xff;
	*(bst->p++) = (val >> 8) & 0xff;
	*(bst->p++) = (val) & 0xff;
}
void tb_bstream_set_s32_be(tb_bstream_t* bst, tb_sint32_t val)
{
	TB_ASSERT(!bst->b);
	*(bst->p++) = (val >> 24) & 0xff;
	*(bst->p++) = (val >> 16) & 0xff;
	*(bst->p++) = (val >> 8) & 0xff;
	*(bst->p++) = (val) & 0xff;
}
void tb_bstream_set_ubits(tb_bstream_t* bst, tb_uint32_t val, tb_size_t nbits)
{
	if (!nbits || !bst) return ;

	val <<= (32 - nbits);
	while (nbits--) 
	{
		*(bst->p) &= ~(0x1 << (7 - bst->b));
		*(bst->p) |= (((val & 0x80000000) >> 31) << (7 - bst->b));

		val <<= 1;
		if (++bst->b > 7) 
		{
			bst->b = 0;
			bst->p++;
		}
	}
}

tb_size_t tb_bstream_set_data(tb_bstream_t* bst, tb_byte_t const* data, tb_size_t size)
{
	TB_ASSERT(bst->e >= bst->p);
	tb_bstream_sync(bst);

	// {
	tb_int_t set_n = size;
	if (bst->e - bst->p < set_n) set_n = bst->e - bst->p;
	if (set_n)
	{
		memcpy(bst->p, data, set_n);
		bst->p += set_n;
	}
	return (set_n > 0? set_n : 0);
	// }
}
/* /////////////////////////////////////////////////////////
 * peek
 */
tb_uint32_t tb_bstream_peek_ubits(tb_bstream_t* bst, tb_size_t nbits)
{
	if (!nbits || !bst) return 0;

	// { save status
	tb_byte_t const* p = bst->p;
	tb_size_t b = bst->b;

	// peek value
	tb_uint32_t val = tb_bstream_get_ubits(bst, nbits);

	// restore status
	bst->p = p;
	bst->b = b;

	return val;
	// }
}
tb_sint32_t tb_bstream_peek_sbits(tb_bstream_t* bst, tb_size_t nbits)
{
	if (!nbits || !bst) return 0;

	// { save status
	tb_byte_t const* p = bst->p;
	tb_size_t b = bst->b;

	// peek value
	tb_sint32_t val = tb_bstream_get_sbits(bst, nbits);

	// restore status
	bst->p = p;
	bst->b = b;

	return val;
	// }
}

