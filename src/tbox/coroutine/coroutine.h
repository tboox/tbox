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
 * @defgroup    coroutine
 *
 */
#ifndef TB_COROUTINE_H
#define TB_COROUTINE_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

/// is dead?
#define tb_coroutine_is_dead(coroutine)         (tb_coroutine_state(coroutine) == TB_STATE_DEAD)

/// is ready?
#define tb_coroutine_is_ready(coroutine)        (tb_coroutine_state(coroutine) == TB_STATE_READY)

/// is running?
#define tb_coroutine_is_running(coroutine)      (tb_coroutine_state(coroutine) == TB_STATE_RUNNING)

/// is suspend?
#define tb_coroutine_is_suspend(coroutine)      (tb_coroutine_state(coroutine) == TB_STATE_SUSPEND)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the coroutine ref type
typedef __tb_typeref__(coroutine);

/// the coroutine function type
typedef tb_int_t        (*tb_coroutine_func_t)(tb_cpointer_t priv);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init coroutine 
 *
 * @param func          the coroutine function
 * @param priv          the passed user private data as the argument of function
 * @param stacksize     the stack size
 *
 * @return              the coroutine 
 */
tb_coroutine_ref_t      tb_coroutine_init(tb_coroutine_func_t func, tb_cpointer_t priv, tb_size_t stacksize);

/*! exit coroutine
 *
 * @param coroutine     the coroutine
 */
tb_void_t               tb_coroutine_exit(tb_coroutine_ref_t coroutine);

/*! resume to the given coroutine
 *
 * @param coroutine     the coroutine
 */
tb_void_t               tb_coroutine_resume(tb_coroutine_ref_t coroutine);

/// yield from the current coroutine
tb_void_t               tb_coroutine_yield(tb_noarg_t);

/*! get the current coroutine
 *
 * @return              the current coroutine
 */
tb_coroutine_ref_t      tb_coroutine_self(tb_noarg_t);

/*! get the coroutine state 
 *
 * - TB_STATE_READY
 * - TB_STATE_SUSPEND
 * - TB_STATE_RUNNING
 * - TB_STATE_DEAD
 *
 * @param coroutine     the coroutine, will get state from the current coroutine if be passed null
 *
 * @return              the state value
 */
tb_size_t               tb_coroutine_state(tb_coroutine_ref_t coroutine);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
