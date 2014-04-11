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
 * @file		malloc.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../malloc.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_native_memory_init()
{
	return tb_true;
}
tb_void_t tb_native_memory_exit()
{
}
tb_pointer_t tb_native_malloc(tb_size_t size)
{
	tb_check_return_val(size, tb_null);
	return malloc(size);
}
tb_pointer_t tb_native_malloc0(tb_size_t size)
{
	tb_check_return_val(size, tb_null);	
	return calloc(1, size);
}
tb_pointer_t tb_native_nalloc(tb_size_t item, tb_size_t size)
{
	tb_check_return_val(item && size, tb_null);	
	return malloc(item * size);
}
tb_pointer_t tb_native_nalloc0(tb_size_t item, tb_size_t size)
{
	tb_check_return_val(item && size, tb_null);		
	return calloc(item, size);
}
tb_pointer_t tb_native_ralloc(tb_pointer_t data, tb_size_t size)
{
	if (!size) 
	{
		free(data);
		return tb_null;
	}
	else if (!data) return malloc(size);
	else return realloc(data, size);
}
tb_bool_t tb_native_free(tb_pointer_t data)
{
	if (data) free(data);
	return tb_true;
}

