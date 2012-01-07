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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		eobject.h
 *
 */
#ifndef TB_EVENT_OBJECT_H
#define TB_EVENT_OBJECT_H


/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* /////////////////////////////////////////////////////////
 * types
 */

// the event type
typedef enum __tb_etype_t
{
 	TB_ETYPE_NULL 	= 0
,	TB_ETYPE_READ 	= 1		//!< for all i/o object
,	TB_ETYPE_WRIT 	= 2		//!< for all i/o object
, 	TB_ETYPE_ACPT 	= 4 	//!< only for TB_EOTYPE_SOCK
, 	TB_ETYPE_CONN 	= 8 	//!< only for TB_EOTYPE_SOCK
,	TB_ETYPE_SIGL 	= 16 	//!< only for TB_EOTYPE_EVET

}tb_etype_t;

// the event object type
typedef enum __tb_eotype_t
{
 	TB_EOTYPE_NULL 	= 0
, 	TB_EOTYPE_DATA 	= 1 	//!< for qbuffer
, 	TB_EOTYPE_FILE 	= 2 	//!< for file
,	TB_EOTYPE_SOCK 	= 3 	//!< for socket
,	TB_EOTYPE_HTTP 	= 4 	//!< for http
,	TB_EOTYPE_GSTM 	= 5 	//!< for gstream
,	TB_EOTYPE_EVET 	= 6 	//!< for event

}tb_eotype_t;

// the event object type
typedef struct __tb_eobject_t
{
	// the object type
	tb_size_t 		otype 	: 8;

	// the event type
	tb_size_t 		etype 	: 24;

	// the object handle
	tb_handle_t 	handle;

}tb_eobject_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */

/// set the event object
tb_void_t tb_eobject_seto(tb_eobject_t* object, tb_handle_t handle, tb_size_t otype, tb_size_t etype);

/// get the object type
tb_size_t tb_eobject_type(tb_eobject_t* object);

/// get the event type
tb_size_t tb_eobject_gete(tb_eobject_t* object);

/// set the event type
tb_size_t tb_eobject_sete(tb_eobject_t* object, tb_size_t etype);

/*!add the event type
 *
 * add the event type by 'or' before waiting it
 *
 * @param 	object 	the event object
 * @param 	type 	the event type
 *
 * @return 	the new event type
 */
tb_size_t tb_eobject_adde(tb_eobject_t* object, tb_size_t etype);

/*!delete the event type
 *
 * delete the event type by 'and' before waiting it
 *
 * @param 	object 	the event object
 * @param 	type 	the event type
 *
 * @return 	the new event type
 */
tb_size_t tb_eobject_dele(tb_eobject_t* object, tb_size_t etype);

/*!wait the event object
 *
 * blocking wait the single event object, so need not epool 
 * return the event type if ok, otherwise return 0 for timeout
 *
 * @param 	object 	the event object
 * @param 	timeout the timeout value, return immediately if 0, infinity if -1
 *
 * @return 	the event type, return 0 if timeout, return -1 if error
 */
tb_long_t tb_eobject_wait(tb_eobject_t* object, tb_long_t timeout);


#endif
