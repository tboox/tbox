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
 * \file		thread.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "../tplat.h"
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
/* /////////////////////////////////////////////////////////
 * implemention
 */

// thread
tplat_handle_t tplat_thread_create(tplat_char_t const* name, void* (*callback)(void*), void* param, tplat_size_t stack_size)
{
	pthread_t hthread;
	if (0 != pthread_create(&hthread, NULL, callback, param)) return TPLAT_INVALID_HANDLE;
	else return ((tplat_handle_t)hthread);
}
void tplat_thread_destroy(tplat_handle_t hthread)
{
}
tplat_bool_t tplat_thread_wait(tplat_handle_t hthread, tplat_int_t timeout)
{
	TPLAT_ASSERT(hthread != TPLAT_INVALID_HANDLE);
	if (hthread == TPLAT_INVALID_HANDLE) return TPLAT_FALSE;

	if (0 != pthread_join(((pthread_t)hthread), NULL)) return TPLAT_FALSE;
	else return TPLAT_TRUE;
}
tplat_bool_t tplat_thread_terminate(tplat_handle_t hthread)
{
	return TPLAT_TRUE;
}
void tplat_thread_exit(void* retval)
{
	pthread_exit(retval);
}
tplat_bool_t tplat_thread_suspend(tplat_handle_t hthread)
{
	return TPLAT_TRUE;
}
tplat_bool_t tplat_thread_resume(tplat_handle_t hthread)
{
	return TPLAT_TRUE;
}

// mutex
tplat_handle_t tplat_mutex_create(tplat_char_t const* name)
{
	pthread_mutex_t* pmutex = malloc(sizeof(pthread_mutex_t));
	if (!pmutex || 0 != pthread_mutex_init(pmutex, NULL)) return TPLAT_INVALID_HANDLE;
	else return ((tplat_handle_t)pmutex);
}
void tplat_mutex_destroy(tplat_handle_t hmutex)
{
	TPLAT_ASSERT(hmutex != TPLAT_INVALID_HANDLE);
	if (hmutex == TPLAT_INVALID_HANDLE) return ;

	pthread_mutex_t* pmutex = (pthread_mutex_t*)hmutex;
	if (pmutex)
	{
		pthread_mutex_destroy(pmutex);
		free((void*)pmutex);
	}
}
tplat_bool_t tplat_mutex_lock(tplat_handle_t hmutex)
{
	TPLAT_ASSERT(hmutex != TPLAT_INVALID_HANDLE);
	if (hmutex == TPLAT_INVALID_HANDLE) return TPLAT_FALSE;

	if (0 != pthread_mutex_lock((pthread_mutex_t*)hmutex)) return TPLAT_FALSE;
	else return TPLAT_TRUE;
}
tplat_bool_t tplat_mutex_trylock(tplat_handle_t hmutex)
{
	TPLAT_ASSERT(hmutex != TPLAT_INVALID_HANDLE);
	if (hmutex == TPLAT_INVALID_HANDLE) return TPLAT_FALSE;

	if (0 != pthread_mutex_trylock((pthread_mutex_t*)hmutex)) return TPLAT_FALSE;
	else return TPLAT_TRUE;
}
tplat_bool_t tplat_mutex_unlock(tplat_handle_t hmutex)
{
	TPLAT_ASSERT(hmutex != TPLAT_INVALID_HANDLE);
	if (hmutex == TPLAT_INVALID_HANDLE) return TPLAT_FALSE;

	if (0 != pthread_mutex_unlock((pthread_mutex_t*)hmutex)) return TPLAT_FALSE;
	else return TPLAT_TRUE;
}
