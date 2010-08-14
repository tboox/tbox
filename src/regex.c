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
 * \file		regex.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "regex.h"
#include "external/pcre/pcre.h"

/* /////////////////////////////////////////////////////////
 * macros
 */

// should be a multiple of 3
#define TB_REGEX_MATCH_MAX_COUNT 		(3 * 20)

/* /////////////////////////////////////////////////////////
 * types
 */
typedef struct __tb_regex_t
{
	pcre* 				re;
	tb_int_t 			matches[TB_REGEX_MATCH_MAX_COUNT];
	tb_int_t 			match_n;
	tb_char_t* 			match_s; 
	tb_char_t const* 	error;
	tb_int_t 		 	error_at;

}tb_regex_t;
/* /////////////////////////////////////////////////////////
 * implemention
 */

tb_handle_t tb_regex_create(tb_char_t const* regex, tb_int_t opts)
{
	TB_ASSERT(regex);
	if (!regex) return TB_INVALID_HANDLE;
	// { init
	tb_regex_t r;
	memset(&r, 0, sizeof(tb_regex_t));

	// compile regex
	r.re = tb_pcre_compile(regex, opts, &(r.error), &(r.error_at), TB_NULL);
	if (!r.re) return TB_INVALID_HANDLE;
	// { malloc
	tb_regex_t* pr = tb_malloc(sizeof(tb_regex_t));
	if (!pr) return TB_INVALID_HANDLE;

	// update
	*pr = r;

	// return handle
	return ((tb_handle_t)pr);
	// }}
}
void tb_regex_destroy(tb_handle_t hregex)
{
	TB_ASSERT(hregex != TB_INVALID_HANDLE);
	if (hregex == TB_INVALID_HANDLE) return ;
	// {
	tb_regex_t* pr = (tb_regex_t*)hregex;
	TB_ASSERT(pr);
	if (!pr) return ;

	if (pr->re) tb_free(pr->re);
	tb_free(pr);
	// }
}

tb_bool_t tb_regex_exec(tb_handle_t hregex, tb_char_t const* s, tb_int_t opts)
{
	TB_ASSERT(hregex != TB_INVALID_HANDLE && s);
	if (hregex == TB_INVALID_HANDLE || !s) return TB_FALSE;
	// {
	tb_regex_t* pr = (tb_regex_t*)hregex;
	TB_ASSERT(pr);
	if (!pr) return TB_FALSE;

	// execute regex
	pr->match_n = tb_pcre_exec(pr->re, opts, s, strlen(s), 0, 0, pr->matches, TB_REGEX_MATCH_MAX_COUNT);
	if (pr->match_n <= 0) return TB_FALSE;

	// save pointer to string
	pr->match_s = s;
	// }
	return TB_TRUE;
}
tb_char_t const* tb_regex_get(tb_handle_t hregex, tb_int_t index, tb_char_t* sub_s, tb_int_t* sub_sn)
{
	TB_ASSERT(hregex != TB_INVALID_HANDLE && index >= 0 && index < TB_REGEX_MATCH_MAX_COUNT);
	if (hregex == TB_INVALID_HANDLE || index < 0 || index >= TB_REGEX_MATCH_MAX_COUNT) return TB_NULL;
	// {
	tb_regex_t* pr = (tb_regex_t*)hregex;
	TB_ASSERT(pr);
	if (!pr) return TB_NULL;
	// {
	tb_int_t sn = pr->matches[(index << 1) + 1] - pr->matches[(index << 1)];
	if (sub_s) 
	{
		strncpy(sub_s, pr->match_s + pr->matches[(index << 1)], sn);
		sub_s[sn] = '\0';
	}
	if (sub_sn) *sub_sn = sn;
	// }}
	return sub_s;
}
tb_int_t tb_regex_count(tb_handle_t hregex)
{
	TB_ASSERT(hregex != TB_INVALID_HANDLE);
	if (hregex == TB_INVALID_HANDLE) return 0;
	// {
	tb_regex_t* pr = (tb_regex_t*)hregex;
	TB_ASSERT(pr);
	if (!pr) return 0;

	return pr->match_n;
	// }
}

tb_char_t const* tb_regex_error(tb_handle_t hregex, tb_int_t* error_at)
{
	TB_ASSERT(hregex != TB_INVALID_HANDLE);
	if (hregex == TB_INVALID_HANDLE) return TB_NULL;
	// {
	tb_regex_t* pr = (tb_regex_t*)hregex;
	if (!pr || !(pr->error)) return TB_NULL;

	if (error_at) *error_at = pr->error_at;
	return (pr->error);
	// }
}
