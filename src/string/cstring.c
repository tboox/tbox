/*!The Tiny Box Library
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
 * \file		cstring.c
 *
 */

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "cstring.h"

/* ////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_char_t* tb_cstring_duplicate(tb_char_t const* s)
{
	if (s)
	{
		tb_size_t 	n = tb_cstring_size(s);
		tb_char_t* 	s2 = tb_malloc(n + 1);
		if (s2) tb_cstring_ncopy(s2, s, n);
		s2[n] = '\0';
		return s2;
	}
	return TB_NULL;
}
tb_char_t* tb_cstring_nduplicate(tb_char_t const* s, tb_size_t n)
{
	if (s && n)
	{
		tb_char_t* 	s2 = tb_malloc(n + 1);
		if (s2) tb_cstring_ncopy(s2, s, n);
		s2[n] = '\0';
		return s2;
	}
	return TB_NULL;
}
tb_int_t tb_cstring_find(tb_char_t const* s1, tb_char_t const* s2)
{
	if (!s1 || !s2 ) return -1;

	tb_int_t idx = -1;
	tb_char_t const* ps = s1;
	tb_char_t const* p1 = ps;
	tb_char_t const* p2 = s2;

	do
	{
		if (!*p2) 
		{
			idx = ps - s1;
			break;
		}
		if (*p2 == *p1)
		{
			++p2;
			++p1;
		} 
		else
		{
			p2 = s2;
			if (!*p1) return -1;
			p1 = ++ps;
		}

	} while (1);

	return (idx < 0? -1 : idx);
}
tb_int_t tb_cstring_find_nocase(tb_char_t const* s1, tb_char_t const* s2)
{
	if (!s1 || !s2) return -1;

	tb_int_t idx = -1;
	tb_char_t const* ps = s1;
	tb_char_t const* p1 = ps;
	tb_char_t const* p2 = s2;

	do
	{
		if (!*p2) 
		{
			idx = ps - s1;
			break;
		}
		if (*p2 == *p1 || TB_CONV_TOLOWER(*p2) == TB_CONV_TOLOWER(*p1))
		{
			++p2;
			++p1;
		} 
		else
		{
			p2 = s2;
			if (!*p1) return -1;
			p1 = ++ps;
		}

	} while (1);

	return ((idx < 0)? -1 : idx);
}
tb_int_t tb_cstring_find_char(tb_char_t const* s, tb_char_t c)
{
	TB_ASSERT_RETURN_VAL(s, -1);
	tb_char_t const* b = s;
	while (*s && *s == c) return s - b;
	return -1;
}
tb_int_t tb_cstring_find_char_nocase(tb_char_t const* s, tb_char_t c)
{
	TB_ASSERT_RETURN_VAL(s, -1);
	tb_char_t const* b = s;
	c = TB_CONV_TOLOWER(c);
	while (*s && TB_CONV_TOLOWER(*s) == c) return s - b;
	return -1;
}

