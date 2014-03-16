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
 * @file		json.c
 * @ingroup 	object
 *
 */
 
/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME 		"object_reader_json"
#define TB_TRACE_MODULE_DEBUG 		(0)

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "json.h"
#include "../object.h"
#include "../../charset/charset.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// the array grow
#ifdef __tb_small__
# 	define TB_OBJECT_JSON_READER_ARRAY_GROW 			(64)
#else
# 	define TB_OBJECT_JSON_READER_ARRAY_GROW 			(256)
#endif

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_object_t* tb_object_json_reader_func_null(tb_object_json_reader_t* reader, tb_char_t type)
{
	// check
	tb_assert_and_check_return_val(reader && reader->stream, tb_null);

	// init data
	tb_sstring_t 	data;
	tb_char_t 		buff[256];
	if (!tb_sstring_init(&data, buff, 256)) return tb_null;

	// init 
	tb_object_t* null = tb_null;

	// append character
	tb_sstring_chrcat(&data, type);

	// walk
	while (tb_stream_left(reader->stream)) 
	{
		// need one character
		tb_byte_t* p = tb_null;
		if (!tb_gstream_need(reader->stream, &p, 1) && p) goto end;

		// the character
		tb_char_t ch = *p;

		// append character
		if (tb_isalpha(ch)) tb_sstring_chrcat(&data, ch);
		else break;

		// skip it
		tb_gstream_skip(reader->stream, 1);
	}

	// check
	tb_assert_and_check_goto(tb_sstring_size(&data), end);

	// trace
	tb_trace_d("null: %s", tb_sstring_cstr(&data));

	// null?
	if (!tb_stricmp(tb_sstring_cstr(&data), "null")) null = tb_null_init();

end:

	// exit data
	tb_sstring_exit(&data);

	// ok?
	return null;
}
static tb_object_t* tb_object_json_reader_func_array(tb_object_json_reader_t* reader, tb_char_t type)
{
	// check
	tb_assert_and_check_return_val(reader && reader->stream && type == '[', tb_null);

	// init array
	tb_object_t* array = tb_array_init(TB_OBJECT_JSON_READER_ARRAY_GROW, tb_false);
	tb_assert_and_check_return_val(array, tb_null);

	// walk
	tb_char_t ch;
	tb_bool_t ok = tb_false;
	while (tb_stream_left(reader->stream)) 
	{
		// read one character
		ch = tb_gstream_bread_s8(reader->stream);

		// end?
		if (ch == ']') break;
		// no space? skip ','
		else if (!tb_isspace(ch) && ch != ',')
		{
			// the func
			tb_object_json_reader_func_t func = tb_object_json_reader_func(ch);
			tb_assert_and_check_goto(func, end);

			// read item
			tb_object_t* item = func(reader, ch);
			tb_assert_and_check_goto(item, end);

			// append item
			tb_array_append(array, item);
		}
	}

	// ok
	ok = tb_true;

end:

	// failed?
	if (!ok && array)
	{
		tb_object_exit(array);
		array = tb_null;
	}

	// ok?
	return array;
}
static tb_object_t* tb_object_json_reader_func_string(tb_object_json_reader_t* reader, tb_char_t type)
{
	// check
	tb_assert_and_check_return_val(reader && reader->stream && (type == '\"' || type == '\''), tb_null);

	// init data
	tb_pstring_t data;
	if (!tb_pstring_init(&data)) return tb_null;

	// walk
	tb_char_t ch;
	while (tb_stream_left(reader->stream)) 
	{
		// read one character
		ch = tb_gstream_bread_s8(reader->stream);

		// end?
		if (ch == '\"' || ch == '\'') break;
		// the escaped character?
		else if (ch == '\\')
		{
			// read one character
			ch = tb_gstream_bread_s8(reader->stream);
			// unicode?
			if (ch == 'u')
			{
				// the unicode string
				tb_char_t unicode_str[5];
				unicode_str[0] = tb_gstream_bread_s8(reader->stream);
				unicode_str[1] = tb_gstream_bread_s8(reader->stream);
				unicode_str[2] = tb_gstream_bread_s8(reader->stream);
				unicode_str[3] = tb_gstream_bread_s8(reader->stream);
				unicode_str[4] = '\0';

				// the unicode value
				tb_uint16_t unicode_val = tb_s16toi32(unicode_str);

				// the utf8 stream
				tb_char_t 		utf8_data[16] = {0};
				tb_bstream_t 	utf8_stream;
				tb_bstream_init(&utf8_stream, utf8_data, sizeof(utf8_data));

				// the unicode stream
				tb_bstream_t 	unicode_stream = {0};
				tb_bstream_init(&unicode_stream, (tb_byte_t*)&unicode_val, 2);

				// unicode to utf8
				tb_long_t utf8_size = tb_charset_conv_bst(TB_CHARSET_TYPE_UCS2 | TB_CHARSET_TYPE_NE, TB_CHARSET_TYPE_UTF8, &unicode_stream, &utf8_stream);
				if (utf8_size > 0) tb_pstring_cstrncat(&data, utf8_data, utf8_size);
			}
			// append escaped character
			else tb_pstring_chrcat(&data, ch);
		}
		// append character
		else tb_pstring_chrcat(&data, ch);
	}

	// init string
	tb_object_t* string = tb_string_init_from_cstr(tb_pstring_cstr(&data));

	// trace
	tb_trace_d("string: %s", tb_pstring_cstr(&data));

	// exit data
	tb_pstring_exit(&data);

	// ok?
	return string;
}
static tb_object_t* tb_object_json_reader_func_number(tb_object_json_reader_t* reader, tb_char_t type)
{
	// check
	tb_assert_and_check_return_val(reader && reader->stream, tb_null);

	// init data
	tb_sstring_t 	data;
	tb_char_t 		buff[256];
	if (!tb_sstring_init(&data, buff, 256)) return tb_null;

	// init
	tb_object_t* number = tb_null;

	// append character
	tb_sstring_chrcat(&data, type);

	// walk
	tb_bool_t bs = (type == '-')? tb_true : tb_false;
	tb_bool_t bf = (type == '.')? tb_true : tb_false;
	while (tb_stream_left(reader->stream)) 
	{
		// need one character
		tb_byte_t* p = tb_null;
		if (!tb_gstream_need(reader->stream, &p, 1) && p) goto end;

		// the character
		tb_char_t ch = *p;

		// is float?
		if (!bf && ch == '.') bf = tb_true;
		else if (bf && ch == '.') goto end;

		// append character
		if (tb_isdigit10(ch) || ch == '.' || ch == 'e' || ch == 'E' || ch == '-' || ch == '+') 
			tb_sstring_chrcat(&data, ch);
		else break;

		// skip it
		tb_gstream_skip(reader->stream, 1);
	}

	// check
	tb_assert_and_check_goto(tb_sstring_size(&data), end);

	// trace
	tb_trace_d("number: %s", tb_sstring_cstr(&data));

	// init number 
#ifdef TB_CONFIG_TYPE_FLOAT
	if (bf) number = tb_number_init_from_float(tb_stof(tb_sstring_cstr(&data)));
#else
	if (bf) tb_trace_noimpl();
#endif
	else if (bs) 
	{
		tb_sint64_t value = tb_stoi64(tb_sstring_cstr(&data));
		tb_size_t 	bytes = tb_object_reader_need_bytes(-value);
		switch (bytes)
		{
		case 1: number = tb_number_init_from_sint8((tb_sint8_t)value); break;
		case 2: number = tb_number_init_from_sint16((tb_sint16_t)value); break;
		case 4: number = tb_number_init_from_sint32((tb_sint32_t)value); break;
		case 8: number = tb_number_init_from_sint64((tb_sint64_t)value); break;
		default: break;
		}
		
	}
	else 
	{
		tb_uint64_t value = tb_stou64(tb_sstring_cstr(&data));
		tb_size_t 	bytes = tb_object_reader_need_bytes(value);
		switch (bytes)
		{
		case 1: number = tb_number_init_from_uint8((tb_uint8_t)value); break;
		case 2: number = tb_number_init_from_uint16((tb_uint16_t)value); break;
		case 4: number = tb_number_init_from_uint32((tb_uint32_t)value); break;
		case 8: number = tb_number_init_from_uint64((tb_uint64_t)value); break;
		default: break;
		}
	}

end:

	// exit data
	tb_sstring_exit(&data);

	// ok?
	return number;
}
static tb_object_t* tb_object_json_reader_func_boolean(tb_object_json_reader_t* reader, tb_char_t type)
{
	// check
	tb_assert_and_check_return_val(reader && reader->stream, tb_null);

	// init data
	tb_sstring_t 	data;
	tb_char_t 		buff[256];
	if (!tb_sstring_init(&data, buff, 256)) return tb_null;

	// init 
	tb_object_t* boolean = tb_null;

	// append character
	tb_sstring_chrcat(&data, type);

	// walk
	while (tb_stream_left(reader->stream)) 
	{
		// need one character
		tb_byte_t* p = tb_null;
		if (!tb_gstream_need(reader->stream, &p, 1) && p) goto end;

		// the character
		tb_char_t ch = *p;

		// append character
		if (tb_isalpha(ch)) tb_sstring_chrcat(&data, ch);
		else break;

		// skip it
		tb_gstream_skip(reader->stream, 1);
	}

	// check
	tb_assert_and_check_goto(tb_sstring_size(&data), end);

	// trace
	tb_trace_d("boolean: %s", tb_sstring_cstr(&data));

	// true?
	if (!tb_stricmp(tb_sstring_cstr(&data), "true")) boolean = tb_boolean_init(tb_true);
	// false?
	else if (!tb_stricmp(tb_sstring_cstr(&data), "false")) boolean = tb_boolean_init(tb_false);

end:

	// exit data
	tb_sstring_exit(&data);

	// ok?
	return boolean;
}
static tb_object_t* tb_object_json_reader_func_dictionary(tb_object_json_reader_t* reader, tb_char_t type)
{
	// check
	tb_assert_and_check_return_val(reader && reader->stream && type == '{', tb_null);

	// init key name
	tb_sstring_t 	kname;
	tb_char_t 		kdata[8192];
	if (!tb_sstring_init(&kname, kdata, 8192)) return tb_null;

	// init dictionary
	tb_object_t* dictionary = tb_dictionary_init(TB_DICTIONARY_SIZE_DEFAULT, tb_false);
	tb_assert_and_check_return_val(dictionary, tb_null);

	// walk
	tb_char_t ch;
	tb_bool_t ok = tb_false;
	tb_bool_t bkey = tb_false;
	tb_size_t bstr = 0;
	while (tb_stream_left(reader->stream)) 
	{
		// read one character
		ch = tb_gstream_bread_s8(reader->stream);

		// end?
		if (ch == '}') break;
		// no space? skip ','
		else if (!tb_isspace(ch) && ch != ',')
		{
			// no key?
			if (!bkey)
			{
				// is str?
				if (ch == '\"' || ch == '\'') bstr = !bstr;
				// is key end?
				else if (!bstr && ch == ':') bkey = tb_true;
				// append key
				else if (bstr) tb_sstring_chrcat(&kname, ch);
			}
			// key ok? read val
			else
			{
				// trace
				tb_trace_d("key: %s", tb_sstring_cstr(&kname));

				// the func
				tb_object_json_reader_func_t func = tb_object_json_reader_func(ch);
				tb_assert_and_check_goto(func, end);

				// read val
				tb_object_t* val = func(reader, ch);
				tb_assert_and_check_goto(val, end);

				// set key => val
				tb_dictionary_set(dictionary, tb_sstring_cstr(&kname), val);

				// reset key
				bstr = 0;
				bkey = tb_false;
				tb_sstring_clear(&kname);
			}
		}
	}

	// ok
	ok = tb_true;

end:

	// failed?
	if (!ok && dictionary)
	{
		tb_object_exit(dictionary);
		dictionary = tb_null;
	}

	// exit key name
	tb_sstring_exit(&kname);

	// ok?
	return dictionary;
}
static tb_object_t* tb_object_json_reader_done(tb_gstream_t* stream)
{
	// check
	tb_assert_and_check_return_val(stream, tb_null);

	// init reader
	tb_object_json_reader_t reader = {0};
	reader.stream = stream;

	// skip spaces
	tb_char_t type;
	while (tb_stream_left(stream)) 
	{
		type = tb_gstream_bread_s8(stream);
		if (!tb_isspace(type)) break;
	}

	// empty?
	tb_check_return_val(tb_stream_left(stream), tb_null);

	// the func
	tb_object_json_reader_func_t func = tb_object_json_reader_func(type);
	tb_assert_and_check_return_val(func, tb_null);

	// read it
	return func(&reader, type);
}
static tb_size_t tb_object_json_reader_probe(tb_gstream_t* stream)
{
	// check
	tb_assert_and_check_return_val(stream, 0);

	// need it
	tb_byte_t* 	p = tb_null;
	if (!tb_gstream_need(stream, &p, 5)) return 0;
	tb_assert_and_check_return_val(p, 0);

	// probe it
	tb_size_t 	s = 10;
	tb_byte_t* 	e = p + 5;
	for (; p < e && *p; p++)
	{
		if (*p == '{' || *p == '[') 
		{
			s = 50;
			break;
		}
		else if (!tb_isgraph(*p)) 
		{
			s = 0;
			break;
		}
	}

	// ok?
	return s;
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_object_reader_t* tb_object_json_reader()
{
	// the reader
	static tb_object_reader_t s_reader = {0};

	// init reader
	s_reader.read 	= tb_object_json_reader_done;
	s_reader.probe 	= tb_object_json_reader_probe;

	// init hooker
	s_reader.hooker = tb_hash_init(TB_HASH_SIZE_MICRO, tb_item_func_uint8(), tb_item_func_ptr(tb_null, tb_null));
	tb_assert_and_check_return_val(s_reader.hooker, tb_null);

	// hook reader 
	tb_hash_set(s_reader.hooker, (tb_pointer_t)'n', tb_object_json_reader_func_null);
	tb_hash_set(s_reader.hooker, (tb_pointer_t)'N', tb_object_json_reader_func_null);
	tb_hash_set(s_reader.hooker, (tb_pointer_t)'[', tb_object_json_reader_func_array);
	tb_hash_set(s_reader.hooker, (tb_pointer_t)'\'', tb_object_json_reader_func_string);
	tb_hash_set(s_reader.hooker, (tb_pointer_t)'\"', tb_object_json_reader_func_string);
	tb_hash_set(s_reader.hooker, (tb_pointer_t)'0', tb_object_json_reader_func_number);
	tb_hash_set(s_reader.hooker, (tb_pointer_t)'1', tb_object_json_reader_func_number);
	tb_hash_set(s_reader.hooker, (tb_pointer_t)'2', tb_object_json_reader_func_number);
	tb_hash_set(s_reader.hooker, (tb_pointer_t)'3', tb_object_json_reader_func_number);
	tb_hash_set(s_reader.hooker, (tb_pointer_t)'4', tb_object_json_reader_func_number);
	tb_hash_set(s_reader.hooker, (tb_pointer_t)'5', tb_object_json_reader_func_number);
	tb_hash_set(s_reader.hooker, (tb_pointer_t)'6', tb_object_json_reader_func_number);
	tb_hash_set(s_reader.hooker, (tb_pointer_t)'7', tb_object_json_reader_func_number);
	tb_hash_set(s_reader.hooker, (tb_pointer_t)'8', tb_object_json_reader_func_number);
	tb_hash_set(s_reader.hooker, (tb_pointer_t)'9', tb_object_json_reader_func_number);
	tb_hash_set(s_reader.hooker, (tb_pointer_t)'.', tb_object_json_reader_func_number);
	tb_hash_set(s_reader.hooker, (tb_pointer_t)'-', tb_object_json_reader_func_number);
	tb_hash_set(s_reader.hooker, (tb_pointer_t)'+', tb_object_json_reader_func_number);
	tb_hash_set(s_reader.hooker, (tb_pointer_t)'e', tb_object_json_reader_func_number);
	tb_hash_set(s_reader.hooker, (tb_pointer_t)'E', tb_object_json_reader_func_number);
	tb_hash_set(s_reader.hooker, (tb_pointer_t)'t', tb_object_json_reader_func_boolean);
	tb_hash_set(s_reader.hooker, (tb_pointer_t)'T', tb_object_json_reader_func_boolean);
	tb_hash_set(s_reader.hooker, (tb_pointer_t)'f', tb_object_json_reader_func_boolean);
	tb_hash_set(s_reader.hooker, (tb_pointer_t)'F', tb_object_json_reader_func_boolean);
	tb_hash_set(s_reader.hooker, (tb_pointer_t)'{', tb_object_json_reader_func_dictionary);

	// ok
	return &s_reader;
}
tb_bool_t tb_object_json_reader_hook(tb_char_t type, tb_object_json_reader_func_t func)
{
	// check
	tb_assert_and_check_return_val(type && func, tb_false);

	// the reader
	tb_object_reader_t* reader = tb_object_reader_get(TB_OBJECT_FORMAT_JSON);
	tb_assert_and_check_return_val(reader && reader->hooker, tb_false);

	// hook it
	tb_hash_set(reader->hooker, (tb_pointer_t)(tb_size_t)type, func);

	// ok
	return tb_true;
}
tb_object_json_reader_func_t tb_object_json_reader_func(tb_char_t type)
{
	// check
	tb_assert_and_check_return_val(type, tb_null);

	// the reader
	tb_object_reader_t* reader = tb_object_reader_get(TB_OBJECT_FORMAT_JSON);
	tb_assert_and_check_return_val(reader && reader->hooker, tb_null);
 
	// the func
	return tb_hash_get(reader->hooker, (tb_pointer_t)(tb_size_t)type);
}

