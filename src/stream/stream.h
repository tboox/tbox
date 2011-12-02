/*!The Tiny Box Library
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

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "bstream.h"
#include "gstream.h"

/* /////////////////////////////////////////////////////////
 * architecture
 *
 *
 *    
 * bstream
 *                  
 *              - hstream
 *             |
 * gstream ----  fstream
 *             |
 *             - dstream
 *             |
 *             |           - estream
 *             |          |
 *             - tstream -|          - rlc
 *                  |     |         |
 *                  |     - zstream - lzsw
 *                  |              |
 *                  |              - gzip
 *                  |
 *                  | 
 *                  - gstream - ...
 *
 *
 *
 * read & writ <= tstream <= tstream ... <= gstream <= (data, file, http, ...)
 */

// c plus plus
#ifdef __cplusplus
}
#endif

#endif

