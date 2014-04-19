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
 * @file		object.h
 * @defgroup 	object
 *
 */
#ifndef TB_OBJECT_H
#define TB_OBJECT_H

/* //////////////////////////////////////////////////////////////////////////////////////
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
#include "object_pool.h"
#include "reader/reader.h"
#include "writer/writer.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init object context
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_object_context_init(tb_noarg_t);

/// exit object context
tb_void_t 			tb_object_context_exit(tb_noarg_t);

/*! init object
 *
 * @param object 	the object
 * @param flag 		the object flag
 * @param type 		the object type
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_object_init(tb_object_t* object, tb_size_t flag, tb_size_t type);

/*! exit object
 *
 * @param object 	the object
 *
 * @note the reference count must be one
 */
tb_void_t 			tb_object_exit(tb_object_t* object);

/*! cler object
 *
 * @param object 	the object
 */
tb_void_t 			tb_object_cler(tb_object_t* object);

/*! set the object private data
 *
 * @param object 	the object
 * @param priv 		the private data
 *
 */
tb_void_t 			tb_object_setp(tb_object_t* object, tb_pointer_t priv);

/*! get the object private data
 *
 * @param object 	the object
 *
 * @return 			the private data
 */
tb_pointer_t 		tb_object_getp(tb_object_t* object);

/*! read object
 *
 * @param stream 	the stream
 *
 * @return 			the object
 */
tb_object_t* 		tb_object_read(tb_basic_stream_t* stream);

/*! read object from url
 *
 * @param url 		the url
 *
 * @return 			the object
 */
tb_object_t* 		tb_object_read_from_url(tb_char_t const* url);

/*! read object from data
 *
 * @param data 		the data
 * @param size 		the size
 *
 * @return 			the object
 */
tb_object_t* 		tb_object_read_from_data(tb_byte_t const* data, tb_size_t size);

/*! writ object
 *
 * @param object 	the object
 * @param stream 	the stream
 * @param format 	the object format
 *
 * @return 			the writed size, failed: -1
 */
tb_long_t 			tb_object_writ(tb_object_t* object, tb_basic_stream_t* stream, tb_size_t format);

/*! writ object to url
 *
 * @param object	the object
 * @param url 		the url
 * @param format 	the format
 *
 * @return 			the writed size, failed: -1
 */
tb_long_t 			tb_object_writ_to_url(tb_object_t* object, tb_char_t const* url, tb_size_t format);

/*! writ object to data
 *
 * @param object	the object
 * @param data 		the data
 * @param size 		the size
 * @param format 	the format
 *
 * @return 			the writed size, failed: -1
 */
tb_long_t 			tb_object_writ_to_data(tb_object_t* object, tb_byte_t* data, tb_size_t size, tb_size_t format);

/*! copy object
 *
 * @param object 	the object
 *
 * @return 			the object copy
 */
tb_object_t* 		tb_object_copy(tb_object_t* object);

/*! the object type
 *
 * @param object 	the object
 *
 * @return 			the object type
 */
tb_size_t 			tb_object_type(tb_object_t* object);

/*! the object data
 *
 * @param object	the object
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

		<key>com.xxx.xxx</key>
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
 * 3. ".array[5].string" 	: hello wolrd!
 * 4. ".com\\.xxx\\.xxx" 	: hello wolrd!
 * 
 * </pre>
 *
 * @param object	the object
 * @param path 		the object path
 * @param type 		the object type, check it if not TB_OBJECT_TYPE_NONE
 *
 *
 * <code>
 * tb_object_t* 	object = tb_object_seek(root, ".array[5].string", TB_OBJECT_TYPE_STRING);
 * if (object)
 * {
 * 		tb_trace_d("%s", tb_string_cstr(object));
 * }
 * <endcode>
 *
 *
 * @return 			the object
 */
tb_object_t* 		tb_object_seek(tb_object_t* object, tb_char_t const* path, tb_size_t type);

/*! dump the object
 *
 * @param object 	the object
 *
 * @return 			the object
 */
tb_object_t* 		tb_object_dump(tb_object_t* object);

/*! the object reference count
 *
 * @param object 	the object
 *
 * @return 			the object reference count
 */
tb_size_t 			tb_object_ref(tb_object_t* object);

/*! increase the object reference count
 *
 * @param object 	the object
 */
tb_void_t 			tb_object_inc(tb_object_t* object);

/*! decrease the object reference count, will free it if --refn == 0
 *
 * @param object 	the object
 */
tb_void_t 			tb_object_dec(tb_object_t* object);

#endif

