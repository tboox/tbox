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
#ifndef TB_MEMORY_IMPL_PREFIX_H
#define TB_MEMORY_IMPL_PREFIX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"
#include "../../libc/libc.h"
#include "../../math/math.h"
#include "../../utils/utils.h"
#include "../../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the pool data magic number
#define TB_POOL_DATA_MAGIC          (0xdead)

// the pool data patch value 
#define TB_POOL_DATA_PATCH          (0xcc)

// the pool data size maximum 
#define TB_POOL_DATA_SIZE_MAXN      (1 << 19)

// save backtrace for the pool data
#define tb_pool_data_save_backtrace(pool_data, skip_frames) \
    do \
    { \
        tb_pool_data_t* _data = (tb_pool_data_t*)(pool_data); \
        tb_size_t nframe = tb_backtrace_frames(_data->backtrace, tb_arrayn(_data->backtrace), skip_frames); \
        if (nframe < tb_arrayn(_data->backtrace)) tb_memset(_data->backtrace + nframe, 0, (tb_arrayn(_data->backtrace) - nframe) * sizeof(tb_cpointer_t)); \
 \
    } while (0) \

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the pool data type enum
typedef enum __tb_pool_data_type_e
{
    TB_POOL_DATA_TYPE_PAGE      = 0
,   TB_POOL_DATA_TYPE_TINY      = 1
,   TB_POOL_DATA_TYPE_CSTR      = 2

}tb_pool_data_type_e;

// the pool data type
typedef struct __tb_pool_data_t
{
#ifdef __tb_debug__

    // the magic
    tb_uint16_t                 magic;

    // the line 
    tb_uint16_t                 line;

    /* the real data size
     * contains the info size and the padding size for alignment
     */
    tb_uint32_t                 real;

    // the file
    tb_char_t const*            file;

    // the func
    tb_char_t const*            func;

    // the backtrace frames
    tb_pointer_t                backtrace[16];

#endif

    // the data address
    tb_pointer_t                data;

    // the data size, <= 512MB
    tb_uint32_t                 size    : 29;

    // the data type
    tb_uint32_t                 type    : 2;

    // free?
    tb_uint32_t                 free    : 1;

}tb_pool_data_t;

#endif
