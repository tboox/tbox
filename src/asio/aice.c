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
 * @file		aice.c
 * @ingroup 	asio
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "aice.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_char_t const* tb_aice_state_cstr(tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aice, tb_null);

	// the state string
	static tb_char_t const* state[] =
	{
	 	"ok"
	, 	"failed"
	, 	"killed"
	, 	"closed"
	, 	"pending"
	, 	"timeout"
	, 	"not supported"
	};
	tb_assert_and_check_return_val(aice->state < tb_arrayn(state), "unknown");

	// ok
	return state[aice->state];
}

