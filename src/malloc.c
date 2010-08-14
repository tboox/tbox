/*!The Tiny Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with TGraphic; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		malloc.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "option.h"
#include "malloc.h"
#include "eplat/eplat.h"

/* /////////////////////////////////////////////////////////
 * interfaces
 */
void* tb_malloc(tb_size_t size)
{
#if 1
	return eplat_malloc(TB_MEMORY_POOL_INDEX, size);
#else
	void* p = eplat_malloc(TB_MEMORY_POOL_INDEX, size);
	if (p) memset(p, 0, size);
	return p;
#endif
}
void* tb_realloc(void* data, tb_size_t size)
{
	if (data) return eplat_realloc(TB_MEMORY_POOL_INDEX, data, size);
	return TB_NULL;
}
void* tb_calloc(tb_size_t item, tb_size_t size)
{
	void* p = eplat_malloc(TB_MEMORY_POOL_INDEX, item * size);
	if (p) memset(p, 0, item * size);
	return p;
}
void tb_free(void* data)
{
	if (data) eplat_free(TB_MEMORY_POOL_INDEX, data);
}

