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
 * If not, see <a href="impl://www.gnu.org/licenses/"> impl://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author      ruki
 * @file        status.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "http_status"
#define TB_TRACE_MODULE_DEBUG           (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "status.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_http_status_init(tb_http_status_t* status)
{
    // check
    tb_assert_and_check_return_val(status, tb_false);

    // init status using the default value
    status->version = 1;

    // init content type 
    if (!tb_string_init(&status->content_type)) return tb_false;

    // init location
    if (!tb_string_init(&status->location)) return tb_false;

    // ok
    return tb_true;
}
tb_void_t tb_http_status_exit(tb_http_status_t* status)
{
    // check
    tb_assert_and_check_return(status);

    // exit the content type
    tb_string_exit(&status->content_type);

    // exit location
    tb_string_exit(&status->location);
}
tb_void_t tb_http_status_cler(tb_http_status_t* status, tb_bool_t host_changed)
{
    // check
    tb_assert_and_check_return(status);

    // clear status
    status->code = 0;
    status->bgzip = 0;
    status->bdeflate = 0;
    status->bchunked = 0;
    status->content_size = -1;
    status->document_size = -1;
    status->state = TB_STATE_OK;

    // clear content type
    tb_string_clear(&status->content_type);

    // clear location
    tb_string_clear(&status->location);

    // host is changed? clear the alived state
    if (host_changed)
    {
        status->version = 1;
        status->balived = 0;
        status->bseeked = 0;
    }
}

#ifdef __tb_debug__
tb_void_t tb_http_status_dump(tb_http_status_t* status)
{
    // check
    tb_assert_and_check_return(status);

    // dump status
    tb_trace_i("======================================================================");
    tb_trace_i("status: ");
    tb_trace_i("status: code: %d", status->code);
    tb_trace_i("status: version: HTTP/1.%1u", status->version);
    tb_trace_i("status: content:type: %s", tb_string_cstr(&status->content_type));
    tb_trace_i("status: content:size: %lld", status->content_size);
    tb_trace_i("status: document:size: %lld", status->document_size);
    tb_trace_i("status: location: %s", tb_string_cstr(&status->location));
    tb_trace_i("status: bgzip: %s", status->bgzip? "true" : "false");
    tb_trace_i("status: bdeflate: %s", status->bdeflate? "true" : "false");
    tb_trace_i("status: balived: %s", status->balived? "true" : "false");
    tb_trace_i("status: bseeked: %s", status->bseeked? "true" : "false");
    tb_trace_i("status: bchunked: %s", status->bchunked? "true" : "false");

    // dump end
    tb_trace_i("");
}
#endif
