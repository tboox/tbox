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
 * @file		astream.h
 * @ingroup 	stream
 *
 */
#ifndef TB_STREAM_ASTREAM_H
#define TB_STREAM_ASTREAM_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../asio/asio.h"
#include "../libc/libc.h"
#include "../network/url.h"
#include "../memory/memory.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

/// the stream command
#define TB_ASTREAM_CTRL(type, ctrl) 								(((type) << 16) | (ctrl))
#define TB_ASTREAM_CTRL_FLTR(type, ctrl) 							TB_ASTREAM_CTRL(TB_ASTREAM_TYPE_FLTR, (((type) << 8) | (ctrl)))

/// the stream state
#define TB_ASTREAM_STATE(type, state) 								(((type) << 16) | (state))

/// open
#define tb_astream_open(astream, func, priv) 						tb_astream_open_impl(astream, func, priv __tb_debug_vals__)

/// read
#define tb_astream_read(astream, func, priv) 						tb_astream_read_impl(astream, func, priv __tb_debug_vals__)

/// writ
#define tb_astream_writ(astream, data, size, func, priv) 			tb_astream_writ_impl(astream, data, size, func, priv __tb_debug_vals__)

/// seek
#define tb_astream_seek(astream, offset, func, priv) 				tb_astream_seek_impl(astream, offset, func, priv __tb_debug_vals__)

/// sync
#define tb_astream_sync(astream, func, priv) 						tb_astream_sync_impl(astream, func, priv __tb_debug_vals__)

/// open and read
#define tb_astream_oread(astream, func, priv) 						tb_astream_oread_impl(astream, func, priv __tb_debug_vals__)

/// open and writ
#define tb_astream_owrit(astream, data, size, func, priv) 			tb_astream_owrit_impl(astream, data, size, func, priv __tb_debug_vals__)

/// open and seek
#define tb_astream_oseek(astream, offset, func, priv) 				tb_astream_oseek_impl(astream, offset, func, priv __tb_debug_vals__)

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/// the astream enum
typedef enum __tb_astream_type_e
{
 	TB_ASTREAM_TYPE_NONE 					= 0
, 	TB_ASTREAM_TYPE_FILE 					= 1
, 	TB_ASTREAM_TYPE_SOCK 					= 2
, 	TB_ASTREAM_TYPE_HTTP 					= 3
, 	TB_ASTREAM_TYPE_FLTR 					= 4

}tb_astream_type_e;

/// the astream filter enum
typedef enum __tb_astream_filter_type_e
{
 	TB_ASTREAM_FLTR_TYPE_NONE 				= 0
, 	TB_ASTREAM_FLTR_TYPE_CHARSET 			= 1
, 	TB_ASTREAM_FLTR_TYPE_ZIP 				= 2
, 	TB_ASTREAM_FLTR_TYPE_CHUNKED 			= 3

}tb_astream_filter_type_e;

