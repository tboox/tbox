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
 * @file		prefix.h
 *
 */
#ifndef TB_PLATFROM_UNIX_PREFIX_H
#define TB_PLATFROM_UNIX_PREFIX_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"
#include "../../libc/libc.h"

/* ///////////////////////////////////////////////////////////////////////
 * inlines
 */

/* transform the path to the unix style
 *
 * /home/ruki/file.txt
 * file:///home/ruki/file.txt
 *
 * => /home/ruki/file.txt
 */
static __tb_inline__ tb_char_t const* tb_path_to_unix(tb_char_t const* path)
{
	// check
	tb_assert_and_check_return_val(path, tb_null);

	// done
	return (!tb_strnicmp(path, "file://", 7))? (path + 7) : path;
}

#endif
