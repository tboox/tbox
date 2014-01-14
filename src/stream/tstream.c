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
#define TB_TRACE_IMPL_TAG 				"tstream"

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
	// check
	tb_assert_and_check_return_val(ostream && istream, -1);	

	// open it first if istream have been not opened
	tb_bool_t opened = tb_false;
	if (!tb_gstream_ctrl(istream, TB_GSTREAM_CTRL_IS_OPENED, &opened)) return -1;
	if (!opened && !tb_gstream_bopen(istream)) return -1;
	
	// open it first if ostream have been not opened
	opened = tb_false;
	if (!tb_gstream_ctrl(ostream, TB_GSTREAM_CTRL_IS_OPENED, &opened)) return -1;
	if (!opened)
	{
		// ctrl file
		if (tb_gstream_type(ostream) == TB_GSTREAM_TYPE_FILE) 
		{
			if (!tb_gstream_ctrl(ostream, TB_GSTREAM_CTRL_FILE_SET_MODE, TB_FILE_MODE_WO | TB_FILE_MODE_CREAT | TB_FILE_MODE_TRUNC)) return -1;
		}

		// open it
		if (!tb_gstream_bopen(ostream)) return -1;
	}

	// writ data
	tb_byte_t 	data[TB_GSTREAM_BLOCK_MAXN];
	tb_hize_t 	writ = 0;
	tb_hize_t 	left = tb_gstream_left(istream);
	tb_hong_t 	base = tb_mclock();
	tb_hong_t 	basc = base;
	tb_hong_t 	time = 0;
	tb_hize_t 	size = 0;
	tb_size_t 	crate = 0;
	tb_long_t 	delay = 0;
	do
	{
		// the need
		tb_size_t need = rate? tb_min(rate, TB_GSTREAM_BLOCK_MAXN) : TB_GSTREAM_BLOCK_MAXN;

		// read data
		tb_long_t real = tb_gstream_aread(istream, data, need);
		if (real > 0)
		{
			// writ data
			if (!tb_gstream_bwrit(ostream, data, real)) break;

			// save writ
			writ += real;

			// has func or limit rate?
			if (func || rate) 
			{
				// the time
				time = tb_mclock();

				// < 1s?
				if (time < basc + 1000)
				{
					// save size
					size += real;
				
					// save current rate if < 1s from base
					if (time < base + 1000) crate = size;
				
					// compute the delay for limit rate
					if (rate) delay = size >= rate? basc + 1000 - time : 0;
				}
				else
				{
					// save current rate
					crate = size;

					// update basc
					basc = time;

					// reset size
					size = 0;

					// reset delay
					delay = 0;
				}

				// done func
				if (func) func(real, crate, priv);

				// wait some time for limit rate
				if (delay) tb_msleep(delay);
			}
		}
		else if (!real) 
		{
			// wait
			tb_long_t wait = tb_gstream_wait(istream, TB_GSTREAM_WAIT_READ, istream->timeout);
			tb_assert_and_check_break(wait >= 0);

			// timeout?
			tb_check_break(wait);

			// has writ?
			tb_assert_and_check_break(wait & TB_GSTREAM_WAIT_READ);
		}
		else break;

		// is end?
		if (left && writ >= left) break;

	} while(1);

	// flush the ostream
	if (!tb_gstream_bfwrit(ostream, tb_null, 0)) return -1;

	// has func?
	if (func) 
	{
		// the time
		time = tb_mclock();

		// compute the total rate
		tb_size_t trate = (writ && (time > base))? (tb_size_t)((writ * 1000) / (time - base)) : 0;
	
		// done func
		func(-1, trate, priv);
	}

	// ok?
	return writ;
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
