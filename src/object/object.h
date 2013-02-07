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
#include "data.h"
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
 * @param format 	the object format
 *
 * @return 			the object pointer
 */
tb_object_t* 		tb_object_read(tb_gstream_t* gst, tb_size_t format);

/*! writ object
 *
 * @param object 	the object pointer
 * @param gst 		the stream
 * @param format 	the object format
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_object_writ(tb_object_t* object, tb_gstream_t* gst, tb_size_t format);

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

/*! the object description
 *
 * @param object 	the object pointer
 * @param data 		the description data
 * @param size 		the description maxn
 *
 * @return 			the object description string
 */
tb_char_t const*	tb_object_desc(tb_object_t* object, tb_char_t* data, tb_size_t maxn);

/*! dump the object
 *
 * @param object 	the object pointer
 */
tb_void_t 			tb_object_dump(tb_object_t* object);

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
 * @param type 		the reader type
 * @param func 		the reader func
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_object_set_xml_reader(tb_char_t const* type, tb_object_xml_reader_func_t func);

/*! get xml object reader
 *
 * @param type 		the reader type
 *
 * @return 			the reader func
 */
tb_pointer_t 		tb_object_get_xml_reader(tb_char_t const* type);

/*! set xml object writer
 *
 * @param type 		the writer type
 * @param func 		the writer func
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_object_set_xml_writer(tb_size_t type, tb_object_xml_writer_func_t func);

/*! get xml object writer
 *
 * @param type 		the writer type
 *
 * @return 			the writer func
 */
tb_pointer_t 		tb_object_get_xml_writer(tb_size_t type);

#endif

