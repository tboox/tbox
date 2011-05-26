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
 * \file		malloc.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "../tplat.h"
#include <stdlib.h>

/* /////////////////////////////////////////////////////////
 * implemention
 */

void* tplat_malloc(tplat_int_t pool_index, tplat_size_t size)
{
	return malloc(size);
}
void* tplat_calloc(tplat_int_t index, tplat_size_t item, tplat_size_t size)
{
	void* p = tplat_pool_allocate(index, item * size);
	if (p) memset(p, 0, item * size);
	return p;
}
void* tplat_realloc(tplat_int_t pool_index, void* data, tplat_size_t size)
{
	return realloc(data, size);
}
void tplat_free(tplat_int_t pool_index, void* data)
{
	if (data) free(data);
}

