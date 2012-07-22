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
 * @file		find.c
 * @ingroup 	algorithm
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "find.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_size_t tb_find(tb_iterator_t* iterator, tb_size_t head, tb_size_t tail, tb_cpointer_t item)
{
	// check
	tb_assert_and_check_return_val(iterator && iterator->mode & TB_ITERATOR_MODE_FORWARD, tail);

	// find
	tb_size_t itor;
	for (itor = head; itor != tail; itor = tb_iterator_next(iterator, itor)) 
		if (!tb_iterator_comp(iterator, tb_iterator_item(iterator, itor), item)) break;

	// ok?
	return itor;
}
tb_size_t tb_find_all(tb_iterator_t* iterator, tb_cpointer_t item)
{
	return tb_find(iterator, tb_iterator_head(iterator), tb_iterator_tail(iterator), item);
}
tb_size_t tb_binary_find(tb_iterator_t* iterator, tb_size_t head, tb_size_t tail, tb_cpointer_t item)
{
	// check
	tb_assert_and_check_return_val(iterator && iterator->mode & TB_ITERATOR_MODE_RACCESS, tail);

	// find
	tb_size_t l = head;
	tb_size_t r = tail;
	tb_size_t m = tb_iterator_midd(iterator, l, r);
	while (tb_iterator_diff(iterator, r, l) > 1)
	{
		if (tb_iterator_comp(iterator, item, tb_iterator_item(iterator, m)) < 0) r = m;
		else l = m;
		m = tb_iterator_midd(iterator, l, r);
	}

	// ok?
	return m;
}
tb_size_t tb_binary_find_all(tb_iterator_t* iterator, tb_cpointer_t item)
{
	return tb_binary_find(iterator, tb_iterator_head(iterator), tb_iterator_tail(iterator), item);
}