/// the astream ctrl enum
typedef enum __tb_astream_ctrl_e
{
	TB_ASTREAM_CTRL_NONE 					= 0

	// the astream
,	TB_ASTREAM_CTRL_GET_URL 				= TB_ASTREAM_CTRL(TB_ASTREAM_TYPE_NONE, 1)
,	TB_ASTREAM_CTRL_GET_HOST 				= TB_ASTREAM_CTRL(TB_ASTREAM_TYPE_NONE, 2)
,	TB_ASTREAM_CTRL_GET_PORT 				= TB_ASTREAM_CTRL(TB_ASTREAM_TYPE_NONE, 3)
,	TB_ASTREAM_CTRL_GET_PATH 				= TB_ASTREAM_CTRL(TB_ASTREAM_TYPE_NONE, 4)
,	TB_ASTREAM_CTRL_GET_TIMEOUT 			= TB_ASTREAM_CTRL(TB_ASTREAM_TYPE_NONE, 5)
,	TB_ASTREAM_CTRL_GET_SIZE 				= TB_ASTREAM_CTRL(TB_ASTREAM_TYPE_NONE, 6)
,	TB_ASTREAM_CTRL_GET_OFFSET 				= TB_ASTREAM_CTRL(TB_ASTREAM_TYPE_NONE, 7)
,	TB_ASTREAM_CTRL_IS_OPENED 				= TB_ASTREAM_CTRL(TB_ASTREAM_TYPE_NONE, 8)

,	TB_ASTREAM_CTRL_SET_URL 				= TB_ASTREAM_CTRL(TB_ASTREAM_TYPE_NONE, 11)
,	TB_ASTREAM_CTRL_SET_HOST 				= TB_ASTREAM_CTRL(TB_ASTREAM_TYPE_NONE, 12)
,	TB_ASTREAM_CTRL_SET_PORT 				= TB_ASTREAM_CTRL(TB_ASTREAM_TYPE_NONE, 13)
,	TB_ASTREAM_CTRL_SET_PATH 				= TB_ASTREAM_CTRL(TB_ASTREAM_TYPE_NONE, 14)
,	TB_ASTREAM_CTRL_SET_TIMEOUT 			= TB_ASTREAM_CTRL(TB_ASTREAM_TYPE_NONE, 15)

	// the astream for file
,	TB_ASTREAM_CTRL_FILE_SET_MODE 			= TB_ASTREAM_CTRL(TB_ASTREAM_TYPE_FILE, 1)
,	TB_ASTREAM_CTRL_FILE_SET_HANDLE 		= TB_ASTREAM_CTRL(TB_ASTREAM_TYPE_FILE, 2)
,	TB_ASTREAM_CTRL_FILE_GET_HANDLE 		= TB_ASTREAM_CTRL(TB_ASTREAM_TYPE_FILE, 3)

	// the astream for sock
,	TB_ASTREAM_CTRL_SOCK_SET_TYPE 			= TB_ASTREAM_CTRL(TB_ASTREAM_TYPE_SOCK, 1)
,	TB_ASTREAM_CTRL_SOCK_SET_HANDLE 		= TB_ASTREAM_CTRL(TB_ASTREAM_TYPE_SOCK, 2)
,	TB_ASTREAM_CTRL_SOCK_GET_HANDLE 		= TB_ASTREAM_CTRL(TB_ASTREAM_TYPE_SOCK, 3)


}tb_astream_ctrl_e;

/// the astream state enum
typedef enum __tb_astream_state_e
{
	TB_ASTREAM_STATE_OK 					= 0
,	TB_ASTREAM_STATE_CLOSED 				= 1
,	TB_ASTREAM_STATE_KILLED 				= 2
,	TB_ASTREAM_STATE_NOT_SUPPORTED 			= 3
,	TB_ASTREAM_STATE_UNKNOWN_ERROR 			= 4

,	TB_ASTREAM_SOCK_STATE_DNS_FAILED 		= TB_ASTREAM_STATE(TB_ASTREAM_TYPE_SOCK, 1)
,	TB_ASTREAM_SOCK_STATE_CONNECT_FAILED	= TB_ASTREAM_STATE(TB_ASTREAM_TYPE_SOCK, 2)
,	TB_ASTREAM_SOCK_STATE_CONNECT_TIMEOUT	= TB_ASTREAM_STATE(TB_ASTREAM_TYPE_SOCK, 3)
,	TB_ASTREAM_SOCK_STATE_RECV_TIMEOUT		= TB_ASTREAM_STATE(TB_ASTREAM_TYPE_SOCK, 4)
,	TB_ASTREAM_SOCK_STATE_SEND_TIMEOUT		= TB_ASTREAM_STATE(TB_ASTREAM_TYPE_SOCK, 5)

}tb_astream_state_e;

/// the asio stream declaration
struct __tb_astream_t;

/*! the astream open func type
 *
 * @param astream 			the astream
 * @param state 			the stream state
 * @param priv 				the func private data
 *
 * @return 					tb_true: ok and continue it if need, tb_false: break it, but not break aicp
 */
