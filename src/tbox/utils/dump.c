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
#include "../libc/libc.h"
#include "../stream/stream.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_void_t tb_dump_data(tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return(data && size);

    // init stream
    tb_stream_ref_t stream = tb_stream_init_from_data(data, size);
    if (stream)
    {
        // open stream
        if (tb_stream_open(stream))
        {
            // dump stream
            tb_dump_data_from_stream(stream);
        }
    
        // exit stream
        tb_stream_exit(stream);
    }
}
tb_void_t tb_dump_data_from_url(tb_char_t const* url)
{
    // check
    tb_assert_and_check_return(url);

    // init stream
    tb_stream_ref_t stream = tb_stream_init_from_url(url);
    if (stream)
    {
        // open stream
        if (tb_stream_open(stream))
        {
            // dump stream
            tb_dump_data_from_stream(stream);
        }
    
        // exit stream
        tb_stream_exit(stream);
    }
}
tb_void_t tb_dump_data_from_stream(tb_stream_ref_t stream)
{
    // check
    tb_assert_and_check_return(stream);

    // init 
    tb_size_t offset = 0;

    // dump head
    tb_trace_i("");

    // done
    tb_char_t info[8192];
    while (!tb_stream_beof(stream))
    {
        // read line
        tb_size_t i = 0;
        tb_size_t n = 147;
        tb_long_t read = 0;
        tb_byte_t line[0x20];
        while (read < 0x20)
        {
            // read data
            tb_long_t real = tb_stream_read(stream, line + read, 0x20 - read);
            // has data?
            if (real > 0) read += real;
            // no data?
            else if (!real)
            {
                // wait
                tb_long_t e = tb_stream_wait(stream, TB_SOCKET_EVENT_RECV, tb_stream_timeout(stream));
                tb_assert_and_check_break(e >= 0);

                // timeout?
                tb_check_break(e);

                // has read?
                tb_assert_and_check_break(e & TB_SOCKET_EVENT_RECV);
            }
            else break;
        }

        // full line?
        tb_char_t* p = info;
        tb_char_t* e = info + sizeof(info);
        if (read == 0x20)
        {
            // dump offset
            if (p < e) p += tb_snprintf(p, e - p, "%08X ", offset);

            // dump data
            for (i = 0; i < 0x20; i++)
            {
                if (!(i & 3) && p < e) p += tb_snprintf(p, e - p, " ");
                if (p < e) p += tb_snprintf(p, e - p, " %02X", line[i]);
            }

            // dump spaces
            if (p < e) p += tb_snprintf(p, e - p, "  ");

            // dump characters
            for (i = 0; i < 0x20; i++)
            {
                if (p < e) p += tb_snprintf(p, e - p, "%c", tb_isgraph(line[i])? line[i] : '.');
            }

            // dump it
            if (p < e)
            {
                // end
                *p = '\0';

                // trace
                tb_trace_i("%s", info);
            }

            // update offset
            offset += 0x20;
        }
        // has left?
        else if (read)
        {
            // init padding
            tb_size_t padding = n - 0x20;

            // dump offset
            if (p < e) p += tb_snprintf(p, e - p, "%08X ", offset); 
            if (padding >= 9) padding -= 9;

            // dump data
            for (i = 0; i < read; i++)
            {
                if (!(i & 3)) 
                {
                    if (p < e) p += tb_snprintf(p, e - p, " ");
                    if (padding) padding--;
                }

                if (p < e) p += tb_snprintf(p, e - p, " %02X", line[i]);
                if (padding >= 3) padding -= 3;
            }

            // dump spaces
            while (padding--) if (p < e) p += tb_snprintf(p, e - p, " ");
                
            // dump characters
            for (i = 0; i < read; i++)
            {
                if (p < e) p += tb_snprintf(p, e - p, "%c", tb_isgraph(line[i])? line[i] : '.');
            }

            // dump it
            if (p < e)
            {
                // end
                *p = '\0';

                // trace
                tb_trace_i("%s", info);
            }

            // update offset
            offset += read;
        }
        // end
        else break;
    }
}
