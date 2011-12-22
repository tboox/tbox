/*!The Treasure Platform Library
 * 
 * TPlat is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TPlat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TPlat; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		eobject.h
 *
 */
#ifndef TB_PLATFORM_EVENT_OBJECT_H
#define TB_PLATFORM_EVENT_OBJECT_H


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
, 	TB_ETYPE_CONN 	= 4 	//!< only for TB_EOTYPE_SOCK
, 	TB_ETYPE_ACPT 	= 8 	//!< only for TB_EOTYPE_SOCK
,	TB_ETYPE_SPAK 	= 16 	//!< only for TB_EOTYPE_EVET

}tb_etype_t;

// the event object type
typedef enum __tb_eotype_t
{
 	TB_EOTYPE_NULL 	= 0
, 	TB_EOTYPE_FILE 	= 1
,	TB_EOTYPE_SOCK 	= 2
,	TB_EOTYPE_HTTP 	= 3
,	TB_EOTYPE_GSTM 	= 4
,	TB_EOTYPE_EVET 	= 5
,	TB_EOTYPE_OTHR 	= 6

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

/// init the event object
tb_bool_t tb_eobject_init(tb_eobject_t* object);

/*!wait the event object
 *
 * blocking wait the single event object, so need not epool 
 * return the event type if ok, otherwise return 0 for timeout
 *
 * @param 	object 	the event object
 * @param 	timeout the timeout value, return immediately if 0, infinity if -1
 *
 * @return 	the event type, return 0 if timeout
 */
tb_size_t tb_eobject_wait(tb_eobject_t* object, tb_long_t timeout);

/*!kill the event object
 *
 * force to cancel waiting operation of the object
 * and tb_eobject_wait() return 0
 *
 * @param 	object 	the event object
 *
 * @return 	return TB_TRUE if cancel successfully
 */
tb_bool_t tb_eobject_kill(tb_eobject_t* object);

/*!spank the event object
 *
 * if the etype is TB_ETYPE_SPAK, force to cancel waiting operation of the object
 * and tb_eobject_wait() return TB_ETYPE_SPAK
 *
 * @param 	object 	the event object
 *
 * @return 	return TB_TRUE if cancel successfully
 */
tb_bool_t tb_eobject_spak(tb_eobject_t* object);



#endif
