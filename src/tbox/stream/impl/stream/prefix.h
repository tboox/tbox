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
 * Copyright (C) 2009 - 2017, ruki All rights reserved.
 *
 * @author      ruki
 * @file        prefix.h
 *
 */
#ifndef TB_STREAM_IMPL_STREAM_PREFIX_H
#define TB_STREAM_IMPL_STREAM_PREFIX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the stream impl
#define tb_stream_impl(stream)          ((stream)? &(((tb_stream_impl_t*)(stream))[-1]) : tb_null)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the stream impl type
typedef struct __tb_stream_impl_t
{   
    // the stream type
    tb_uint8_t          type;

    // is writed?
    tb_uint8_t          bwrited;

    // the url
    tb_url_t            url;

    /* the internal state for killing stream in the other thread
     *
     * <pre>
     * TB_STATE_CLOSED
     * TB_STATE_OPENED
     * TB_STATE_KILLED
     * TB_STATE_OPENING
     * TB_STATE_KILLING
     * </pre>
     */
    tb_atomic_t         istate;

    // the timeout
    tb_long_t           timeout;

    /* the stream state
     *
     * <pre>
     * TB_STATE_KILLED
     * TB_STATE_WAIT_FAILED
     * </pre>
     */
    tb_size_t           state;

    // the offset
    tb_hize_t           offset;

    // the cache
    tb_queue_buffer_t   cache;

    // wait 
    tb_long_t           (*wait)(tb_stream_ref_t stream, tb_size_t wait, tb_long_t timeout);

    // open
    tb_bool_t           (*open)(tb_stream_ref_t stream);

    // clos
    tb_bool_t           (*clos)(tb_stream_ref_t stream);

    // read
    tb_long_t           (*read)(tb_stream_ref_t stream, tb_byte_t* data, tb_size_t size);

    // writ
    tb_long_t           (*writ)(tb_stream_ref_t stream, tb_byte_t const* data, tb_size_t size);

    // seek
    tb_bool_t           (*seek)(tb_stream_ref_t stream, tb_hize_t offset);

    // sync
    tb_bool_t           (*sync)(tb_stream_ref_t stream, tb_bool_t bclosing);

    // ctrl 
    tb_bool_t           (*ctrl)(tb_stream_ref_t stream, tb_size_t ctrl, tb_va_list_t args);

    // exit
    tb_void_t           (*exit)(tb_stream_ref_t stream);

    // kill
    tb_void_t           (*kill)(tb_stream_ref_t stream);

}tb_stream_impl_t;


#endif
