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
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author		ruki
 * @path		path.c
 * @ingroup 	platform
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
//#define TB_TRACE_IMPL_TAG 			"path"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "path.h"
#include "directory.h"
#include "../libc/libc.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_char_t const* tb_path_full(tb_char_t const* path, tb_char_t* full, tb_size_t maxn)
{
	// check
	tb_assert_and_check_return_val(path && full && maxn, tb_null);
	tb_trace_impl("path: %s", path);

	// unix path?
	if (path[0] == '/' || !tb_strnicmp(path, "file://", 7)) 
	{
		// skip prefix
		if (path[0] != '/') path += 7;

		// full? 
		if (path[0] == '/')
		{
			tb_strlcpy(full, path, maxn - 1);
			full[maxn - 1] = '\0';
			tb_trace_impl("full: %s", full);
			return full;
		}
	}
	// windows path?
	else if (tb_isalpha(path[0]) && path[1] == ':' && (path[2] == '/' || path[2] == '\\'))
	{
		tb_strlcpy(full, path, maxn - 1);
		full[maxn - 1] = '\0';
		tb_trace_impl("full: %s", full);
		return full;
	}
	
	// the current directory
	tb_size_t size = 0;
	if (!(size = tb_directory_curt(full, maxn))) return tb_null;
	tb_trace_impl("curt: %s", full);

	// is windows path?
	tb_size_t 	w = tb_false;
	tb_char_t* 	b = full;
	if (size > 2 && tb_isalpha(b[0]) && b[1] == ':' && (b[2] == '/' || b[2] == '\\'))
	{
		// skip the drive prefix
		b += 2;
		size -= 2;

		// set root 
		b[0] = '\\';

		// windows path: true
		w = tb_true;
	}

	// remove the last '/' or '\\'
	tb_size_t n = tb_strlen(b);
	if (n > 1 && (b[n - 1] == '/' || b[n - 1] == '\\'))
		b[n - 1] = '\0';

	// path => full
	tb_char_t const* 	p = path;
	tb_char_t const* 	t = p;
	tb_char_t* 			q = b + size;
	tb_char_t const* 	e = b + maxn - 1;
	while (1)
	{
		if (*p == '/' || *p == '\\' || !*p)
		{
			// the item size
			n = p - t;

			// ..? remove item
			if (n == 2 && t[0] == '.' && t[1] == '.')
			{
				// find the last '/'
				for (; q > b && (*q != '/' && *q != '\\'); q--) ;

				// strip it
				*q = '\0';
			}
			// .? continue it
			else if (n == 1 && t[0] == '.') ;
			// append item
			else if (n && q + 1 + n < e)
			{
				*q++ = w? '\\' : '/';
				tb_strlcpy(q, t, n);
				q += n;
			}
			// empty item
			else if (!n) ;
			// too small?
			else 
			{
				tb_trace("[path]: the full path is too small for %s", path);
				return tb_null;
			}

			// break
			tb_check_break(*p);

			// next
			t = p + 1;
		}

		// next
		p++;
	}

	// end
	if (q > b) *q = '\0';
	// root?
	else
	{
		*q++ = w? '\\' : '/';
		*q = '\0';
	}

	// trace	
	tb_trace_impl("full: %s", full);
	
	// ok?
	return full;
}
