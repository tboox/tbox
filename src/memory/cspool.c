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
 * @file		cspool.c
 * @ingroup 	memory
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "cspool.h"
#include "spool.h"
#include "../libc/libc.h"
#include "../platform/platform.h"
#include "../container/container.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/* ///////////////////////////////////////////////////////////////////////
 * globals
 */

// the string pool
static tb_handle_t 		g_pool = tb_null;

// the string hash
static tb_hash_t* 		g_hash = tb_null;

// the string mutx
static tb_handle_t 		g_mutx = tb_null;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_cspool_init(tb_size_t align)
{
	// init mutx
	if (!g_mutx) g_mutx = tb_mutex_init(tb_null);
	tb_assert_and_check_return_val(g_mutx, tb_false);

	// init pool
	if (!g_pool) g_pool = tb_spool_init(TB_SPOOL_GROW_DEFAULT, align);
	tb_assert_and_check_return_val(g_pool, tb_false);

	// init hash
	if (!g_hash) g_hash = tb_hash_init(TB_HASH_SIZE_DEFAULT, tb_item_func_str(tb_true, g_pool), tb_item_func_size());
	tb_assert_and_check_return_val(g_hash, tb_false);

	// ok
	return tb_true;
}
tb_void_t tb_cspool_exit()
{
	// enter
//	if ()
}
tb_void_t tb_cspool_clear()
{
}
tb_char_t* tb_cspool_strdup(tb_char_t const* data)
{
	return tb_null;
}
tb_char_t* tb_cspool_strndup(tb_char_t const* data, tb_size_t size)
{
	return tb_null;
}
tb_void_t tb_cspool_free(tb_char_t* data)
{
}
tb_void_t tb_cspool_dump()
{
}
