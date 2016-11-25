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
 * Copyright (C) 2009 - 2017, ruki All rights reserved.
 *
 * @author      ruki
 * @file        coroutine.h
 *
 */
#ifndef TB_COROUTINE_IMPL_STACKLESS_H
#define TB_COROUTINE_IMPL_STACKLESS_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the stackless coroutine type
typedef struct __tb_lo_coroutine_t
{
    // the coroutine core
    tb_lo_core_t                core;

    // the list entry
    tb_list_entry_t             entry;

    // the coroutine function
    tb_lo_coroutine_func_t      func;

    // the user private data of the coroutine function
    tb_cpointer_t               priv;

    // the scheduler
    tb_lo_scheduler_ref_t       scheduler;

}tb_lo_coroutine_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/* init coroutine 
 *
 * @param scheduler     the scheduler
 * @param func          the coroutine function
 * @param priv          the passed user private data as the argument of function
 *
 * @return              the coroutine 
 */
tb_lo_coroutine_t*      tb_lo_coroutine_init(tb_lo_scheduler_ref_t scheduler, tb_lo_coroutine_func_t func, tb_cpointer_t priv);

/* reinit the given coroutine 
 *
 * @param coroutine     the coroutine
 * @param func          the coroutine function
 * @param priv          the passed user private data as the argument of function
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_lo_coroutine_reinit(tb_lo_coroutine_t* coroutine, tb_lo_coroutine_func_t func, tb_cpointer_t priv);

/* exit coroutine
 *
 * @param coroutine     the coroutine
 */
tb_void_t               tb_lo_coroutine_exit(tb_lo_coroutine_t* coroutine);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
