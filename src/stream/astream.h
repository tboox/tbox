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

/// open
#define tb_astream_open(astream, func, priv) 									tb_astream_open_impl(astream, func, priv __tb_debug_vals__)

/// read
#define tb_astream_read(astream, maxn, func, priv) 								tb_astream_read_impl(astream, maxn, func, priv __tb_debug_vals__)

/// writ
#define tb_astream_writ(astream, data, size, func, priv) 						tb_astream_writ_impl(astream, data, size, func, priv __tb_debug_vals__)

/// seek
#define tb_astream_seek(astream, offset, func, priv) 							tb_astream_seek_impl(astream, offset, func, priv __tb_debug_vals__)

/// sync
#define tb_astream_sync(astream, bclosing, func, priv) 							tb_astream_sync_impl(astream, bclosing, func, priv __tb_debug_vals__)

/// task
#define tb_astream_task(astream, delay, func, priv) 							tb_astream_task_impl(astream, delay, func, priv __tb_debug_vals__)

/// open and read
#define tb_astream_oread(astream, maxn, func, priv) 							tb_astream_oread_impl(astream, maxn, func, priv __tb_debug_vals__)

/// open and writ
#define tb_astream_owrit(astream, data, size, func, priv) 						tb_astream_owrit_impl(astream, data, size, func, priv __tb_debug_vals__)

/// open and seek
#define tb_astream_oseek(astream, offset, func, priv) 							tb_astream_oseek_impl(astream, offset, func, priv __tb_debug_vals__)

/// read after delay
#define tb_astream_read_after(astream, delay, maxn, func, priv) 				tb_astream_read_after_impl(astream, delay, maxn, func, priv __tb_debug_vals__)

/// writ after delay
#define tb_astream_writ_after(astream, delay, data, size, func, priv) 			tb_astream_writ_after_impl(astream, delay, data, size, func, priv __tb_debug_vals__)

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/// the asio stream declaration
struct __tb_astream_t;

/*! the astream open func type
 *
 * @param astream 			the astream
 * @param state 			the stream state
 * @param priv 				the func private data
 *
 * @return 					tb_true: ok, tb_false: error, but not break aicp
 */
typedef tb_bool_t 			(*tb_astream_open_func_t)(struct __tb_astream_t* astream, tb_size_t state, tb_pointer_t priv);

/*! the astream read func type
 *
 * @param astream 			the astream
 * @param state 			the stream state
 * @param data 				the readed data
 * @param real 				the real size, maybe zero
 * @param size 				the need size
 * @param priv 				the func private data
 *
 * @return 					tb_true: ok and continue it if need, tb_false: break it, but not break aicp
 */
typedef tb_bool_t 			(*tb_astream_read_func_t)(struct __tb_astream_t* astream, tb_size_t state, tb_byte_t const* data, tb_size_t real, tb_size_t size, tb_pointer_t priv);

/*! the astream writ func type
 *
 * @param astream 			the astream
 * @param state 			the stream state
 * @param data 				the writed data
 * @param real 				the real size, maybe zero
 * @param size 				the need size
 * @param priv 				the func private data
 *
 * @return 					tb_true: ok and continue it if need, tb_false: break it, but not break aicp
 */
typedef tb_bool_t 			(*tb_astream_writ_func_t)(struct __tb_astream_t* astream, tb_size_t state, tb_byte_t const* data, tb_size_t real, tb_size_t size, tb_pointer_t priv);

/*! the astream seek func type
 *
 * @param astream 			the astream
 * @param state 			the stream state
 * @param offset 			the real offset
 * @param priv 				the func private data
 *
 * @return 					tb_true: ok, tb_false: error, but not break aicp
 */
typedef tb_bool_t 			(*tb_astream_seek_func_t)(struct __tb_astream_t* astream, tb_size_t state, tb_hize_t offset, tb_pointer_t priv);

/*! the astream sync func type
 *
 * @param astream 			the astream
 * @param state 			the stream state
 * @param priv 				the func private data
 *
 * @return 					tb_true: ok, tb_false: error, but not break aicp
 */
