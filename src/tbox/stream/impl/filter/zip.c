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
 * @file        zip.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../../zip/zip.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the zip filter type
typedef struct __tb_stream_filter_zip_t
{
    // the filter base
    tb_stream_filter_impl_t     base;

    // the algo
    tb_size_t                   algo;

    // the action
    tb_size_t                   action;

    // the zip 
    tb_zip_ref_t                zip;

}tb_stream_filter_zip_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_stream_filter_zip_t* tb_stream_filter_zip_cast(tb_stream_filter_impl_t* filter)
{
    // check
    tb_assert_and_check_return_val(filter && filter->type == TB_STREAM_FILTER_TYPE_ZIP, tb_null);
    return (tb_stream_filter_zip_t*)filter;
}
static tb_bool_t tb_stream_filter_zip_open(tb_stream_filter_impl_t* filter)
{
    // check
    tb_stream_filter_zip_t* zfilter = tb_stream_filter_zip_cast(filter);
    tb_assert_and_check_return_val(zfilter && !zfilter->zip, tb_false);

    // init zip
    zfilter->zip = tb_zip_init(zfilter->algo, zfilter->action);
    tb_assert_and_check_return_val(zfilter->zip, tb_false);

    // ok
    return tb_true;
}
static tb_void_t tb_stream_filter_zip_clos(tb_stream_filter_impl_t* filter)
{
    // check
    tb_stream_filter_zip_t* zfilter = tb_stream_filter_zip_cast(filter);
    tb_assert_and_check_return(zfilter);

    // exit zip
    if (zfilter->zip) tb_zip_exit(zfilter->zip);
    zfilter->zip = tb_null;
}
static tb_long_t tb_stream_filter_zip_spak(tb_stream_filter_impl_t* filter, tb_static_stream_ref_t istream, tb_static_stream_ref_t ostream, tb_long_t sync)
{
    // check
    tb_stream_filter_zip_t* zfilter = tb_stream_filter_zip_cast(filter);
    tb_assert_and_check_return_val(zfilter && zfilter->zip && istream && ostream, -1);

    // spak it
    return tb_zip_spak(zfilter->zip, istream, ostream, sync);
}
static tb_void_t tb_stream_filter_zip_exit(tb_stream_filter_impl_t* filter)
{
    // check
    tb_stream_filter_zip_t* zfilter = tb_stream_filter_zip_cast(filter);
    tb_assert_and_check_return(zfilter);

    // exit zip
    if (zfilter->zip) tb_zip_exit(zfilter->zip);
    zfilter->zip = tb_null;
}
static tb_bool_t tb_stream_filter_zip_ctrl(tb_stream_filter_impl_t* filter, tb_size_t ctrl, tb_va_list_t args)
{
    // check
    tb_stream_filter_zip_t* zfilter = tb_stream_filter_zip_cast(filter);
    tb_assert_and_check_return_val(zfilter && ctrl, tb_false);

    // ctrl
    switch (ctrl)
    {
    case TB_STREAM_FILTER_CTRL_ZIP_GET_ALGO:
        {
            // the palgo
            tb_size_t* palgo = (tb_size_t*)tb_va_arg(args, tb_size_t*);
            tb_assert_and_check_break(palgo);

            // get algo
            *palgo = zfilter->algo;

            // ok
            return tb_true;
        }
    case TB_STREAM_FILTER_CTRL_ZIP_SET_ALGO:
        {
            // set algo
            zfilter->algo = (tb_size_t)tb_va_arg(args, tb_size_t);

            // ok
            return tb_true;
        }
    case TB_STREAM_FILTER_CTRL_ZIP_GET_ACTION:
        {
            // the paction
            tb_size_t* paction = (tb_size_t*)tb_va_arg(args, tb_size_t*);
            tb_assert_and_check_break(paction);

            // get action
            *paction = zfilter->action;

            // ok
            return tb_true;
        }
    case TB_STREAM_FILTER_CTRL_ZIP_SET_ACTION:
        {
            // set action
            zfilter->action = (tb_size_t)tb_va_arg(args, tb_size_t);

            // ok
            return tb_true;
        }
    default:
        break;
    }
    return tb_false;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_stream_filter_ref_t tb_stream_filter_init_from_zip(tb_size_t algo, tb_size_t action)
{
    // done
    tb_bool_t               ok = tb_false;
    tb_stream_filter_zip_t* filter = tb_null;
    do
    {
        // make filter
        filter = tb_malloc0_type(tb_stream_filter_zip_t);
        tb_assert_and_check_break(filter);

        // init filter 
        if (!tb_stream_filter_impl_init((tb_stream_filter_impl_t*)filter, TB_STREAM_FILTER_TYPE_ZIP)) break;
        filter->base.open   = tb_stream_filter_zip_open;
        filter->base.clos   = tb_stream_filter_zip_clos;
        filter->base.spak   = tb_stream_filter_zip_spak;
        filter->base.exit   = tb_stream_filter_zip_exit;
        filter->base.ctrl   = tb_stream_filter_zip_ctrl;
        filter->algo        = algo;
        filter->action      = action;

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

