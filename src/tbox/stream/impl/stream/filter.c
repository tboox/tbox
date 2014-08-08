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
 * @file        filter.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the stream impl type
typedef struct __tb_stream_filter_impl_t
{
    // the filter 
    tb_stream_filter_ref_t  filter;

    // the filter is referenced? need not exit it
    tb_bool_t               bref;

    // is eof?
    tb_bool_t               beof;

    // is wait?
    tb_bool_t               wait;

    // the last
    tb_long_t               last;

    // the mode, none: 0, read: 1, writ: -1
    tb_long_t               mode;

    // the stream
    tb_stream_ref_t         stream;

}tb_stream_filter_impl_t;
 
/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_stream_filter_impl_t* tb_stream_filter_impl_cast(tb_stream_ref_t stream)
{
    // check
    tb_assert_and_check_return_val(stream && tb_stream_type(stream) == TB_STREAM_TYPE_FLTR, tb_null);

    // ok?
    return (tb_stream_filter_impl_t*)stream;
}
static tb_bool_t tb_stream_filter_impl_open(tb_stream_ref_t stream)
{
    // check
    tb_stream_filter_impl_t* impl = tb_stream_filter_impl_cast(stream);
    tb_assert_and_check_return_val(impl && impl->stream, tb_false);

    // clear mode
    impl->mode = 0;

    // clear last
    impl->last = 0;

    // clear wait
    impl->wait = tb_false;

    // clear eof
    impl->beof = tb_false;

    // open filter
    if (impl->filter && !tb_stream_filter_open(impl->filter)) return tb_false;

    // ok
    return tb_stream_open(impl->stream);
}
static tb_bool_t tb_stream_filter_impl_clos(tb_stream_ref_t stream)
{
    // check
    tb_stream_filter_impl_t* impl = tb_stream_filter_impl_cast(stream);
    tb_assert_and_check_return_val(impl && impl->stream, tb_false);
    
    // sync the end filter data
    if (impl->filter && impl->mode == -1)
    {
        // spak data
        tb_byte_t const*    data = tb_null;
        tb_long_t           size = tb_stream_filter_spak(impl->filter, tb_null, 0, &data, 0, -1);
        if (size > 0 && data)
        {
            // writ data
            if (!tb_stream_bwrit(impl->stream, data, size)) return tb_false;
        }
    }

    // done
    tb_bool_t ok = tb_stream_clos(impl->stream);

    // ok?
    if (ok) 
    {
        // clear mode
        impl->mode = 0;

        // clear last
        impl->last = 0;

        // clear wait
        impl->wait = tb_false;

        // clear eof
        impl->beof = tb_false;

        // close the filter
        if (impl->filter) tb_stream_filter_clos(impl->filter);
    }

    // ok?
    return ok;
}
static tb_void_t tb_stream_filter_impl_exit(tb_stream_ref_t stream)
{   
    // check
    tb_stream_filter_impl_t* impl = tb_stream_filter_impl_cast(stream);
    tb_assert_and_check_return(impl);

    // exit it
    if (!impl->bref && impl->filter) tb_stream_filter_exit(impl->filter);
    impl->filter = tb_null;
    impl->bref = tb_false;
}
static tb_void_t tb_stream_filter_impl_kill(tb_stream_ref_t stream)
{   
    // check
    tb_stream_filter_impl_t* impl = tb_stream_filter_impl_cast(stream);
    tb_assert_and_check_return(impl);

    // kill it
    if (impl->stream) tb_stream_kill(impl->stream);
}
static tb_long_t tb_stream_filter_impl_read(tb_stream_ref_t stream, tb_byte_t* data, tb_size_t size)
{
    // check
    tb_stream_filter_impl_t* impl = tb_stream_filter_impl_cast(stream);
    tb_assert_and_check_return_val(impl && impl->stream, -1);

    // read 
    tb_long_t real = tb_stream_read(impl->stream, data, size);

    // done filter
    if (impl->filter)
    {
        // save mode: read
        if (!impl->mode) impl->mode = 1;

        // check mode
        tb_assert_and_check_return_val(impl->mode == 1, -1);

        // save last
        impl->last = real;

        // eof?
        if (real < 0 || (!real && impl->wait) || tb_stream_filter_beof(impl->filter))
            impl->beof = tb_true;
        // clear wait
        else if (real > 0) impl->wait = tb_false;

        // spak data
        tb_byte_t const* odata = tb_null;
        if (real) real = tb_stream_filter_spak(impl->filter, data, real < 0? 0 : real, &odata, size, impl->beof? -1 : 0);
        // no data? try to sync it
        if (!real) real = tb_stream_filter_spak(impl->filter, tb_null, 0, &odata, size, impl->beof? -1 : 1);

        // has data? save it
        if (real > 0 && odata) tb_memcpy(data, odata, real);

        // eof?
        if (impl->beof && !real) real = -1;
    }

    // ok? 
    return real;
}
static tb_long_t tb_stream_filter_impl_writ(tb_stream_ref_t stream, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_stream_filter_impl_t* impl = tb_stream_filter_impl_cast(stream);
    tb_assert_and_check_return_val(impl && impl->stream, -1);

    // done filter
    if (impl->filter && data && size)
    {
        // save mode: writ
        if (!impl->mode) impl->mode = -1;

        // check mode
        tb_assert_and_check_return_val(impl->mode == -1, -1);

        // spak data
        tb_long_t real = tb_stream_filter_spak(impl->filter, data, size, &data, size, 0);
        tb_assert_and_check_return_val(real >= 0, -1);

        // no data?
        tb_check_return_val(real, 0);

        // save size
        size = real;
    }

    // writ 
    return tb_stream_writ(impl->stream, data, size);
}
static tb_bool_t tb_stream_filter_impl_sync(tb_stream_ref_t stream, tb_bool_t bclosing)
{
    // check
    tb_stream_filter_impl_t* impl = tb_stream_filter_impl_cast(stream);
    tb_assert_and_check_return_val(impl && impl->stream, tb_false);

    // done filter
    if (impl->filter)
    {
        // save mode: writ
        if (!impl->mode) impl->mode = -1;

        // check mode
        tb_assert_and_check_return_val(impl->mode == -1, tb_false);

        // spak data
        tb_byte_t const*    data = tb_null;
        tb_long_t           real = -1;
        while ( !tb_stream_is_killed(stream)
            &&  (real = tb_stream_filter_spak(impl->filter, tb_null, 0, &data, 0, bclosing? -1 : 1)) > 0
            &&  data)
        {
            if (!tb_stream_bwrit(impl->stream, data, real)) return tb_false;
        }
    }

    // writ 
    return tb_stream_sync(impl->stream, bclosing);
}
static tb_long_t tb_stream_filter_impl_wait(tb_stream_ref_t stream, tb_size_t wait, tb_long_t timeout)
{
    // check
    tb_stream_filter_impl_t* impl = tb_stream_filter_impl_cast(stream);
    tb_assert_and_check_return_val(impl && impl->stream, -1);

    // done
    tb_long_t ok = -1;
    if (impl->filter && impl->mode == 1)
    {
        // wait ok
        if (impl->last > 0) ok = wait;
        // need wait
        else if (!impl->last && !impl->beof && !tb_stream_filter_beof(impl->filter))
        {
            // wait
            ok = tb_stream_wait(impl->stream, wait, timeout);

            // eof?
            if (!ok) 
            {
                // wait ok and continue to read or writ
                ok = wait;

                // set eof
                impl->beof = tb_true;
            }
            // wait ok
            else impl->wait = tb_true;
        }
        // eof
        else 
        {   
            // wait ok and continue to read or writ
            ok = wait;

            // set eof
            impl->beof = tb_true;
        }
    }
    else ok = tb_stream_wait(impl->stream, wait, timeout);

    // ok?
    return ok;
}
static tb_bool_t tb_stream_filter_impl_ctrl(tb_stream_ref_t stream, tb_size_t ctrl, tb_va_list_t args)
{
    // check
    tb_stream_filter_impl_t* impl = tb_stream_filter_impl_cast(stream);
    tb_assert_and_check_return_val(impl, tb_false);

    // ctrl
    switch (ctrl)
    {
    case TB_STREAM_CTRL_FLTR_SET_STREAM:
        {
            // check
            tb_assert_and_check_break(tb_stream_is_closed(stream));

            // set stream
            impl->stream = (tb_stream_ref_t)tb_va_arg(args, tb_stream_ref_t);

            // ok
            return tb_true;
        }
    case TB_STREAM_CTRL_FLTR_GET_STREAM:
        {
            // the pstream
            tb_stream_ref_t* pstream = (tb_stream_ref_t*)tb_va_arg(args, tb_stream_ref_t*);
            tb_assert_and_check_break(pstream);

            // set stream
            *pstream = impl->stream;

            // ok
            return tb_true;
        }
    case TB_STREAM_CTRL_FLTR_SET_FILTER:
        {
            // check
            tb_assert_and_check_break(tb_stream_is_closed(stream));

            // exit filter first if exists
            if (!impl->bref && impl->filter) tb_stream_filter_exit(impl->filter);

            // set filter
            tb_stream_filter_ref_t filter = (tb_stream_filter_ref_t)tb_va_arg(args, tb_stream_filter_ref_t);
            impl->filter = filter;
            impl->bref = filter? tb_true : tb_false;

            // ok
            return tb_true;
        }
    case TB_STREAM_CTRL_FLTR_GET_FILTER:
        {
            // the pfilter
            tb_stream_filter_ref_t* pfilter = (tb_stream_filter_ref_t*)tb_va_arg(args, tb_stream_filter_ref_t*);
            tb_assert_and_check_break(pfilter);

            // set filter
            *pfilter = impl->filter;

            // ok
            return tb_true;
        }
    default:
        break;
    }

    // failed
    return tb_false;
}
/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_stream_ref_t tb_stream_init_filter()
{
    return tb_stream_init(  TB_STREAM_TYPE_FLTR
                        ,   sizeof(tb_stream_filter_impl_t)
                        ,   0
                        ,   tb_stream_filter_impl_open
                        ,   tb_stream_filter_impl_clos
                        ,   tb_stream_filter_impl_exit
                        ,   tb_stream_filter_impl_ctrl
                        ,   tb_stream_filter_impl_wait
                        ,   tb_stream_filter_impl_read
                        ,   tb_stream_filter_impl_writ
                        ,   tb_null
                        ,   tb_stream_filter_impl_sync
                        ,   tb_stream_filter_impl_kill);
}
tb_stream_ref_t tb_stream_init_filter_from_null(tb_stream_ref_t stream)
{
    // check
    tb_assert_and_check_return_val(stream, tb_null);

    // done
    tb_bool_t           ok = tb_false;
    tb_stream_ref_t     impl = tb_null;
    do
    {
        // init stream
        impl = tb_stream_init_filter();
        tb_assert_and_check_break(impl);

        // set stream
        if (!tb_stream_ctrl(impl, TB_STREAM_CTRL_FLTR_SET_STREAM, stream)) break;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (impl) tb_stream_exit(impl);
        impl = tb_null;
    }

    // ok
    return impl;
}
#ifdef TB_CONFIG_MODULE_HAVE_ZIP
tb_stream_ref_t tb_stream_init_filter_from_zip(tb_stream_ref_t stream, tb_size_t algo, tb_size_t action)
{
    // check
    tb_assert_and_check_return_val(stream, tb_null);

    // done
    tb_bool_t           ok = tb_false;
    tb_stream_ref_t     impl = tb_null;
    do
    {
        // init stream
        impl = tb_stream_init_filter();
        tb_assert_and_check_break(impl);

        // set stream
        if (!tb_stream_ctrl(impl, TB_STREAM_CTRL_FLTR_SET_STREAM, stream)) break;

        // set filter
        ((tb_stream_filter_impl_t*)impl)->bref = tb_false;
        ((tb_stream_filter_impl_t*)impl)->filter = tb_stream_filter_init_from_zip(algo, action);
        tb_assert_and_check_break(((tb_stream_filter_impl_t*)impl)->filter);
 
        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (impl) tb_stream_exit(impl);
        impl = tb_null;
    }

    // ok
    return impl;
}
#endif
tb_stream_ref_t tb_stream_init_filter_from_cache(tb_stream_ref_t stream, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(stream, tb_null);

    // done
    tb_bool_t           ok = tb_false;
    tb_stream_ref_t     impl = tb_null;
    do
    {
        // init stream
        impl = tb_stream_init_filter();
        tb_assert_and_check_break(impl);

        // set stream
        if (!tb_stream_ctrl(impl, TB_STREAM_CTRL_FLTR_SET_STREAM, stream)) break;

        // set filter
        ((tb_stream_filter_impl_t*)impl)->bref = tb_false;
        ((tb_stream_filter_impl_t*)impl)->filter = tb_stream_filter_init_from_cache(size);
        tb_assert_and_check_break(((tb_stream_filter_impl_t*)impl)->filter);
 
        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (impl) tb_stream_exit(impl);
        impl = tb_null;
    }

    // ok
    return impl;
}
#ifdef TB_CONFIG_MODULE_HAVE_CHARSET
tb_stream_ref_t tb_stream_init_filter_from_charset(tb_stream_ref_t stream, tb_size_t fr, tb_size_t to)
{
    // check
    tb_assert_and_check_return_val(stream, tb_null);

    // done
    tb_bool_t           ok = tb_false;
    tb_stream_ref_t     impl = tb_null;
    do
    {
        // init stream
        impl = tb_stream_init_filter();
        tb_assert_and_check_break(impl);

        // set stream
        if (!tb_stream_ctrl(impl, TB_STREAM_CTRL_FLTR_SET_STREAM, stream)) break;

        // set filter
        ((tb_stream_filter_impl_t*)impl)->bref = tb_false;
        ((tb_stream_filter_impl_t*)impl)->filter = tb_stream_filter_init_from_charset(fr, to);
        tb_assert_and_check_break(((tb_stream_filter_impl_t*)impl)->filter);
 
        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (impl) tb_stream_exit(impl);
        impl = tb_null;
    }

    // ok
    return impl;
}
#endif
tb_stream_ref_t tb_stream_init_filter_from_chunked(tb_stream_ref_t stream, tb_bool_t dechunked)
{
    // check
    tb_assert_and_check_return_val(stream, tb_null);

    // done
    tb_bool_t           ok = tb_false;
    tb_stream_ref_t     impl = tb_null;
    do
    {
        // init stream
        impl = tb_stream_init_filter();
        tb_assert_and_check_break(impl);

        // set stream
        if (!tb_stream_ctrl(impl, TB_STREAM_CTRL_FLTR_SET_STREAM, stream)) break;

        // set filter
        ((tb_stream_filter_impl_t*)impl)->bref = tb_false;
        ((tb_stream_filter_impl_t*)impl)->filter = tb_stream_filter_init_from_chunked(dechunked);
        tb_assert_and_check_break(((tb_stream_filter_impl_t*)impl)->filter);
 
        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (impl) tb_stream_exit(impl);
        impl = tb_null;
    }

    // ok
    return impl;
}