typedef tb_bool_t 			(*tb_astream_open_func_t)(struct __tb_astream_t* astream, tb_size_t state, tb_pointer_t priv);

/*! the astream read func type
 *
 * @param astream 			the astream
 * @param state 			the stream state
 * @param data 				the readed data
 * @param size 				the readed size
 * @param priv 				the func private data
 *
 * @return 					tb_true: ok and continue it if need, tb_false: break it, but not break aicp
 */
typedef tb_bool_t 			(*tb_astream_read_func_t)(struct __tb_astream_t* astream, tb_size_t state, tb_byte_t const* data, tb_size_t size, tb_pointer_t priv);

/*! the astream writ func type
 *
 * @param astream 			the astream
 * @param state 			the stream state
 * @param real 				the real size
 * @param size 				the need size
 * @param priv 				the func private data
 *
 * @return 					tb_true: ok and continue it if need, tb_false: break it, but not break aicp
 */
typedef tb_bool_t 			(*tb_astream_writ_func_t)(struct __tb_astream_t* astream, tb_size_t state, tb_size_t real, tb_size_t size, tb_pointer_t priv);

/*! the astream seek func type
 *
 * @param astream 			the astream
 * @param state 			the stream state
 * @param offset 			the real offset
 * @param priv 				the func private data
 *
 * @return 					tb_true: ok and continue it if need, tb_false: break it, but not break aicp
 */
typedef tb_bool_t 			(*tb_astream_seek_func_t)(struct __tb_astream_t* astream, tb_size_t state, tb_hize_t offset, tb_pointer_t priv);

/*! the astream sync func type
 *
 * @param astream 			the astream
 * @param state 			the stream state
 * @param priv 				the func private data
 *
 * @return 					tb_true: ok and continue it if need, tb_false: break it, but not break aicp
 */
typedef tb_bool_t 			(*tb_astream_sync_func_t)(struct __tb_astream_t* astream, tb_size_t state, tb_pointer_t priv);

/// the astream open and read type
typedef struct __tb_astream_oread_t
{
	// the func
	tb_astream_read_func_t 	func;

	// the priv
	tb_pointer_t 			priv;

}tb_astream_oread_t;

/// the astream open and writ type
typedef struct __tb_astream_owrit_t
{
	// the func
	tb_astream_writ_func_t 	func;

	// the priv
	tb_pointer_t 			priv;

	// the data
	tb_byte_t const* 		data;

	// the size
	tb_size_t 				size;

}tb_astream_owrit_t;

/// the astream open and seek type
typedef struct __tb_astream_oseek_t
{
	// the func
	tb_astream_seek_func_t 	func;

	// the priv
	tb_pointer_t 			priv;

	// the offset
	tb_hize_t 				offset;

}tb_astream_oseek_t;

/// the asio stream type
typedef struct __tb_astream_t
{	
	/// the url
	tb_url_t 				url;

	/// the aicp
	tb_aicp_t* 				aicp;

	/// the type
	tb_uint8_t 				type;

	// is opened?
	tb_atomic_t 			opened;

	// is stoped?
	tb_atomic_t 			stoped;

	// the timeout
	tb_long_t 				timeout;

#ifdef __tb_debug__
	/// the func
	tb_char_t const* 		func;

	/// the file
	tb_char_t const* 		file;

	/// the line
	tb_size_t 				line;
#endif

	/// the open and read, writ, seek, ...
	union
	{
		tb_astream_oread_t 	read;
		tb_astream_owrit_t 	writ;
		tb_astream_oseek_t 	seek;

	} 						open_and;

	/// open
	tb_bool_t 				(*open)(struct __tb_astream_t* astream, tb_astream_open_func_t func, tb_pointer_t priv);

	/// read
	tb_bool_t 				(*read)(struct __tb_astream_t* astream, tb_astream_read_func_t func, tb_pointer_t priv);

	/// writ
	tb_bool_t 				(*writ)(struct __tb_astream_t* astream, tb_byte_t const* data, tb_size_t size, tb_astream_writ_func_t func, tb_pointer_t priv);

	/// seek
	tb_bool_t 				(*seek)(struct __tb_astream_t* astream, tb_hize_t offset, tb_astream_seek_func_t func, tb_pointer_t priv);

	/// sync
	tb_bool_t 				(*sync)(struct __tb_astream_t* astream, tb_astream_sync_func_t func, tb_pointer_t priv);

	/// kill
	tb_void_t 				(*kill)(struct __tb_astream_t* astream);

	/// exit
	tb_void_t 				(*exit)(struct __tb_astream_t* astream);

	/// ctrl
	tb_bool_t 				(*ctrl)(struct __tb_astream_t* astream, tb_size_t ctrl, tb_va_list_t args);

}tb_astream_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init file stream 
 *
 * @param aicp 		the aicp
 *
 * @return 			the stream
 */
