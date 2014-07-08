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
#include "../../container/container.h"
#include "../../algorithm/algorithm.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the pool data magic number
#define TB_POOL_DATA_MAGIC          (0xdead)

// the pool data patch value 
#define TB_POOL_DATA_PATCH          (0xcc)

// the pool data size maximum 
#define TB_POOL_DATA_SIZE_MAXN      (1 << 19)

// the pool data address alignment 
#define TB_POOL_DATA_ALIGN          TB_CPU_BITBYTE

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

#ifdef __tb_debug__
// the pool data debug head type
typedef struct __tb_pool_data_debug_head_t
{
    // the magic
    tb_uint16_t                 magic;

    // the line 
    tb_uint16_t                 line;

    /* the real data size
     * includes the info size and the padding size for alignment
     * excludes the self debug head size
     */
    tb_uint32_t                 real;

    // the file
    tb_char_t const*            file;

    // the func
    tb_char_t const*            func;

    // the backtrace frames
    tb_pointer_t                backtrace[16];

}tb_pool_data_debug_head_t;
#endif

// the pool data head type
typedef __tb_aligned__(TB_POOL_DATA_ALIGN) struct __tb_pool_data_head_t
{
#ifdef __tb_debug__
    // the debug head
    tb_pool_data_debug_head_t   debug;
#endif

    // the size
    tb_uint32_t                 size : 29;

    // is cstr?
    tb_uint32_t                 cstr : 1;

    // is free?
    tb_uint32_t                 free : 1;

}__tb_aligned__(TB_POOL_DATA_ALIGN) tb_pool_data_head_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

#ifdef __tb_debug__
/* dump data info
 *
 * @param data                  the data address
 * @param verbose               dump verbose info?
 * @param prefix                the prefix info
 */
tb_void_t                       tb_pool_data_dump(tb_byte_t const* data, tb_bool_t verbose, tb_char_t const* prefix);

/* save backtrace
 *
 * @param data_head             the data head
 * @param skip_frames           the skiped frame count
 */
static tb_void_t __tb_inline__  tb_pool_data_save_backtrace(tb_pool_data_head_t* data_head, tb_size_t skip_frames)
{ 
    tb_size_t nframe = tb_backtrace_frames(data_head->debug.backtrace, tb_arrayn(data_head->debug.backtrace), skip_frames); 
    if (nframe < tb_arrayn(data_head->debug.backtrace)) tb_memset(data_head->debug.backtrace + nframe, 0, (tb_arrayn(data_head->debug.backtrace) - nframe) * sizeof(tb_cpointer_t)); 
}

#endif


#endif
