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
 * @file        prefix.h
 *
 */
#ifndef TB_ASIO_IMPL_PREFIX_H
#define TB_ASIO_IMPL_PREFIX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"
#include "../aicp.h"
#include "../aiop.h"
#include "../../memory/memory.h"
#include "../../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the aioo impl type
typedef struct __tb_aioo_impl_t
{
    // the code
    tb_size_t                   code;

    // the priv
    tb_cpointer_t               priv;

    // the socket
    tb_socket_ref_t             sock;

}tb_aioo_impl_t;

// the aico impl type
typedef struct __tb_aico_impl_t
{
    // the aicp
    tb_aicp_ref_t               aicp;

    // the type
    tb_size_t                   type;

    // the handle
    tb_handle_t                 handle;

    /*! the state
     *
     * <pre>
     * TB_STATE_OK
     * TB_STATE_KILLED
     * TB_STATE_KILLING
     * TB_STATE_PENDING
     * TB_STATE_EXITING
     * </pre>
     */
    tb_atomic_t                 state;

    // the timeout for aice
    tb_atomic_t                 timeout[TB_AICO_TIMEOUT_MAXN];

    // the exit func
    tb_aico_exit_func_t         exit;

    // the private data
    tb_cpointer_t               priv;

#ifdef __tb_debug__
    // the func
    tb_char_t const*            func;

    // the file
    tb_char_t const*            file;

    // the line
    tb_size_t                   line;
#endif

}tb_aico_impl_t;

// the aicp proactor impl type
struct __tb_aicp_impl_t;
typedef struct __tb_aicp_ptor_impl_t
{
    // aicp
    struct __tb_aicp_impl_t*    aicp;

    // the aico step
    tb_size_t                   step;

    // kill
    tb_void_t                   (*kill)(struct __tb_aicp_ptor_impl_t* ptor);

    // exit
    tb_void_t                   (*exit)(struct __tb_aicp_ptor_impl_t* ptor);

    // addo
    tb_bool_t                   (*addo)(struct __tb_aicp_ptor_impl_t* ptor, tb_aico_impl_t* aico);

    // delo
    tb_bool_t                   (*delo)(struct __tb_aicp_ptor_impl_t* ptor, tb_aico_impl_t* aico);
    
    // kilo
    tb_void_t                   (*kilo)(struct __tb_aicp_ptor_impl_t* ptor, tb_aico_impl_t* aico);
    
    // post
    tb_bool_t                   (*post)(struct __tb_aicp_ptor_impl_t* ptor, tb_aice_t const* aice);

    // loop: init
    tb_handle_t                 (*loop_init)(struct __tb_aicp_ptor_impl_t* ptor);

    // loop: exit
    tb_void_t                   (*loop_exit)(struct __tb_aicp_ptor_impl_t* ptor, tb_handle_t loop);

    // loop: spak
    tb_long_t                   (*loop_spak)(struct __tb_aicp_ptor_impl_t* ptor, tb_handle_t loop, tb_aice_t* resp, tb_long_t timeout);

}tb_aicp_ptor_impl_t;

// the aiop reactor impl type
struct __tb_aiop_impl_t;
typedef struct __tb_aiop_rtor_impl_t
{
    // aiop
    struct __tb_aiop_impl_t*    aiop;

    // the supported aioe code
    tb_size_t                   code;

    // exit
    tb_void_t                   (*exit)(struct __tb_aiop_rtor_impl_t* rtor);

    // cler
    tb_void_t                   (*cler)(struct __tb_aiop_rtor_impl_t* rtor);

    // addo
    tb_bool_t                   (*addo)(struct __tb_aiop_rtor_impl_t* rtor, tb_aioo_impl_t const* aioo);

    // delo
    tb_bool_t                   (*delo)(struct __tb_aiop_rtor_impl_t* rtor, tb_aioo_impl_t const* aioo);

    // post
    tb_bool_t                   (*post)(struct __tb_aiop_rtor_impl_t* rtor, tb_aioe_t const* aioe);

    // wait
    tb_long_t                   (*wait)(struct __tb_aiop_rtor_impl_t* rtor, tb_aioe_t* list, tb_size_t maxn, tb_long_t timeout);

}tb_aiop_rtor_impl_t;

