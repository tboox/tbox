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
 * @file        reader.c
 * @ingroup     object
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "reader.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the object reader
static tb_object_reader_t*  g_reader[TB_OBJECT_FORMAT_MAXN] = {tb_null};

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_object_reader_set(tb_size_t format, tb_object_reader_t* reader)
{
    // check
    format &= 0x00ff;
    tb_assert_and_check_return_val(reader && (format < tb_arrayn(g_reader)), tb_false);

    // exit the older reader if exists
    tb_object_reader_del(format);

    // set
    g_reader[format] = reader;

    // ok
    return tb_true;
}
tb_void_t tb_object_reader_del(tb_size_t format)
{
    // check
    format &= 0x00ff;
    tb_assert_and_check_return((format < tb_arrayn(g_reader)));

    // exit it
    if (g_reader[format])
    {
        // exit hooker
        if (g_reader[format]->hooker) tb_hash_map_exit(g_reader[format]->hooker);
        g_reader[format]->hooker = tb_null;
        
        // clear it
        g_reader[format] = tb_null;
    }
}
tb_object_reader_t* tb_object_reader_get(tb_size_t format)
{
    // check
    format &= 0x00ff;
    tb_assert_and_check_return_val((format < tb_arrayn(g_reader)), tb_null);

    // ok
    return g_reader[format];
}
tb_object_ref_t tb_object_reader_done(tb_stream_ref_t stream)
{
    // check
    tb_assert_and_check_return_val(stream, tb_null);

    // probe it
    tb_size_t i = 0;
    tb_size_t n = tb_arrayn(g_reader);
    tb_size_t m = 0;
    tb_size_t f = 0;
    for (i = 0; i < n && m < 100; i++)
    {
        // the reader
        tb_object_reader_t* reader = g_reader[i];
        if (reader && reader->probe)
        {
            // the probe score
            tb_size_t score = reader->probe(stream);
            if (score > m) 
            {
                m = score;
                f = i;
            }
        }
    }

    // ok? read it
    return (m && g_reader[f] && g_reader[f]->read)? g_reader[f]->read(stream) : tb_null;
}
