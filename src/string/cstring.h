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
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		cstring.h
 *
 */
#ifndef TB_CSTRING_H
#define TB_CSTRING_H

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../conv.h"
/* ////////////////////////////////////////////////////////////////////////
 * types
 */


/* ////////////////////////////////////////////////////////////////////////
 * interfaces
 */

// compare
static __tplat_inline__ tb_int_t tb_cstring_compare(tb_char_t const* s1, tb_char_t const* s2)
{
	TB_ASSERT(s1 && s2);
	if (s1 == s2) return 0;
	for (; (*s1 == *s2) && *s1; s1++, s2++) ;
	return (*s1 - *s2);
}
static __tplat_inline__ tb_int_t tb_cstring_ncompare(tb_char_t const* s1, tb_char_t const* s2, tb_size_t n)
{
	TB_ASSERT(s1 && s2);
	if (s1 == s2 || !n) return 0;
	for (; (*s1 == *s2) && *s1 && --n; s1++, s2++) ;
	return (*s1 - *s2);
}
static __tplat_inline__ tb_int_t tb_cstring_compare_nocase(tb_char_t const* s1, tb_char_t const* s2)
{
	TB_ASSERT(s1 && s2);
	if (s1 == s2) return 0;
	for (; ((*s1 == *s2) || (TB_CONV_TOLOWER(*s1) == TB_CONV_TOLOWER(*s2))) && *s1; s1++, s2++) ;
	return (TB_CONV_TOLOWER(*s1) - TB_CONV_TOLOWER(*s2));
}
static __tplat_inline__ tb_int_t tb_cstring_ncompare_nocase(tb_char_t const* s1, tb_char_t const* s2, tb_size_t n)
{
	TB_ASSERT(s1 && s2);
	if (s1 == s2 || !n) return 0;
	for (; ((*s1 == *s2) || (TB_CONV_TOLOWER(*s1) == TB_CONV_TOLOWER(*s2))) && *s1 && --n; s1++, s2++) ;
	return (TB_CONV_TOLOWER(*s1) - TB_CONV_TOLOWER(*s2));
}

// length
static __tplat_inline__ tb_size_t tb_cstring_size(tb_char_t const* s)
{
#if 0
	TB_ASSERT(s);
	tb_char_t const* p = s;
	if (!p || !*p) return 0;
	while (*p) p++;
	return (p - s);
#else
	TB_ASSERT(s);
	tb_char_t const* p = s;
	if (!p || !*p) return 0;
	while (1) 
	{
		if (!p[0]) return (p - s + 0);
		if (!p[1]) return (p - s + 1);
		if (!p[2]) return (p - s + 2);
		if (!p[3]) return (p - s + 3);
		p += 4;
	}
	return 0;
#endif
}

// copy
static __tplat_inline__ tb_char_t* tb_cstring_copy(tb_char_t* s1, tb_char_t const* s2)
{
#if 0
	TB_ASSERT(s1 && s2);
	tb_char_t* p = s1;
	if (s1 == s2) return p;
	while (*s1++ = *s2++) ;
	return p;
#else
	TB_ASSERT(s1 && s2);
	tb_char_t* p = s1;
	if (s1 == s2) return p;
	while (1) 
	{
		if (!(s1[0] = s2[0])) break;
		if (!(s1[1] = s2[1])) break;
		if (!(s1[2] = s2[2])) break;
		if (!(s1[3] = s2[3])) break;
		s1 += 4;
		s2 += 4;
	}
	return p;
#endif
}
static __tplat_inline__ tb_char_t* tb_cstring_ncopy(tb_char_t* s1, tb_char_t const* s2, tb_size_t n)
{
#if 0
	TB_ASSERT(s1 && s2);
	tb_char_t* p = s1;
	if (s1 == s2 || !n) return p;
	while ((*s1++ = *s2++) && --n) ;
	*s1 = '\0';
	return p;
#else
	TB_ASSERT(s1 && s2);
	tb_char_t* p = s1;
	if (s1 == s2 || !n) return p;
	while (1) 
	{
		if (!(s1[0] = s2[0]) || !--n) {s1[1] = '\0'; break;}
		if (!(s1[1] = s2[1]) || !--n) {s1[2] = '\0'; break;}
		if (!(s1[2] = s2[2]) || !--n) {s1[3] = '\0'; break;}
		if (!(s1[3] = s2[3]) || !--n) {s1[4] = '\0'; break;}
		s1 += 4;
		s2 += 4;
	}
	return p;
#endif
}

#endif