tb_astream_t* 		tb_astream_init_file(tb_aicp_t* aicp);

/*! init sock stream 
 *
 * @param aicp 		the aicp
 *
 * @return 			the stream
 */
tb_astream_t* 		tb_astream_init_sock(tb_aicp_t* aicp);

/*! init http stream 
 *
 * @param aicp 		the aicp
 *
 * @return 			the stream
 */
tb_astream_t* 		tb_astream_init_http(tb_aicp_t* aicp);

/*! exit stream
 *
 * @param astream 	the stream
 */
tb_void_t 			tb_astream_exit(tb_astream_t* astream);

/*! init stream from url
 *
 * @param aicp 		the aicp
 * @param url 		the url
 * <pre>
 * file://path or unix path: e.g. /root/xxxx/file
 * sock://host:port?tcp=
 * sock://host:port?udp=
 * http://host:port/path?arg0=&arg1=...
 * </pre>
 *
 * @return 			the stream
 */
tb_astream_t* 		tb_astream_init_from_url(tb_aicp_t* aicp, tb_char_t const* url);

/*! init stream from file
 *
 * @param aicp 		the aicp
 * @param path 		the file path
 *
 * @return 			the stream
 */
tb_astream_t* 		tb_astream_init_from_file(tb_aicp_t* aicp, tb_char_t const* path);

/*! init stream from sock
 *
 * @param aicp 		the aicp
 * @param host 		the host
 * @param port 		the port
 * @param type 		the socket type, tcp or udp
 * @param bssl 		enable ssl?
 *
 * @return 			the stream
 */
tb_astream_t* 		tb_astream_init_from_sock(tb_aicp_t* aicp, tb_char_t const* host, tb_size_t port, tb_size_t type, tb_bool_t bssl);

/*! init stream from http or https
 *
 * @param aicp 		the aicp
 * @param host 		the host
 * @param port 		the port
 * @param path 		the path
 * @param bssl 		enable ssl?
 *
 * @return 			the stream
 */
tb_astream_t* 		tb_astream_init_from_http(tb_aicp_t* aicp, tb_char_t const* host, tb_size_t port, tb_char_t const* path, tb_bool_t bssl);

/*! kill stream
 *
 * @param astream 	the stream
 */
tb_void_t 			tb_astream_kill(tb_astream_t* astream);

