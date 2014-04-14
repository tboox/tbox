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
 * @file		thread_pool.h
 * @ingroup 	platform
 *
 */
#ifndef TB_PLATFORM_THREAD_POOL_H
#define TB_PLATFORM_THREAD_POOL_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init thread pool
 *
 * @param maxn 			the thread max count
 * @param stack 		the thread stack, using the default stack size if be zero 
 *
 * @return 				the thread pool handle
 */
tb_handle_t 			tb_thread_pool_init(tb_size_t maxn, tb_size_t stack);

/*! exit thread pool
 *
 * @param pool 			the thread pool handle
 */
tb_void_t 				tb_thread_pool_exit(tb_handle_t pool);

/*! kill thread pool
 *
 * @param pool 			the thread pool handle
 */
tb_void_t 				tb_thread_pool_kill(tb_handle_t pool);

/*! the thread pool instance
 *
 * @param pool 			the thread pool handle
 */
tb_handle_t 			tb_thread_pool_instance(tb_noarg_t);

#endif
