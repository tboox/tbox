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
 * \file		object.h
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
, 	TB_ETYPE_OPEN 	= 1
,	TB_ETYPE_READ 	= 2
,	TB_ETYPE_WRIT 	= 4
,	TB_ETYPE_CLOS 	= 8

}tb_etype_t;

// the event object type
typedef enum __tb_eotype_t
{
 	TB_EOTYPE_NULL 	= 0
, 	TB_EOTYPE_DATA 	= 1
, 	TB_EOTYPE_FILE 	= 2
,	TB_EOTYPE_SOCK 	= 3
,	TB_EOTYPE_HTTP 	= 4
,	TB_EOTYPE_GSTM 	= 5
,	TB_EOTYPE_MUTX 	= 6
,	TB_EOTYPE_OTHR 	= 7

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

	// the callback

}tb_eobject_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */

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

#endif
