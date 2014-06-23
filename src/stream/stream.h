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
 * @author      ruki
 * @file        stream.h
 * @defgroup    stream
 *
 */
#ifndef TB_STREAM_H
#define TB_STREAM_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "basic_stream.h"
#include "async_stream.h"
#include "static_stream.h"
#include "transfer.h"
#include "transfer_pool.h"
#include "filter/filter.h"

/*!architecture
 *
 *
 * <pre>   
 *                                                                  wait - loop
 *                                                                   | 
 *                                                                   |                                          - data
 *                                                                 [aioo]                                       |
 *                                                           ----- basic_stream -------- basic_stream ----------- file
 *                                                           |                        |                         |
 *                                                           |                        |                         - sock 
 *                                                           |                        |                         |
 *                                                           |                        |                         - http
 *                                                           |                        |           - charset
 *                                data -                     |                        |          |
 *                                     |  [istream]          |                        - filter - |- chunked 
 *                                url  |-- stream ---------  |                                   |        
 *                                     |      |              |                                   |- cache
 *                                ... -       |              |                                   |
 *                                            |              |                                    - zip
 *                                            |              |
 *                                            |              |
 *                                            |              |
 *                                            |              |            - loop
 *                                            |              |     [asio] |
 *                                            |              |      aicp -| loop
 *                                            |              |       |    |
 *                                            |              |       |    - ...                                  - data
 *                                            |              |     [aico]                                        |
 *                                            |              ----- async_stream -------- async_stream ------------ file
 *                                            |                                       |                          |
 *                                            |                                       |                          - sock
 *                            ----------------                                        |                          |
 *                            |                                                       |                          - http
 *                 -----  transfer ---------- |                                       |           - charset
 *                |           |           [ostream]                                   |          |
 *                |           ------------ stream                                     - filter - |- chunked 
 *                |                                                                        |     |        
 *  transfer_pool  -----  transfer                                                         |     |- cache
 *                |                                                                        |     |
 *                |                                                                        |      - zip    
 *                |                                                                        |
 *                 -----   ...                                                             |
 *                                                                                      static_stream - [data, size]
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

#endif

