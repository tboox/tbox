/*!The Tiny Box Library
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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		malloc.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include "malloc.h"

/* /////////////////////////////////////////////////////////
 * globals
 */
static tb_handle_t g_mpool = TB_NULL;
static tb_handle_t g_mutex = TB_NULL; 

/* /////////////////////////////////////////////////////////
 * implemention
 */
tb_bool_t tb_memory_init(tb_void_t* data, tb_size_t size)
{
	TB_ASSERT_RETURN_VAL(data && size, TB_FALSE);

	if (!g_mutex) g_mutex = tb_mutex_init("the memory pool");
	TB_ASSERT_RETURN_VAL(g_mutex, TB_FALSE);

	if (!tb_mutex_lock(g_mutex)) return TB_FALSE;
	if (!g_mpool) g_mpool = tb_mpool_init(data, size);
	if (!tb_mutex_unlock(g_mutex)) return TB_FALSE;

	return g_mpool? TB_TRUE : TB_FALSE;
}
tb_void_t tb_memory_exit()
{
	if (tb_mutex_lock(g_mutex))
	{
		if (g_mpool)
		{
			tb_mpool_exit(g_mpool);
			g_mpool = TB_NULL;
		}
		tb_mutex_unlock(g_mutex);
	}

	if (g_mutex)	
	{
		tb_mutex_exit(g_mutex);
		g_mutex = TB_NULL;
	}
}

#ifndef TB_DEBUG
tb_void_t* tb_memory_allocate(tb_size_t size)
#else
tb_void_t* tb_memory_allocate(tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	TB_ASSERT_RETURN_VAL(g_mpool && g_mutex, TB_NULL);

	// lock
	if (!tb_mutex_lock(g_mutex)) return TB_NULL;
	tb_byte_t* p = tb_mpool_allocate(g_mpool, size, func, line, file);
	tb_mutex_unlock(g_mutex);

	// align by 4 bytes
	TB_ASSERT(!(((tb_size_t)p) & TB_CPU_BITALIGN));

	TB_ASSERTM(p, "cannot alloc at %s(): %d, file: %s", func? func : "null", line, file? file : "null");
	return p;
}
#ifndef TB_DEBUG
tb_void_t* tb_memory_callocate(tb_size_t item, tb_size_t size)
#else
tb_void_t* tb_memory_callocate(tb_size_t item, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	TB_ASSERT_RETURN_VAL(g_mpool && g_mutex, TB_NULL);

	// lock
	if (!tb_mutex_lock(g_mutex)) return TB_NULL;
	tb_byte_t* p = tb_mpool_callocate(g_mpool, item, size, func, line, file);
	tb_mutex_unlock(g_mutex);

	// align by 4 bytes
	TB_ASSERT(!(((tb_size_t)p) & TB_CPU_BITALIGN));

	TB_ASSERTM(p, "cannot calloc at %s(): %d, file: %s", func? func : "null", line, file? file : "null");
	return p;
}

#ifndef TB_DEBUG
tb_void_t* tb_memory_reallocate(tb_void_t* data, tb_size_t size)
#else
tb_void_t* tb_memory_reallocate(tb_void_t* data, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	TB_ASSERT_RETURN_VAL(g_mpool && g_mutex, TB_NULL);

	// lock
	if (!tb_mutex_lock(g_mutex)) return TB_NULL;
	tb_byte_t* p = tb_mpool_reallocate(g_mpool, data, size, func, line, file);
	tb_mutex_unlock(g_mutex);

	// align by 4 bytes
	TB_ASSERT(!(((tb_size_t)p) & TB_CPU_BITALIGN));

	TB_ASSERTM(p, "invalid realloc data address:%x at %s(): %d, file: %s", data, func? func : "null", line, file? file : "null");
	return p;
}
#ifndef TB_DEBUG
tb_void_t tb_memory_deallocate(tb_void_t* data)
#else
tb_void_t tb_memory_deallocate(tb_void_t* data, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	TB_ASSERT_RETURN(g_mpool && g_mutex);

	// lock
	if (!tb_mutex_lock(g_mutex)) return ;
	tb_bool_t ret = tb_mpool_deallocate(g_mpool, data, func, line, file);
	tb_mutex_unlock(g_mutex);

	TB_ASSERTM(ret, "invalid free data address:%x at %s(): %d, file: %s", data, func? func : "null", line, file? file : "null");
}

tb_void_t tb_memory_dump()
{
	// check 
	TB_ASSERT_RETURN(g_mpool && g_mutex);

	// lock
	if (!tb_mutex_lock(g_mutex)) return ;
	tb_mpool_dump(g_mpool);
	tb_mutex_unlock(g_mutex);
}
tb_bool_t tb_memory_check()
{
	// check 
	TB_ASSERT_RETURN_VAL(g_mpool && g_mutex, TB_FALSE);

	// lock
	if (!tb_mutex_lock(g_mutex)) return TB_FALSE;
	tb_bool_t ret = tb_mpool_check(g_mpool);
	tb_mutex_unlock(g_mutex);
	return ret;
}
