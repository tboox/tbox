/*!The Treasure Box dynamic
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
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * @author		ruki
 * @file		dynamic.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../dynamic.h"
#include <dlfcn.h>

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_handle_t tb_dynamic_init(tb_char_t const* name)
{
	tb_assert_and_check_return_val(name, TB_NULL);
	tb_handle_t dynamic = dlopen(name, RTLD_LAZY);
	if (dlerror()) 
	{
		dlclose(dynamic);
		dynamic = TB_NULL;
	}
	return dynamic;
}
tb_void_t tb_dynamic_exit(tb_handle_t dynamic)
{
	tb_assert_and_check_return(dynamic);
	dlclose(dynamic);
	tb_assert(!dlerror());
}
tb_pointer_t tb_dynamic_func(tb_handle_t dynamic, tb_char_t const* name)
{
	tb_assert_and_check_return_val(dynamic && name, TB_NULL);
	return (tb_pointer_t)dlsym(dynamic, name);
}
