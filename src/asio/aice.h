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
 * @file		aice.h
 * @ingroup 	asio
 *
 */
#ifndef TB_ASIO_AICE_H
#define TB_ASIO_AICE_H


/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

struct __tb_aice_t;
struct __tb_aicp_t;
/// the callback type
typedef tb_bool_t (*tb_aicb_t)(struct __tb_aicp_t* aicp, struct __tb_aice_t const* aice);

/// the aice code enum
typedef enum __tb_aice_code_e
{
 	TB_AICE_CODE_NULL 		= 0 	//!< for null
, 	TB_AICE_CODE_ACPT 		= 1 	//!< for sock
, 	TB_AICE_CODE_CONN 		= 2 	//!< for sock
,	TB_AICE_CODE_RECV 		= 3		//!< for sock
,	TB_AICE_CODE_SEND 		= 4		//!< for sock
,	TB_AICE_CODE_READ 		= 5		//!< for file
,	TB_AICE_CODE_WRIT 		= 6		//!< for file
, 	TB_AICE_CODE_MAXN 		= 7

}tb_aice_code_e;

/// the aice state code enum
typedef enum __tb_aice_state_e
{
 	TB_AICE_STATE_OK 					= 0
, 	TB_AICE_STATE_FAILED 				= 1
,	TB_AICE_STATE_CLOSED 				= 2
,	TB_AICE_STATE_TIMEOUT 				= 3

}tb_aice_state_e;

/// the acpt aice type
typedef struct __tb_aice_acpt_t
{
	/// the client socket 
	tb_handle_t 			sock;

}tb_aice_acpt_t;

/// the conn aice type
typedef struct __tb_aice_conn_t
{
	/// the port
	tb_size_t 				port;

	/// the host, @note: reference only
	tb_char_t const* 		host;

}tb_aice_conn_t;

/// the read aice type
typedef struct __tb_aice_read_t
{
	/// the read data
	tb_byte_t* 				data;

	/// the data size
	tb_size_t 				size;

	/// the data real
	tb_size_t 				real;

	/// the file seek
	tb_hize_t 				seek;

}tb_aice_read_t;

/// the writ aice type
typedef struct __tb_aice_writ_t
{
	/// the writ data
	tb_byte_t* 				data;

	/// the data size
	tb_size_t 				size;

	/// the data real
	tb_size_t 				real;

	/// the file seek
	tb_hize_t 				seek;

}tb_aice_writ_t;

/// the recv aice type
typedef struct __tb_aice_recv_t
{
	/// the recv data
	tb_byte_t* 				data;

	/// the data size
	tb_size_t 				size;

	/// the data real
	tb_size_t 				real;

}tb_aice_recv_t;

/// the send aice type
typedef struct __tb_aice_send_t
{
	/// the send data
	tb_byte_t* 				data;

	/// the data size
	tb_size_t 				size;

	/// the data real
	tb_size_t 				real;

}tb_aice_send_t;

/// the aice type
typedef struct __tb_aice_t
{
	/// the aice code
	tb_size_t 				code 	: 8;

	/// the state
	tb_size_t 				state 	: 8;

	/// the aico type
	tb_size_t 				type 	: 8;

	/// the handle
	tb_handle_t 			handle;

	/// the aicb
	tb_aicb_t 				aicb;

	/// the data
	tb_cpointer_t 			data;

	/// the uion
	union
	{
		tb_aice_acpt_t 		acpt;
		tb_aice_conn_t 		conn;
		tb_aice_read_t 		read;
		tb_aice_writ_t 		writ;
		tb_aice_recv_t 		recv;
		tb_aice_send_t 		send;

	} u;

}tb_aice_t;



#endif
