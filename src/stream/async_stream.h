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
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author		ruki
 * @file		async_stream.h
 * @ingroup 	stream
 *
 */
#ifndef TB_STREAM_ASYNC_STREAM_H
#define TB_STREAM_ASYNC_STREAM_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../asio/asio.h"
#include "../libc/libc.h"
#include "../network/url.h"
#include "../memory/memory.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

/// open
#define tb_async_stream_open(stream, func, priv) 									tb_async_stream_open_(stream, func, priv __tb_debug_vals__)

/// read
#define tb_async_stream_read(stream, size, func, priv) 							tb_async_stream_read_(stream, size, func, priv __tb_debug_vals__)

/// writ
#define tb_async_stream_writ(stream, data, size, func, priv) 						tb_async_stream_writ_(stream, data, size, func, priv __tb_debug_vals__)

/// seek
#define tb_async_stream_seek(stream, offset, func, priv) 							tb_async_stream_seek_(stream, offset, func, priv __tb_debug_vals__)

/// sync
#define tb_async_stream_sync(stream, bclosing, func, priv) 						tb_async_stream_sync_(stream, bclosing, func, priv __tb_debug_vals__)

/// task
#define tb_async_stream_task(stream, delay, func, priv) 							tb_async_stream_task_(stream, delay, func, priv __tb_debug_vals__)

/// open and read
#define tb_async_stream_oread(stream, size, func, priv) 							tb_async_stream_oread_(stream, size, func, priv __tb_debug_vals__)

/// open and writ
#define tb_async_stream_owrit(stream, data, size, func, priv) 						tb_async_stream_owrit_(stream, data, size, func, priv __tb_debug_vals__)

/// open and seek
#define tb_async_stream_oseek(stream, offset, func, priv) 							tb_async_stream_oseek_(stream, offset, func, priv __tb_debug_vals__)

/// read after delay
#define tb_async_stream_read_after(stream, delay, size, func, priv) 				tb_async_stream_read_after_(stream, delay, size, func, priv __tb_debug_vals__)

/// writ after delay
#define tb_async_stream_writ_after(stream, delay, data, size, func, priv) 			tb_async_stream_writ_after_(stream, delay, data, size, func, priv __tb_debug_vals__)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the asio stream declaration
struct __tb_async_stream_t;

/*! the stream open func type
 *
 * @param stream 				the stream
 * @param state 				the state
 * @param priv 					the func private data
 *
 * @return 						tb_true: ok, tb_false: error, but not break aicp
 */
typedef tb_bool_t 				(*tb_async_stream_open_func_t)(struct __tb_async_stream_t* stream, tb_size_t state, tb_pointer_t priv);

/*! the stream read func type
 *
 * @param stream 				the stream
 * @param state 				the state
 * @param data 					the readed data
 * @param real 					the real size, maybe zero
 * @param size 					the need size
 * @param priv 					the func private data
 *
 * @return 						tb_true: ok and continue it if need, tb_false: break it, but not break aicp
 */
typedef tb_bool_t 				(*tb_async_stream_read_func_t)(struct __tb_async_stream_t* stream, tb_size_t state, tb_byte_t const* data, tb_size_t real, tb_size_t size, tb_pointer_t priv);

/*! the stream writ func type
 *
 * @param stream 				the stream
 * @param state 				the state
 * @param data 					the writed data
 * @param real 					the real size, maybe zero
 * @param size 					the need size
 * @param priv 					the func private data
 *
 * @return 						tb_true: ok and continue it if need, tb_false: break it, but not break aicp
 */
typedef tb_bool_t 				(*tb_async_stream_writ_func_t)(struct __tb_async_stream_t* stream, tb_size_t state, tb_byte_t const* data, tb_size_t real, tb_size_t size, tb_pointer_t priv);

