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
 	TB_AIOO_ETYPE_NONE 	= 0 	//!< for null
, 	TB_AIOO_ETYPE_CONN 	= 1 	//!< for socket
, 	TB_AIOO_ETYPE_ACPT 	= 2 	//!< for socket
,	TB_AIOO_ETYPE_READ 	= 4		//!< for all i/o object
,	TB_AIOO_ETYPE_WRIT 	= 8		//!< for all i/o object
, 	TB_AIOO_ETYPE_EALL 	= TB_AIOO_ETYPE_READ | TB_AIOO_ETYPE_WRIT | TB_AIOO_ETYPE_ACPT | TB_AIOO_ETYPE_CONN //!< for all

}tb_aioo_etype_t;

// the object type
typedef enum __tb_aioo_otype_t
{
 	TB_AIOO_OTYPE_NONE 	= 0 	//!< for null
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

/*! set the aioo
 *
 * @param aioo 		the aioo
 * @param handle 	the aioo native handle
 * @param otype 	the aioo type
 * @param etype 	the aioo event type
 * @param odata 	the aioo data
 */
tb_void_t 			tb_aioo_seto(tb_aioo_t* aioo, tb_handle_t handle, tb_size_t otype, tb_size_t etype, tb_pointer_t odata);

/*! the aioo type
 *
 * @param aioo 		the aioo
 *
 * @return 			the aioo type
 */
tb_size_t 			tb_aioo_type(tb_aioo_t* aioo);

/*! the aioo event type
 *
 * @param aioo 		the aioo
 *
 * @return 			the aioo event type
 */
tb_size_t 			tb_aioo_gete(tb_aioo_t* aioo);

/*! set the aioo event type
 *
 * @param aioo 		the aioo
 * @param etype 	the aioo event type
 */
tb_void_t 			tb_aioo_sete(tb_aioo_t* aioo, tb_size_t etype);

/*! add the aioo event type
 *
 * add the event type by 'or' before waiting it
 *
 * @param aioo 		the aioo
 * @param type 		the aioo event type
 *
 * @return 			the aioo new event type
 */
tb_size_t 			tb_aioo_adde(tb_aioo_t* aioo, tb_size_t etype);

/*! del the aioo event type
 *
 * del the event type by 'and' before waiting it
 *
 * @param aioo 		the aioo
 * @param type 		the aioo event type
 *
 * @return 			the aioo new event type
 */
tb_size_t 			tb_aioo_dele(tb_aioo_t* aioo, tb_size_t etype);

/*! wait the aio aioo
 *
 * blocking wait the single event aioo, so need not aipp 
 * return the event type if ok, otherwise return 0 for timeout
 *
 * @param aioo 		the aio aioo
 * @param timeout 	the timeout value, return immediately if 0, infinity if -1
 *
 * @return 			the event type or timeout: 0 or error: -1
 */
tb_long_t 			tb_aioo_wait(tb_aioo_t* aioo, tb_long_t timeout);


#endif
