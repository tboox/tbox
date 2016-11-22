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
 * @file        dump.c
 * @ingroup     utils
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "dump.h"
#include "../stream/stream.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_void_t tb_dump_data(tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return(data && size);

    // dump head
    tb_trace_i("");

    // walk
    tb_size_t           i = 0;
    tb_size_t           n = 147;
    tb_byte_t const*    p = data;
    tb_byte_t const*    e = data + size;
    tb_char_t           info[8192];
    while (p < e)
    {
        // full line?
        tb_char_t* q = info;
        tb_char_t* d = info + sizeof(info);
        if (p + 0x20 <= e)
        {
            // dump offset
            if (q < d) q += tb_snprintf(q, d - q, "%08X ", p - data);

            // dump data
            for (i = 0; i < 0x20; i++)
            {
                if (!(i & 3) && q < d) q += tb_snprintf(q, d - q, " ");
                if (q < d) q += tb_snprintf(q, d - q, " %02X", p[i]);
            }

            // dump spaces
            if (q < d) q += tb_snprintf(q, d - q, "  ");

            // dump characters
            for (i = 0; i < 0x20; i++)
            {
                if (q < d) q += tb_snprintf(q, d - q, "%c", tb_isgraph(p[i])? p[i] : '.');
            }

            // dump it
            if (q < d)
            {
                // end
                *q = '\0';

                // trace
                tb_trace_i("%s", info);
            }

            // update p
            p += 0x20;
        }
        // has left?
        else if (p < e)
        {
            // init padding
            tb_size_t padding = n - 0x20;

            // dump offset
            if (q < d) q += tb_snprintf(q, d - q, "%08X ", p - data); 
            if (padding >= 9) padding -= 9;

            // dump data
            tb_size_t left = e - p;
            for (i = 0; i < left; i++)
            {
                if (!(i & 3)) 
                {
                    if (q < d) q += tb_snprintf(q, d - q, " ");
                    if (padding) padding--;
                }

                if (q < d) q += tb_snprintf(q, d - q, " %02X", p[i]);
                if (padding >= 3) padding -= 3;
            }

            // dump spaces
            while (padding--) if (q < d) q += tb_snprintf(q, d - q, " ");
                
            // dump characters
            for (i = 0; i < left; i++)
            {
                if (q < d) q += tb_snprintf(q, d - q, "%c", tb_isgraph(p[i])? p[i] : '.');
            }

            // dump it
            if (q < d)
            {
                // end
                *q = '\0';

                // trace
                tb_trace_i("%s", info);
            }

            // update p
            p += left;
        }
        // end
        else break;
    }
}
