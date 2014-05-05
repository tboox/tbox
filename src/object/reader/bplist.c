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
 
/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME 		"object_reader_bplist"
#define TB_TRACE_MODULE_DEBUG 		(0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "bplist.h"
#include "../object.h"
#include "../../charset/charset.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the array grow
#ifdef __tb_small__
# 	define TB_OBJECT_BPLIST_READER_ARRAY_GROW 			(64)
#else
# 	define TB_OBJECT_BPLIST_READER_ARRAY_GROW 			(256)
#endif

// get bits
#define tb_object_bplist_bits_get(p, n) \
({ \
	tb_size_t __val = 0; \
	switch ((n)) \
	{ \
	case 1: __val = tb_bits_get_u8((p)); break; \
	case 2: __val = tb_bits_get_u16_be((p)); break; \
	case 4: __val = tb_bits_get_u32_be((p)); break; \
	case 8: __val = tb_bits_get_u64_be((p)); break; \
	default: break; \
	} \
 	__val; \
})

/* //////////////////////////////////////////////////////////////////////////////////////
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

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_time_t tb_object_bplist_reader_time_apple2host(tb_time_t time)
{
	tb_tm_t tm = {0};
	if (tb_localtime(time, &tm))
	{
		if (tm.year < 2000) tm.year += 31;
		time = tb_mktime(&tm);
	}
	return time;
}
static tb_object_t* tb_object_bplist_reader_func_object(tb_object_bplist_reader_t* reader, tb_size_t item_size)
{
	// check
	tb_assert_and_check_return_val(reader && reader->stream, tb_null);

	// read the object type & size
	tb_uint8_t type = tb_basic_stream_bread_u8(reader->stream);
	tb_uint8_t size = type & 0x0f; type &= 0xf0;
	tb_trace_d("type: %x, size: %x", type, size);

	// the func
	tb_object_bplist_reader_func_t func = tb_object_bplist_reader_func(type);
	tb_assert_and_check_return_val(func, tb_null);

	// read
	return func(reader, type, size, item_size);
}
static tb_long_t tb_object_bplist_reader_func_size(tb_object_bplist_reader_t* reader, tb_size_t item_size)
{
	// check
	tb_assert_and_check_return_val(reader && reader->stream, -1);

	// read size
	tb_object_t* object = tb_object_bplist_reader_func_object(reader, item_size);
	tb_assert_and_check_return_val(object, -1);

	tb_long_t size = -1;
 	if (tb_object_type(object) == TB_OBJECT_TYPE_NUMBER)
		size = tb_number_uint32(object);

	// exit
	tb_object_exit(object);

	// size
	return size;
}
static tb_object_t* tb_object_bplist_reader_func_data(tb_object_bplist_reader_t* reader, tb_size_t type, tb_size_t size, tb_size_t item_size)
{
	// check
	tb_assert_and_check_return_val(reader && reader->stream, tb_null);

	// init 
	tb_byte_t* 		data = tb_null;
	tb_object_t* 	object = tb_null;

	// size is too large?
	if (size == 0x0f)
	{
		// read size
		tb_long_t val = tb_object_bplist_reader_func_size(reader, item_size);
		tb_assert_and_check_return_val(val >= 0, tb_null);
		size = (tb_size_t)val;
	}

	// no empty?
	if (size)
	{
		// make data
		data = tb_malloc(size);
		tb_assert_and_check_return_val(data, tb_null);

		// read data
		if (tb_basic_stream_bread(reader->stream, data, size))
			object = tb_data_init_from_data(data, size);
	}
	else object = tb_data_init_from_data(tb_null, 0);

	// exit
	if (data) tb_free(data);

	// ok?
	return object;
}
static tb_object_t* tb_object_bplist_reader_func_array(tb_object_bplist_reader_t* reader, tb_size_t type, tb_size_t size, tb_size_t item_size)
{
	// check
	tb_assert_and_check_return_val(reader && reader->stream, tb_null);

	// init 
	tb_object_t* object = tb_null;

	// size is too large?
	if (size == 0x0f)
	{
		// read size
		tb_long_t val = tb_object_bplist_reader_func_size(reader, item_size);
		tb_assert_and_check_return_val(val >= 0, tb_null);
		size = (tb_size_t)val;
	}

	// init array
	object = tb_array_init(size? size : 16, tb_false);
	tb_assert_and_check_return_val(object, tb_null);

	// init items data
	if (size)
	{
		tb_byte_t* data = tb_malloc(sizeof(tb_uint32_t) + (size * item_size));
		if (data)
		{
			if (tb_basic_stream_bread(reader->stream, data + sizeof(tb_uint32_t), size * item_size))
			{
				tb_bits_set_u32_ne(data, (tb_uint32_t)size);

				// FIXME: not using the user private data
				tb_object_setp(object, data);
			}
			else tb_free(data);
		}
	}

	// ok?
	return object;
}
static tb_object_t* tb_object_bplist_reader_func_string(tb_object_bplist_reader_t* reader, tb_size_t type, tb_size_t size, tb_size_t item_size)
{
	// check
	tb_assert_and_check_return_val(reader && reader->stream, tb_null);

	// init 
	tb_char_t* 		utf8 = tb_null;
	tb_char_t* 		utf16 = tb_null;
	tb_object_t* 	object = tb_null;

	// read
	switch (type)
	{
	case TB_OBJECT_BPLIST_TYPE_STRING:
		{
			// size is too large?
			if (size == 0x0f)
			{
				// read size
				tb_long_t val = tb_object_bplist_reader_func_size(reader, item_size);
				tb_assert_and_check_return_val(val >= 0, tb_null);
				size = (tb_size_t)val;
			}

			// read string
			if (size)
			{
				// init utf8
				utf8 = tb_malloc(size + 1);
				tb_assert_and_check_break(utf8);

				// read utf8
				if (!tb_basic_stream_bread(reader->stream, (tb_byte_t*)utf8, size)) break;
				utf8[size] = '\0';
			}

			// init object
			object = tb_string_init_from_cstr(utf8);
		}
		break;
	case TB_OBJECT_BPLIST_TYPE_UNICODE:
		{
#ifdef TB_CONFIG_MODULE_HAVE_CHARSET
			// size is too large?
			if (size == 0x0f)
			{
				// read size
				tb_long_t val = tb_object_bplist_reader_func_size(reader, item_size);
				tb_assert_and_check_return_val(val >= 0, tb_null);
				size = (tb_size_t)val;
			}

			// read string
			if (size)
			{
				// init utf8 & utf16 data
				utf8 = tb_malloc((size + 1) << 2);
				utf16 = tb_malloc(size << 1);
				tb_assert_and_check_break(utf8 && utf16);

				// read utf16
				if (!tb_basic_stream_bread(reader->stream, (tb_byte_t*)utf16, size << 1)) break;
				
				// utf16 to utf8
				tb_long_t osize = tb_charset_conv_data(TB_CHARSET_TYPE_UTF16, TB_CHARSET_TYPE_UTF8, (tb_byte_t*)utf16, size << 1, (tb_byte_t*)utf8, (size + 1) << 2);
				tb_assert_and_check_break(osize > 0 && osize < ((size + 1) << 2));
				utf8[osize] = '\0';

				// init object
				object = tb_string_init_from_cstr(utf8);
			}
#else
			// trace
			tb_trace1_e("unicode type is not supported, please enable charset module config if you want to use it!");
#endif
		}
		break;
	default:
		break;
	}

	// exit
	if (utf8) tb_free(utf8);
	if (utf16) tb_free(utf16);

	// ok?
	return object;
}
static tb_object_t* tb_object_bplist_reader_func_number(tb_object_bplist_reader_t* reader, tb_size_t type, tb_size_t size, tb_size_t item_size)
{
	// check
	tb_assert_and_check_return_val(reader && reader->stream, tb_null);

	// init 
	tb_object_t* object = tb_null;

	// read
	size = 1 << size;
	switch (size)
	{
	case 1:
		{
			tb_uint8_t val = tb_basic_stream_bread_u8(reader->stream);
			object = tb_number_init_from_uint8(val);
		}
		break;
	case 2:
		{
			tb_uint16_t val = tb_basic_stream_bread_u16_be(reader->stream);
			object = tb_number_init_from_uint16(val);
		}
		break;
	case 4:
		{
			switch (type)
			{
			case TB_OBJECT_BPLIST_TYPE_UINT:
				{
					tb_uint32_t val = tb_basic_stream_bread_u32_be(reader->stream);
					object = tb_number_init_from_uint32(val);
				}
				break;
			case TB_OBJECT_BPLIST_TYPE_REAL:
				{
#ifdef TB_CONFIG_TYPE_FLOAT
					tb_float_t val = tb_basic_stream_bread_float_be(reader->stream);
					object = tb_number_init_from_float(val);
#else
					tb_trace_e("real type is not supported! please enable float config.");
#endif
				}
				break;
			default:
				tb_assert(0);
				break;
			}
		}
		break;
	case 8:
		{
			switch (type)
			{
			case TB_OBJECT_BPLIST_TYPE_UINT:
				{
					tb_uint64_t val = tb_basic_stream_bread_u64_be(reader->stream);
					object = tb_number_init_from_uint64(val);
				}
				break;
			case TB_OBJECT_BPLIST_TYPE_REAL:
				{
#ifdef TB_CONFIG_TYPE_FLOAT
					tb_double_t val = tb_basic_stream_bread_double_bbe(reader->stream);
					object = tb_number_init_from_double(val);
#else
					tb_trace_e("real type is not supported! please enable float config.");
#endif
				}
				break;
			default:
				tb_assert(0);
				break;
			}
		}
		break;
	default:
		tb_assert(0);
		break;
	}

	// ok?
	return object;
}
static tb_object_t* tb_object_bplist_reader_func_date(tb_object_bplist_reader_t* reader, tb_size_t type, tb_size_t size, tb_size_t item_size)
{
	// check
	tb_assert_and_check_return_val(reader && reader->stream, tb_null);

	// the date data
	tb_object_t* data = tb_object_bplist_reader_func_number(reader, TB_OBJECT_BPLIST_TYPE_REAL, size, item_size);
	tb_assert_and_check_return_val(data, tb_null);

	// init date
	tb_object_t* date = tb_date_init_from_time(tb_object_bplist_reader_time_apple2host((tb_time_t)tb_number_uint64(data)));

	// exit data
	tb_object_exit(data);

	// ok?
	return date;
}
static tb_object_t* tb_object_bplist_reader_func_boolean(tb_object_bplist_reader_t* reader, tb_size_t type, tb_size_t size, tb_size_t item_size)
{
	// init 
	tb_object_t* object = tb_null;

	// read 
	switch (size)
	{
	case TB_OBJECT_BPLIST_TYPE_TRUE:
		object = tb_boolean_init(tb_true);
		break;
	case TB_OBJECT_BPLIST_TYPE_FALSE:
		object = tb_boolean_init(tb_false);
		break;
	default:
		tb_assert(0);
		break;
	}
	return object;
}
static tb_object_t* tb_object_bplist_reader_func_dictionary(tb_object_bplist_reader_t* reader, tb_size_t type, tb_size_t size, tb_size_t item_size)
{
	// check
	tb_assert_and_check_return_val(reader && reader->stream, tb_null);

	// init 
	tb_object_t* object = tb_null;

	// size is too large?
	if (size == 0x0f)
	{
		// read size
		tb_long_t val = tb_object_bplist_reader_func_size(reader, item_size);
		tb_assert_and_check_return_val(val >= 0, tb_null);
		size = (tb_size_t)val;
	}

	// init dictionary
	object = tb_dictionary_init(TB_DICTIONARY_SIZE_MICRO, tb_false);
	tb_assert_and_check_return_val(object, tb_null);

	// init items data
	if (size)
	{
		item_size <<= 1;
		tb_byte_t* data = tb_malloc(sizeof(tb_uint32_t) + (size * item_size));
		if (data)
		{
			if (tb_basic_stream_bread(reader->stream, data + sizeof(tb_uint32_t), size * item_size))
			{
				tb_bits_set_u32_ne(data, (tb_uint32_t)size);
				tb_object_setp(object, data);
			}
			else tb_free(data);
		}
	}

	// ok?
	return object;
}
static tb_object_t* tb_object_bplist_reader_done(tb_basic_stream_t* stream)
{
	// check
	tb_assert_and_check_return_val(stream, tb_null);

	// init root
	tb_object_t* root = tb_null;

	// init reader
	tb_object_bplist_reader_t reader = {0};
	reader.stream = stream;

	// init size
	tb_hize_t size = tb_stream_size(stream);
	tb_assert_and_check_return_val(size, tb_null);

	// init data
	tb_byte_t data[32] = {0};
	
	// read magic & version
	if (!tb_basic_stream_bread(stream, data, 8)) return tb_null;

	// check magic & version
	if (tb_strncmp((tb_char_t const*)data, "bplist00", 8)) return tb_null;

	// seek to tail
	if (!tb_basic_stream_seek(stream, size - 26)) return tb_null;
	
	// read offset size
	tb_size_t offset_size = tb_basic_stream_bread_u8(stream);
	tb_trace_d("offset_size: %lu", offset_size);
	
	// read item size for array and dictionary
	tb_size_t item_size = tb_basic_stream_bread_u8(stream);
	tb_trace_d("item_size: %lu", item_size);
	
	// read object count
	tb_uint64_t object_count = tb_basic_stream_bread_u64_be(stream);
	tb_trace_d("object_count: %llu", object_count);
	
	// read root object
	tb_uint64_t root_object = tb_basic_stream_bread_u64_be(stream);
	tb_trace_d("root_object: %llu", root_object);

	// read offset table index
	tb_uint64_t offset_table_index = tb_basic_stream_bread_u64_be(stream);
	tb_trace_d("offset_table_index: %llu", offset_table_index);

	// check
	tb_assert_and_check_return_val(item_size && offset_size && object_count, tb_null);

	// init object hash
	tb_object_t** object_hash = tb_malloc0(sizeof(tb_object_t*) * object_count);
	tb_assert_and_check_return_val(object_hash, tb_null);

	// walk
	tb_size_t i = 0;
	for (i = 0; i < object_count; i++)
	{
		// seek to the offset entry
		if (!tb_basic_stream_seek(stream, offset_table_index + i * offset_size)) goto end;

		// read the object offset
		tb_hize_t offset = 0;
		switch (offset_size)
		{
		case 1:
			offset = tb_basic_stream_bread_u8(stream);
			break;
		case 2:
			offset = tb_basic_stream_bread_u16_be(stream);
			break;
		case 4:
			offset = tb_basic_stream_bread_u32_be(stream);
			break;
		case 8:
			offset = tb_basic_stream_bread_u64_be(stream);
			break;
		default:
			return tb_null;
			break;
		}

		// seek to the object offset 
		if (!tb_basic_stream_seek(stream, offset)) goto end;

		// read object
		object_hash[i] = tb_object_bplist_reader_func_object(&reader, item_size);
//		if (object_hash[i]) tb_object_dump(object_hash[i]);
	}

	// build array & dictionary items
	for (i = 0; i < object_count; i++)
	{
		tb_object_t* object = object_hash[i];
		if (object)
		{
			switch (tb_object_type(object))
			{
			case TB_OBJECT_TYPE_ARRAY:
				{
					// the priv data
					tb_byte_t* priv = tb_object_getp(object);
					if (priv)
					{
						// count
						tb_size_t count = (tb_size_t)tb_bits_get_u32_ne(priv);
						if (count)
						{
							// goto item data
							tb_byte_t const* p = priv + sizeof(tb_uint32_t);

							// walk items
							tb_size_t j = 0;
							for (i = 0; j < count; j++)
							{
								// the item index
								tb_size_t item = tb_object_bplist_bits_get(p + j * item_size, item_size);
								tb_assert(item < object_count && object_hash[item]);
//								tb_trace_d("item: %d", item);

								// append item
								if (item < object_count && object_hash[item])
								{
									tb_object_inc(object_hash[item]);
									tb_array_append(object, object_hash[item]);
								}
							}
						}

						// exit priv
						tb_free(priv);
						tb_object_setp(object, tb_null);
//						tb_object_dump(object);
					}
				}
				break;
			case TB_OBJECT_TYPE_DICTIONARY:
				{ 
					// the priv data
					tb_byte_t* priv = tb_object_getp(object);
					if (priv)
					{
						// count
						tb_size_t count = (tb_size_t)tb_bits_get_u32_ne(priv);
						if (count)
						{
							// goto item data
							tb_byte_t const* p = priv + sizeof(tb_uint32_t);

							// walk items
							tb_size_t j = 0;
							for (i = 0; j < count; j++)
							{
								// the key & val
								tb_size_t key = tb_object_bplist_bits_get(p + j * item_size, item_size);
								tb_size_t val = tb_object_bplist_bits_get(p + (count + j) * item_size, item_size);
								tb_assert(key < object_count && object_hash[key]);
								tb_assert(val < object_count && object_hash[val]);
//								tb_trace_d("key_val: %u => %lu", key, val);

								// append the key & val
								if (key < object_count && val < object_count && object_hash[key] && object_hash[val])
								{
									// key must be string now.
									tb_assert(tb_object_type(object_hash[key]) == TB_OBJECT_TYPE_STRING);
									if (tb_object_type(object_hash[key]) == TB_OBJECT_TYPE_STRING)
									{
										// set key => val
										tb_char_t const* skey = tb_string_cstr(object_hash[key]);
										if (skey) 
										{
											tb_object_inc(object_hash[val]);
											tb_dictionary_set(object, skey, object_hash[val]);
										}
										tb_assert(skey);
									}
								}
							}
						}

						// exit priv
						tb_free(priv);
						tb_object_setp(object, tb_null);
//						tb_object_dump(object);
					}
				}
				break;
			default:
				break;
			}
		}
	}	

end:

	if (object_hash)
	{
		// root
		if (root_object < object_count) root = object_hash[root_object];

		// refn--
		for (i = 0; i < object_count; i++)
		{
			if (object_hash[i] && i != root_object)
				tb_object_dec(object_hash[i]);
		}

		// exit object hash
		tb_free(object_hash);
		object_hash = tb_null;
	}

	// ok?
	return root;
}
static tb_size_t tb_object_bplist_reader_probe(tb_basic_stream_t* stream)
{
	// check
	tb_assert_and_check_return_val(stream, 0);

	// need it
	tb_byte_t* p = tb_null;
	if (!tb_basic_stream_need(stream, &p, 6)) return 0;
	tb_assert_and_check_return_val(p, 0);

	// ok?
	return !tb_strnicmp((tb_char_t const*)p, "bplist", 6)? 80 : 0;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_object_reader_t* tb_object_bplist_reader()
{
	// the reader
	static tb_object_reader_t s_reader = {0};

	// init reader
	s_reader.read 	= tb_object_bplist_reader_done;
	s_reader.probe 	= tb_object_bplist_reader_probe;

	// init hooker
	s_reader.hooker = tb_hash_init(TB_HASH_SIZE_MICRO, tb_item_func_uint32(), tb_item_func_ptr(tb_null, tb_null));
	tb_assert_and_check_return_val(s_reader.hooker, tb_null);

	// hook reader 
	tb_hash_set(s_reader.hooker, (tb_pointer_t)TB_OBJECT_BPLIST_TYPE_DATE, 		tb_object_bplist_reader_func_date);
	tb_hash_set(s_reader.hooker, (tb_pointer_t)TB_OBJECT_BPLIST_TYPE_DATA, 		tb_object_bplist_reader_func_data);
	tb_hash_set(s_reader.hooker, (tb_pointer_t)TB_OBJECT_BPLIST_TYPE_UID, 		tb_object_bplist_reader_func_array);
	tb_hash_set(s_reader.hooker, (tb_pointer_t)TB_OBJECT_BPLIST_TYPE_ARRAY, 	tb_object_bplist_reader_func_array);
	tb_hash_set(s_reader.hooker, (tb_pointer_t)TB_OBJECT_BPLIST_TYPE_STRING, 	tb_object_bplist_reader_func_string);
	tb_hash_set(s_reader.hooker, (tb_pointer_t)TB_OBJECT_BPLIST_TYPE_UNICODE, 	tb_object_bplist_reader_func_string);
	tb_hash_set(s_reader.hooker, (tb_pointer_t)TB_OBJECT_BPLIST_TYPE_UINT, 		tb_object_bplist_reader_func_number);
	tb_hash_set(s_reader.hooker, (tb_pointer_t)TB_OBJECT_BPLIST_TYPE_REAL, 		tb_object_bplist_reader_func_number);
	tb_hash_set(s_reader.hooker, (tb_pointer_t)TB_OBJECT_BPLIST_TYPE_NONE, 		tb_object_bplist_reader_func_boolean);
	tb_hash_set(s_reader.hooker, (tb_pointer_t)TB_OBJECT_BPLIST_TYPE_SET, 		tb_object_bplist_reader_func_dictionary);
	tb_hash_set(s_reader.hooker, (tb_pointer_t)TB_OBJECT_BPLIST_TYPE_DICT, 		tb_object_bplist_reader_func_dictionary);

	// ok
	return &s_reader;
}
tb_bool_t tb_object_bplist_reader_hook(tb_size_t type, tb_object_bplist_reader_func_t func)
{
	// check
	tb_assert_and_check_return_val(func, tb_false);

	// the reader
	tb_object_reader_t* reader = tb_object_reader_get(TB_OBJECT_FORMAT_BPLIST);
	tb_assert_and_check_return_val(reader && reader->hooker, tb_false);

	// hook it
	tb_hash_set(reader->hooker, (tb_pointer_t)type, func);

	// ok
	return tb_true;
}
tb_object_bplist_reader_func_t tb_object_bplist_reader_func(tb_size_t type)
{
	// the reader
	tb_object_reader_t* reader = tb_object_reader_get(TB_OBJECT_FORMAT_BPLIST);
	tb_assert_and_check_return_val(reader && reader->hooker, tb_null);

	// the func
	return tb_hash_get(reader->hooker, (tb_pointer_t)type);
}

