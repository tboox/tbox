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
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author		ruki
 * @file		bplist.c
 * @ingroup 	object
 *
 */
 
/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME 		"object_writer_bplist"
#define TB_TRACE_MODULE_DEBUG 		(0)

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "bplist.h"
#include "../object.h"
#include "../../charset/charset.h"
#include "../../algorithm/algorithm.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// set bits
#define tb_object_bplist_writer_bits_set(p, v, n) \
do { \
	switch ((n)) \
	{ \
	case 1: tb_bits_set_u8((p), (tb_uint8_t)(v)); break; \
	case 2: tb_bits_set_u16_be((p), (tb_uint16_t)(v)); break; \
	case 4: tb_bits_set_u32_be((p), (tb_uint32_t)(v)); break; \
	case 8: tb_bits_set_u64_be((p), (tb_uint64_t)(v)); break; \
	default: break; \
	} \
} while (0)

// bytes
#define tb_object_bplist_writer_need_bytes(x) \
(((tb_uint64_t)(x)) < (1ull << 8) ? 1 : \
(((tb_uint64_t)(x)) < (1ull << 16) ? 2 : \
(((tb_uint64_t)(x)) < (1ull << 24) ? 3 : \
(((tb_uint64_t)(x)) < (1ull << 32) ? 4 : 8))))

// number
#define tb_object_bplist_writer_init_number(x) \
(((tb_uint64_t)(x)) < (1ull << 8) ? tb_number_init_from_uint8((tb_uint8_t)(x)) : \
(((tb_uint64_t)(x)) < (1ull << 16) ? tb_number_init_from_uint16((tb_uint16_t)(x)) : \
(((tb_uint64_t)(x)) < (1ull << 32) ? tb_number_init_from_uint32((tb_uint32_t)(x)) : tb_number_init_from_uint64((x)))))

// object list grow
#ifdef __tb_small__
# 	define TB_OBJECT_BPLIST_LIST_GROW 			(64)
#else
# 	define TB_OBJECT_BPLIST_LIST_GROW 			(256)
#endif

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the bplist type enum
typedef enum __tb_object_bplist_type_e
{
	TB_OBJECT_BPLIST_TYPE_NONE 		= 0x00
,	TB_OBJECT_BPLIST_TYPE_FALSE 	= 0x08
,	TB_OBJECT_BPLIST_TYPE_TRUE 		= 0x09
,	TB_OBJECT_BPLIST_TYPE_UINT 		= 0x10
,	TB_OBJECT_BPLIST_TYPE_REAL 		= 0x20
,	TB_OBJECT_BPLIST_TYPE_DATE 		= 0x30
,	TB_OBJECT_BPLIST_TYPE_DATA 		= 0x40
,	TB_OBJECT_BPLIST_TYPE_STRING 	= 0x50
,	TB_OBJECT_BPLIST_TYPE_UNICODE 	= 0x60
,	TB_OBJECT_BPLIST_TYPE_UID 		= 0x70
,	TB_OBJECT_BPLIST_TYPE_ARRAY 	= 0xA0
,	TB_OBJECT_BPLIST_TYPE_SET 		= 0xC0
,	TB_OBJECT_BPLIST_TYPE_DICT 		= 0xD0
,	TB_OBJECT_BPLIST_TYPE_MASK 		= 0xF0

}tb_object_bplist_type_e;

/* ///////////////////////////////////////////////////////////////////////
 * declaration
 */
