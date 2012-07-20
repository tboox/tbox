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
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * @author		ruki
 * @file		iterator.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "iterator.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_iterator_t tb_iterator_int(tb_long_t* data, tb_size_t size)
{
	// check
	tb_assert(data && size);

	// init
	tb_iterator_t itor;
	itor.mode = TB_ITERATOR_MODE_FORWARD | TB_ITERATOR_MODE_REVERSE;
	itor.data = (tb_pointer_t)data;
	itor.size = size;
	itor.step = sizeof(tb_long_t);

	// ok
	return itor;
}
tb_iterator_t tb_iterator_str(tb_char_t** data, tb_size_t size)
{	
	// check
	tb_assert(data && size);

	// init
	tb_iterator_t itor;
	itor.mode = TB_ITERATOR_MODE_FORWARD | TB_ITERATOR_MODE_REVERSE;
	itor.data = (tb_pointer_t)data;
	itor.size = size;
	itor.step = sizeof(tb_char_t*);

	// ok
	return itor;
}
tb_iterator_t tb_iterator_ptr(tb_pointer_t* data, tb_size_t size)
{
	// check
	tb_assert(data && size);

	// init
	tb_iterator_t itor;
	itor.mode = TB_ITERATOR_MODE_FORWARD | TB_ITERATOR_MODE_REVERSE;
	itor.data = (tb_pointer_t)data;
	itor.size = size;
	itor.step = sizeof(tb_pointer_t*);

	// ok
	return itor;
}
tb_iterator_t tb_iterator_mem(tb_pointer_t data, tb_size_t size, tb_size_t step)
{
	// check
	tb_assert(data && size && step);

	// init
	tb_iterator_t itor;
	itor.mode = TB_ITERATOR_MODE_FORWARD | TB_ITERATOR_MODE_REVERSE;
	itor.data = (tb_pointer_t)data;
	itor.size = size;
	itor.step = step;

	// ok
	return itor;
}
