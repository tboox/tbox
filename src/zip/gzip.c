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
 * @file        gzip.c
 * @ingroup     zip
 *
 */
/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "gzip"
#define TB_TRACE_MODULE_DEBUG               (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "gzip.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implements
 */
static __tb_inline__ tb_zip_gzip_t* tb_zip_gzip_cast(tb_zip_t* zip)
{
    tb_assert_and_check_return_val(zip && zip->algo == TB_ZIP_ALGO_GZIP, tb_null);
    return (tb_zip_gzip_t*)zip;
}
static tb_long_t tb_zip_gzip_spak_deflate(tb_zip_t* zip, tb_static_stream_t* ist, tb_static_stream_t* ost, tb_long_t sync)
{
    tb_zip_gzip_t* gzip = tb_zip_gzip_cast(zip);
    tb_assert_and_check_return_val(gzip && ist && ost, -1);

    // the input stream, @note maybe null for flush the end data
    tb_byte_t* ip = ist->p;
    tb_byte_t* ie = ist->e;

    // the output stream
    tb_byte_t* op = ost->p;
    tb_byte_t* oe = ost->e;
    tb_assert_and_check_return_val(op && oe, -1);

    // attach zst
    gzip->zst.next_in = (Bytef*)ip;
    gzip->zst.avail_in = (uInt)(ie - ip);

    gzip->zst.next_out = (Bytef*)op;
    gzip->zst.avail_out = (uInt)(oe - op);

    // deflate 
    tb_int_t r = deflate(&gzip->zst, sync > 0? Z_SYNC_FLUSH : (sync < 0? Z_FINISH : Z_NO_FLUSH));
    tb_assert_and_check_return_val(r == Z_OK || r == Z_STREAM_END, -1);
    tb_trace_d("deflate: %u => %u, sync: %ld", (tb_size_t)(ie - ip), (tb_size_t)((tb_byte_t*)gzip->zst.next_out - op), sync);

    // update 
    ist->p = (tb_byte_t*)gzip->zst.next_in;
    ost->p = (tb_byte_t*)gzip->zst.next_out;

    // end?
    tb_check_return_val(r != Z_STREAM_END || ost->p > op, -1);

    // ok?
    return (ost->p - op);
}
static tb_long_t tb_zip_gzip_spak_inflate(tb_zip_t* zip, tb_static_stream_t* ist, tb_static_stream_t* ost, tb_long_t sync)
{
    tb_zip_gzip_t* gzip = tb_zip_gzip_cast(zip);
    tb_assert_and_check_return_val(gzip && ist && ost, -1);

    // the input stream
    tb_byte_t* ip = ist->p;
    tb_byte_t* ie = ist->e;
    tb_check_return_val(ip && ip < ie, 0);

    // the output stream
    tb_byte_t* op = ost->p;
    tb_byte_t* oe = ost->e;
    tb_assert_and_check_return_val(op && oe, -1);

    // attach zst
    gzip->zst.next_in = (Bytef*)ip;
    gzip->zst.avail_in = (uInt)(ie - ip);

    gzip->zst.next_out = (Bytef*)op;
    gzip->zst.avail_out = (uInt)(oe - op);

    // inflate 
    tb_int_t r = inflate(&gzip->zst, !sync? Z_NO_FLUSH : Z_SYNC_FLUSH);
    tb_assert_and_check_return_val(r == Z_OK || r == Z_STREAM_END, -1);
    tb_trace_d("inflate: %u => %u, sync: %ld", ie - ip, (tb_byte_t*)gzip->zst.next_out - op, sync);

    // update 
    ist->p = (tb_byte_t*)gzip->zst.next_in;
    ost->p = (tb_byte_t*)gzip->zst.next_out;

    // end?
    tb_check_return_val(r != Z_STREAM_END || ost->p > op, -1);

    // ok?
    return (ost->p - op);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_zip_t* tb_zip_gzip_init(tb_size_t action)
{   
    // alloc
    tb_zip_t* zip = (tb_zip_t*)tb_malloc0(sizeof(tb_zip_gzip_t));
    tb_assert_and_check_return_val(zip, tb_null);
    
    // init zip
    zip->algo       = TB_ZIP_ALGO_GZIP;
    zip->action     = action;

    // open zst
    switch (action)
    {
    case TB_ZIP_ACTION_INFLATE:
        {
            // init spak
            zip->spak = tb_zip_gzip_spak_inflate;

            // init inflate
            if (inflateInit2(&((tb_zip_gzip_t*)zip)->zst, 47) != Z_OK) goto fail;
        }
        break;
    case TB_ZIP_ACTION_DEFLATE:
        {
            // init spak
            zip->spak = tb_zip_gzip_spak_deflate;

            // init deflate
            if (deflateInit2(&((tb_zip_gzip_t*)zip)->zst, Z_DEFAULT_COMPRESSION, Z_DEFLATED, MAX_WBITS + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) goto fail;
        }
        break;
    default:
        break;
    }

    // ok
    return zip;

fail:
    if (zip) tb_free(zip);
    return tb_null;
}
tb_void_t tb_zip_gzip_exit(tb_zip_t* zip)
{
    tb_zip_gzip_t* gzip = tb_zip_gzip_cast(zip);
    if (gzip) 
    {
        // close zst
        if (zip->action == TB_ZIP_ACTION_INFLATE) inflateEnd(&(gzip->zst));
        else if (zip->action == TB_ZIP_ACTION_DEFLATE) deflateEnd(&(gzip->zst));

        // free it
        tb_free(gzip);
    }
}