/*! the stream seek func type
 *
 * @param stream 				the stream
 * @param state 				the state
 * @param offset 				the real offset
 * @param priv 					the func private data
 *
 * @return 						tb_true: ok, tb_false: error, but not break aicp
 */
typedef tb_bool_t 				(*tb_async_stream_seek_func_t)(struct __tb_async_stream_t* stream, tb_size_t state, tb_hize_t offset, tb_pointer_t priv);

/*! the stream sync func type
 *
 * @param stream 				the stream
 * @param state 				the state
 * @param bclosing 				is closing?
 * @param priv 					the func private data
 *
 * @return 						tb_true: ok, tb_false: error, but not break aicp
 */
typedef tb_bool_t 				(*tb_async_stream_sync_func_t)(struct __tb_async_stream_t* stream, tb_size_t state, tb_bool_t bclosing, tb_pointer_t priv);

/*! the stream task func type
 *
 * @param stream 				the stream
 * @param state 				the state
 * @param priv 					the func private data
 *
 * @return 						tb_true: ok, tb_false: error, but not break aicp
 */
typedef tb_bool_t 				(*tb_async_stream_task_func_t)(struct __tb_async_stream_t* stream, tb_size_t state, tb_pointer_t priv);

/// the stream open and read type
typedef struct __tb_async_stream_oread_t
{
	/// the func
	tb_async_stream_read_func_t func;

	/// the priv
	tb_pointer_t 				priv;

	/// the size
	tb_size_t 					size;

}tb_async_stream_oread_t;

/// the stream open and writ type
typedef struct __tb_async_stream_owrit_t
{
	/// the func
	tb_async_stream_writ_func_t func;

	/// the priv
	tb_pointer_t 				priv;

	/// the data
	tb_byte_t const* 			data;

	/// the size
	tb_size_t 					size;

}tb_async_stream_owrit_t;

/// the stream open and seek type
typedef struct __tb_async_stream_oseek_t
{
	/// the func
	tb_async_stream_seek_func_t	func;

	/// the priv
	tb_pointer_t 				priv;

	/// the offset
	tb_hize_t 					offset;

}tb_async_stream_oseek_t;

/// the stream cache and writ type
typedef struct __tb_async_stream_cwrit_t
{
	/// the func
	tb_async_stream_writ_func_t	func;

	/// the data
	tb_byte_t const* 			data;

	/// the size
	tb_size_t 					size;

}tb_async_stream_cwrit_t;

/// the stream cache and sync type
typedef struct __tb_async_stream_csync_t
{
	/// the func
	tb_async_stream_sync_func_t func;

	/// is closing
	tb_bool_t 					bclosing;

}tb_async_stream_csync_t;

/// the stream sync and read type
typedef struct __tb_async_stream_sread_t
{
	/// the func
	tb_async_stream_read_func_t func;

	/// the priv
	tb_pointer_t 				priv;

	/// the size
	tb_size_t 					size;

}tb_async_stream_sread_t;

/// the stream sync and seek type
typedef struct __tb_async_stream_sseek_t
{
	/// the func
	tb_async_stream_seek_func_t	func;

	/// the priv
	tb_pointer_t 				priv;

	/// the offset
	tb_hize_t 					offset;

}tb_async_stream_sseek_t;

