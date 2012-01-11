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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		epool.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../epool.h"
#include "windows.h"

/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_handle_t tb_epool_init(tb_size_t maxn)
{
	return TB_NULL;
}
tb_void_t tb_epool_exit(tb_handle_t epool)
{
}

tb_size_t tb_epool_maxn(tb_handle_t epool)
{
	return 0;
}

tb_size_t tb_epool_size(tb_handle_t epool)
{
	return 0;
}

tb_handle_t tb_epool_adde(tb_handle_t epool, tb_handle_t event, tb_pointer_t edata, tb_bool_t bsignal)
{
	return TB_NULL;
}

tb_void_t tb_epool_dele(tb_handle_t epool, tb_handle_t event)
{
}

tb_void_t tb_epool_post(tb_handle_t epool, tb_handle_t event)
{
}

tb_long_t tb_epool_wait(tb_handle_t epool, tb_long_t timeout)
{
	return 0;
}

tb_handle_t* tb_epool_objs(tb_handle_t epool)
{
	return TB_NULL;
}

