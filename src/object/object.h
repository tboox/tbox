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
 * @file		object.h
 * @defgroup 	object
 *
 */
#ifndef TB_OBJECT_H
#define TB_OBJECT_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "null.h"
#include "data.h"
#include "date.h"
#include "array.h"
#include "string.h"
#include "number.h"
#include "boolean.h"
#include "dictionary.h"

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init object reader
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_object_init_reader();

/*! exit object reader
 *
 * @return 			tb_true or tb_false
 */
tb_void_t 			tb_object_exit_reader();

/*! init object writer
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_object_init_writer();

/*! exit object writer
 *
 * @return 			tb_true or tb_false
 */
tb_void_t 			tb_object_exit_writer();

/*! init object
 *
 * @param object 	the object pointer
 * @param flag 		the object flag
 * @param type 		the object type
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_object_init(tb_object_t* object, tb_size_t flag, tb_size_t type);

/*! exit object
 *
 * @param object 	the object pointer
 *
 * @note the reference count must be one
 */
tb_void_t 			tb_object_exit(tb_object_t* object);

/*! cler object
 *
 * @param object 	the object pointer
 */
tb_void_t 			tb_object_cler(tb_object_t* object);

/*! set the object private data
 *
 * @param object 	the object pointer
 * @param priv 		the private data
 *
 */
tb_void_t 			tb_object_setp(tb_object_t* object, tb_cpointer_t priv);

/*! get the object private data
 *
 * @param object 	the object pointer
 *
 * @return 			the private data
 */
tb_cpointer_t 		tb_object_getp(tb_object_t* object);

/*! read object
 *
 * @param gst 		the stream
 *
 * @return 			the object pointer
 */
tb_object_t* 		tb_object_read(tb_gstream_t* gst);

/*! read object from data
 *
 * @param data 		the data
 * @param size 		the size
 *
 * @return 			the object pointer
 */
tb_object_t* 		tb_object_read_from_data(tb_byte_t const* data, tb_size_t size);

/*! read object from url
 *
 * @param url 		the url
 *
 * @return 			the object pointer
 */
tb_object_t* 		tb_object_read_from_url(tb_char_t const* url);

/*! writ object
 *
 * @param object 	the object pointer
 * @param gst 		the stream
 * @param format 	the object format
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_object_writ(tb_object_t* object, tb_gstream_t* gst, tb_size_t format);

/*! writ object to url
 *
 * @param object	the object pointer
 * @param url 		the url
 * @param format 	the format
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_object_writ_to_url(tb_object_t* object, tb_char_t const* url, tb_size_t format);

/*! copy object
 *
 * @param object 	the object pointer
 *
 * @return 			the object copy
 */
tb_object_t* 		tb_object_copy(tb_object_t* object);

/*! the object type
 *
 * @param object 	the object pointer
 *
 * @return 			the object type
 */
tb_size_t 			tb_object_type(tb_object_t* object);

/*! the object data
 *
 * @param object	the object pointer
 * @param format 	the format
 *
 * @return 			the data object
 */
tb_object_t* 		tb_object_data(tb_object_t* object, tb_size_t format);

/*! seek to the object for the gived path
 *
 * <pre>
 *
 * xml:
 *
 * <dict>
		<key>string</key>
		<string>hello wolrd!</string>

		<key>integer</key>
		<number>31415926</number>

		<key>array</key>
		<array>
			<string>hello wolrd!</string>
			<number>31415926</number>
			<number>3.1415926</number>
			<false/>
			<true/>
			<dict>
				<key>string</key>
				<string>hello wolrd!</string>
			</dict>
		</array>
	</dict>
 *
 * path:
 *
 * 1. ".string" 			: hello wolrd!
 * 2. ".array[1]" 			: 31415926
 * 2. ".array[5].string" 	: hello wolrd!
 * 
 * </pre>
 *
 * @param object	the object pointer
 * @param path 		the object path
 * @param type 		the object type, check it if not TB_OBJECT_TYPE_NONE
 *
 *
 * <code>
 * id_object_t* 	object = id_object_seek(root, ".array[5].string", TB_OBJECT_TYPE_STRING);
 * if (object)
 * {
 * 		tb_print("%s", tb_string_cstr(object));
 * }
 * <endcode>
 *
 *
 * @return 			the object pointer
 */
tb_object_t* 		tb_object_seek(tb_object_t* object, tb_char_t const* path, tb_size_t type);

/*! dump the object
 *
 * @param object 	the object pointer
 *
 * @return 			the object pointer
 */
tb_object_t* 		tb_object_dump(tb_object_t* object);

/*! the object reference count
 *
 * @param object 	the object pointer
 *
 * @return 			the object reference count
 */
tb_size_t 			tb_object_ref(tb_object_t* object);

/*! increase the object reference count
 *
 * @param object 	the object pointer
 */
tb_void_t 			tb_object_inc(tb_object_t* object);

/*! decrease the object reference count, will free it if --refn == 0
 *
 * @param object 	the object pointer
 */
tb_void_t 			tb_object_dec(tb_object_t* object);

/*! set xml object reader
 *
 * @param type 		the object type name
 * @param func 		the reader func
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_object_set_xml_reader(tb_char_t const* type, tb_object_xml_reader_func_t func);

/*! get xml object reader
 *
 * @param type 		the object type name
 *
 * @return 			the reader func
 */
tb_pointer_t 		tb_object_get_xml_reader(tb_char_t const* type);

/*! set xml object writer
 *
 * @param type 		the object type
 * @param func 		the writer func
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_object_set_xml_writer(tb_size_t type, tb_object_xml_writer_func_t func);

/*! get xml object writer
 *
 * @param type 		the object type
 *
 * @return 			the writer func
 */
tb_pointer_t 		tb_object_get_xml_writer(tb_size_t type);

/*! set bin object reader
 *
 * @param type 		the object type
 * @param func 		the reader func
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_object_set_bin_reader(tb_size_t type, tb_object_bin_reader_func_t func);

/*! get bin object reader
 *
 * @param type 		the object type
 *
 * @return 			the reader func
 */
tb_pointer_t 		tb_object_get_bin_reader(tb_size_t type);

/*! set bin object writer
 *
 * @param type 		the object type
 * @param func 		the writer func
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_object_set_bin_writer(tb_size_t type, tb_object_bin_writer_func_t func);

/*! get bin object writer
 *
 * @param type 		the object type
 *
 * @return 			the writer func
 */
tb_pointer_t 		tb_object_get_bin_writer(tb_size_t type);

/*! set jsn object reader
 *
 * @param type 		the object type name
 * @param func 		the reader func
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_object_set_jsn_reader(tb_char_t type, tb_object_jsn_reader_func_t func);

/*! get jsn object reader
 *
 * @param type 		the object type name
 *
 * @return 			the reader func
 */
tb_pointer_t 		tb_object_get_jsn_reader(tb_char_t type);

/*! set jsn object writer
 *
 * @param type 		the object type
 * @param func 		the writer func
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_object_set_jsn_writer(tb_size_t type, tb_object_jsn_writer_func_t func);

/*! get jsn object writer
 *
 * @param type 		the object type
 *
 * @return 			the writer func
 */
tb_pointer_t 		tb_object_get_jsn_writer(tb_size_t type);

#endif