typedef tb_bool_t 			(*tb_astream_sync_func_t)(struct __tb_astream_t* astream, tb_size_t state, tb_pointer_t priv);

/*! the astream task func type
 *
 * @param astream 			the astream
 * @param state 			the stream state
 * @param priv 				the func private data
 *
 * @return 					tb_true: ok, tb_false: error, but not break aicp
 */
typedef tb_bool_t 			(*tb_astream_task_func_t)(struct __tb_astream_t* astream, tb_size_t state, tb_pointer_t priv);

/// the astream open and read type
typedef struct __tb_astream_oread_t
{
	// the func
	tb_astream_read_func_t 	func;

	// the priv
	tb_pointer_t 			priv;

	// the maxn
	tb_size_t 				maxn;

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
	/// the base
	tb_stream_t 			base;

	/// the aicp
	tb_aicp_t* 				aicp;

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
	tb_bool_t 				(*open)(tb_handle_t astream, tb_astream_open_func_t func, tb_pointer_t priv);

	/// read
	tb_bool_t 				(*read)(tb_handle_t astream, tb_size_t delay, tb_size_t maxn, tb_astream_read_func_t func, tb_pointer_t priv);

	/// writ
	tb_bool_t 				(*writ)(tb_handle_t astream, tb_size_t delay, tb_byte_t const* data, tb_size_t size, tb_astream_writ_func_t func, tb_pointer_t priv);

	/// seek
	tb_bool_t 				(*seek)(tb_handle_t astream, tb_hize_t offset, tb_astream_seek_func_t func, tb_pointer_t priv);

	/// sync
	tb_bool_t 				(*sync)(tb_handle_t astream, tb_bool_t bclosing, tb_astream_sync_func_t func, tb_pointer_t priv);

	/// task
	tb_bool_t 				(*task)(tb_handle_t astream, tb_size_t delay, tb_astream_task_func_t func, tb_pointer_t priv);

	/// kill
	tb_void_t 				(*kill)(tb_handle_t astream);

	/// clos
	tb_void_t 				(*clos)(tb_handle_t astream, tb_bool_t bcalling);

	/// exit
	tb_void_t 				(*exit)(tb_handle_t astream, tb_bool_t bcalling);

}tb_astream_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init data stream 
 *
 * @param aicp 		the aicp
 *
 * @return 			the stream
 */
tb_astream_t* 		tb_astream_init_data(tb_aicp_t* aicp);

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

/*! init filter stream 
 *
 * @param aicp 		the aicp
 *
 * @return 			the stream
 */
tb_astream_t* 		tb_astream_init_filter(tb_aicp_t* aicp);

/*! close stream, will block it if be pending
 *
 * @param astream 	the stream
 * @param bcalling 	close it from the calling callback?
 */
tb_void_t 			tb_astream_clos(tb_astream_t* astream, tb_bool_t bcalling);

/*! exit stream, will block it if be pending
 *
 * @param astream 	the stream
 * @param bcalling 	exit it from the calling callback?
 */
tb_void_t 			tb_astream_exit(tb_astream_t* astream, tb_bool_t bcalling);

/*! init stream from url
 *
 * @param aicp 		the aicp
 * @param url 		the url
 * <pre>
 * data://base64
 * file://path or unix path: e.g. /root/xxxx/file
 * sock://host:port?tcp=
 * sock://host:port?udp=
 * socks://host:port
 * http://host:port/path?arg0=&arg1=...
 * https://host:port/path?arg0=&arg1=...
 * </pre>
 *
 * @return 			the stream
 */
tb_astream_t* 		tb_astream_init_from_url(tb_aicp_t* aicp, tb_char_t const* url);

/*! init stream from data
 *
 * @param aicp 		the aicp
 * @param data 		the data
 * @param size 		the size
 *
 * @return 			the stream
 */
tb_astream_t* 		tb_astream_init_from_data(tb_aicp_t* aicp, tb_byte_t const* data, tb_size_t size);

/*! init stream from file
 *
 * @param aicp 		the aicp
 * @param path 		the file path
 * @param mode 		the file mode, using the default ro mode if zero
 *
 * @return 			the stream
 */
