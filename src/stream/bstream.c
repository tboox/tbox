/*!The Tiny Box Library
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
 * \file		bstream.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include "bstream.h"
#include "gstream.h"
#include "../memory/memory.h"

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
tb_size_t tb_bstream_load(tb_bstream_t* bst, tb_gstream_t* ist)
{
	TB_ASSERT_RETURN_VAL(bst && ist, 0);

	// sync it first
	tb_bstream_sync(bst);

	// load
	tb_byte_t 		data[TB_GSTREAM_BLOCK_SIZE];
	tb_size_t 		load = 0;
	tb_size_t 		time = (tb_size_t)tb_mclock();
	tb_size_t 		left = tb_gstream_left(ist);

	while(1)
	{
		tb_int_t ret = tb_gstream_read(ist, data, TB_GSTREAM_BLOCK_SIZE);
		//TB_DBG("ret: %d", ret);
		if (ret < 0) break;
		else if (!ret) 
		{
			// timeout?
			tb_size_t timeout = ((tb_size_t)tb_mclock()) - time;
			if (timeout > TB_GSTREAM_TIMEOUT) break;
		}
		else
		{
			load += ret;
			if (tb_bstream_set_data(bst, data, ret) != ret) break;
			time = (tb_size_t)tb_mclock();
		}

		// is end?
		if (left && load >= left) break;
	}

	return load;
}
tb_size_t tb_bstream_save(tb_bstream_t* bst, tb_gstream_t* ost)
{
	TB_ASSERT_RETURN_VAL(bst && ost, 0);

	// sync it first
	tb_bstream_sync(bst);

	// load
	tb_byte_t 		data[TB_GSTREAM_BLOCK_SIZE];
	tb_size_t 		save = 0;
	tb_size_t 		time = (tb_size_t)tb_mclock();
	while(1)
	{
		// get data
		tb_int_t size = tb_bstream_get_data(bst, data, TB_GSTREAM_BLOCK_SIZE);
		//TB_DBG("ret: %d", ret);

		// is end?
		if (size)
		{
			// write it
			tb_int_t write = 0;
			while (write < size)
			{
				tb_int_t ret = tb_gstream_write(ost, data + write, size - write);
				if (ret < 0) break;
				else if (!ret)
				{
					// timeout?
					tb_size_t timeout = ((tb_size_t)tb_mclock()) - time;
					if (timeout > TB_GSTREAM_TIMEOUT) break;
				}
				else
				{
					write += ret;
					time = (tb_size_t)tb_mclock();
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
	
	tb_uint16_t val = tb_bits_get_u16_be(bst->p);
	bst->p += 2;
	return val;
	
}
tb_sint16_t tb_bstream_get_s16_be(tb_bstream_t* bst)
{
	TB_ASSERT(!bst->b);
	
	tb_sint16_t val = tb_bits_get_s16_be(bst->p);
	bst->p += 2;
	return val;
	
}
tb_uint16_t tb_bstream_get_u16_le(tb_bstream_t* bst)
{
	TB_ASSERT(!bst->b);
	
	tb_uint16_t val = tb_bits_get_u16_le(bst->p);
	bst->p += 2;
	return val;
	
}
tb_sint16_t tb_bstream_get_s16_le(tb_bstream_t* bst)
{
	TB_ASSERT(!bst->b);
	
	tb_sint16_t val = tb_bits_get_s16_le(bst->p);
	bst->p += 2;
	return val;
	
}
tb_uint32_t tb_bstream_get_u24_be(tb_bstream_t* bst)
{
	TB_ASSERT(!bst->b);
	
	tb_uint32_t val = tb_bits_get_u24_be(bst->p);
	bst->p += 3;
	return val;
	
}
tb_sint32_t tb_bstream_get_s24_be(tb_bstream_t* bst)
{
	TB_ASSERT(!bst->b);
	
	tb_sint32_t val = tb_bits_get_s24_be(bst->p);
	bst->p += 3;
	return val;
	
}
tb_uint32_t tb_bstream_get_u32_be(tb_bstream_t* bst)
{
	TB_ASSERT(!bst->b);
	
	tb_uint32_t val = tb_bits_get_u32_be(bst->p);;
	bst->p += 4;
	return val;
	
}
tb_sint32_t tb_bstream_get_s32_be(tb_bstream_t* bst)
{
	TB_ASSERT(!bst->b);
	
	tb_sint32_t val = tb_bits_get_s32_be(bst->p);
	bst->p += 4;
	return val;
	
}
tb_uint32_t tb_bstream_get_u24_le(tb_bstream_t* bst)
{
	TB_ASSERT(!bst->b);
	
	tb_uint32_t val = tb_bits_get_u24_le(bst->p);
	bst->p += 3;
	return val;
	
}
tb_sint32_t tb_bstream_get_s24_le(tb_bstream_t* bst)
{
	TB_ASSERT(!bst->b);
	
	tb_sint32_t val = tb_bits_get_s24_le(bst->p);
	bst->p += 3;
	return val;
	
}
tb_uint32_t tb_bstream_get_u32_le(tb_bstream_t* bst)
{
	TB_ASSERT(!bst->b);
	
	tb_uint32_t val = tb_bits_get_u32_le(bst->p);
	bst->p += 4;
	return val;
	
}
tb_sint32_t tb_bstream_get_s32_le(tb_bstream_t* bst)
{
	TB_ASSERT(!bst->b);
	
	tb_sint32_t val = tb_bits_get_s32_le(bst->p);
	bst->p += 4;
	return val;
	
}

#ifdef TB_CONFIG_TYPE_FLOAT
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

#ifdef TB_FLOAT_BIGENDIAN
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

#ifdef TB_FLOAT_BIGENDIAN
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

#ifdef TB_FLOAT_BIGENDIAN
	conv.i[0] = tb_bstream_get_u32_ne(bst);
	conv.i[1] = tb_bstream_get_u32_ne(bst);
#else
	conv.i[1] = tb_bstream_get_u32_ne(bst);
	conv.i[0] = tb_bstream_get_u32_ne(bst);
#endif

	return (tb_float_t)conv.f;
}
#endif

tb_uint32_t tb_bstream_get_ubits(tb_bstream_t* bst, tb_size_t nbits)
{
	if (!nbits || !bst) return 0;

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
}
tb_sint32_t tb_bstream_get_sbits(tb_bstream_t* bst, tb_size_t nbits)
{
	if (!nbits || !bst) return 0;

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
}
tb_char_t const* tb_bstream_get_string(tb_bstream_t* bst)
{
	TB_ASSERT(bst && bst->p <= bst->e);
	tb_bstream_sync(bst);

	// find '\0'
	tb_byte_t const* p = bst->p;
	while (*p && p < bst->e) p++;

	// is string with '\0' ?
	if ((*p)) return TB_NULL;

	tb_char_t const* s = (tb_char_t const*)bst->p;
	bst->p += p - bst->p + 1;

	return s;
}
tb_size_t tb_bstream_get_data(tb_bstream_t* bst, tb_byte_t* data, tb_size_t size)
{
	TB_ASSERT(bst->e >= bst->p);
	tb_bstream_sync(bst);
	
	tb_int_t get_n = size;
	if (bst->e - bst->p < get_n) get_n = bst->e - bst->p;
	if (get_n)
	{
		tb_memcpy(data, bst->p, get_n);
		bst->p += get_n;
	}
	return (get_n > 0? get_n : 0);
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

	tb_int_t set_n = size;
	if (bst->e - bst->p < set_n) set_n = bst->e - bst->p;
	if (set_n)
	{
		tb_memcpy(bst->p, data, set_n);
		bst->p += set_n;
	}
	return (set_n > 0? set_n : 0);
}

tb_char_t* tb_bstream_set_string(tb_bstream_t* bst, tb_char_t const* s)
{
	TB_ASSERT(bst->e >= bst->p);
	tb_bstream_sync(bst);

	tb_char_t* b = bst->p;
	tb_char_t* p = bst->p;
	tb_char_t* e = bst->e - 1;
	while (*s && p < e) *p++ = *s++;
	*p++ = '\0';
	bst->p = p;

	return b;
}
/* /////////////////////////////////////////////////////////
 * peek
 */
