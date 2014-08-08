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
 * @file        chunked.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "chunked"
#define TB_TRACE_MODULE_DEBUG           (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the chunked filter type
typedef struct __tb_stream_filter_chunked_t
{
    // the filter base
    tb_stream_filter_impl_t     base;

    // the chunked size
    tb_size_t                   size;

    // the chunked read
    tb_size_t                   read;

    // the cache line
    tb_string_t                 line;

}tb_stream_filter_chunked_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_stream_filter_chunked_t* tb_stream_filter_chunked_cast(tb_stream_filter_impl_t* filter)
{
    // check
    tb_assert_and_check_return_val(filter && filter->type == TB_STREAM_FILTER_TYPE_CHUNKED, tb_null);
    return (tb_stream_filter_chunked_t*)filter;
}
/* chunked_data
 *
 *   head     data   tail
 * ea5\r\n ..........\r\n e65\r\n..............\r\n 0\r\n\r\n
 * ---------------------- ------------------------- ---------
 *        chunk0                  chunk1               end
 */
static tb_long_t tb_stream_filter_chunked_spak(tb_stream_filter_impl_t* filter, tb_static_stream_ref_t istream, tb_static_stream_ref_t ostream, tb_long_t sync)
{
    // check
    tb_stream_filter_chunked_t* cfilter = tb_stream_filter_chunked_cast(filter);
    tb_assert_and_check_return_val(cfilter && istream && ostream, -1);
    tb_assert_and_check_return_val(tb_static_stream_valid(istream) && tb_static_stream_valid(ostream), -1);

    // the idata
    tb_byte_t const*    ip = tb_static_stream_pos(istream);
    tb_byte_t const*    ie = tb_static_stream_end(istream);

    // trace
    tb_trace_d("[%p]: isize: %lu, beof: %d", cfilter, tb_static_stream_size(istream), filter->beof);

    // find the eof: '\r\n 0\r\n\r\n'
    if (    !filter->beof
        &&  ip + 6 < ie
        &&  ie[-7] == '\r'
        &&  ie[-6] == '\n'
        &&  ie[-5] == '0'
        &&  ie[-4] == '\r'
        &&  ie[-3] == '\n'
        &&  ie[-2] == '\r'
        &&  ie[-1] == '\n')
    {
        // is eof
        filter->beof = tb_true;
    }

    // the odata
    tb_byte_t*          op = (tb_byte_t*)tb_static_stream_pos(ostream);
    tb_byte_t*          oe = (tb_byte_t*)tb_static_stream_end(ostream);
    tb_byte_t*          ob = op;

    // parse chunked head and chunked tail
    if (!cfilter->size || cfilter->read >= cfilter->size)
    {
        // walk
        while (ip < ie)
        {
            // the charactor
            tb_char_t ch = *ip++;

            // trace
            tb_trace_d("[%p]: character: %x", cfilter, ch);

            // check
            tb_assert_and_check_return_val(ch, -1);
        
            // append char to line
            if (ch != '\n') tb_string_chrcat(&cfilter->line, ch);
            // is line end?
            else
            {
                // check
                tb_char_t const*    pb = tb_string_cstr(&cfilter->line);
                tb_size_t           pn = tb_string_size(&cfilter->line);
                tb_assert_and_check_return_val(pb, -1);

                // trace
                tb_trace_d("[%p]: line: %s", cfilter, tb_string_cstr(&cfilter->line));

                // strip '\r' if exists
                if (pb[pn - 1] == '\r') tb_string_strip(&cfilter->line, pn - 1);

                // is chunked tail? only "\r\n"
                if (!tb_string_size(&cfilter->line)) 
                {
                    // reset size
                    cfilter->read = 0;
                    cfilter->size = 0;

                    // trace
                    tb_trace_d("[%p]: tail", cfilter);

                    // continue
                    continue ;
                }
                // is chunked head? parse size
                else
                {
                    // parse size
                    cfilter->size = tb_s16tou32(pb);

                    // trace
                    tb_trace_d("[%p]: size: %lu", cfilter, cfilter->size);

                    // clear data
                    tb_string_clear(&cfilter->line);

                    // is eof? "0\r\n\r\n"
                    if (!cfilter->size)
                    {
                        // trace
                        tb_trace_d("[%p]: eof", cfilter);

                        // is eof
                        filter->beof = tb_true;

                        // continue to spak the end data 
                        continue ;
                    }

                    // ok
                    break;
                }
            }
        }
    }

    // check
    tb_assert_and_check_return_val(cfilter->read <= cfilter->size, -1);

    // read chunked data
    tb_size_t size = tb_min3(ie - ip, oe - op, cfilter->size - cfilter->read);
    if (size) 
    {
        // copy data
        tb_memcpy((tb_byte_t*)op, ip, size);
        ip += size;
        op += size;

        // update read
        cfilter->read += size;
    }

    // update stream
    tb_static_stream_goto(istream, (tb_byte_t*)ip);
    tb_static_stream_goto(ostream, (tb_byte_t*)op);

    // trace
    tb_trace_d("[%p]: read: %lu, size: %lu, beof: %u, ileft: %lu", cfilter, cfilter->read, cfilter->size, filter->beof, tb_static_stream_left(istream));

    // ok
    return (op - ob);
}
static tb_void_t tb_stream_filter_chunked_clos(tb_stream_filter_impl_t* filter)
{
    // check
    tb_stream_filter_chunked_t* cfilter = tb_stream_filter_chunked_cast(filter);
    tb_assert_and_check_return(cfilter);

    // clear size
    cfilter->size = 0;

    // clear read
    cfilter->read = 0;

    // clear line
    tb_string_clear(&cfilter->line);
}
static tb_void_t tb_stream_filter_chunked_exit(tb_stream_filter_impl_t* filter)
{
    // check
    tb_stream_filter_chunked_t* cfilter = tb_stream_filter_chunked_cast(filter);
    tb_assert_and_check_return(cfilter);

    // exit line
    tb_string_exit(&cfilter->line);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_stream_filter_ref_t tb_stream_filter_init_from_chunked(tb_bool_t dechunked)
{
    // done
    tb_bool_t                   ok = tb_false;
    tb_stream_filter_chunked_t* filter = tb_null;
    do
    {
        // noimpl for encoding chunked
        if (!dechunked)
        {
            tb_trace_noimpl();
            break;
        }

        // make filter
        filter = tb_malloc0_type(tb_stream_filter_chunked_t);
        tb_assert_and_check_break(filter);

        // init filter 
        if (!tb_stream_filter_impl_init((tb_stream_filter_impl_t*)filter, TB_STREAM_FILTER_TYPE_CHUNKED)) break;
        filter->base.spak = tb_stream_filter_chunked_spak;
        filter->base.clos = tb_stream_filter_chunked_clos;
        filter->base.exit = tb_stream_filter_chunked_exit;

        // init line
        if (!tb_string_init(&filter->line)) break;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit filter
        tb_stream_filter_exit((tb_stream_filter_ref_t)filter);
        filter = tb_null;
    }

    // ok?
    return (tb_stream_filter_ref_t)filter;
}

