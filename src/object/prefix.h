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
 * @file		prefix.h
 * @ingroup 	object
 *
 */
#ifndef TB_OBJECT_PREFIX_H
#define TB_OBJECT_PREFIX_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"
#include "../stream/stream.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/// the object type enum
typedef enum __tb_object_type_e
{
	TB_OBJECT_TYPE_NONE 		= 0
,	TB_OBJECT_TYPE_DATA 		= 1
,	TB_OBJECT_TYPE_DATE 		= 2
,	TB_OBJECT_TYPE_ARRAY 		= 3
,	TB_OBJECT_TYPE_STRING 		= 4
,	TB_OBJECT_TYPE_NUMBER 		= 5
,	TB_OBJECT_TYPE_BOOLEAN 		= 6
,	TB_OBJECT_TYPE_DICTIONARY 	= 7
,	TB_OBJECT_TYPE_USER 		= 8 //!< the user defined type, ...

}tb_object_type_e;

/// the object flag enum
typedef enum __tb_object_flag_e
{
	TB_OBJECT_FLAG_NONE 		= 0
,	TB_OBJECT_FLAG_READONLY 	= 1
,	TB_OBJECT_FLAG_SINGLETON 	= 2

}tb_object_flag_e;

/// the object type
typedef struct __tb_object_t
{
	/// the object flag
	tb_size_t 				flag 	: 1;

	/// the object type
	tb_size_t 				type 	: 31;

	/// the object reference count
	tb_size_t 				refn;

	/// the copy func
	struct __tb_object_t* 	(*copy)(struct __tb_object_t* object);

	/// the exit func
	tb_void_t 				(*exit)(struct __tb_object_t* object);

}tb_object_t;

#endif
