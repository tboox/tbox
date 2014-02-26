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
 * @file		stream.h
 * @defgroup 	stream
 *
 */
#ifndef TB_STREAM_H
#define TB_STREAM_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "bstream.h"
#include "gstream.h"
#include "astream.h"
#include "tstream.h"

/*!architecture
 *
 *
 * <pre>   
 *     bstream
 *        |         
 *        |                                          - data
 *    (url, ...)                                    |
 *     gstream ------------- gstream ---------------- file
 *     [aioo]        |                              |
 *                   |                              - sock 
 *                   |                              |
 *                   |                              - http
 *                   |           - charset
 *                   |          |
 *                   - filter - |- chunked 
 *                              |        
 *                              |- cache
 *                              |
 *                               - zip
 *
 *
 *
 *                                                    - data
 *    (url, ...)                                     |
 *     astream ------------- astream ----------------- file
 *      [aicp]       |                               |
 *                   |                               - sock
 *                   |                               |
 *                   |                               - http
 *                   |           - charset
 *                   |          |
 *                   - filter - |- chunked 
 *                              |        
 *                              |- cache
 *                              |
 *                               - zip    
 *
 *                -- gstream
 *    tstream - |
 *                -- astream
 *
 *                         
 * url: 
 * data://base64
 * file://path or unix path: e.g. /root/xxxx/file
 * sock://host:port?tcp=
 * sock://host:port?udp=
 * socks://host:port
 * http://host:port/path?arg0=&arg1=...
 * https://host:port/path?arg0=&arg1=...
 * </pre>
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! the stream mode
 *
 * @param stream 	the stream
 *
 * @return 			the stream mode
 */
tb_size_t 			tb_stream_mode(tb_handle_t stream);

/*! the stream type
 *
 * @param stream 	the stream
 *
 * @return 			the stream type
 */
tb_size_t 			tb_stream_type(tb_handle_t stream);

/*! the stream size and not seeking it
 *
 * @param stream 	the stream
 *
 * @return 			the stream size, no size: -1, empty or error: 0
 */
tb_hong_t 			tb_stream_size(tb_handle_t stream);

/*! the stream left size and not seeking it 
 *
 * @param stream 	the stream
 *
 * @return 			the stream left size, no size: infinity, empty or end: 0
 */
tb_hize_t 			tb_stream_left(tb_handle_t stream);

/*! the stream offset
 *
 * the offset is read + writ and using seek for modifying it if size != -1, .e.g: data, file, .. 
 * the offset is calculated from the last read/writ and not seeking it if size == -1, .e.g: sock, filter, ..
 *
 * @param stream 	the stream
 *
 * @return 			the stream offset
 */
tb_hize_t 			tb_stream_offset(tb_handle_t stream);

/*! is opened?
 *
 * @param stream 	the stream
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_stream_is_opened(tb_handle_t stream);

/*! the stream timeout
 *
 * @param stream 	the stream
 *
 * @return 			the stream timeout
 */
tb_long_t 			tb_stream_timeout(tb_handle_t stream);

/*! the stream state c-string
 *
 * @param state 	the state
 *
 * @return 			the stream state c-string
 */
tb_char_t const* 	tb_stream_state_cstr(tb_size_t state);

/*! ctrl stream
 *
 * @param stream 	the stream
 * @param ctrl 		the ctrl command
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_stream_ctrl(tb_handle_t stream, tb_size_t ctrl, ...);

#endif