/// the asio stream type
typedef struct __tb_async_stream_t
{	
	/// the base
	tb_stream_t 				base;

	/// the aicp
	tb_aicp_t* 					aicp;

#ifdef __tb_debug__
	/// the func
	tb_char_t const* 			func;

	/// the file
	tb_char_t const* 			file;

	/// the line
	tb_size_t 					line;
#endif

	/// the open and read, writ, seek, ...
	union
	{
		tb_async_stream_oread_t read;
		tb_async_stream_owrit_t writ;
		tb_async_stream_oseek_t seek;

	} 							open_and;

	/// the sync and read, writ, seek, ...
	union
	{
		tb_async_stream_sread_t read;
		tb_async_stream_sseek_t seek;

	} 							sync_and;

	/// the wcache and writ, sync, ... 
	union
	{
		tb_async_stream_cwrit_t writ;
		tb_async_stream_csync_t sync;

	} 							wcache_and;

	/// the read cache data
	tb_scoped_buffer_t 			rcache_data;

	/// the read cache maxn
	tb_size_t 					rcache_maxn;

	/// the writ cache data
	tb_scoped_buffer_t 			wcache_data;

	/// the writ cache maxn
	tb_size_t 					wcache_maxn;

	/// open
	tb_bool_t 					(*open)(tb_handle_t stream, tb_async_stream_open_func_t func, tb_pointer_t priv);

	/// read
	tb_bool_t 					(*read)(tb_handle_t stream, tb_size_t delay, tb_byte_t* data, tb_size_t size, tb_async_stream_read_func_t func, tb_pointer_t priv);

	/// writ
	tb_bool_t 					(*writ)(tb_handle_t stream, tb_size_t delay, tb_byte_t const* data, tb_size_t size, tb_async_stream_writ_func_t func, tb_pointer_t priv);

	/// seek
	tb_bool_t 					(*seek)(tb_handle_t stream, tb_hize_t offset, tb_async_stream_seek_func_t func, tb_pointer_t priv);

	/// sync
	tb_bool_t 					(*sync)(tb_handle_t stream, tb_bool_t bclosing, tb_async_stream_sync_func_t func, tb_pointer_t priv);

	/// task
	tb_bool_t 					(*task)(tb_handle_t stream, tb_size_t delay, tb_async_stream_task_func_t func, tb_pointer_t priv);

	/// kill
	tb_void_t 					(*kill)(tb_handle_t stream);

	/// clos
	tb_void_t 					(*clos)(tb_handle_t stream, tb_bool_t bcalling);

	/// exit
	tb_void_t 					(*exit)(tb_handle_t stream, tb_bool_t bcalling);

}tb_async_stream_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init data stream 
 *
 * @param aicp 			the aicp
 *
 * @return 				the stream
 */
tb_async_stream_t* 		tb_async_stream_init_data(tb_aicp_t* aicp);

/*! init file stream 
 *
 * @param aicp 			the aicp
 *
 * @return 				the stream
 */
tb_async_stream_t* 		tb_async_stream_init_file(tb_aicp_t* aicp);

/*! init sock stream 
 *
 * @param aicp 			the aicp
 *
 * @return 				the stream
 */
tb_async_stream_t* 		tb_async_stream_init_sock(tb_aicp_t* aicp);

/*! init http stream 
 *
 * @param aicp 			the aicp
 *
 * @return 				the stream
 */
tb_async_stream_t* 		tb_async_stream_init_http(tb_aicp_t* aicp);

/*! init filter stream 
 *
 * @param aicp 			the aicp
 *
 * @return 				the stream
 */
tb_async_stream_t* 		tb_async_stream_init_filter(tb_aicp_t* aicp);

/*! close stream, will block it if be pending
 *
 * @param stream 		the stream
 * @param bcalling 		close it from the calling callback?
 */
tb_void_t 				tb_async_stream_clos(tb_async_stream_t* stream, tb_bool_t bcalling);

/*! exit stream, will block it if be pending
 *
 * @param stream 		the stream
 * @param bcalling 		exit it from the calling callback?
 */
tb_void_t 				tb_async_stream_exit(tb_async_stream_t* stream, tb_bool_t bcalling);

/*! init stream from url
 *
 * @param aicp 			the aicp
 * @param url 			the url
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
 * @return 				the stream
 */
tb_async_stream_t* 		tb_async_stream_init_from_url(tb_aicp_t* aicp, tb_char_t const* url);

/*! init stream from data
 *
 * @param aicp 			the aicp
 * @param data 			the data
 * @param size 			the size
 *
 * @return 				the stream
 */