// the aicp impl type
typedef struct __tb_aicp_impl_t
{
    // the object maxn
    tb_size_t                   maxn;

    // the ptor
    tb_aicp_ptor_impl_t*        ptor;

    // the worker size
    tb_atomic_t                 work;

    // the pool
    tb_fixed_pool_ref_t         pool;

    // the pool lock
    tb_spinlock_t               lock;

    // kill it?
    tb_atomic_t                 kill;

    // killall it?
    tb_atomic_t                 kill_all;

}tb_aicp_impl_t;

// the aiop impl type 
typedef struct __tb_aiop_impl_t
{
    // the aioo maxn
    tb_size_t                   maxn;

    // the aioo pool
    tb_fixed_pool_ref_t         pool;

    // the pool lock
    tb_spinlock_t               lock;

    // the reactor
    tb_aiop_rtor_impl_t*        rtor;

    // the spak
    tb_socket_ref_t             spak[2];
    
}tb_aiop_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/* init aicp proactor 
 *
 * @param aicp          the aicp impl
 *
 * @return              the aicp proactor impl
 */
tb_aicp_ptor_impl_t*    tb_aicp_ptor_impl_init(tb_aicp_impl_t* aicp);

/* init aiop reactor 
 *
 * @param aiop          the aiop impl
 *
 * @return              the aiop reactor impl
 */
tb_aiop_rtor_impl_t*    tb_aiop_rtor_impl_init(tb_aiop_impl_t* aiop);

/* //////////////////////////////////////////////////////////////////////////////////////
 * inlines
 */
static __tb_inline__ tb_bool_t tb_aico_impl_is_killed(tb_aico_impl_t* aico)
{
    // check
    tb_assert_and_check_return_val(aico, tb_false);

    // the state
    tb_size_t state = tb_atomic_get(&aico->state);

    // killing or exiting or killed?
    return (state == TB_STATE_KILLING) || (state == TB_STATE_EXITING) || (state == TB_STATE_KILLED);
}
static __tb_inline__ tb_size_t tb_aice_impl_priority(tb_aice_t const* aice)
{
    // the priorities
    static tb_size_t s_priorities[] =
    {
        1

    ,   1   
    ,   0   // acpt
    ,   0   // conn
    ,   1
    ,   1
    ,   1
    ,   1
    ,   1
    ,   1
    ,   1
    ,   1
    ,   1

    ,   1
    ,   1
    ,   1
    ,   1
    ,   1

    ,   0   // task
    };
    tb_assert_and_check_return_val(aice->code && aice->code < tb_arrayn(s_priorities), 1);
    
    // the priority
    return s_priorities[aice->code];
}
static __tb_inline__ tb_long_t tb_aico_impl_timeout_from_code(tb_aico_impl_t* aico, tb_size_t code)
{
    // init the timeout type
    static tb_size_t type[] = 
    {
        -1

    ,   TB_AICO_TIMEOUT_ACPT
    ,   TB_AICO_TIMEOUT_CONN
    ,   TB_AICO_TIMEOUT_RECV
    ,   TB_AICO_TIMEOUT_SEND
    ,   TB_AICO_TIMEOUT_RECV
    ,   TB_AICO_TIMEOUT_SEND
    ,   TB_AICO_TIMEOUT_RECV
    ,   TB_AICO_TIMEOUT_SEND
    ,   TB_AICO_TIMEOUT_RECV
    ,   TB_AICO_TIMEOUT_SEND
    ,   TB_AICO_TIMEOUT_SEND

    ,   -1
    ,   -1
    ,   -1
    ,   -1
    ,   -1

    ,   -1
    };
    tb_assert_and_check_return_val(code < tb_arrayn(type) && type[code] != (tb_size_t)-1, -1);

    // timeout
    return tb_aico_timeout((tb_aico_ref_t)aico, type[code]);
}


#endif
