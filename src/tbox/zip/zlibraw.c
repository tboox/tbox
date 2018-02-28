/*!The Treasure Box Library
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * Copyright (C) 2009 - 2018, TBOOX Open Source Group.
 *
 * @author      ruki
 * @file        zlibraw.c
 * @ingroup     zip
 *
 */
/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "zlibraw"
#define TB_TRACE_MODULE_DEBUG               (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "zlibraw.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implements
 */
static __tb_inline__ tb_zip_zlibraw_t* tb_zip_zlibraw_cast(tb_zip_ref_t zip)
{
    tb_assert_and_check_return_val(zip && zip->algo == TB_ZIP_ALGO_ZLIBRAW, tb_null);
    return (tb_zip_zlibraw_t*)zip;
}
static tb_long_t tb_zip_zlibraw_spak_deflate(tb_zip_ref_t zip, tb_static_stream_ref_t ist, tb_static_stream_ref_t ost, tb_long_t sync)
{
    tb_zip_zlibraw_t* zlibraw = tb_zip_zlibraw_cast(zip);
    tb_assert_and_check_return_val(zlibraw && ist && ost, -1);

    // the input stream
    tb_byte_t* ip = ist->p;
    tb_byte_t* ie = ist->e;
    tb_check_return_val(ip && ip < ie, 0);

    // the output stream
    tb_byte_t* op = ost->p;
    tb_byte_t* oe = ost->e;
    tb_assert_and_check_return_val(op && oe, -1);

    // attach zstream
    zlibraw->zstream.next_in = (Bytef*)ip;
    zlibraw->zstream.avail_in = (uInt)(ie - ip);

    zlibraw->zstream.next_out = (Bytef*)op;
    zlibraw->zstream.avail_out = (uInt)(oe - op);

    // deflate 
    tb_int_t r = deflate(&zlibraw->zstream, !sync? Z_NO_FLUSH : Z_SYNC_FLUSH);
    tb_assertf_and_check_return_val(r == Z_OK || r == Z_STREAM_END, -1, "sync: %ld, error: %d", sync, r);
    tb_trace_d("deflate: %u => %u, sync: %ld", ie - ip, (tb_byte_t*)zlibraw->zstream.next_out - op, sync);

    // update 
    ist->p = (tb_byte_t*)zlibraw->zstream.next_in;
    ost->p = (tb_byte_t*)zlibraw->zstream.next_out;

    // end?
    tb_check_return_val(r != Z_STREAM_END || ost->p > op, -1);

    // ok?
    return (ost->p - op);
}
static tb_long_t tb_zip_zlibraw_spak_inflate(tb_zip_ref_t zip, tb_static_stream_ref_t ist, tb_static_stream_ref_t ost, tb_long_t sync)
{
    tb_zip_zlibraw_t* zlibraw = tb_zip_zlibraw_cast(zip);
    tb_assert_and_check_return_val(zlibraw && ist && ost, -1);

    // the input stream
    tb_byte_t* ip = ist->p;
    tb_byte_t* ie = ist->e;
    tb_check_return_val(ip && ip < ie, 0);

    // the output stream
    tb_byte_t* op = ost->p;
    tb_byte_t* oe = ost->e;
    tb_assert_and_check_return_val(op && oe, -1);

    // attach zstream
    zlibraw->zstream.next_in = (Bytef*)ip;
    zlibraw->zstream.avail_in = (uInt)(ie - ip);

    zlibraw->zstream.next_out = (Bytef*)op;
    zlibraw->zstream.avail_out = (uInt)(oe - op);

    // inflate 
    tb_int_t r = inflate(&zlibraw->zstream, !sync? Z_NO_FLUSH : Z_SYNC_FLUSH);
    tb_assertf_and_check_return_val(r == Z_OK || r == Z_STREAM_END, -1, "sync: %ld, error: %d", sync, r);
    tb_trace_d("inflate: %u => %u, sync: %ld", ie - ip, (tb_byte_t*)zlibraw->zstream.next_out - op, sync);

    // update 
    ist->p = (tb_byte_t*)zlibraw->zstream.next_in;
    ost->p = (tb_byte_t*)zlibraw->zstream.next_out;

    // end?
    tb_check_return_val(r != Z_STREAM_END || ost->p > op, -1);

    // ok?
    return (ost->p - op);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_zip_ref_t tb_zip_zlibraw_init(tb_size_t action)
{   
    // done
    tb_bool_t           ok = tb_false;
    tb_zip_zlibraw_t*   zip = tb_null;
    do
    {
        // make zip
        zip = tb_malloc0_type(tb_zip_zlibraw_t);
        tb_assert_and_check_break(zip);
        
        // init algo
        zip->base.algo = TB_ZIP_ALGO_ZLIBRAW;

        // open zstream
        if (action == TB_ZIP_ACTION_INFLATE)
        {
            // init spak
            zip->base.spak = tb_zip_zlibraw_spak_inflate;

            // init zstream, no zlib header
            if (inflateInit(&((tb_zip_zlibraw_t*)zip)->zstream) != Z_OK) break;
        }
        else if (action == TB_ZIP_ACTION_DEFLATE)
        {
            // init spak
            zip->base.spak = tb_zip_zlibraw_spak_deflate;

            // init zstream
            if (deflateInit(&((tb_zip_zlibraw_t*)zip)->zstream, Z_DEFAULT_COMPRESSION) != Z_OK) break;
        }

        // init action after initializing zstream
        zip->base.action = (tb_uint16_t)action;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (zip) tb_zip_zlibraw_exit((tb_zip_ref_t)zip);
        zip = tb_null;
    }

    // ok?
    return (tb_zip_ref_t)zip;
}
tb_void_t tb_zip_zlibraw_exit(tb_zip_ref_t zip)
{
    // check
    tb_zip_zlibraw_t* zlibraw = tb_zip_zlibraw_cast(zip);
    tb_assert_and_check_return(zlibraw);

    // exit zstream
    if (zip->action == TB_ZIP_ACTION_INFLATE) inflateEnd(&(zlibraw->zstream));
    else if (zip->action == TB_ZIP_ACTION_DEFLATE) deflateEnd(&(zlibraw->zstream));

    // free it
    tb_free(zlibraw);
}