tb_async_stream_t* 		tb_async_stream_init_from_data(tb_aicp_t* aicp, tb_byte_t const* data, tb_size_t size);

/*! init stream from file
 *
 * @param aicp 			the aicp
 * @param path 			the file path
 * @param mode 			the file mode, using the default ro mode if zero
 *
 * @return 				the stream
 */
tb_async_stream_t* 		tb_async_stream_init_from_file(tb_aicp_t* aicp, tb_char_t const* path, tb_size_t mode);

/*! init stream from sock
 *
 * @param aicp 			the aicp
 * @param host 			the host
 * @param port 			the port
 * @param type 			the socket type, tcp or udp
 * @param bssl 			enable ssl?
 *
 * @return 				the stream
 */
tb_async_stream_t* 		tb_async_stream_init_from_sock(tb_aicp_t* aicp, tb_char_t const* host, tb_size_t port, tb_size_t type, tb_bool_t bssl);

/*! init stream from http or https
 *
 * @param aicp 			the aicp
 * @param host 			the host
 * @param port 			the port
 * @param path 			the path
 * @param bssl 			enable ssl?
 *
 * @return 				the stream
 */
tb_async_stream_t* 		tb_async_stream_init_from_http(tb_aicp_t* aicp, tb_char_t const* host, tb_size_t port, tb_char_t const* path, tb_bool_t bssl);

/*! init filter stream from null
 *
 * @param stream 		the stream
 *
 * @return 				the stream
 */
tb_async_stream_t* 		tb_async_stream_init_filter_from_null(tb_async_stream_t* stream);

/*! init filter stream from zip
 *
 * @param stream 		the stream
 * @param algo 			the zip algorithm
 * @param action 		the zip action
 *
 * @return 				the stream
 */
tb_async_stream_t* 		tb_async_stream_init_filter_from_zip(tb_async_stream_t* stream, tb_size_t algo, tb_size_t action);

/*! init filter stream from cache
 *
 * @param stream 		the stream
 * @param size 			the initial cache size, using the default size if be zero
 *
 * @return 				the stream
 */
tb_async_stream_t* 		tb_async_stream_init_filter_from_cache(tb_async_stream_t* stream, tb_size_t size);

/*! init filter stream from charset
 *
 * @param stream 		the stream
 * @param fr 			the from charset
 * @param to 			the to charset
 *
 * @return 				the stream
 */
tb_async_stream_t* 		tb_async_stream_init_filter_from_charset(tb_async_stream_t* stream, tb_size_t fr, tb_size_t to);

/*! init filter stream from chunked
 *
 * @param stream 		the stream
 * @param dechunked 	decode the chunked data?
 *
 * @return 				the stream
 */
tb_async_stream_t* 		tb_async_stream_init_filter_from_chunked(tb_async_stream_t* stream, tb_bool_t dechunked);

