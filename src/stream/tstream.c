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
 * @file		tstream.c
 * @ingroup 	stream
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
//#define TB_TRACE_IMPL_TAG 				"tstream"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tstream.h"
#include "../network/network.h"
#include "../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_hong_t tb_tstream_save_ga(tb_gstream_t* istream, tb_astream_t* ostream, tb_size_t rate, tb_tstream_save_func_t func, tb_pointer_t priv)
{
	return 0;
}
tb_hong_t tb_tstream_save_gg(tb_gstream_t* istream, tb_gstream_t* ostream, tb_size_t rate, tb_tstream_save_func_t func, tb_pointer_t priv)
{
	return 0;
}
tb_hong_t tb_tstream_save_uu(tb_char_t const* iurl, tb_char_t const* ourl, tb_size_t rate, tb_tstream_save_func_t func, tb_pointer_t priv)
{
	// check
	tb_assert_and_check_return_val(iurl && ourl, -1);

	// done
	tb_hong_t 		size = -1;
	tb_gstream_t* 	istream = tb_null;
	tb_gstream_t* 	ostream = tb_null;
	do
	{
		// init istream
		istream = tb_gstream_init_from_url(iurl);
		tb_assert_and_check_break(istream);

		// init ostream
		ostream = tb_gstream_init_from_url(ourl);
		tb_assert_and_check_break(ostream);

		// save stream
		size = tb_tstream_save_gg(istream, ostream, rate, func, priv);

	} while (0);

	// exit istream
	if (istream) tb_gstream_exit(istream);
	istream = tb_null;

	// exit ostream
	if (ostream) tb_gstream_exit(ostream);
	ostream = tb_null;

	// ok?
	return size;
}
tb_handle_t tb_tstream_init_aa(tb_astream_t* istream, tb_astream_t* ostream, tb_tstream_save_func_t func, tb_pointer_t priv)
{
	return tb_null;
}
tb_handle_t tb_tstream_init_ag(tb_astream_t* istream, tb_gstream_t* ostream, tb_tstream_save_func_t func, tb_pointer_t priv)
{
	return tb_null;
}
tb_handle_t tb_tstream_init_uu(tb_char_t const* iurl, tb_char_t const* ourl, tb_tstream_save_func_t func, tb_pointer_t priv)
{
	return tb_null;
}
tb_bool_t tb_tstream_start(tb_handle_t tstream)
{
	return tb_false;
}
tb_void_t tb_tstream_pause(tb_handle_t tstream)
{
}
tb_void_t tb_tstream_limit(tb_handle_t tstream, tb_size_t rate)
{
}
tb_void_t tb_tstream_stop(tb_handle_t tstream)
{
}
tb_void_t tb_tstream_exit(tb_handle_t tstream)
{
}
