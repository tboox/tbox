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
static tb_bool_t tb_aicp_file_init(tb_aicp_proactor_aiop_t* ptor)
{
    return tb_true;
}
static tb_void_t tb_aicp_file_exit(tb_aicp_proactor_aiop_t* ptor)
{
}
static tb_bool_t tb_aicp_file_addo(tb_aicp_proactor_aiop_t* ptor, tb_aico_t* aico)
{
    return tb_true;
}
static tb_bool_t tb_aicp_file_delo(tb_aicp_proactor_aiop_t* ptor, tb_aico_t* aico)
{
    return tb_true;
}
static tb_void_t tb_aicp_file_kilo(tb_aicp_proactor_aiop_t* ptor, tb_aico_t* aico)
{
    if (aico && aico->handle) tb_file_exit(aico->handle);
}
static tb_bool_t tb_aicp_file_post(tb_aicp_proactor_aiop_t* ptor, tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(ptor && aice, tb_false);
        
    // the priority
    tb_size_t priority = tb_aiop_aice_priority(aice);
    tb_assert_and_check_return_val(priority < tb_arrayn(ptor->spak) && ptor->spak[priority], tb_false);

    // enter 
    tb_spinlock_enter(&ptor->lock);

    // post aice
    tb_bool_t ok = tb_true;
    if (!tb_queue_full(ptor->spak[priority])) 
    {
        // put
        tb_queue_put(ptor->spak[priority], aice);

        // trace
        tb_trace_d("post: code: %lu, priority: %lu, size: %lu", aice->code, priority, tb_queue_size(ptor->spak[priority]));
    }
    else
    {
        // failed
        ok = tb_false;

        // assert
        tb_assert(0);
    }

    // leave 
    tb_spinlock_leave(&ptor->lock);

    // ok?
    return ok;
}
static tb_long_t tb_aicp_file_spak_read(tb_aicp_proactor_aiop_t* ptor, tb_aice_t* aice)
{
    // check
    tb_assert_and_check_return_val(ptor && aice && aice->code == TB_AICE_CODE_READ, -1);
    tb_assert_and_check_return_val(aice->u.read.data && aice->u.read.size, -1);

    // the handle 
    tb_handle_t handle = aice->aico? aice->aico->handle : tb_null;
    tb_assert_and_check_return_val(handle, -1);

    // read it from the given offset
    tb_long_t real = tb_file_pread(handle, aice->u.read.data, aice->u.read.size, aice->u.read.seek);

    // trace
    tb_trace_d("read[%p]: %ld", handle, real);

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
static tb_long_t tb_aicp_file_spak_writ(tb_aicp_proactor_aiop_t* ptor, tb_aice_t* aice)
{
    // check
    tb_assert_and_check_return_val(ptor && aice && aice->code == TB_AICE_CODE_WRIT, -1);
    tb_assert_and_check_return_val(aice->u.writ.data && aice->u.writ.size, -1);

    // the handle 
    tb_handle_t handle = aice->aico? aice->aico->handle : tb_null;
    tb_assert_and_check_return_val(handle, -1);

    // writ it from the given offset
    tb_long_t real = tb_file_pwrit(handle, aice->u.writ.data, aice->u.read.size, aice->u.writ.seek);

    // trace
    tb_trace_d("writ[%p]: %ld", handle, real);

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
static tb_long_t tb_aicp_file_spak_readv(tb_aicp_proactor_aiop_t* ptor, tb_aice_t* aice)
{
    // check
    tb_assert_and_check_return_val(ptor && aice && aice->code == TB_AICE_CODE_READV, -1);
    tb_assert_and_check_return_val(aice->u.readv.list && aice->u.readv.size, -1);

    // the handle 
    tb_handle_t handle = aice->aico? aice->aico->handle : tb_null;
    tb_assert_and_check_return_val(handle, -1);

    // read it from the given offset
    tb_long_t real = tb_file_preadv(handle, aice->u.readv.list, aice->u.readv.size, aice->u.readv.seek);

    // trace
    tb_trace_d("readv[%p]: %ld", handle, real);

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
static tb_long_t tb_aicp_file_spak_writv(tb_aicp_proactor_aiop_t* ptor, tb_aice_t* aice)
{
    // check
    tb_assert_and_check_return_val(ptor && aice && aice->code == TB_AICE_CODE_WRITV, -1);
    tb_assert_and_check_return_val(aice->u.writv.list && aice->u.writv.size, -1);

    // the handle 
    tb_handle_t handle = aice->aico? aice->aico->handle : tb_null;
    tb_assert_and_check_return_val(handle, -1);

    // read it from the given offset
    tb_long_t real = tb_file_pwritv(handle, aice->u.writv.list, aice->u.writv.size, aice->u.writv.seek);

    // trace
    tb_trace_d("writv[%p]: %ld", handle, real);

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
static tb_long_t tb_aicp_file_spak_fsync(tb_aicp_proactor_aiop_t* ptor, tb_aice_t* aice)
{
    // check
    tb_assert_and_check_return_val(ptor && aice && aice->code == TB_AICE_CODE_FSYNC, -1);

    // the handle 
    tb_handle_t handle = aice->aico? aice->aico->handle : tb_null;
    tb_assert_and_check_return_val(handle, -1);

    // done sync
    tb_bool_t ok = tb_file_sync(handle);

    // trace
    tb_trace_d("fsync[%p]: %s", handle, ok? "ok" : "no");

    // ok?
    aice->state = ok? TB_STATE_OK : TB_STATE_FAILED;

    // ok?
    return 1;
}
static tb_void_t tb_aicp_file_kill(tb_aicp_proactor_aiop_t* ptor)
{
}
static tb_void_t tb_aicp_file_poll(tb_aicp_proactor_aiop_t* ptor)
{
}