/*! try to open the stream for stream: file, filter, ... 
 *
 * @param stream 		the stream
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_async_stream_open_try(tb_async_stream_t* stream);

/*! open the stream 
 *
 * @param stream 		the stream
 * @param func 			the func
 * @param priv 			the func data
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_async_stream_open_(tb_async_stream_t* stream, tb_async_stream_open_func_t func, tb_pointer_t priv __tb_debug_decl__);

/*! read the stream 
 *
 * @param stream 		the stream
 * @param size 			the read size, using the default size if be zero
 * @param func 			the func
 * @param priv 			the func data
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_async_stream_read_(tb_async_stream_t* stream, tb_size_t size, tb_async_stream_read_func_t func, tb_pointer_t priv __tb_debug_decl__);

/*! writ the stream 
 *
 * @param stream 		the stream
 * @param data 			the data
 * @param size 			the size
 * @param func 			the func
 * @param priv 			the func data
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_async_stream_writ_(tb_async_stream_t* stream, tb_byte_t const* data, tb_size_t size, tb_async_stream_writ_func_t func, tb_pointer_t priv __tb_debug_decl__);

/*! seek the stream
 *
 * @param stream 		the stream
 * @param offset 		the offset
 * @param func 			the func
 * @param priv 			the func data
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_async_stream_seek_(tb_async_stream_t* stream, tb_hize_t offset, tb_async_stream_seek_func_t func, tb_pointer_t priv __tb_debug_decl__);

/*! sync the stream
 *
 * @param stream 		the stream
 * @param bclosing 		sync the tail data for closing
 * @param func 			the func
 * @param priv 			the func data
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_async_stream_sync_(tb_async_stream_t* stream, tb_bool_t bclosing, tb_async_stream_sync_func_t func, tb_pointer_t priv __tb_debug_decl__);

/*! task the stream
 *
 * @param stream 		the stream
 * @param delay 		the delay time, ms
 * @param func 			the func
 * @param priv 			the func data
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_async_stream_task_(tb_async_stream_t* stream, tb_size_t delay, tb_async_stream_task_func_t func, tb_pointer_t priv __tb_debug_decl__);

/*! open and read the stream, open it first if not opened 
 *
 * @param stream 		the stream
 * @param size 			the read size, using the default size if be zero
 * @param func 			the func
 * @param priv 			the func data
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_async_stream_oread_(tb_async_stream_t* stream, tb_size_t size, tb_async_stream_read_func_t func, tb_pointer_t priv __tb_debug_decl__);

/*! open and writ the stream, open it first if not opened 
 *
 * @param stream 		the stream
 * @param data 			the data
 * @param size 			the size
 * @param func 			the func
 * @param priv 			the func data
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_async_stream_owrit_(tb_async_stream_t* stream, tb_byte_t const* data, tb_size_t size, tb_async_stream_writ_func_t func, tb_pointer_t priv __tb_debug_decl__);

/*! open and seek the stream, open it first if not opened 
 *
 * @param stream 		the stream
 * @param offset 		the offset
 * @param func 			the func
 * @param priv 			the func data
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_async_stream_oseek_(tb_async_stream_t* stream, tb_hize_t offset, tb_async_stream_seek_func_t func, tb_pointer_t priv __tb_debug_decl__);
	
/*! read the stream after the delay time
 *
 * @param stream 		the stream
 * @param delay 		the delay time, ms
 * @param size 			the read size, using the default size if be zero
 * @param func 			the func
 * @param priv 			the func data
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_async_stream_read_after_(tb_async_stream_t* stream, tb_size_t delay, tb_size_t size, tb_async_stream_read_func_t func, tb_pointer_t priv __tb_debug_decl__);

/*! writ the stream after the delay time
 *
 * @param stream 		the stream
 * @param delay 		the delay time, ms
 * @param data 			the data
 * @param size 			the size
 * @param func 			the func
 * @param priv 			the func data
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_async_stream_writ_after_(tb_async_stream_t* stream, tb_size_t delay, tb_byte_t const* data, tb_size_t size, tb_async_stream_writ_func_t func, tb_pointer_t priv __tb_debug_decl__);

/*! the stream aicp
 *
 * @param stream 		the stream
 *
 * @return 				the stream aicp
 */
tb_aicp_t* 				tb_async_stream_aicp(tb_async_stream_t* stream);

#ifdef __tb_debug__
/*! the stream func name from post for debug
 *
 * @param stream 		the stream
 *
 * @return 				the stream func name
 */
tb_char_t const* 		tb_async_stream_func(tb_async_stream_t* stream);

/*! the stream file name from post for debug
 *
 * @param stream 		the stream
 *
 * @return 				the stream file name
 */
tb_char_t const* 		tb_async_stream_file(tb_async_stream_t* stream);

/*! the stream line number from post for debug
 *
 * @param stream 		the stream
 *
 * @return 				the stream line number
 */
tb_size_t 				tb_async_stream_line(tb_async_stream_t* stream);
#endif

#endif
