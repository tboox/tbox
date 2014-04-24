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
 * @author		ruki
 * @file		thread.h
 * @ingroup 	platform
 *
 */
#ifndef TB_PLATFORM_THREAD_H
#define TB_PLATFORM_THREAD_H


/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init thread
 *
 * @param name 			the thread name, maybe null
 * @param func 			the thread func
 * @param data 			the thread data
 * @param stack 		the thread stack, using the default stack size if be zero 
 *
 * @return 				the thread handle
 */
tb_handle_t 			tb_thread_init(tb_char_t const* name, tb_pointer_t (*func)(tb_cpointer_t), tb_cpointer_t data, tb_size_t stack);

/*! exit thread
 *
 * @param handle 		the thread handle
 */
tb_void_t 				tb_thread_exit(tb_handle_t handle);

/*! wait thread
 *
 * @param handle 		the thread handle
 * @param timeout 		the timeout
 *
 * @return 				ok: 1, timeout: 0, error: -1
 */
tb_long_t 				tb_thread_wait(tb_handle_t handle, tb_long_t timeout);

/*! suspend thread
 *
 * @param handle 		the thread handle
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_thread_suspend(tb_handle_t handle);

/*! resume thread
 *
 * @param handle 		the thread handle
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_thread_resume(tb_handle_t handle);

/*! the self thread identifier
 *
 * @return 				the self thread identifier
 */
tb_size_t 				tb_thread_self(tb_noarg_t);

/*! return the thread value
 *
 * @param value 		the value pointer
 */
tb_void_t 				tb_thread_return(tb_pointer_t value);

#endif
