/*!The Tiny Platform Library
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
 * \file		mutex.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>

/* /////////////////////////////////////////////////////////
 * implemention
 */

tb_handle_t tb_mutex_init(tb_char_t const* name)
{
	pthread_mutex_t* pmutex = malloc(sizeof(pthread_mutex_t));
	if (!pmutex || 0 != pthread_mutex_init(pmutex, NULL)) return TB_NULL;
	else return ((tb_handle_t)pmutex);
}
tb_void_t tb_mutex_exit(tb_handle_t hmutex)
{
	TB_ASSERT_RETURN(hmutex);

	pthread_mutex_t* pmutex = (pthread_mutex_t*)hmutex;
	if (pmutex)
	{
		pthread_mutex_destroy(pmutex);
		free((tb_void_t*)pmutex);
	}
}
tb_bool_t tb_mutex_lock(tb_handle_t hmutex)
{
	TB_ASSERT_RETURN_VAL(hmutex, TB_FALSE);

	if (0 != pthread_mutex_lock((pthread_mutex_t*)hmutex)) return TB_FALSE;
	else return TB_TRUE;
}
tb_bool_t tb_mutex_trylock(tb_handle_t hmutex)
{
	TB_ASSERT_RETURN_VAL(hmutex, TB_FALSE);

	if (0 != pthread_mutex_trylock((pthread_mutex_t*)hmutex)) return TB_FALSE;
	else return TB_TRUE;
}
tb_bool_t tb_mutex_unlock(tb_handle_t hmutex)
{
	TB_ASSERT_RETURN_VAL(hmutex, TB_FALSE);

	if (0 != pthread_mutex_unlock((pthread_mutex_t*)hmutex)) return TB_FALSE;
	else return TB_TRUE;
}
