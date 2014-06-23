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
 * @file        writer.c
 * @ingroup     object
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "writer.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the object writer
static tb_object_writer_t*  g_writer[TB_OBJECT_FORMAT_MAXN] = {tb_object_null};

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_object_writer_set(tb_size_t format, tb_object_writer_t* writer)
{
    // check
    format &= 0x00ff;
    tb_assert_and_check_return_val(writer && (format < tb_object_arrayn(g_writer)), tb_false);

    // exit the older writer if exists
    tb_object_writer_del(format);

    // set
    g_writer[format] = writer;

    // ok
    return tb_true;
}
tb_void_t tb_object_writer_del(tb_size_t format)
{
    // check
    format &= 0x00ff;
    tb_assert_and_check_return((format < tb_object_arrayn(g_writer)));

    // exit it
    if (g_writer[format])
    {
        // exit hooker
        if (g_writer[format]->hooker) tb_hash_exit(g_writer[format]->hooker);
        g_writer[format]->hooker = tb_object_null;
        
        // clear it
        g_writer[format] = tb_object_null;
    }
}
tb_object_writer_t* tb_object_writer_get(tb_size_t format)
{
    // check
    format &= 0x00ff;
    tb_assert_and_check_return_val((format < tb_object_arrayn(g_writer)), tb_object_null);

    // ok
    return g_writer[format];
}
tb_long_t tb_object_writer_done(tb_object_t* object, tb_basic_stream_t* stream, tb_size_t format)
{
    // check
    tb_assert_and_check_return_val(object && stream, -1);

    // the writer
    tb_object_writer_t* writer = tb_object_writer_get(format);
    tb_assert_and_check_return_val(writer && writer->writ, -1);

    // writ it
    return writer->writ(stream, object, (format & TB_OBJECT_FORMAT_DEFLATE)? tb_true : tb_false);
}
