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
 * @file		pfind.c
 * @ingroup 	algorithm
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "pfind.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_size_t tb_pfind(tb_iterator_t* iterator, tb_size_t* pprev, tb_size_t head, tb_size_t tail, tb_cpointer_t data, tb_iterator_comp_t comp)
{
	// check
	tb_assert_and_check_return_val(iterator && iterator->mode & TB_ITERATOR_MODE_FORWARD && pprev, tail);

	// null?
	if (head == tail)
	{
		*pprev = tail;
		return tail;
	}

	// the comparer
	if (!comp) comp = tb_iterator_comp;

	// find
	tb_size_t prev = tail;
	tb_size_t itor = head;
	tb_long_t find = -1;
	for (; itor != tail; prev = itor, itor = tb_iterator_next(iterator, itor)) 
	{
		find = comp(iterator, tb_iterator_item(iterator, itor), data);
		if (find >= 0) break;
	}

	// save prev
	if (pprev) *pprev = find? prev : tail;

	// ok?
	return !find? itor : tail;
}
tb_size_t tb_pfind_all(tb_iterator_t* iterator, tb_size_t* pprev, tb_cpointer_t data, tb_iterator_comp_t comp)
{
	return tb_pfind(iterator, pprev, tb_iterator_head(iterator), tb_iterator_tail(iterator), data, comp);
}
tb_size_t tb_binary_pfind(tb_iterator_t* iterator, tb_size_t* pprev, tb_size_t head, tb_size_t tail, tb_cpointer_t data, tb_iterator_comp_t comp)
{
	// check
	tb_assert_and_check_return_val(iterator && iterator->mode & TB_ITERATOR_MODE_RACCESS && pprev, tail);

	// null?
	if (head == tail)
	{
		*pprev = tail;
		return tail;
	}

	// the comparer
	if (!comp) comp = tb_iterator_comp;

	// find
	tb_size_t l = head;
	tb_size_t r = tail;
	tb_size_t m = (l + r) >> 1;
	tb_long_t c = -1;
	while (l < r)
	{
		c = comp(iterator, tb_iterator_item(iterator, m), data);
		if (c > 0) r = m;
		else if (c < 0) l = m + 1;
		else break;
		m = (l + r) >> 1;
	}

	// save prev
	if (pprev) 
	{
		// no finded?
		if (c) 
		{
			// find the subrange for prev
			tb_size_t p = r;
			tb_pfind(iterator, &p, l, r, data, comp);

			// save prev
			*pprev = (p != r)? p : ((c < 0)? tail - 1 : tail);
		}
		// find it? not using prev
		else *pprev = tail;
	}

	// ok?
	return !c? m : tail;
}
tb_size_t tb_binary_pfind_all(tb_iterator_t* iterator, tb_size_t* pprev, tb_cpointer_t data, tb_iterator_comp_t comp)
{
	return tb_binary_pfind(iterator, pprev, tb_iterator_head(iterator), tb_iterator_tail(iterator), data, comp);
}