tb_astream_t* 		tb_astream_init_from_file(tb_aicp_t* aicp, tb_char_t const* path, tb_size_t mode);

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

/*! init filter stream from null
 *
 * @param astream 	the stream
 *
 * @return 			the stream
 */
tb_astream_t* 		tb_astream_init_filter_from_null(tb_astream_t* astream);

/*! init filter stream from zip
 *
 * @param astream 	the stream
 * @param algo 		the zip algorithom
 * @param action 	the zip action
 *
 * @return 			the stream
 */
tb_astream_t* 		tb_astream_init_filter_from_zip(tb_astream_t* astream, tb_size_t algo, tb_size_t action);

/*! init filter stream from cache
 *
 * @param astream 	the stream
 * @param size 		the initial cache size, using the default size if be zero
 *
 * @return 			the stream
 */
tb_astream_t* 		tb_astream_init_filter_from_cache(tb_astream_t* astream, tb_size_t size);

/*! init filter stream from charset
 *
 * @param astream 	the stream
 * @param fr 		the from charset
 * @param to 		the to charset
 *
 * @return 			the stream
 */
tb_astream_t* 		tb_astream_init_filter_from_charset(tb_astream_t* astream, tb_size_t fr, tb_size_t to);

/*! init filter stream from chunked
 *
 * @param astream 	the stream
 * @param dechunked decode the chunked data?
 *
 * @return 			the stream
 */
tb_astream_t* 		tb_astream_init_filter_from_chunked(tb_astream_t* astream, tb_bool_t dechunked);

/*! try to open the stream for stream: file, filter, ... 
 *
 * @param astream 	the stream
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_astream_open_try(tb_astream_t* astream);

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
 * @param maxn 		the read maxn, using the default maxn if be zero
 * @param func 		the func
 * @param priv 		the func data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_astream_read_impl(tb_astream_t* astream, tb_size_t maxn, tb_astream_read_func_t func, tb_pointer_t priv __tb_debug_decl__);

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
 * @param astream 	the stream
 * @param bclosing 	sync the tail data for closing
 * @param func 		the func
 * @param priv 		the func data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_astream_sync_impl(tb_astream_t* astream, tb_bool_t bclosing, tb_astream_sync_func_t func, tb_pointer_t priv __tb_debug_decl__);

/*! task the stream
 *
 * @param astream 	the stream
 * @param delay 	the delay time, ms
 * @param func 		the func
 * @param priv 		the func data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_astream_task_impl(tb_astream_t* astream, tb_size_t delay, tb_astream_task_func_t func, tb_pointer_t priv __tb_debug_decl__);

/*! open and read the stream, open it first if not opened 
 *
 * @param astream 	the stream
 * @param maxn 		the read maxn, using the default maxn if be zero
 * @param func 		the func
 * @param priv 		the func data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_astream_oread_impl(tb_astream_t* astream, tb_size_t maxn, tb_astream_read_func_t func, tb_pointer_t priv __tb_debug_decl__);

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

/*! read the stream after the delay time
 *
 * @param astream 	the stream
 * @param delay 	the delay time, ms
 * @param maxn 		the read maxn, using the default maxn if be zero
 * @param func 		the func
 * @param priv 		the func data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_astream_read_after_impl(tb_astream_t* astream, tb_size_t delay, tb_size_t maxn, tb_astream_read_func_t func, tb_pointer_t priv __tb_debug_decl__);

/*! writ the stream after the delay time
 *
 * @param astream 	the stream
 * @param delay 	the delay time, ms
 * @param data 		the data
 * @param size 		the size
 * @param func 		the func
 * @param priv 		the func data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_astream_writ_after_impl(tb_astream_t* astream, tb_size_t delay, tb_byte_t const* data, tb_size_t size, tb_astream_writ_func_t func, tb_pointer_t priv __tb_debug_decl__);

/*! the stream aicp
 *
 * @param astream 	the stream
 *
 * @return 			the stream aicp
 */
tb_aicp_t* 			tb_astream_aicp(tb_astream_t* astream);

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
