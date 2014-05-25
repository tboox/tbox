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
 * @file        aiop.c
 * @ingroup     asio
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "aiop"
#define TB_TRACE_MODULE_DEBUG               (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "aiop.h"
#include "aioo.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../memory/memory.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */
tb_aiop_reactor_t* tb_aiop_reactor_init(tb_aiop_t* aiop);

/* //////////////////////////////////////////////////////////////////////////////////////
 * aioo
 */
static tb_aioo_t* tb_aiop_aioo_init(tb_aiop_t* aiop, tb_handle_t handle, tb_size_t code, tb_pointer_t data)
{
    // check
    tb_assert_and_check_return_val(aiop && aiop->pool, tb_null);

    // enter 
    tb_spinlock_enter(&aiop->lock);

    // make aioo
    tb_aioo_t* aioo = (tb_aioo_t*)tb_fixed_pool_malloc0(aiop->pool);

    // init aioo
    if (aioo)
    {
        aioo->code = code;
        aioo->data = data;
        aioo->handle = handle;
    }

    // leave 
    tb_spinlock_leave(&aiop->lock);
    
    // ok?
    return aioo;
}
static tb_void_t tb_aiop_aioo_exit(tb_aiop_t* aiop, tb_handle_t aioo)
{
    // check
    tb_assert_and_check_return(aiop && aiop->pool);

    // enter 
    tb_spinlock_enter(&aiop->lock);

    // exit aioo
    if (aioo) tb_fixed_pool_free(aiop->pool, aioo);

    // leave 
    tb_spinlock_leave(&aiop->lock);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_aiop_t* tb_aiop_init(tb_size_t maxn)
{
    // check
    tb_assert_and_check_return_val(maxn, tb_null);

    // alloc aiop
    tb_aiop_t* aiop = tb_malloc0(sizeof(tb_aiop_t));
    tb_assert_and_check_return_val(aiop, tb_null);

    // init aiop
    aiop->maxn = maxn;

    // init lock
    if (!tb_spinlock_init(&aiop->lock)) goto fail;

    // init pool
    aiop->pool = tb_fixed_pool_init((maxn >> 4) + 16, sizeof(tb_aioo_t), 0);
    tb_assert_and_check_goto(aiop->pool, fail);

    // init spak
    if (!tb_socket_pair(TB_SOCKET_TYPE_TCP, aiop->spak)) goto fail;

    // init reactor
    aiop->rtor = tb_aiop_reactor_init(aiop);
    tb_assert_and_check_goto(aiop->rtor, fail);

    // addo spak
    if (!tb_aiop_addo(aiop, aiop->spak[1], TB_AIOE_CODE_RECV, tb_null)) goto fail;  

    // register lock profiler
#ifdef TB_LOCK_PROFILER_ENABLE
    tb_lock_profiler_register(tb_lock_profiler(), (tb_pointer_t)&aiop->lock, TB_TRACE_MODULE_NAME);
#endif

    // ok
    return aiop;

fail:
    if (aiop) tb_aiop_exit(aiop);
    return tb_null;
}

tb_void_t tb_aiop_exit(tb_aiop_t* aiop)
{
    // check
    tb_assert_and_check_return(aiop);

    // exit reactor
    if (aiop->rtor && aiop->rtor->exit)
        aiop->rtor->exit(aiop->rtor);

    // exit spak
    if (aiop->spak[0]) tb_socket_clos(aiop->spak[0]);
    if (aiop->spak[1]) tb_socket_clos(aiop->spak[1]);
    aiop->spak[0] = tb_null;
    aiop->spak[1] = tb_null;

    // exit pool
    tb_spinlock_enter(&aiop->lock);
    if (aiop->pool) tb_fixed_pool_exit(aiop->pool);
    aiop->pool = tb_null;
    tb_spinlock_leave(&aiop->lock);

    // exit lock
    tb_spinlock_exit(&aiop->lock);

    // free aiop
    tb_free(aiop);
}
tb_void_t tb_aiop_cler(tb_aiop_t* aiop)
{
    // check
    tb_assert_and_check_return(aiop);

    // clear reactor
    if (aiop->rtor && aiop->rtor->cler)
        aiop->rtor->cler(aiop->rtor);

    // clear pool
    tb_spinlock_enter(&aiop->lock);
    if (aiop->pool) tb_fixed_pool_clear(aiop->pool);
    tb_spinlock_leave(&aiop->lock);

    // addo spak
    if (aiop->spak[1]) tb_aiop_addo(aiop, aiop->spak[1], TB_AIOE_CODE_RECV, tb_null);   
}
tb_void_t tb_aiop_kill(tb_aiop_t* aiop)
{
    // check
    tb_assert_and_check_return(aiop);

    // kill it
    if (aiop->spak[0]) tb_socket_send(aiop->spak[0], (tb_byte_t const*)"k", 1);
}
tb_void_t tb_aiop_spak(tb_aiop_t* aiop)
{
    // check
    tb_assert_and_check_return(aiop);

    // spak it
    if (aiop->spak[0]) tb_socket_send(aiop->spak[0], (tb_byte_t const*)"p", 1);
}
tb_handle_t tb_aiop_addo(tb_aiop_t* aiop, tb_handle_t handle, tb_size_t code, tb_pointer_t data)
{
    // check
    tb_assert_and_check_return_val(aiop && aiop->rtor && aiop->rtor->addo && handle, tb_null);

    // done
    tb_bool_t   ok = tb_false;
    tb_aioo_t*  aioo = tb_null;
    do
    {
        // init aioo
        aioo = tb_aiop_aioo_init(aiop, handle, code, data);
        tb_assert_and_check_break(aioo);
        
        // addo aioo
        if (!aiop->rtor->addo(aiop->rtor, aioo)) break;

        // ok
        ok = tb_true;

    } while (0);

    // failed? remove aioo
    if (!ok && aioo) 
    {
        tb_aiop_aioo_exit(aiop, aioo);
        aioo = tb_null;
    }

    // ok?
    return (tb_handle_t)aioo;
}
tb_void_t tb_aiop_delo(tb_aiop_t* aiop, tb_handle_t aioo)
{
    // check
    tb_assert_and_check_return(aiop && aiop->rtor && aiop->rtor->delo && aioo);

    // delo aioo
    if (aiop->rtor->delo(aiop->rtor, aioo)) tb_aiop_aioo_exit(aiop, aioo);
}
tb_bool_t tb_aiop_post(tb_aiop_t* aiop, tb_aioe_t const* aioe)
{
    // check
    tb_assert_and_check_return_val(aiop && aiop->rtor && aiop->rtor->post && aioe, tb_false);

    // post
    return aiop->rtor->post(aiop->rtor, aioe);
}
tb_bool_t tb_aiop_sete(tb_aiop_t* aiop, tb_handle_t aioo, tb_size_t code, tb_pointer_t data)
{
    // check
    tb_assert_and_check_return_val(aiop && aioo && tb_aioo_handle(aioo) && code, tb_false);
 
    // init aioe
    tb_aioe_t aioe;
    aioe.code = code;
    aioe.data = data;
    aioe.aioo = aioo;

    // post aioe
    return tb_aiop_post(aiop, &aioe);
}
tb_long_t tb_aiop_wait(tb_aiop_t* aiop, tb_aioe_t* list, tb_size_t maxn, tb_long_t timeout)
{   
    // check
    tb_assert_and_check_return_val(aiop && aiop->rtor && aiop->rtor->wait && list, -1);

    // wait 
    return aiop->rtor->wait(aiop->rtor, list, maxn, timeout);
}

