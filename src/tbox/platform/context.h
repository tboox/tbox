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
 * @file        context.h
 * @ingroup     platform
 *
 */
#ifndef TB_PLATFORM_CONTEXT_H
#define TB_PLATFORM_CONTEXT_H

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

/// get the user private data for the context function
#define tb_context_priv(priv_hi, priv_lo)   ((tb_cpointer_t)(tb_size_t)((((tb_uint64_t)(priv_hi) << 32)) | (priv_lo)))

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the context ref type
typedef __tb_typeref__(context);

/// the context func type
typedef tb_void_t (*tb_context_func_t)(tb_uint32_t priv_hi, tb_uint32_t priv_lo);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! get the context buffer size
 *
 * @return              the context buffer size
 */
tb_size_t               tb_context_size(tb_noarg_t);

/*! init the context
 *
 * @param data          the context data buffer
 * @param size          the context data size
 *
 * @return              the context
 */
tb_context_ref_t        tb_context_init(tb_byte_t* data, tb_size_t size);

/*! exit context 
 *
 * @param context       the context
 */
tb_void_t               tb_context_exit(tb_context_ref_t context);

/*! saves the current thread's execution context
 *
 * @param context       the context
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_context_save(tb_context_ref_t context);

/*! switchs to the given thread's execution context
 *
 * @param context       the context
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_context_switch(tb_context_ref_t context);

/*! make context with a given function and stack
 *
 * modifies the user thread context pointed to by context, which must have previously been initialized by a
 * call to tb_context_get() and had a stack allocated for it
 *
 * @param context       the context
 * @param context_link  the context link, determines the action to take when func() returns
 *                      if equal to tb_null, the process exits, otherwise, tb_context_set(context_link) is implicitly invoked.
 * @param stack         the stack address
 * @param stacksize     the stack size
 * @param func          the function
 * @param priv          the user private data
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_context_make(tb_context_ref_t context, tb_context_ref_t context_link, tb_pointer_t stack, tb_size_t stacksize, tb_context_func_t func, tb_cpointer_t priv);

/*! saves the current thread context in context and makes context_new the currently active context.
 *
 * @param context       the old context
 * @param context_new   the new context
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_context_swap(tb_context_ref_t context, tb_context_ref_t context_new);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
