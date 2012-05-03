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
 * @file		aioo.h
 * @ingroup 	aio
 *
 */
#ifndef TB_AIO_OBJECT_H
#define TB_AIO_OBJECT_H


/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the event type
typedef enum __tb_aioo_etype_t
{
 	TB_AIOO_ETYPE_NULL 	= 0 	//!< for null
, 	TB_AIOO_ETYPE_CONN 	= 1 	//!< for socket
, 	TB_AIOO_ETYPE_ACPT 	= 2 	//!< for socket
,	TB_AIOO_ETYPE_READ 	= 4		//!< for all i/o object
,	TB_AIOO_ETYPE_WRIT 	= 8		//!< for all i/o object
, 	TB_AIOO_ETYPE_EALL 	= TB_AIOO_ETYPE_READ | TB_AIOO_ETYPE_WRIT | TB_AIOO_ETYPE_ACPT | TB_AIOO_ETYPE_CONN //!< for all

}tb_aioo_etype_t;

// the object type
typedef enum __tb_aioo_otype_t
{
 	TB_AIOO_OTYPE_NULL 	= 0 	//!< for null
, 	TB_AIOO_OTYPE_FILE 	= 1 	//!< for file
,	TB_AIOO_OTYPE_SOCK 	= 2 	//!< for socket

}tb_aioo_otype_t;

// the aio object type
typedef struct __tb_aioo_t
{
	// the object type
	tb_size_t 		otype 	: 8;

	// the event type
	tb_size_t 		etype 	: 24;

	// the object data
	tb_pointer_t 	odata;

	// the object handle
	tb_handle_t 	handle;

}tb_aioo_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/// set the aio object
tb_void_t tb_aioo_seto(tb_aioo_t* object, tb_handle_t handle, tb_size_t otype, tb_size_t etype, tb_pointer_t odata);

/// get the object type
tb_size_t tb_aioo_type(tb_aioo_t* object);

/// get the event type
tb_size_t tb_aioo_gete(tb_aioo_t* object);

/// set the event type
tb_size_t tb_aioo_sete(tb_aioo_t* object, tb_size_t etype);

/*!add the event type
 *
 * add the event type by 'or' before waiting it
 *
 * @param 	object 	the aio object
 * @param 	type 	the event type
 *
 * @return 	the new event type
 */
tb_size_t tb_aioo_adde(tb_aioo_t* object, tb_size_t etype);

/*!delete the event type
 *
 * delete the event type by 'and' before waiting it
 *
 * @param 	object 	the aio object
 * @param 	type 	the event type
 *
 * @return 	the new event type
 */
tb_size_t tb_aioo_dele(tb_aioo_t* object, tb_size_t etype);

/*!wait the aio object
 *
 * blocking wait the single event object, so need not aipp 
 * return the event type if ok, otherwise return 0 for timeout
 *
 * @param 	object 	the aio object
 * @param 	timeout the timeout value, return immediately if 0, infinity if -1
 *
 * @return 	the event type
 *          return 0 if timeout
 *          return -1 if error
 *          return the need event type if object->etype == TB_AIOO_ETYPE_NULL
 */
tb_long_t tb_aioo_wait(tb_aioo_t* object, tb_long_t timeout);


#endif
