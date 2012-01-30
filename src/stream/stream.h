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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		stream.h
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
#include "tstream/tstream.h"

/* ///////////////////////////////////////////////////////////////////////
 * architecture
 *
 *
 *    
 *     bstream
 *        |         
 *        |                                               - dstream(data)
 *    (url, ...)   (aio)                     (aio)       |
 *     gstream ------------- gstream ---------------------  fstream(file)
 *                   |              \                    |
 *                   |               -- hstream(http) ----- sstream(sock)
 *                   |
 *                   |
 *                   |           - estream(utf8, gb2312, gbk)
 *                   |          |
 *                   - tstream -| lstream(ssl)      
 *                              |        
 *                              - zstream(rlc, lzsw, gzip)
 *                        
 *                         
 * url: 
 *     unix: /home/path/file...
 * or
 *     file://...
 *     sock://...
 *     http://...
 *     files://...
 *     socks://...
 *     https://...
 *
 *
 */

#endif

