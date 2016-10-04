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
#include "scheduler.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the coroutine ref type
typedef __tb_typeref__(coroutine);

/// the coroutine function type
typedef tb_void_t       (*tb_coroutine_func_t)(tb_cpointer_t priv);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! start coroutine 
 *
 * @param scheduler     the scheduler, uses the current scheduler if be null
 * @param func          the coroutine function
 * @param priv          the passed user private data as the argument of function
 * @param stacksize     the stack size
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_coroutine_start(tb_scheduler_ref_t scheduler, tb_coroutine_func_t func, tb_cpointer_t priv, tb_size_t stacksize);

/// yield the current coroutine
tb_void_t               tb_coroutine_yield(tb_noarg_t);

/*! sleep some times (ms)
 *
 * @param interval      the interval (ms)
 */
tb_void_t               tb_coroutine_sleep(tb_size_t interval);

/*! get the current coroutine
 *
 * @return              the current coroutine
 */
tb_coroutine_ref_t      tb_coroutine_self(tb_noarg_t);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