static tb_bool_t tb_object_bplist_writer_func_number(tb_object_bplist_writer_t* writer, tb_object_t* object, tb_size_t item_size);

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */	
static __tb_inline__ tb_time_t tb_object_bplist_writer_time_host2apple(tb_time_t time)
{
	tb_tm_t tm = {0};
	if (tb_localtime(time, &tm))
	{
		if (tm.year >= 31) tm.year -= 31;
		time = tb_mktime(&tm);
	}
	return time;
}
static tb_bool_t tb_object_bplist_writer_func_rdata(tb_object_bplist_writer_t* writer, tb_uint8_t bype, tb_byte_t const* data, tb_size_t size, tb_size_t item_size)
{
    // check
	tb_assert_and_check_return_val(writer && writer->stream && !data == !size, tb_false);

	// writ flag
    tb_uint8_t flag = bype | (size < 15 ? size : 0xf);
	if (!tb_gstream_bwrit_u8(writer->stream, flag)) return tb_false;

	// writ size
    if (size >= 15)
    {
		// init object
        tb_object_t* object = tb_object_bplist_writer_init_number(size);
		tb_assert_and_check_return_val(object, tb_false);

		// writ it
		if (!tb_object_bplist_writer_func_number(writer, object, item_size)) 
		{
			tb_object_exit(object);
			return tb_false;
		}

		// exit object
		tb_object_exit(object);
    }
	
	// unicode? adjust size
	if (bype == TB_OBJECT_BPLIST_TYPE_UNICODE) size <<= 1;

	// writ data
	if (data) if (!tb_gstream_bwrit(writer->stream, data, size)) return tb_false;

	// ok
	return tb_true;
}
static tb_bool_t tb_object_bplist_writer_func_date(tb_object_bplist_writer_t* writer, tb_object_t* object, tb_size_t item_size)
{
	// check
	tb_assert_and_check_return_val(writer && writer->stream && object, tb_false);

	// writ date time
	tb_byte_t data[8];
	tb_bits_set_double_bbe(data, (tb_double_t)tb_object_bplist_writer_time_host2apple(tb_date_time(object)));
	if (!tb_gstream_bwrit_u8(writer->stream, TB_OBJECT_BPLIST_TYPE_DATE | 3)) return tb_false;
	if (!tb_gstream_bwrit(writer->stream, data, 8)) return tb_false;

	// ok
	return tb_true;
}
static tb_bool_t tb_object_bplist_writer_func_data(tb_object_bplist_writer_t* writer, tb_object_t* object, tb_size_t item_size)
{
	// check
	tb_assert_and_check_return_val(writer && writer->stream && object, tb_false);

	// writ
	return tb_object_bplist_writer_func_rdata(writer, TB_OBJECT_BPLIST_TYPE_DATA, tb_data_getp(object), tb_data_size(object), item_size);
}
static tb_bool_t tb_object_bplist_writer_func_array(tb_object_bplist_writer_t* writer, tb_object_t* object, tb_size_t item_size)
{
	// check
	tb_assert_and_check_return_val(writer && writer->stream && object, tb_false);

	// index tables
	tb_byte_t* index_tables = tb_object_getp(object);

	// size
	tb_size_t size = tb_array_size(object);
	tb_assert_and_check_return_val(!size == !index_tables, tb_false);

	// writ flag
    tb_uint8_t flag = TB_OBJECT_BPLIST_TYPE_ARRAY | (size < 15 ? size : 0xf);
	if (!tb_gstream_bwrit_u8(writer->stream, flag)) return tb_false;

	// writ size
    if (size >= 15)
    {
		// init osize
        tb_object_t* osize = tb_object_bplist_writer_init_number(size);
		tb_assert_and_check_return_val(osize, tb_false);

		// writ it
		if (!tb_object_bplist_writer_func_number(writer, osize, item_size)) 
		{
			tb_object_exit(osize);
			return tb_false;
		}

		// exit osize
		tb_object_exit(osize);
    }

	// writ index tables
	if (index_tables)
	{
		if (!tb_gstream_bwrit(writer->stream, index_tables, size * item_size)) return tb_false;
	}

	// ok
	return tb_true;
}
static tb_bool_t tb_object_bplist_writer_func_string(tb_object_bplist_writer_t* writer, tb_object_t* object, tb_size_t item_size)
{
	// check
	tb_assert_and_check_return_val(writer && writer->stream && object, tb_false);
#if 0
	// writ utf8
	return tb_object_bplist_writer_func_rdata(writer, TB_OBJECT_BPLIST_TYPE_STRING, tb_string_cstr(object), tb_string_size(object), item_size);
#else
	// writ utf16
	tb_char_t const* 	utf8 = tb_string_cstr(object);
	tb_size_t 			size = tb_string_size(object);
	if (utf8 && size)
	{
		// done
		tb_bool_t 	ok = tb_false;
		tb_char_t* 	utf16 = tb_null;
		tb_size_t 	osize = 0;
		do
		{
			// init utf16 data
			utf16 = tb_malloc((size + 1) << 2);
			tb_assert_and_check_break(utf16);

			// utf8 to utf16
			osize = tb_charset_conv_data(TB_CHARSET_TYPE_UTF8, TB_CHARSET_TYPE_UTF16, (tb_byte_t const*)utf8, size, (tb_byte_t*)utf16, (size + 1) << 2);
			tb_assert_and_check_break(osize > 0 && osize < (size + 1) << 2);
			tb_assert_and_check_break(!(osize & 1));

			// ok
			ok = tb_true;

		} while (0);
		
		// ok?
		if (ok) 
		{
			// only ascii? writ utf8
			if (osize == (size << 1)) ok = tb_object_bplist_writer_func_rdata(writer, TB_OBJECT_BPLIST_TYPE_STRING, (tb_byte_t*)utf8, size, item_size);
			// writ utf16
			else ok = tb_object_bplist_writer_func_rdata(writer, TB_OBJECT_BPLIST_TYPE_UNICODE, (tb_byte_t*)utf16, osize >> 1, item_size);

		}

		// exit utf16
		if (utf16) tb_free(utf16);
		utf16 = tb_null;

		// ok?
		return ok;
	}
	// writ empty
	else return tb_object_bplist_writer_func_rdata(writer, TB_OBJECT_BPLIST_TYPE_STRING, tb_null, 0, item_size);
#endif
}
static tb_bool_t tb_object_bplist_writer_func_number(tb_object_bplist_writer_t* writer, tb_object_t* object, tb_size_t item_size)
{
	// check
	tb_assert_and_check_return_val(writer && writer->stream && object, tb_false);

	// done
	switch (tb_number_type(object))
	{
	case TB_NUMBER_TYPE_UINT64:
		if (!tb_gstream_bwrit_u8(writer->stream, TB_OBJECT_BPLIST_TYPE_UINT | 3)) return tb_false;
		if (!tb_gstream_bwrit_u64_be(writer->stream, tb_number_uint64(object))) return tb_false;
		break;
	case TB_NUMBER_TYPE_SINT64:
		if (!tb_gstream_bwrit_u8(writer->stream, TB_OBJECT_BPLIST_TYPE_UINT | 3)) return tb_false;
		if (!tb_gstream_bwrit_s64_be(writer->stream, tb_number_sint64(object))) return tb_false;
		break;
	case TB_NUMBER_TYPE_UINT32:
		if (!tb_gstream_bwrit_u8(writer->stream, TB_OBJECT_BPLIST_TYPE_UINT | 2)) return tb_false;
		if (!tb_gstream_bwrit_u32_be(writer->stream, tb_number_uint32(object))) return tb_false;
		break;
	case TB_NUMBER_TYPE_SINT32:
		if (!tb_gstream_bwrit_u8(writer->stream, TB_OBJECT_BPLIST_TYPE_UINT | 2)) return tb_false;
		if (!tb_gstream_bwrit_s32_be(writer->stream, tb_number_sint32(object))) return tb_false;
		break;
	case TB_NUMBER_TYPE_UINT16:
		if (!tb_gstream_bwrit_u8(writer->stream, TB_OBJECT_BPLIST_TYPE_UINT | 1)) return tb_false;
		if (!tb_gstream_bwrit_u16_be(writer->stream, tb_number_uint16(object))) return tb_false;
		break;
	case TB_NUMBER_TYPE_SINT16:
		if (!tb_gstream_bwrit_u8(writer->stream, TB_OBJECT_BPLIST_TYPE_UINT | 1)) return tb_false;
		if (!tb_gstream_bwrit_s16_be(writer->stream, tb_number_sint16(object))) return tb_false;
		break;
	case TB_NUMBER_TYPE_UINT8:
		if (!tb_gstream_bwrit_u8(writer->stream, TB_OBJECT_BPLIST_TYPE_UINT)) return tb_false;
		if (!tb_gstream_bwrit_u8(writer->stream, tb_number_uint8(object))) return tb_false;
		break;
	case TB_NUMBER_TYPE_SINT8:
		if (!tb_gstream_bwrit_u8(writer->stream, TB_OBJECT_BPLIST_TYPE_UINT)) return tb_false;
		if (!tb_gstream_bwrit_s8(writer->stream, tb_number_sint8(object))) return tb_false;
		break;
	case TB_NUMBER_TYPE_FLOAT:
		{
			tb_byte_t 	data[4];
			tb_bits_set_float_be(data, tb_number_float(object));
			if (!tb_gstream_bwrit_u8(writer->stream, TB_OBJECT_BPLIST_TYPE_REAL | 2)) return tb_false;
			if (!tb_gstream_bwrit(writer->stream, data, 4)) return tb_false;
		}
		break;
	case TB_NUMBER_TYPE_DOUBLE:
		{
			tb_byte_t 	data[8];
			tb_bits_set_double_bbe(data, tb_number_double(object));
			if (!tb_gstream_bwrit_u8(writer->stream, TB_OBJECT_BPLIST_TYPE_REAL | 3)) return tb_false;
			if (!tb_gstream_bwrit(writer->stream, data, 8)) return tb_false;
		}
		break;
	default:
		tb_assert_and_check_return_val(0, tb_false);
		break;
	}

	// ok
	return tb_true;
}
static tb_bool_t tb_object_bplist_writer_func_boolean(tb_object_bplist_writer_t* writer, tb_object_t* object, tb_size_t item_size)
{
	// check
	tb_assert_and_check_return_val(writer && writer->stream && object, tb_false);

	// writ it
	return tb_gstream_bwrit_u8(writer->stream, TB_OBJECT_BPLIST_TYPE_NONE | (tb_boolean_bool(object)? TB_OBJECT_BPLIST_TYPE_TRUE : TB_OBJECT_BPLIST_TYPE_FALSE));
}
static tb_bool_t tb_object_bplist_writer_func_dictionary(tb_object_bplist_writer_t* writer, tb_object_t* object, tb_size_t item_size)
{
	// check
	tb_assert_and_check_return_val(writer && writer->stream && object, tb_false);

	// index tables
	tb_byte_t* index_tables = tb_object_getp(object);

	// size
	tb_size_t size = tb_dictionary_size(object);
	tb_assert_and_check_return_val(!size == !index_tables, tb_false);

	// writ flag
    tb_uint8_t flag = TB_OBJECT_BPLIST_TYPE_DICT | (size < 15 ? size : 0xf);
	if (!tb_gstream_bwrit_u8(writer->stream, flag)) return tb_false;

	// writ size
    if (size >= 15)
    {
		// init osize
        tb_object_t* osize = tb_object_bplist_writer_init_number(size);
		tb_assert_and_check_return_val(osize, tb_false);

		// writ it
		if (!tb_object_bplist_writer_func_number(writer, osize, item_size)) 
		{
			tb_object_exit(osize);
			return tb_false;
		}

		// exit osize
		tb_object_exit(osize);
    }

	// writ index tables
	if (index_tables)
	{
		if (!tb_gstream_bwrit(writer->stream, index_tables, (size << 1) * item_size)) return tb_false;
	}

	// ok
	return tb_true;
}
static tb_uint64_t tb_object_bplist_writer_builder_maxn(tb_object_t* object)
{
	// check
	tb_assert_and_check_return_val(object, 0);

	// walk
	tb_uint64_t size = 0;
	switch (tb_object_type(object))
	{
	case TB_OBJECT_TYPE_ARRAY:
		{
			// walk
			tb_for_all (tb_object_t*, item, tb_array_itor(object))
			{
				if (item) size += tb_object_bplist_writer_builder_maxn(item);
			}
		}
		break;
	case TB_OBJECT_TYPE_DICTIONARY:
		{
			// walk
			tb_for_all (tb_dictionary_item_t*, item, tb_dictionary_itor(object))
			{
				// item
				if (item && item->key && item->val)
					size += 1 + tb_object_bplist_writer_builder_maxn(item->val);
			}
		}
		break;
	default:
		break;
	}

	return size + 1;
}
static tb_size_t tb_object_bplist_writer_builder_addo(tb_object_t* object, tb_object_t* list, tb_hash_t* hash)
{
	// check
	tb_assert_and_check_return_val(object && list && hash, 0);

	// the object index
	tb_size_t index = (tb_size_t)tb_hash_get(hash, object);

	// new object?
	if (!index) 
	{
		// append object
		tb_array_append(list, object);

		// index
		index = tb_array_size(list);

		// set index
		tb_hash_set(hash, object, (tb_pointer_t)index);
		tb_object_inc(object);

		// check
		tb_assert(!tb_object_getp(object));
	}

	// ok?
	return index;
}
static tb_void_t tb_object_bplist_writer_builder_init(tb_object_t* object, tb_object_t* list, tb_hash_t* hash, tb_size_t item_size)
{
	// check
	tb_assert_and_check_return(object && list && hash);

	// build items
	switch (tb_object_type(object))
	{
	case TB_OBJECT_TYPE_ARRAY:
		{
			// make index tables
			tb_byte_t* 	index_tables = tb_null;
			tb_size_t 	size = tb_array_size(object);
			if (size) 
			{
				index_tables = tb_object_getp(object);
				if (!index_tables)
				{
					// make it
					index_tables = tb_malloc0(size * item_size);

					// FIXME: not using the user private data
					tb_object_setp(object, index_tables);
				}
			}

			// walk
			tb_size_t i = 0;
			tb_for_all (tb_object_t*, item, tb_array_itor(object))
			{
				// build item
				if (item) 
				{
					// add item to builder
					tb_size_t index = tb_object_bplist_writer_builder_addo(item, list, hash);

					// add index to tables
					if (index && index_tables) tb_object_bplist_writer_bits_set(index_tables + i++ * item_size, index - 1, item_size);
//					tb_trace_d("item: %p[%lu]", index_tables, index - 1);

					// init next
					tb_object_bplist_writer_builder_init(item, list, hash, item_size);
				}
			}
		}
		break;
	case TB_OBJECT_TYPE_DICTIONARY:
		{
			// make index tables
			tb_byte_t* 	index_tables = tb_null;
			tb_size_t 	size = tb_dictionary_size(object);
			if (size) 
			{
				index_tables = tb_object_getp(object);
				if (!index_tables)
				{
					// make it
					index_tables = tb_malloc0((size << 1) * item_size);

					// FIXME: not using the user private data
					tb_object_setp(object, index_tables);
				}
			}

			// walk keys
			{
				tb_size_t i = 0;
				tb_for_all (tb_dictionary_item_t*, item, tb_dictionary_itor(object))
				{
					// item
					if (item && item->key && item->val)
					{
						// make key object
						tb_object_t* key = tb_string_init_from_cstr(item->key);
						if (key)
						{
							// add key to builder
							tb_size_t index = tb_object_bplist_writer_builder_addo(key, list, hash);

							// add index to tables
							if (index && index_tables) tb_object_bplist_writer_bits_set(index_tables + i++ * item_size, index - 1, item_size);
//							tb_trace_d("keys: %p[%lu]", index_tables, index - 1);

							// build key
							tb_object_bplist_writer_builder_init(key, list, hash, item_size);
							tb_object_dec(key);

						}
					}
				}
			}

			// walk vals
			{
				tb_size_t 		i = 0;
				tb_for_all (tb_dictionary_item_t*, item, tb_dictionary_itor(object))
				{
					// item
					if (item && item->key && item->val)
					{
						// add val to builder
						tb_size_t index = tb_object_bplist_writer_builder_addo(item->val, list, hash);

						// add index to tables
						if (index && index_tables) tb_object_bplist_writer_bits_set(index_tables + (size + i++) * item_size, index - 1, item_size);
//						tb_trace_d("vals: %p[%lu]", index_tables, index - 1);

						// build val
						tb_object_bplist_writer_builder_init(item->val, list, hash, item_size);
					}
				}
			}
		}
		break;
	default:
		break;
	}
}
static tb_void_t tb_object_bplist_writer_builder_exit(tb_object_t* list, tb_hash_t* hash)
{
	// exit hash
	if (hash)
	{
		// walk
		tb_for_all (tb_hash_item_t*, item, hash)
		{
			// exit item
			if (item && item->name)
			{
				tb_byte_t* priv = tb_object_getp(item->name);
				if (priv)
				{
					tb_free(priv);
					tb_object_setp(item->name, tb_null);
				}

				tb_object_dec(item->name);
			}
		}

		// exit it
		tb_hash_exit(hash);
	}

	// exit list
	if (list) tb_object_exit(list);
}
static tb_long_t tb_object_bplist_writer_done(tb_gstream_t* stream, tb_object_t* object, tb_bool_t deflate)
{
	// check
	tb_assert_and_check_return_val(object && stream, -1);

	// init 
	tb_bool_t 		ok 					= tb_false;
	tb_size_t 		i 					= 0;
	tb_byte_t 		pad[6] 				= {0};
	tb_object_t* 	list 				= tb_null;
	tb_hash_t* 		hash 				= tb_null;
	tb_uint64_t 	object_count 		= 0;
	tb_uint64_t 	object_maxn 		= 0;
	tb_uint64_t 	root_object 		= 0;
	tb_uint64_t 	offset_table_index 	= 0;
	tb_size_t 		offset_size 		= 0;
	tb_size_t 		item_size 			= 0;
	tb_uint64_t* 	offsets 			= tb_null;
	tb_hize_t 		bof 				= 0;
	tb_hize_t 		eof 				= 0;

	// init writer
	tb_object_bplist_writer_t writer = {0};
	writer.stream = stream;
	
	// init list
	list = tb_array_init(TB_OBJECT_BPLIST_LIST_GROW, tb_true);
	tb_assert_and_check_goto(list, end);

	// init hash
	hash = tb_hash_init(TB_HASH_SIZE_DEFAULT, tb_item_func_ptr(tb_null, tb_null), tb_item_func_uint32());
	tb_assert_and_check_goto(hash, end);

	// object maxn
	object_maxn = tb_object_bplist_writer_builder_maxn(object);
	item_size 	= tb_object_bplist_writer_need_bytes(object_maxn);
	tb_trace_d("object_maxn: %llu", object_maxn);
	tb_trace_d("item_size: %lu", item_size);

	// add root object to builder
	tb_object_bplist_writer_builder_addo(object, list, hash);

	// init object builder
	tb_object_bplist_writer_builder_init(object, list, hash, item_size);

	// init object count
	object_count = tb_array_size(list);
	tb_trace_d("object_count: %llu", object_count);

	// init offsets
	offsets = tb_malloc0(object_count * sizeof(tb_uint64_t));
	tb_assert_and_check_goto(offsets, end);

	// the begin offset
	bof = tb_stream_offset(stream);

	// writ magic & version
	if (!tb_gstream_bwrit(stream, (tb_byte_t const*)"bplist00", 8)) goto end;

	// writ objects
	if (object_count)
	{
		i = 0;
		tb_for_all (tb_object_t*, item, tb_array_itor(list))
		{
			// item
			if (item) 
			{
				// check
				tb_assert_and_check_goto(i < object_count, end);

				// save offset
				offsets[i++] = tb_stream_offset(stream);

				// the func
				tb_object_bplist_writer_func_t func = tb_object_bplist_writer_func(tb_object_type(item));
				tb_assert_and_check_continue(func);

				// writ object
				if (!func(&writer, item, item_size)) goto end;
			}
		}
	}

	// offset table index
	offset_table_index = tb_stream_offset(stream);
	offset_size = tb_object_bplist_writer_need_bytes(offset_table_index);
	tb_trace_d("offset_table_index: %llu", offset_table_index);
	tb_trace_d("offset_size: %lu", offset_size);

	// writ offset table
	for (i = 0; i < object_count; i++)
	{
		switch (offset_size)
		{
		case 1:
			if (!tb_gstream_bwrit_u8(stream, (tb_uint8_t)offsets[i])) goto end;
			break;
		case 2:
			if (!tb_gstream_bwrit_u16_be(stream, (tb_uint16_t)offsets[i])) goto end;
			break;
		case 4:
			if (!tb_gstream_bwrit_u32_be(stream, (tb_uint32_t)offsets[i])) goto end;
			break;
		case 8:
			if (!tb_gstream_bwrit_u64_be(stream, (tb_uint64_t)offsets[i])) goto end;
			break;
		default:
			tb_assert_and_check_goto(0, end);
			break;
		}
	}

	// writ pad, like apple?
	if (!tb_gstream_bwrit(stream, pad, 6)) goto end;
	
	// writ tail
	if (!tb_gstream_bwrit_u8(stream, (tb_uint8_t)offset_size)) goto end;
	if (!tb_gstream_bwrit_u8(stream, (tb_uint8_t)item_size)) goto end;
	if (!tb_gstream_bwrit_u64_be(stream, object_count)) goto end;
	if (!tb_gstream_bwrit_u64_be(stream, root_object)) goto end;
	if (!tb_gstream_bwrit_u64_be(stream, offset_table_index)) goto end;

	// flush
	if (!tb_gstream_sync(stream, tb_true)) goto end;

	// the end offset
	eof = tb_stream_offset(stream);

	// ok
	ok = tb_true;

end:

	// exit offsets
	if (offsets) tb_free(offsets);
	offsets = tb_null;

	// exit object builder
	tb_object_bplist_writer_builder_exit(list, hash);
	list = tb_null;
	hash = tb_null;

	// ok?
	return (ok && (eof >= bof))? (tb_long_t)(eof - bof) : -1;
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_object_writer_t* tb_object_bplist_writer()
{
	// the writer
	static tb_object_writer_t s_writer = {0};
  
	// init writer
	s_writer.writ = tb_object_bplist_writer_done;
 
	// init hooker
	s_writer.hooker = tb_hash_init(TB_HASH_SIZE_MICRO, tb_item_func_uint32(), tb_item_func_ptr(tb_null, tb_null));
	tb_assert_and_check_return_val(s_writer.hooker, tb_null);

	// hook writer 
	tb_hash_set(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_DATE, tb_object_bplist_writer_func_date);
	tb_hash_set(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_DATA, tb_object_bplist_writer_func_data);
	tb_hash_set(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_ARRAY, tb_object_bplist_writer_func_array);
	tb_hash_set(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_STRING, tb_object_bplist_writer_func_string);
	tb_hash_set(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_NUMBER, tb_object_bplist_writer_func_number);
	tb_hash_set(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_BOOLEAN, tb_object_bplist_writer_func_boolean);
	tb_hash_set(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_DICTIONARY, tb_object_bplist_writer_func_dictionary);

	// ok
	return &s_writer;
}
tb_bool_t tb_object_bplist_writer_hook(tb_size_t type, tb_object_bplist_writer_func_t func)
{
	// check
	tb_assert_and_check_return_val(func, tb_false);
 
	// the writer
	tb_object_writer_t* writer = tb_object_writer_get(TB_OBJECT_FORMAT_BPLIST);
	tb_assert_and_check_return_val(writer && writer->hooker, tb_false);

	// hook it
	tb_hash_set(writer->hooker, (tb_pointer_t)type, func);

	// ok
	return tb_true;
}
tb_object_bplist_writer_func_t tb_object_bplist_writer_func(tb_size_t type)
{
	// the writer
	tb_object_writer_t* writer = tb_object_writer_get(TB_OBJECT_FORMAT_BPLIST);
	tb_assert_and_check_return_val(writer && writer->hooker, tb_null);

	// the func
	return tb_hash_get(writer->hooker, (tb_pointer_t)type);
}

