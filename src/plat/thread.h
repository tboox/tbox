/*!The Tiny Platform Library
 * 
 * TPlat is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * TPlat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with TPlat; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		thread.h
 *
 */
#ifndef TPLAT_THREAD_H
#define TPLAT_THREAD_H

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
#define TPLAT_THREAD_TIMEOUT_INFINITY 		(-1)

/* /////////////////////////////////////////////////////////
 * interfaces
 */

// thread
tplat_handle_t 	tplat_thread_create(tplat_char_t const* name, void* (*callback)(void*), void* param, tplat_size_t stack_size);
void 			tplat_thread_destroy(tplat_handle_t hthread);
tplat_bool_t 	tplat_thread_wait(tplat_handle_t hthread, tplat_int_t timeout);
tplat_bool_t 	tplat_thread_suspend(tplat_handle_t hthread);
tplat_bool_t 	tplat_thread_resume(tplat_handle_t hthread);
tplat_bool_t 	tplat_thread_terminate(tplat_handle_t hthread);
void 			tplat_thread_exit(void* retval);

// mutex
tplat_handle_t 	tplat_mutex_create(tplat_char_t const* name);
void 			tplat_mutex_destroy(tplat_handle_t hmutex);
tplat_bool_t 	tplat_mutex_lock(tplat_handle_t hmutex);
tplat_bool_t 	tplat_mutex_trylock(tplat_handle_t hmutex);
tplat_bool_t 	tplat_mutex_unlock(tplat_handle_t hmutex);
	
// c plus plus
#ifdef __cplusplus
}
#endif

#endif
