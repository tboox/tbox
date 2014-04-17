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
 * @file		xml.c
 * @ingroup 	object
 *
 */
 
/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME 		"object_reader_xml"
#define TB_TRACE_MODULE_DEBUG 		(0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "xml.h"
#include "../object.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the array grow
#ifdef __tb_small__
# 	define TB_OBJECT_XML_READER_ARRAY_GROW 			(64)
#else
# 	define TB_OBJECT_XML_READER_ARRAY_GROW 			(256)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_object_t* tb_object_xml_reader_func_null(tb_object_xml_reader_t* reader, tb_size_t event)
{
	// check
	tb_assert_and_check_return_val(reader && reader->reader && event, tb_null);

	// ok
	return (tb_object_t*)tb_null_init();
}
static tb_object_t* tb_object_xml_reader_func_date(tb_object_xml_reader_t* reader, tb_size_t event)
{
	// check
	tb_assert_and_check_return_val(reader && reader->reader && event, tb_null);

	// empty?
	if (event == TB_XML_READER_EVENT_ELEMENT_EMPTY) 
		return tb_date_init_from_time(0);

	// walk
	tb_object_t* date = tb_null;
	while (event = tb_xml_reader_next(reader->reader))
	{
		switch (event)
		{
		case TB_XML_READER_EVENT_ELEMENT_END: 
			{
				// name
				tb_char_t const* name = tb_xml_reader_element(reader->reader);
				tb_assert_and_check_goto(name, end);
				
				// is end?
				if (!tb_stricmp(name, "date"))
				{
					// empty?
					if (!date) date = tb_date_init_from_time(0);
					goto end;
				}
			}
			break;
		case TB_XML_READER_EVENT_TEXT: 
			{
				// text
				tb_char_t const* text = tb_xml_reader_text(reader->reader);
				tb_assert_and_check_goto(text, end);
				tb_trace_d("date: %s", text);

				// done date: %04ld-%02ld-%02ld %02ld:%02ld:%02ld
				tb_tm_t tm = {0};
				tb_char_t const* p = text;
				tb_char_t const* e = text + tb_strlen(text);

				// init year
				while (p < e && *p && !tb_isdigit(*p)) p++;
				tb_assert_and_check_goto(p < e, end);
				tm.year = tb_atoi(p);

				// init month
				while (p < e && *p && tb_isdigit(*p)) p++;
				while (p < e && *p && !tb_isdigit(*p)) p++;
				tb_assert_and_check_goto(p < e, end);
				tm.month = tb_atoi(p);
				
				// init day
				while (p < e && *p && tb_isdigit(*p)) p++;
				while (p < e && *p && !tb_isdigit(*p)) p++;
				tb_assert_and_check_goto(p < e, end);
				tm.mday = tb_atoi(p);
				
				// init hour
				while (p < e && *p && tb_isdigit(*p)) p++;
				while (p < e && *p && !tb_isdigit(*p)) p++;
				tb_assert_and_check_goto(p < e, end);
				tm.hour = tb_atoi(p);
						
				// init minute
				while (p < e && *p && tb_isdigit(*p)) p++;
				while (p < e && *p && !tb_isdigit(*p)) p++;
				tb_assert_and_check_goto(p < e, end);
				tm.minute = tb_atoi(p);
				
				// init second
				while (p < e && *p && tb_isdigit(*p)) p++;
				while (p < e && *p && !tb_isdigit(*p)) p++;
				tb_assert_and_check_goto(p < e, end);
				tm.second = tb_atoi(p);
			
				// time
				tb_time_t time = tb_mktime(&tm);
				tb_assert_and_check_goto(time >= 0, end);

				// date
				date = tb_date_init_from_time(time);
			}
			break;
		default:
			break;
		}
	}

end:

	// ok?
	return date;
}
static tb_object_t* tb_object_xml_reader_func_data(tb_object_xml_reader_t* reader, tb_size_t event)
{
	// check
	tb_assert_and_check_return_val(reader && reader->reader && event, tb_null);

	// empty?
	if (event == TB_XML_READER_EVENT_ELEMENT_EMPTY) 
		return tb_data_init_from_data(tb_null, 0);

	// walk
	tb_char_t* 		base64 	= tb_null;
	tb_object_t* 	data 	= tb_null;
	while (event = tb_xml_reader_next(reader->reader))
	{
		switch (event)
		{
		case TB_XML_READER_EVENT_ELEMENT_END: 
			{
				// name
				tb_char_t const* name = tb_xml_reader_element(reader->reader);
				tb_assert_and_check_goto(name, end);
				
				// is end?
				if (!tb_stricmp(name, "data"))
				{
					// empty?
					if (!data) data = tb_data_init_from_data(tb_null, 0);
					goto end;
				}
			}
			break;
		case TB_XML_READER_EVENT_TEXT: 
			{
				// text
				tb_char_t const* text = tb_xml_reader_text(reader->reader);
				tb_assert_and_check_goto(text, end);
				tb_trace_d("data: %s", text);

				// base64
				base64 = tb_strdup(text);
				tb_char_t* p = base64;
				tb_char_t* q = p;
				for (; *p; p++) if (!tb_isspace(*p)) *q++ = *p;
				*q = '\0';

				// decode base64 data
				tb_char_t const* 	ib = base64;
				tb_size_t 			in = tb_strlen(base64); 
				if (in)
				{
					tb_size_t 			on = in;
					tb_byte_t* 			ob = tb_malloc0(on);
					tb_assert_and_check_goto(ob && on, end);
					on = tb_base64_decode(ib, in, ob, on);
					tb_trace_d("base64: %u => %u", in, on);

					// init data
					data = tb_data_init_from_data(ob, on); tb_free(ob);
				}
				else data = tb_data_init_from_data(tb_null, 0);
				tb_assert_and_check_goto(data, end);
			}
			break;
		default:
			break;
		}
	}

end:

	// free
	if (base64) tb_free(base64);

	// ok?
	return data;
}
static tb_object_t* tb_object_xml_reader_func_array(tb_object_xml_reader_t* reader, tb_size_t event)
{
	// check
	tb_assert_and_check_return_val(reader && reader->reader && event, tb_null);

	// empty?
	if (event == TB_XML_READER_EVENT_ELEMENT_EMPTY) 
		return tb_array_init(TB_OBJECT_XML_READER_ARRAY_GROW, tb_false);

	// init array
	tb_object_t* array = tb_array_init(TB_OBJECT_XML_READER_ARRAY_GROW, tb_false);
	tb_assert_and_check_return_val(array, tb_null);

	// walk
	tb_bool_t ok = tb_false;
	while (!ok && (event = tb_xml_reader_next(reader->reader)))
	{
		switch (event)
		{
		case TB_XML_READER_EVENT_ELEMENT_BEG: 
		case TB_XML_READER_EVENT_ELEMENT_EMPTY: 
			{
				// name
				tb_char_t const* name = tb_xml_reader_element(reader->reader);
				tb_assert_and_check_goto(name, end);
				tb_trace_d("item: %s", name);

				// func
				tb_object_xml_reader_func_t func = tb_object_xml_reader_func(name);
				tb_assert_and_check_goto(func, end);

				// read
				tb_object_t* object = func(reader, event);

				// append object
				if (object) tb_array_append(array, object);
			}
			break;
		case TB_XML_READER_EVENT_ELEMENT_END: 
			{
				// name
				tb_char_t const* name = tb_xml_reader_element(reader->reader);
				tb_assert_and_check_goto(name, end);
				
				// is end?
				if (!tb_stricmp(name, "array")) ok = tb_true;
			}
			break;
		default:
			break;
		}
	}

	// ok
	ok = tb_true;

end:

	// fail
	if (!ok)
	{
		if (array) tb_object_exit(array);
		array = tb_null;
	}

	// ok?
	return array;
}
static tb_object_t* tb_object_xml_reader_func_string(tb_object_xml_reader_t* reader, tb_size_t event)
{
	// check
	tb_assert_and_check_return_val(reader && reader->reader && event, tb_null);

	// empty?
	if (event == TB_XML_READER_EVENT_ELEMENT_EMPTY) 
		return tb_string_init_from_cstr(tb_null);

	// walk
	tb_object_t* string = tb_null;
	while (event = tb_xml_reader_next(reader->reader))
	{
		switch (event)
		{
		case TB_XML_READER_EVENT_ELEMENT_END: 
			{
				// name
				tb_char_t const* name = tb_xml_reader_element(reader->reader);
				tb_assert_and_check_goto(name, end);
				
				// is end?
				if (!tb_stricmp(name, "string"))
				{
					// empty?
					if (!string) string = tb_string_init_from_cstr(tb_null);
					goto end;
				}
			}
			break;
		case TB_XML_READER_EVENT_TEXT: 
			{
				// text
				tb_char_t const* text = tb_xml_reader_text(reader->reader);
				tb_assert_and_check_goto(text, end);
				tb_trace_d("string: %s", text);
				
				// string
				string = tb_string_init_from_cstr(text);
				tb_assert_and_check_goto(string, end);
			}
			break;
		default:
			break;
		}
	}

end:

	// ok?
	return string;
}
static tb_object_t* tb_object_xml_reader_func_number(tb_object_xml_reader_t* reader, tb_size_t event)
{
	// check
	tb_assert_and_check_return_val(reader && reader->reader && event, tb_null);

	// empty?
	if (event == TB_XML_READER_EVENT_ELEMENT_EMPTY) 
		return tb_number_init_from_uint32(0);

	// walk
	tb_object_t* number = tb_null;
	while (event = tb_xml_reader_next(reader->reader))
	{
		switch (event)
		{
		case TB_XML_READER_EVENT_ELEMENT_END: 
			{
				// name
				tb_char_t const* name = tb_xml_reader_element(reader->reader);
				tb_assert_and_check_goto(name, end);
				
				// is end?
				if (!tb_stricmp(name, "number")) goto end;
			}
			break;
		case TB_XML_READER_EVENT_TEXT: 
			{
				// text
				tb_char_t const* text = tb_xml_reader_text(reader->reader);
				tb_assert_and_check_goto(text, end);
				tb_trace_d("number: %s", text);

				// has sign? is float?
				tb_size_t s = 0;
				tb_size_t f = 0;
				tb_char_t const* p = text;
				for (; *p; p++)
				{
					if (!s && *p == '-') s = 1;
					if (!f && *p == '.') f = 1;
					if (s && f) break;
				}
				
				// number
#ifdef TB_CONFIG_TYPE_FLOAT
				if (f) number = tb_number_init_from_double(tb_atof(text));
#else
				if (f) tb_trace_noimpl();
#endif
				else number = s? tb_number_init_from_sint64(tb_stoi64(text)) : tb_number_init_from_uint64(tb_stou64(text));
				tb_assert_and_check_goto(number, end);
			}
			break;
		default:
			break;
		}
	}

end:

	// ok?
	return number;
}
static tb_object_t* tb_object_xml_reader_func_boolean(tb_object_xml_reader_t* reader, tb_size_t event)
{
	// check
	tb_assert_and_check_return_val(reader && reader->reader && event, tb_null);

	// name
	tb_char_t const* name = tb_xml_reader_element(reader->reader);
	tb_assert_and_check_return_val(name, tb_null);
	tb_trace_d("boolean: %s", name);

	// the boolean value
	tb_bool_t val = tb_false;
	if (!tb_stricmp(name, "true")) val = tb_true;
	else if (!tb_stricmp(name, "false")) val = tb_false;
	else return tb_null;

	// ok?
	return (tb_object_t*)tb_boolean_init(val);
}
static tb_object_t* tb_object_xml_reader_func_dictionary(tb_object_xml_reader_t* reader, tb_size_t event)
{
	// check
	tb_assert_and_check_return_val(reader && reader->reader && event, tb_null);

	// empty?
	if (event == TB_XML_READER_EVENT_ELEMENT_EMPTY) 
		return tb_dictionary_init(TB_DICTIONARY_SIZE_MICRO, tb_false);

	// init key name
	tb_sstring_t 	kname;
	tb_char_t 		kdata[8192];
	if (!tb_sstring_init(&kname, kdata, 8192)) return tb_null;

	// init dictionary
	tb_object_t* dictionary = tb_dictionary_init(TB_DICTIONARY_SIZE_DEFAULT, tb_false);
	tb_assert_and_check_return_val(dictionary, tb_null);

	// walk
	tb_bool_t 	ok = tb_false;
	tb_bool_t 	key = tb_false;
	while (!ok && (event = tb_xml_reader_next(reader->reader)))
	{
		switch (event)
		{
		case TB_XML_READER_EVENT_ELEMENT_BEG: 
		case TB_XML_READER_EVENT_ELEMENT_EMPTY: 
			{
				// name
				tb_char_t const* name = tb_xml_reader_element(reader->reader);
				tb_assert_and_check_goto(name, end);
				tb_trace_d("%s", name);

				// is key
				if (!tb_stricmp(name, "key")) key = tb_true;
				else if (!key)
				{
					// func
					tb_object_xml_reader_func_t func = tb_object_xml_reader_func(name);
					tb_assert_and_check_goto(func, end);

					// read
					tb_object_t* object = func(reader, event);
					tb_trace_d("%s => %p", tb_sstring_cstr(&kname), object);
					tb_assert_and_check_goto(object, end);

					// set key & value
					if (tb_sstring_size(&kname) && dictionary) 
						tb_dictionary_set(dictionary, tb_sstring_cstr(&kname), object);

					// clear key name
					tb_sstring_clear(&kname);
				}
			}
			break;
		case TB_XML_READER_EVENT_ELEMENT_END: 
			{
				// name
				tb_char_t const* name = tb_xml_reader_element(reader->reader);
				tb_assert_and_check_goto(name, end);
				
				// is end?
				if (!tb_stricmp(name, "dict")) ok = tb_true;
				else if (!tb_stricmp(name, "key")) key = tb_false;
			}
			break;
		case TB_XML_READER_EVENT_TEXT: 
			{
				if (key)
				{
					// text
					tb_char_t const* text = tb_xml_reader_text(reader->reader);
					tb_assert_and_check_goto(text, end);

					// writ key name
					tb_sstring_cstrcpy(&kname, text);
				}
			}
			break;
		default:
			break;
		}
	}

	// ok
	ok = tb_true;

end:

	// fail
	if (!ok) 
	{
		tb_object_exit(dictionary);
		dictionary = tb_null;
	}

	// exit key name
	tb_sstring_exit(&kname);

	// ok?
	return dictionary;
}
static tb_object_t* tb_object_xml_reader_done(tb_gstream_t* stream)
{
	// init reader 
	tb_object_xml_reader_t reader = {0};
	reader.reader = tb_xml_reader_init(stream);
	tb_assert_and_check_return_val(reader.reader, tb_null);

	// init object
	tb_object_t* object = tb_null;

	// walk
	tb_size_t event = TB_XML_READER_EVENT_NONE;
	while (!object && (event = tb_xml_reader_next(reader.reader)))
	{
		switch (event)
		{
		case TB_XML_READER_EVENT_ELEMENT_EMPTY: 
		case TB_XML_READER_EVENT_ELEMENT_BEG: 
			{
				// name
				tb_char_t const* name = tb_xml_reader_element(reader.reader);
				tb_assert_and_check_goto(name, end);

				// func
				tb_object_xml_reader_func_t func = tb_object_xml_reader_func(name);
				tb_assert_and_check_goto(func, end);

				// read
				object = func(&reader, event);
			}
			break;
		default:
			break;
		}
	}

end:

	// exit reader
	tb_xml_reader_exit(reader.reader);

	// ok?
	return object;
}
static tb_size_t tb_object_xml_reader_probe(tb_gstream_t* stream)
{
	// check
	tb_assert_and_check_return_val(stream, 0);

	// need it
	tb_byte_t* p = tb_null;
	if (!tb_gstream_need(stream, &p, 5)) return 0;
	tb_assert_and_check_return_val(p, 0);

	// ok?
	return !tb_strnicmp(p, "<?xml", 5)? 50 : 0;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_object_reader_t* tb_object_xml_reader()
{
	// the reader
	static tb_object_reader_t s_reader = {0};

	// init reader
	s_reader.read 	= tb_object_xml_reader_done;
	s_reader.probe 	= tb_object_xml_reader_probe;

	// init hooker
	s_reader.hooker = tb_hash_init(TB_HASH_SIZE_MICRO, tb_item_func_str(tb_false, tb_null), tb_item_func_ptr(tb_null, tb_null));
	tb_assert_and_check_return_val(s_reader.hooker, tb_null);

	// hook reader 
	tb_hash_set(s_reader.hooker, "null", tb_object_xml_reader_func_null);
	tb_hash_set(s_reader.hooker, "date", tb_object_xml_reader_func_date);
	tb_hash_set(s_reader.hooker, "data", tb_object_xml_reader_func_data);
	tb_hash_set(s_reader.hooker, "array", tb_object_xml_reader_func_array);
	tb_hash_set(s_reader.hooker, "string", tb_object_xml_reader_func_string);
	tb_hash_set(s_reader.hooker, "number", tb_object_xml_reader_func_number);
	tb_hash_set(s_reader.hooker, "true", tb_object_xml_reader_func_boolean);
	tb_hash_set(s_reader.hooker, "false", tb_object_xml_reader_func_boolean);
	tb_hash_set(s_reader.hooker, "dict", tb_object_xml_reader_func_dictionary);

	// ok
	return &s_reader;
}
tb_bool_t tb_object_xml_reader_hook(tb_char_t const* type, tb_object_xml_reader_func_t func)
{
	// check
	tb_assert_and_check_return_val(type && func, tb_false);

	// the reader
	tb_object_reader_t* reader = tb_object_reader_get(TB_OBJECT_FORMAT_XML);
	tb_assert_and_check_return_val(reader && reader->hooker, tb_false);

	// hook it
	tb_hash_set(reader->hooker, type, func);

	// ok
	return tb_true;
}
tb_object_xml_reader_func_t tb_object_xml_reader_func(tb_char_t const* type)
{
	// check
	tb_assert_and_check_return_val(type, tb_null);

	// the reader
	tb_object_reader_t* reader = tb_object_reader_get(TB_OBJECT_FORMAT_XML);
	tb_assert_and_check_return_val(reader && reader->hooker, tb_null);

	// the func
	return tb_hash_get(reader->hooker, type);
}