tb_uint32_t tb_bstream_peek_ubits(tb_bstream_t* bst, tb_size_t nbits)
{
	if (!nbits || !bst) return 0;

	// save status
	tb_byte_t const* p = bst->p;
	tb_size_t b = bst->b;

	// peek value
	tb_uint32_t val = tb_bstream_get_ubits(bst, nbits);

	// restore status
	bst->p = p;
	bst->b = b;

	return val;
}
tb_sint32_t tb_bstream_peek_sbits(tb_bstream_t* bst, tb_size_t nbits)
{
	if (!nbits || !bst) return 0;

	// save status
	tb_byte_t const* p = bst->p;
	tb_size_t b = bst->b;

	// peek value
	tb_sint32_t val = tb_bstream_get_sbits(bst, nbits);

	// restore status
	bst->p = p;
	bst->b = b;

	return val;
}

tb_uint32_t tb_bstream_crc32(tb_bstream_t* bst)
{
	TB_ASSERT_RETURN_VAL(bst && !bst->b && bst->p && bst->e >= bst->p, 0);
	static tb_uint32_t table[256] =
	{
		0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
		0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
		0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
		0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
		0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
		0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
		0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
		0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
		0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
		0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
		0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
		0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
		0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
		0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
		0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
		0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,

		0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
		0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
		0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
		0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
		0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
		0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
		0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
		0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
		0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
		0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
		0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
		0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
		0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
		0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
		0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
		0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,

		0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
		0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
		0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
		0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
		0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
		0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
		0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
		0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
		0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
		0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
		0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
		0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
		0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
		0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
		0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
		0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,

		0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
		0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
		0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
		0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
		0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
		0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
		0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
		0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
		0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
		0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
		0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
		0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
		0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
		0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
		0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
		0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d,
	};

	tb_uint32_t crc32 = 0xffffffff;
	tb_byte_t* 	p = bst->p;
	tb_byte_t* 	e = bst->e;
	while (p < e) crc32 = (crc32 >> 8) ^ table[(crc32 & 0xff) ^ *p++]; 
	return (crc32 ^ 0xffffffff);
}
