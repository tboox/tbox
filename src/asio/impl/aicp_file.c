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
 * @file        aicp_file.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_aicp_file_init(tb_aiop_ptor_impl_t* impl)
{
    return tb_true;
}
static tb_void_t tb_aicp_file_exit(tb_aiop_ptor_impl_t* impl)
{
}
static tb_bool_t tb_aicp_file_addo(tb_aiop_ptor_impl_t* impl, tb_aico_impl_t* aico)
{
    return tb_true;
}
static tb_void_t tb_aicp_file_kilo(tb_aiop_ptor_impl_t* impl, tb_aico_impl_t* aico)
{
    // check
    tb_file_ref_t file = tb_aico_file((tb_aico_ref_t)aico);
    tb_assert_and_check_return(file);
    
    // kill it
    tb_file_exit(file);
}
static tb_bool_t tb_aicp_file_post(tb_aiop_ptor_impl_t* impl, tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(impl && aice, tb_false);
        
    // the priority
    tb_size_t priority = tb_aice_impl_priority(aice);
    tb_assert_and_check_return_val(priority < tb_arrayn(impl->spak) && impl->spak[priority], tb_false);

    // enter 
    tb_spinlock_enter(&impl->lock);

    // post aice
    tb_bool_t ok = tb_true;
    if (!tb_queue_full(impl->spak[priority])) 
    {
        // put
        tb_queue_put(impl->spak[priority], aice);

        // trace
        tb_trace_d("post: code: %lu, priority: %lu, size: %lu", aice->code, priority, tb_queue_size(impl->spak[priority]));
    }
    else
    {
        // failed
        ok = tb_false;

        // assert
        tb_assert(0);
    }

    // leave 
    tb_spinlock_leave(&impl->lock);

    // ok?
    return ok;
}
static tb_long_t tb_aicp_file_spak_read(tb_aiop_ptor_impl_t* impl, tb_aice_t* aice)
{
    // check
    tb_assert_and_check_return_val(impl && aice && aice->code == TB_AICE_CODE_READ, -1);
    tb_assert_and_check_return_val(aice->u.read.data && aice->u.read.size, -1);

    // the file 
    tb_file_ref_t file = tb_aico_file(aice->aico);
    tb_assert_and_check_return_val(file, -1);

    // read it from the given offset
    tb_long_t real = tb_file_pread(file, aice->u.read.data, aice->u.read.size, aice->u.read.seek);

    // trace
    tb_trace_d("read[%p]: %ld", file, real);

    // ok?
    if (real > 0) 
    {
        aice->u.read.real = real;
        aice->state = TB_STATE_OK;
    }
    // closed?
    else if (!real) aice->state = TB_STATE_CLOSED;
    // failed?
    else aice->state = TB_STATE_FAILED;

    // ok?
    return 1;
}
static tb_long_t tb_aicp_file_spak_writ(tb_aiop_ptor_impl_t* impl, tb_aice_t* aice)
{
    // check
    tb_assert_and_check_return_val(impl && aice && aice->code == TB_AICE_CODE_WRIT, -1);
    tb_assert_and_check_return_val(aice->u.writ.data && aice->u.writ.size, -1);

    // the file 
    tb_file_ref_t file = tb_aico_file(aice->aico);
    tb_assert_and_check_return_val(file, -1);

    // writ it from the given offset
    tb_long_t real = tb_file_pwrit(file, aice->u.writ.data, aice->u.read.size, aice->u.writ.seek);

    // trace
    tb_trace_d("writ[%p]: %ld", file, real);

    // ok?
    if (real > 0) 
    {
        aice->u.writ.real = real;
        aice->state = TB_STATE_OK;
    }
    // closed?
    else if (!real) aice->state = TB_STATE_CLOSED;
    // failed?
    else aice->state = TB_STATE_FAILED;

    // ok?
    return 1;
}
static tb_long_t tb_aicp_file_spak_readv(tb_aiop_ptor_impl_t* impl, tb_aice_t* aice)
{
    // check
    tb_assert_and_check_return_val(impl && aice && aice->code == TB_AICE_CODE_READV, -1);
    tb_assert_and_check_return_val(aice->u.readv.list && aice->u.readv.size, -1);

    // the file 
    tb_file_ref_t file = tb_aico_file(aice->aico);
    tb_assert_and_check_return_val(file, -1);

    // read it from the given offset
    tb_long_t real = tb_file_preadv(file, aice->u.readv.list, aice->u.readv.size, aice->u.readv.seek);

    // trace
    tb_trace_d("readv[%p]: %ld", file, real);

    // ok?
    if (real > 0) 
    {
        aice->u.readv.real = real;
        aice->state = TB_STATE_OK;
    }
    // closed?
    else if (!real) aice->state = TB_STATE_CLOSED;
    // failed?
    else aice->state = TB_STATE_FAILED;

    // ok?
    return 1;
}
static tb_long_t tb_aicp_file_spak_writv(tb_aiop_ptor_impl_t* impl, tb_aice_t* aice)
{
    // check
    tb_assert_and_check_return_val(impl && aice && aice->code == TB_AICE_CODE_WRITV, -1);
    tb_assert_and_check_return_val(aice->u.writv.list && aice->u.writv.size, -1);

    // the file 
    tb_file_ref_t file = tb_aico_file(aice->aico);
    tb_assert_and_check_return_val(file, -1);

    // read it from the given offset
    tb_long_t real = tb_file_pwritv(file, aice->u.writv.list, aice->u.writv.size, aice->u.writv.seek);

    // trace
    tb_trace_d("writv[%p]: %ld", file, real);

    // ok?
    if (real > 0) 
    {
        aice->u.writv.real = real;
        aice->state = TB_STATE_OK;
    }
    // closed?
    else if (!real) aice->state = TB_STATE_CLOSED;
    // failed?
    else aice->state = TB_STATE_FAILED;

    // ok?
    return 1;
}
static tb_long_t tb_aicp_file_spak_fsync(tb_aiop_ptor_impl_t* impl, tb_aice_t* aice)
{
    // check
    tb_assert_and_check_return_val(impl && aice && aice->code == TB_AICE_CODE_FSYNC, -1);

    // the file 
    tb_file_ref_t file = tb_aico_file(aice->aico);
    tb_assert_and_check_return_val(file, -1);

    // done sync
    tb_bool_t ok = tb_file_sync(file);

    // trace
    tb_trace_d("fsync[%p]: %s", file, ok? "ok" : "no");

    // ok?
    aice->state = ok? TB_STATE_OK : TB_STATE_FAILED;

    // ok?
    return 1;
}
static tb_void_t tb_aicp_file_kill(tb_aiop_ptor_impl_t* impl)
{
}
static tb_void_t tb_aicp_file_poll(tb_aiop_ptor_impl_t* impl)
{
}

