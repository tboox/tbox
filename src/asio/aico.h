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
 * @file		aico.h
 * @ingroup 	asio
 *
 */
#ifndef TB_ASIO_AICO_H
#define TB_ASIO_AICO_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../platform/prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

struct __tb_aice_t;
/// the callback type
typedef tb_bool_t (*tb_aicb_t)(struct __tb_aice_t const* aice);

/// the aico type enum
typedef enum __tb_aico_type_e
{
 	TB_AICO_TYPE_NONE 		= 0 	//!< for null
,	TB_AICO_TYPE_SOCK 		= 1 	//!< for sock
, 	TB_AICO_TYPE_FILE 		= 2 	//!< for file
, 	TB_AICO_TYPE_MAXN 		= 3

}tb_aico_type_e;

/// the aico timeout enum, only for sock
typedef enum __tb_aico_timeout_e
{
 	TB_AICO_TIMEOUT_ACPT 	= 0
,	TB_AICO_TIMEOUT_CONN 	= 1
, 	TB_AICO_TIMEOUT_RECV 	= 2
, 	TB_AICO_TIMEOUT_SEND 	= 3
, 	TB_AICO_TIMEOUT_MAXN 	= 4

}tb_aico_timeout_e;

/// the aico type
typedef struct __tb_aico_t
{
	/// the aicp
	tb_handle_t 		aicp;

	/// the type
	tb_size_t 			type;

	/// the handle
	tb_handle_t 		handle;

	/// the timeout for aice
	tb_atomic_t 		timeout[TB_AICO_TIMEOUT_MAXN];

	/// the post size, must be zero or one
#ifdef __tb_debug__
	tb_atomic_t 		post;
#endif

}tb_aico_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! the aico aicp
 *
 * @param aico 		the aico
 *
 * @return 			the aico aicp
 */
tb_handle_t 		tb_aico_aicp(tb_handle_t aico);

/*! the aico type
 *
 * @param aico 		the aico
 *
 * @return 			the aico type
 */
tb_size_t 			tb_aico_type(tb_handle_t aico);

/*! the aico handle
 *
 * @param aico 		the aico
 *
 * @return 			the aico handle
 */
tb_handle_t 		tb_aico_handle(tb_handle_t aico);

/*! the aico timeout
 *
 * @param aico 		the aico
 * @param type 		the timeout type
 *
 * @return 			the timeout
 */
tb_long_t 			tb_aico_timeout(tb_handle_t aico, tb_size_t type);

/*! set the aico timeout
 *
 * @param aico 		the aico
 * @param type 		the timeout type
 * @param timeout 	the timeout
 */
tb_void_t 			tb_aico_timeout_set(tb_handle_t aico, tb_size_t type, tb_long_t timeout);

/*! post the acpt event
 *
 * @param aico 		the aico
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_acpt(tb_handle_t aico, tb_aicb_t aicb_func, tb_cpointer_t aicb_data);

/*! post the conn event
 *
 * @param aico 		the aico
 * @param host 		the host
 * @param port 		the port
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_conn(tb_handle_t aico, tb_char_t const* host, tb_size_t port, tb_aicb_t aicb_func, tb_cpointer_t aicb_data);

/*! post the recv event for sock
 *
 * @param aico 		the aico
 * @param data 		the data
 * @param size 		the size
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_recv(tb_handle_t aico, tb_byte_t* data, tb_size_t size, tb_aicb_t aicb_func, tb_cpointer_t aicb_data);

/*! post the send event for file
 *
 * @param aico 		the aico
 * @param data 		the data
 * @param size 		the size, send the left file data if size == 0
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_send(tb_handle_t aico, tb_byte_t const* data, tb_size_t size, tb_aicb_t aicb_func, tb_cpointer_t aicb_data);

/*! post the recvv event for sock
 *
 * @param aico 		the aico
 * @param list 		the list
 * @param size 		the size
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_recvv(tb_handle_t aico, tb_iovec_t const* list, tb_size_t size, tb_aicb_t aicb_func, tb_cpointer_t aicb_data);

/*! post the sendv event for file
 *
 * @param aico 		the aico
 * @param list 		the list
 * @param size 		the size
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_sendv(tb_handle_t aico, tb_iovec_t const* list, tb_size_t size, tb_aicb_t aicb_func, tb_cpointer_t aicb_data);

/*! post the sendfile event for file
 *
 * @param aico 		the aico
 * @param file 		the file
 * @param seek 		the seek
 * @param size 		the size, send the left data if size == 0
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_sendfile(tb_handle_t aico, tb_handle_t file, tb_hize_t seek, tb_hize_t size, tb_aicb_t aicb_func, tb_cpointer_t aicb_data);

/*! post the read event for file
 *
 * @param aico 		the aico
 * @param seek 		the seek
 * @param data 		the data
 * @param size 		the size
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_read(tb_handle_t aico, tb_hize_t seek, tb_byte_t* data, tb_size_t size, tb_aicb_t aicb_func, tb_cpointer_t aicb_data);

/*! post the writ event for file
 *
 * @param aico 		the aico
 * @param seek 		the seek
 * @param data 		the data
 * @param size 		the size
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_writ(tb_handle_t aico, tb_hize_t seek, tb_byte_t const* data, tb_size_t size, tb_aicb_t aicb_func, tb_cpointer_t aicb_data);

/*! post the readv event for file
 *
 * @param aico 		the aico
 * @param seek 		the seek
 * @param list 		the list
 * @param size 		the size
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_readv(tb_handle_t aico, tb_hize_t seek, tb_iovec_t const* list, tb_size_t size, tb_aicb_t aicb_func, tb_cpointer_t aicb_data);

/*! post the writv event for file
 *
 * @param aico 		the aico
 * @param seek 		the seek
 * @param list 		the list
 * @param size 		the size
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_writv(tb_handle_t aico, tb_hize_t seek, tb_iovec_t const* list, tb_size_t size, tb_aicb_t aicb_func, tb_cpointer_t aicb_data);

/*! post the fsync event for file
 *
 * @param aico 		the aico
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_fsync(tb_handle_t aico, tb_aicb_t aicb_func, tb_cpointer_t aicb_data);

/* ///////////////////////////////////////////////////////////////////////
 * inline
 */
static __tb_inline__ tb_long_t tb_aico_timeout_from_code(tb_handle_t aico, tb_size_t code)
{
	// init the timeout type
	static tb_size_t type[] = 
	{
		-1

	, 	TB_AICO_TIMEOUT_ACPT
	, 	TB_AICO_TIMEOUT_CONN
	, 	TB_AICO_TIMEOUT_RECV
	, 	TB_AICO_TIMEOUT_SEND
	, 	TB_AICO_TIMEOUT_RECV
	, 	TB_AICO_TIMEOUT_SEND
	, 	TB_AICO_TIMEOUT_SEND

	, 	-1
	, 	-1
	, 	-1
	, 	-1
	, 	-1
	};
	tb_assert_and_check_return_val(code < tb_arrayn(type) && type[code] != (tb_size_t)-1, -1);

	// timeout
	return tb_aico_timeout(aico, type[code]);
}


#endif
