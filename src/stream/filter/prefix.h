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
 * @file        prefix.h
 *
 */
#ifndef TB_STREAM_FILTER_PREFIX_H
#define TB_STREAM_FILTER_PREFIX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"
#include "../static_stream.h"
#include "../../memory/memory.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

/// the stream filter ctrl
#define TB_STREAM_FILTER_CTRL(type, ctrl)               (((type) << 16) | (ctrl))

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the filter type enum
typedef enum __tb_stream_filter_type_e
{
    TB_STREAM_FILTER_TYPE_NONE      = 0
,   TB_STREAM_FILTER_TYPE_ZIP       = 1
,   TB_STREAM_FILTER_TYPE_CACHE     = 2
,   TB_STREAM_FILTER_TYPE_CHARSET   = 3
,   TB_STREAM_FILTER_TYPE_CHUNKED   = 4

}tb_stream_filter_type_e;

/// the filter ctrl enum
typedef enum __tb_stream_filter_ctrl_e
{
    TB_STREAM_FILTER_CTRL_NONE                  = 0
,   TB_STREAM_FILTER_CTRL_ZIP_GET_ALGO          = TB_STREAM_FILTER_CTRL(TB_STREAM_FILTER_TYPE_ZIP, 1)
,   TB_STREAM_FILTER_CTRL_ZIP_GET_ACTION        = TB_STREAM_FILTER_CTRL(TB_STREAM_FILTER_TYPE_ZIP, 2)
,   TB_STREAM_FILTER_CTRL_ZIP_SET_ALGO          = TB_STREAM_FILTER_CTRL(TB_STREAM_FILTER_TYPE_ZIP, 3)
,   TB_STREAM_FILTER_CTRL_ZIP_SET_ACTION        = TB_STREAM_FILTER_CTRL(TB_STREAM_FILTER_TYPE_ZIP, 4)

}tb_stream_filter_ctrl_e;

/// the stream filter ref type
typedef struct{}*       tb_stream_filter_ref_t;

#endif
