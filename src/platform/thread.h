/*!The Treasure Platform Library
 * 
 * TPlat is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TPlat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TPlat; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		thread.h
 *
 */
#ifndef TB_PLATFORM_THREAD_H
#define TB_PLATFORM_THREAD_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif


/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* /////////////////////////////////////////////////////////
 * macros
 */
#define TB_THREAD_TIMEOUT_INFINITY 		(-1)

/* /////////////////////////////////////////////////////////
 * interfaces
 */

// thread
tb_handle_t 	tb_thread_open(tb_char_t const* name, tb_pointer_t (*callback)(tb_pointer_t), tb_pointer_t param, tb_size_t stack_size);
tb_void_t 		tb_thread_close(tb_handle_t hthread);
tb_bool_t 		tb_thread_wait(tb_handle_t hthread, tb_int_t timeout);
tb_bool_t 		tb_thread_suspend(tb_handle_t hthread);
tb_bool_t 		tb_thread_resume(tb_handle_t hthread);
tb_bool_t 		tb_thread_terminate(tb_handle_t hthread);
tb_void_t 		tb_thread_exit(tb_pointer_t retval);

// c plus plus
#ifdef __cplusplus
}
#endif

#endif
