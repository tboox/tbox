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
 * @ingroup     object
 *
 */
#ifndef TB_OBJECT_IMPL_PREFIX_H
#define TB_OBJECT_IMPL_PREFIX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"
#include "../object.h"
#include "../../stream/stream.h"
#include "../../charset/charset.h"
#include "../../container/container.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// need bytes
#define tb_oc_object_need_bytes(x)     \
                                    (((tb_uint64_t)(x)) < (1ull << 8) ? 1 : \
                                    (((tb_uint64_t)(x)) < (1ull << 16) ? 2 : \
                                    (((tb_uint64_t)(x)) < (1ull << 32) ? 4 : 8)))

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the object reader type
typedef struct __tb_oc_reader_t
{
    /// the hooker
    tb_hash_map_ref_t           hooker;

    /// probe format
    tb_size_t                   (*probe)(tb_stream_ref_t stream);

    /// read it
    tb_oc_object_ref_t          (*read)(tb_stream_ref_t stream);

}tb_oc_reader_t;

// the object writer type
typedef struct __tb_oc_writer_t
{
    /// the hooker
    tb_hash_map_ref_t           hooker;

    /// writ it
    tb_long_t                   (*writ)(tb_stream_ref_t stream, tb_oc_object_ref_t object, tb_bool_t deflate);

}tb_oc_writer_t;

#endif
