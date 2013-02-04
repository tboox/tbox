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

/*! load object
 *
 * @param gst 		the stream
 * @param format 	the object format
 *
 * @return 			the object pointer
 */
tb_object_t* 		tb_object_load(tb_gstream_t* gst, tb_size_t format);

/*! save object
 *
 * @param object 	the object pointer
 * @param gst 		the stream
 * @param format 	the object format
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_object_save(tb_object_t* object, tb_gstream_t* gst, tb_size_t format);

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

#endif

