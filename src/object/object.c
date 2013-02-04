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
 * @file		object.c
 * @ingroup 	object
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_IMPL_TAG 		"object"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "object.h"
#include "../xml/xml.h"

/* ///////////////////////////////////////////////////////////////////////
 * saver
 */

/* ///////////////////////////////////////////////////////////////////////
 * loader
 */
static tb_object_t* tb_object_load_number_from_xml(tb_handle_t reader, tb_size_t event);
static tb_object_t* tb_object_load_string_from_xml(tb_handle_t reader, tb_size_t event);
static tb_object_t* tb_object_load_boolean_from_xml(tb_handle_t reader, tb_size_t event);
static tb_object_t* tb_object_load_dictionary_from_xml(tb_handle_t reader, tb_size_t event);
static tb_object_t* tb_object_load_data_from_xml(tb_handle_t reader, tb_size_t event)
{
	// check
	tb_assert_and_check_return_val(reader && event, tb_null);

	// empty?
	if (event == TB_XML_READER_EVENT_ELEMENT_EMPTY) 
		return tb_data_init_from_data(tb_null, 0);

	// walk
	tb_object_t* data = tb_null;
	while (event = tb_xml_reader_next(reader))
	{
		switch (event)
		{
		case TB_XML_READER_EVENT_ELEMENT_END: 
			{
				// name
				tb_char_t const* name = tb_xml_reader_element(reader);
				tb_assert_and_check_goto(name, end);
				
				// is end?
				if (!tb_stricmp(name, "data")) goto end;
			}
			break;
		case TB_XML_READER_EVENT_TEXT: 
			{
				// text
				tb_char_t const* text = tb_xml_reader_text(reader);
				tb_assert_and_check_goto(text, end);
//				tb_trace_impl("data: %s", text);
				
				// data
				data = tb_data_init_from_data(tb_null, 0);
				tb_assert_and_check_goto(data, end);
			}
			break;
		default:
			break;
		}
	}

end:

	// ok?
	return data;
}
static tb_object_t* tb_object_load_array_from_xml(tb_handle_t reader, tb_size_t event)
{
	// check
	tb_assert_and_check_return_val(reader && event, tb_null);

	// empty?
	if (event == TB_XML_READER_EVENT_ELEMENT_EMPTY) 
		return tb_array_init(64);

	// init array
	tb_object_t* array = tb_array_init(256);
	tb_assert_and_check_return_val(array, tb_null);

	// walk
	tb_bool_t ok = tb_false;
	while ((event = tb_xml_reader_next(reader)) && !ok)
	{
		switch (event)
		{
		case TB_XML_READER_EVENT_ELEMENT_BEG: 
		case TB_XML_READER_EVENT_ELEMENT_EMPTY: 
			{
				// name
				tb_char_t const* name = tb_xml_reader_element(reader);
				tb_assert_and_check_goto(name, end);
//				tb_trace_impl("item: %s", name);

				// load
				tb_object_t* object = tb_null;
				if (!tb_stricmp(name, "dict")) 			object = tb_object_load_dictionary_from_xml(reader, event);
				else if (!tb_stricmp(name, "data")) 	object = tb_object_load_data_from_xml(reader, event);
				else if (!tb_stricmp(name, "array")) 	object = tb_object_load_array_from_xml(reader, event);
				else if (!tb_stricmp(name, "string")) 	object = tb_object_load_string_from_xml(reader, event);
				else if (!tb_stricmp(name, "number")) 	object = tb_object_load_number_from_xml(reader, event);
				else if (!tb_stricmp(name, "true")) 	object = tb_object_load_boolean_from_xml(reader, event);
				else if (!tb_stricmp(name, "false")) 	object = tb_object_load_boolean_from_xml(reader, event);
				else goto end;

				// append object
				if (array && object) tb_array_append(array, object);
			}
			break;
		case TB_XML_READER_EVENT_ELEMENT_END: 
			{
				// name
				tb_char_t const* name = tb_xml_reader_element(reader);
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
static tb_object_t* tb_object_load_string_from_xml(tb_handle_t reader, tb_size_t event)
{
	// check
	tb_assert_and_check_return_val(reader && event, tb_null);

	// empty?
	if (event == TB_XML_READER_EVENT_ELEMENT_EMPTY) 
		return tb_string_init_from_cstr(tb_null);

	// walk
	tb_object_t* string = tb_null;
	while (event = tb_xml_reader_next(reader))
	{
		switch (event)
		{
		case TB_XML_READER_EVENT_ELEMENT_END: 
			{
				// name
				tb_char_t const* name = tb_xml_reader_element(reader);
				tb_assert_and_check_goto(name, end);
				
				// is end?
				if (!tb_stricmp(name, "string")) goto end;
			}
			break;
		case TB_XML_READER_EVENT_TEXT: 
			{
				// text
				tb_char_t const* text = tb_xml_reader_text(reader);
				tb_assert_and_check_goto(text, end);
//				tb_trace_impl("string: %s", text);
				
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
static tb_object_t* tb_object_load_number_from_xml(tb_handle_t reader, tb_size_t event)
{
	// check
	tb_assert_and_check_return_val(reader && event, tb_null);

	// empty?
	if (event == TB_XML_READER_EVENT_ELEMENT_EMPTY) 
		return tb_number_init_from_uint32(0);

	// walk
	tb_object_t* number = tb_null;
	while (event = tb_xml_reader_next(reader))
	{
		switch (event)
		{
		case TB_XML_READER_EVENT_ELEMENT_END: 
			{
				// name
				tb_char_t const* name = tb_xml_reader_element(reader);
				tb_assert_and_check_goto(name, end);
				
				// is end?
				if (!tb_stricmp(name, "number")) goto end;
			}
			break;
		case TB_XML_READER_EVENT_TEXT: 
			{
				// text
				tb_char_t const* text = tb_xml_reader_text(reader);
				tb_assert_and_check_goto(text, end);
				tb_trace_impl("number: %s", text);
				
				// number
				number = tb_number_init_from_uint32(0);
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
static tb_object_t* tb_object_load_boolean_from_xml(tb_handle_t reader, tb_size_t event)
{
	// check
	tb_assert_and_check_return_val(reader && event, tb_null);

	// name
	tb_char_t const* name = tb_xml_reader_element(reader);
	tb_assert_and_check_return_val(name, tb_null);
//	tb_trace_impl("boolean: %s", name);

	// the boolean value
	tb_bool_t val = tb_false;
	if (!tb_stricmp(name, "true")) val = tb_true;
	else if (!tb_stricmp(name, "false")) val = tb_false;
	else return tb_null;
	
	// ok?
	return tb_boolean_init(val);
}
static tb_object_t* tb_object_load_dictionary_from_xml(tb_handle_t reader, tb_size_t event)
{
	// check
	tb_assert_and_check_return_val(reader && event, tb_null);

	// empty?
	if (event == TB_XML_READER_EVENT_ELEMENT_EMPTY) 
		return tb_dictionary_init(TB_DICTIONARY_SIZE_MICRO);

	// init key name
	tb_sstring_t 	kname;
	tb_char_t 		kdata[8192];
	if (!tb_sstring_init(&kname, kdata, 8192)) return tb_null;

	// init dictionary
	tb_object_t* dictionary = tb_dictionary_init(TB_DICTIONARY_SIZE_DEFAULT);
	tb_assert_and_check_return_val(dictionary, tb_null);

	// walk
	tb_bool_t 	ok = tb_false;
	tb_bool_t 	key = tb_false;
	while ((event = tb_xml_reader_next(reader)) && !ok)
	{
		switch (event)
		{
		case TB_XML_READER_EVENT_ELEMENT_BEG: 
		case TB_XML_READER_EVENT_ELEMENT_EMPTY: 
			{
				// name
				tb_char_t const* name = tb_xml_reader_element(reader);
				tb_assert_and_check_goto(name, end);

				// is key
				if (!tb_stricmp(name, "key")) key = tb_true;
				else if (!key)
				{
					// load value
					tb_object_t* object = tb_null;
					if (!tb_stricmp(name, "dict")) 			object = tb_object_load_dictionary_from_xml(reader, event);
					else if (!tb_stricmp(name, "data")) 	object = tb_object_load_data_from_xml(reader, event);
					else if (!tb_stricmp(name, "array")) 	object = tb_object_load_array_from_xml(reader, event);
					else if (!tb_stricmp(name, "string")) 	object = tb_object_load_string_from_xml(reader, event);
					else if (!tb_stricmp(name, "number")) 	object = tb_object_load_number_from_xml(reader, event);
					else if (!tb_stricmp(name, "true")) 	object = tb_object_load_boolean_from_xml(reader, event);
					else if (!tb_stricmp(name, "false")) 	object = tb_object_load_boolean_from_xml(reader, event);
					else goto end;
//					tb_trace_impl("%s => %p", tb_sstring_cstr(&kname), object);
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
				tb_char_t const* name = tb_xml_reader_element(reader);
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
					tb_char_t* text = tb_xml_reader_text(reader);
					tb_assert_and_check_goto(text, end);

					// save key name
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

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_object_t* tb_object_load_from_xml(tb_gstream_t* gst)
{
	// init reader 
	tb_handle_t reader = tb_xml_reader_init(gst);
	tb_assert_and_check_return_val(reader, tb_null);

	// init object
	tb_object_t* object = tb_null;

	// walk
	tb_size_t event = TB_XML_READER_EVENT_NONE;
	while ((event = tb_xml_reader_next(reader)) && !object)
	{
		switch (event)
		{
		case TB_XML_READER_EVENT_ELEMENT_EMPTY: 
		case TB_XML_READER_EVENT_ELEMENT_BEG: 
			{
				// name
				tb_char_t const* name = tb_xml_reader_element(reader);
				tb_assert_and_check_goto(name, end);

				// load
				if (!tb_stricmp(name, "dict")) 			object = tb_object_load_dictionary_from_xml(reader, event);
				else if (!tb_stricmp(name, "data")) 	object = tb_object_load_data_from_xml(reader, event);
				else if (!tb_stricmp(name, "array")) 	object = tb_object_load_array_from_xml(reader, event);
				else if (!tb_stricmp(name, "string")) 	object = tb_object_load_string_from_xml(reader, event);
				else if (!tb_stricmp(name, "number")) 	object = tb_object_load_number_from_xml(reader, event);
				else if (!tb_stricmp(name, "true")) 	object = tb_object_load_boolean_from_xml(reader, event);
				else if (!tb_stricmp(name, "false")) 	object = tb_object_load_boolean_from_xml(reader, event);
				else goto end;
			}
			break;
		default:
			break;
		}
	}

end:

	// exit reader
	tb_xml_reader_exit(reader);

	// ok?
	return object;
}
static tb_object_t* tb_object_load_from_bin(tb_gstream_t* gst)
{
	tb_trace_noimpl();
	return tb_null;
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_bool_t tb_object_init(tb_object_t* object, tb_size_t flag, tb_size_t type)
{
	// check
	tb_assert_and_check_return_val(object, tb_false);

	// init
	tb_memset(object, 0, sizeof(tb_object_t));
	object->flag = flag;
	object->type = type;
	object->refn = 1;
}
tb_void_t tb_object_exit(tb_object_t* object)
{
	// check
	tb_assert_and_check_return(object);

	// readonly?
	tb_check_return(!(object->flag & TB_OBJECT_FLAG_READONLY));

	// refn must be 1
	tb_size_t refn = tb_object_ref(object);
	tb_assert_and_check_return(refn == 1);

	// exit
	tb_object_dec(object);
}
tb_void_t tb_object_cler(tb_object_t* object)
{
	// check
	tb_assert_and_check_return(object);

	// clear
	if (object->cler) object->cler(object);
}
tb_object_t* tb_object_copy(tb_object_t* object)
{
	// check
	tb_assert_and_check_return_val(object && object->copy, tb_null);

	// copy
	return object->copy(object);
}
tb_size_t tb_object_type(tb_object_t* object)
{
	tb_assert_and_check_return_val(object, TB_OBJECT_TYPE_NONE);
	return object->type;
}
tb_size_t tb_object_ref(tb_object_t* object)
{
	tb_assert_and_check_return_val(object, 0);
	return object->refn;
}
tb_void_t tb_object_inc(tb_object_t* object)
{
	// check
	tb_assert_and_check_return(object);

	// readonly?
	tb_check_return(!(object->flag & TB_OBJECT_FLAG_READONLY));

	// refn++
	object->refn++;
}
tb_void_t tb_object_dec(tb_object_t* object)
{
	// check
	tb_assert_and_check_return(object);

	// readonly?
	tb_check_return(!(object->flag & TB_OBJECT_FLAG_READONLY));

	// refn--
	if (object->refn > 1) object->refn--;
	else if (object->exit) object->exit(object);
}
tb_object_t* tb_object_load(tb_gstream_t* gst, tb_size_t format)
{
	// check
	tb_assert_and_check_return_val(gst && format, tb_null);

	// load
	switch (format)
	{
	case TB_OBJECT_FORMAT_XML:
		return tb_object_load_from_xml(gst);
	case TB_OBJECT_FORMAT_BIN:
		return tb_object_load_from_bin(gst);
	default:
		break;
	}

	return tb_null;
}
tb_bool_t tb_object_save(tb_object_t* object, tb_gstream_t* gst, tb_size_t format)
{
	// check
	tb_assert_and_check_return_val(object && gst && format, tb_false);

	// save
	switch (format)
	{
	case TB_OBJECT_FORMAT_XML:
		return tb_object_save_to_xml(object, gst);
	case TB_OBJECT_FORMAT_BIN:
		return tb_object_save_to_bin(object, gst);
	default:
		break;
	}

	return tb_false;
}