/*! open the stream 
 *
 * @param astream 	the stream
 * @param func 		the func
 * @param priv 		the func data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_astream_open_impl(tb_astream_t* astream, tb_astream_open_func_t func, tb_pointer_t priv __tb_debug_decl__);

/*! read the stream 
 *
 * @param astream 	the stream
 * @param func 		the func
 * @param priv 		the func data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_astream_read_impl(tb_astream_t* astream, tb_astream_read_func_t func, tb_pointer_t priv __tb_debug_decl__);

/*! writ the stream 
 *
 * @param astream 	the stream
 * @param data 		the data
 * @param size 		the size
 * @param func 		the func
 * @param priv 		the func data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_astream_writ_impl(tb_astream_t* astream, tb_byte_t const* data, tb_size_t size, tb_astream_writ_func_t func, tb_pointer_t priv __tb_debug_decl__);

/*! seek the stream
 *
 * @param astream 	the stream
 * @param offset 	the offset
 * @param func 		the func
 * @param priv 		the func data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_astream_seek_impl(tb_astream_t* astream, tb_hize_t offset, tb_astream_seek_func_t func, tb_pointer_t priv __tb_debug_decl__);

/*! sync the stream
 *
 * @note will be block returned if func and priv is tb_null
 *
 * @param astream 	the stream
 * @param func 		the func
 * @param priv 		the func data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_astream_sync_impl(tb_astream_t* astream, tb_astream_sync_func_t func, tb_pointer_t priv __tb_debug_decl__);

/*! open and read the stream, open it first if not opened 
 *
 * @param astream 	the stream
 * @param func 		the func
 * @param priv 		the func data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_astream_oread_impl(tb_astream_t* astream, tb_astream_read_func_t func, tb_pointer_t priv __tb_debug_decl__);

/*! open and writ the stream, open it first if not opened 
 *
 * @param astream 	the stream
 * @param data 		the data
 * @param size 		the size
 * @param func 		the func
 * @param priv 		the func data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_astream_owrit_impl(tb_astream_t* astream, tb_byte_t const* data, tb_size_t size, tb_astream_writ_func_t func, tb_pointer_t priv __tb_debug_decl__);

/*! open and seek the stream, open it first if not opened 
 *
 * @param astream 	the stream
 * @param offset 	the offset
 * @param func 		the func
 * @param priv 		the func data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_astream_oseek_impl(tb_astream_t* astream, tb_hize_t offset, tb_astream_seek_func_t func, tb_pointer_t priv __tb_debug_decl__);

/*! the stream aicp
 *
 * @param astream 	the stream
 *
 * @return 			the stream aicp
 */
tb_aicp_t* 			tb_astream_aicp(tb_astream_t* astream);

/*! the stream type
 *
 * @param astream 	the stream
 *
 * @return 			the stream type
 */
tb_size_t 			tb_astream_type(tb_astream_t const* astream);

/*! the stream size
 *
 * @param astream 	the stream
 *
 * @return 			the stream size
 */
tb_hize_t 			tb_astream_size(tb_astream_t const* astream);

/*! the stream left size
 *
 * @param astream 	the stream
 *
 * @return 			the stream left size, failed: -1
 */
tb_hong_t 			tb_astream_left(tb_astream_t const* astream);

/*! the stream offset
 *
 * @param astream 	the stream
 *
 * @return 			the stream offset, failed: -1
 */
tb_hong_t 			tb_astream_offset(tb_astream_t const* astream);

/*! the stream timeout
 *
 * @param astream 	the stream
 *
 * @return 			the stream timeout
 */
tb_size_t 			tb_astream_timeout(tb_astream_t const* astream);

/*! the stream state c-string
 *
 * @param state 	the state
 *
 * @return 			the stream state c-string
 */
tb_char_t const* 	tb_astream_state_cstr(tb_size_t state);

/*! ctrl stream
 *
 * @param astream 	the stream
 * @param ctrl 		the ctrl command
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_astream_ctrl(tb_astream_t* astream, tb_size_t ctrl, ...);

#ifdef __tb_debug__
/*! the stream func name from post for debug
 *
 * @param astream 	the stream
 *
 * @return 			the stream func name
 */
tb_char_t const* 	tb_astream_func(tb_astream_t* astream);

/*! the stream file name from post for debug
 *
 * @param astream 	the stream
 *
 * @return 			the stream file name
 */
tb_char_t const* 	tb_astream_file(tb_astream_t* astream);

/*! the stream line number from post for debug
 *
 * @param astream 	the stream
 *
 * @return 			the stream line number
 */
tb_size_t 			tb_astream_line(tb_astream_t* astream);
#endif

#endif
