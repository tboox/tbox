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
 * @file		mstream.h
 * @ingroup 	stream
 *
 */
#ifndef TB_STREAM_MSTREAM_H
#define TB_STREAM_MSTREAM_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "gstream.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the generic stream type
typedef struct __tb_mstream_t
{	


}tb_mstream_t;


/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

// exit stream
tb_void_t 			tb_mstream_exit(tb_mstream_t* mst);

// init stream
tb_mstream_t* 		tb_mstream_init_data();
tb_mstream_t* 		tb_mstream_init_file();
tb_mstream_t* 		tb_mstream_init_sock();
tb_mstream_t* 		tb_mstream_init_http();
tb_mstream_t* 		tb_mstream_init_null();
tb_mstream_t* 		tb_mstream_init_zip();
tb_mstream_t* 		tb_mstream_init_encoding();
tb_bool_t 			tb_mstream_init(tb_mstream_t* mst);

/* add stream from url
 *
 * file://path or unix path: e.g. /root/xxxx/file
 * files://home/file
 * sock://host:port?tcp=
 * socks://host:port?udp=
 * http://host:port/path?arg0=&arg1=...
 * https://host:port/path?arg0=&arg1=...
 */
tb_handle_t 		tb_mstream_add_from_url(tb_mstream_t* mst, tb_char_t const* url);

// add stream from data
tb_handle_t 		tb_mstream_add_from_data(tb_mstream_t* mst, tb_byte_t const* data, tb_size_t size);

// add stream from file
tb_handle_t 		tb_mstream_add_from_file(tb_mstream_t* mst, tb_char_t const* path);

// add stream from sock
tb_handle_t 		tb_mstream_add_from_sock(tb_mstream_t* mst, tb_char_t const* host, tb_size_t port, tb_size_t type, tb_bool_t bssl);

// add stream from http or https
tb_handle_t 		tb_mstream_add_from_http(tb_mstream_t* mst, tb_char_t const* host, tb_size_t port, tb_char_t const* path, tb_bool_t bssl);

// clr stream, remove the all streams and close it
tb_void_t 			tb_mstream_clr(tb_mstream_t* mst);

// post mstream
tb_void_t 			tb_mstream_post(tb_mstream_t* mst);

// wait mstream
tb_long_t 			tb_mstream_wait(tb_mstream_t* mst, tb_long_t timeout);

// del stream, and close it
tb_void_t 			tb_mstream_del(tb_mstream_t* mst, tb_handle_t handle);

#if 0
// open stream 
tb_long_t 			tb_mstream_open(tb_mstream_t* mst, tb_handle_t handle);

// close stream
tb_long_t 			tb_mstream_close(tb_mstream_t* mst, tb_handle_t handle);

// clear stream cache and reset status, but not close it
tb_void_t 			tb_mstream_clear(tb_mstream_t* mst, tb_handle_t handle);
#endif

// ctrl stream
tb_bool_t 			tb_mstream_ctrl(tb_mstream_t* mst, tb_handle_t handle, tb_size_t cmd, ...);

#endif

