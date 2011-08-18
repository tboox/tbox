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
 * \file		malloc.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <stdlib.h>

/* /////////////////////////////////////////////////////////
 * implemention
 */

tb_void_t* tb_malloc(tb_size_t size)
{
	return malloc(size);
}
tb_void_t* tb_calloc(tb_size_t item, tb_size_t size)
{
	return calloc(item, size);
}
tb_void_t* tb_realloc(tb_void_t* data, tb_size_t size)
{
	return realloc(data, size);
}
tb_void_t tb_free(tb_void_t* data)
{
	if (data) free(data);
}

