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
 * \file		tbox.c
 *
 */

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"

/* ////////////////////////////////////////////////////////////////////////
 * implemention
 */

tb_bool_t tb_init(tb_byte_t* data, tb_size_t size)
{
	TB_DBG("init: %x %d", data, size);

#ifdef TB_CONFIG_MEMORY_POOL_ENABLE
	// init memory pool
	if (!tb_mpool_init(data, size)) return TB_FALSE;
#endif

	// ok
	TB_DBG("init: ok");

	return TB_TRUE;
}

void tb_exit()
{
#ifdef TB_CONFIG_MEMORY_POOL_ENABLE
	//tb_pool_dump();
	tb_mpool_exit();
#endif
	
	// ok
	TB_DBG("exit: ok");
}
