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
 * types
 */

// the tstream type enum
typedef enum __tb_tstream_type_e
{
	TB_TSTREAM_TYPE_AA 		= 0
,	TB_TSTREAM_TYPE_AG 		= 1

}tb_tstream_type_e;

// the tstream type
typedef struct __tb_tstream_t
{
	// the type
	tb_size_t 				type;

	// the istream
	tb_astream_t* 			istream;

	// the istream is owner?
	tb_bool_t 				iowner;

	// the save func
	tb_tstream_save_func_t 	func;

	// the save func priv
	tb_pointer_t 			priv;

	// is paused?
	tb_atomic_t 			paused;

	// is stoped?
	tb_atomic_t 			stoped;

	// the seek offset
	tb_hong_t 				offset;

	// the base time
	tb_hong_t 				base;

	// the basc time
	tb_hong_t 				basc;

	// the total size
	tb_hize_t 				size;

	// the size for 1s
	tb_size_t 				size1s;

	// the limit rate
	tb_atomic_t 			lrate;

	// the current rate
	tb_size_t 				crate;

}tb_tstream_t;

// the tstream aa type
typedef struct __tb_tstream_aa_t
{
	// the base
	tb_tstream_t 			base;

	// the ostream
	tb_astream_t* 			ostream;

	// the ostream is owner?
	tb_bool_t 				oowner;

}tb_tstream_aa_t;

// the tstream ag type
typedef struct __tb_tstream_ag_t
{
	// the base
	tb_tstream_t 			base;

	// the ostream
	tb_gstream_t* 			ostream;

}tb_tstream_ag_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_tstream_istream_read_func(tb_astream_t* astream, tb_size_t state, tb_byte_t const* data, tb_size_t size, tb_pointer_t priv);
static tb_bool_t tb_tstream_istream_writ_func(tb_astream_t* astream, tb_size_t state, tb_size_t real, tb_size_t size, tb_pointer_t priv)
{
	// check
	tb_tstream_t* tstream = (tb_tstream_t*)priv;
	tb_assert_and_check_return_val(astream && tstream && tstream->func, tb_false);

	// trace
	tb_trace_impl("writ: real: %lu, size: %lu, state: %s", real, size, tb_astream_state_cstr(state));

	// the time
	tb_hong_t time = tb_aicp_time(tb_astream_aicp(astream));

	// done
	tb_bool_t bwrit = tb_false;
	do
	{
		// ok?
		tb_check_break(state == TB_ASTREAM_STATE_OK);
			
		// save size
		tstream->size += real;

		// save size for 1s
		tstream->size1s += real;

		// < 1s?
		if (time < tstream->basc + 1000)
		{
			// save current rate if < 1s from base
			if (time < tstream->base + 1000) tstream->crate = tstream->size1s;
		}
		else
		{
			// save current rate
			tstream->crate = tstream->size1s;

			// update basc
			tstream->basc = time;

			// reset size
			tstream->size1s = 0;
		}

		// done func
		if (!tstream->func(state, real, tstream->crate, tstream->priv)) break;

		// reset state
		state = TB_ASTREAM_STATE_UNKNOWN_ERROR;
			
		// stoped?
		if (tb_atomic_get(&tstream->stoped))
		{
			state = TB_ASTREAM_STATE_KILLED;
			break;
		}

		// not finished? continue to writ
		if (real < size) bwrit = tb_true;
		// not paused?
		else if (!tb_atomic_get(&tstream->paused))
		{
			// continue to read it
			if (!tb_astream_read(tstream->istream, tb_tstream_istream_read_func, tstream)) break;
		}

		// ok
		state = TB_ASTREAM_STATE_OK;

	} while (0);

	// failed? 
	if (state != TB_ASTREAM_STATE_OK) 
	{
		// compute the total rate
		tb_size_t trate = (tstream->size && (time > tstream->base))? (tb_size_t)((tstream->size * 1000) / (time - tstream->base)) : (tb_size_t)tstream->size;

		// done func
		tstream->func(state, 0, trate, tstream->priv);
	}

	// continue to writ or break it
	return bwrit;
}
static tb_bool_t tb_tstream_istream_read_func(tb_astream_t* astream, tb_size_t state, tb_byte_t const* data, tb_size_t size, tb_pointer_t priv)
{
	// check
	tb_tstream_t* tstream = (tb_tstream_t*)priv;
	tb_assert_and_check_return_val(astream && tstream && tstream->func, tb_false);

	// trace
	tb_trace_impl("read: size: %lu, state: %s", size, tb_astream_state_cstr(state));

	// done
	tb_bool_t bread = tb_false;
	do
	{
		// ok?
		tb_check_break(state == TB_ASTREAM_STATE_OK);

		// reset state
		state = TB_ASTREAM_STATE_UNKNOWN_ERROR;

		// stoped?
		if (tb_atomic_get(&tstream->stoped))
		{
			state = TB_ASTREAM_STATE_KILLED;
			break;
		}

		// check
		tb_assert_and_check_break(data && size);

		// for astream
		if (tstream->type == TB_TSTREAM_TYPE_AA)
		{
			// check
			tb_assert_and_check_break(((tb_tstream_aa_t*)tstream)->ostream);

			// writ it
			if (!tb_astream_writ(((tb_tstream_aa_t*)tstream)->ostream, data, size, tb_tstream_istream_writ_func, tstream)) break;
		}
		// for gstream
		else if (tstream->type == TB_TSTREAM_TYPE_AG)
		{
			// check
			tb_assert_and_check_break(((tb_tstream_ag_t*)tstream)->ostream);
 
			// writ it
			if (!tb_gstream_bwrit(((tb_tstream_ag_t*)tstream)->ostream, data, size)) break;

			// the time
			tb_hong_t time = tb_aicp_time(tb_astream_aicp(astream));

			// save size 
			tstream->size += size;

			// save size for 1s
			tstream->size1s += size;

			// < 1s?
			if (time < tstream->basc + 1000)
			{
			
				// save current rate if < 1s from base
				if (time < tstream->base + 1000) tstream->crate = tstream->size1s;
			}
			else
			{
				// save current rate
				tstream->crate = tstream->size1s;

				// update basc
				tstream->basc = time;

				// reset size
				tstream->size1s = 0;
			}

			// done func
			if (!tstream->func(TB_ASTREAM_STATE_OK, size, tstream->crate, tstream->priv)) break;

			// continue to read it if not paused
			if (!tb_atomic_get(&tstream->paused)) bread = tb_true;
		}
		else 
		{
			tb_assert_and_check_break(0);
		}

		// ok
		state = TB_ASTREAM_STATE_OK;

	} while (0);

	// closed or failed?
	if (state != TB_ASTREAM_STATE_OK) 
	{
		// flush it
		if (tstream->type == TB_TSTREAM_TYPE_AG && ((tb_tstream_ag_t*)tstream)->ostream)
			tb_gstream_bfwrit(((tb_tstream_ag_t*)tstream)->ostream, tb_null, 0);

		// the time
		tb_hong_t time = tb_aicp_time(tb_astream_aicp(astream));

		// compute the total rate
		tb_size_t trate = (tstream->size && (time > tstream->base))? (tb_size_t)((tstream->size * 1000) / (time - tstream->base)) : (tb_size_t)tstream->size;

		// done func
		tstream->func(state, 0, trate, tstream->priv);
	}

	// continue to read or break it
	return bread;
}
static tb_bool_t tb_tstream_istream_seek_func(tb_astream_t* astream, tb_size_t state, tb_hize_t offset, tb_pointer_t priv)
{
	// check
	tb_tstream_t* tstream = (tb_tstream_t*)priv;
	tb_assert_and_check_return_val(astream && tstream && tstream->func, tb_false);

	// trace
	tb_trace_impl("seek: offset: %llu, state: %s", offset, tb_astream_state_cstr(state));

	// done
	do
	{
		// ok?
		tb_check_break(state == TB_ASTREAM_STATE_OK);

		// reset state
		state = TB_ASTREAM_STATE_UNKNOWN_ERROR;
			
		// stoped?
		if (tb_atomic_get(&tstream->stoped))
		{
			state = TB_ASTREAM_STATE_KILLED;
			break;
		}

		// read it
		if (!tb_astream_read(tstream->istream, tb_tstream_istream_read_func, tstream)) break;

		// ok
		state = TB_ASTREAM_STATE_OK;

	} while (0);

	// failed? done func
	if (state != TB_ASTREAM_STATE_OK) tstream->func(state, 0, 0, tstream->priv);

	// ok
	return tb_true;
}
static tb_bool_t tb_tstream_ostream_open_func(tb_astream_t* astream, tb_size_t state, tb_pointer_t priv)
{
	// check
	tb_tstream_t* tstream = (tb_tstream_t*)priv;
	tb_assert_and_check_return_val(astream && tstream && tstream->func, tb_false);

	// trace
	tb_trace_impl("open: state: %s", tb_astream_state_cstr(state));

	// done
	do
	{
		// ok?
		tb_check_break(state == TB_ASTREAM_STATE_OK);

		// reset state
		state = TB_ASTREAM_STATE_UNKNOWN_ERROR;

		// check
		tb_assert_and_check_break(tstream->istream);

		// stoped?
		if (tb_atomic_get(&tstream->stoped))
		{
			state = TB_ASTREAM_STATE_KILLED;
			break;
		}

		// need seek?
		if (tstream->offset >= 0)
		{
			// open and seek it
			if (!tb_astream_oseek(tstream->istream, tstream->offset, tb_tstream_istream_seek_func, tstream)) break;
		}
		else
		{
			// open and read it
			if (!tb_astream_oread(tstream->istream, tb_tstream_istream_read_func, tstream)) break;
		}

		// ok
		state = TB_ASTREAM_STATE_OK;

	} while (0);

	// failed? done func
	if (state != TB_ASTREAM_STATE_OK) tstream->func(state, 0, 0, tstream->priv);

	// ok
	return tb_true;
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_hong_t tb_tstream_save_gg(tb_gstream_t* istream, tb_gstream_t* ostream, tb_size_t lrate, tb_tstream_save_func_t func, tb_pointer_t priv)
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
	tb_size_t 	crate = 0;
	tb_long_t 	delay = 0;
	tb_hize_t 	size1s = 0;
	do
	{
		// the need
		tb_size_t need = lrate? tb_min(lrate, TB_GSTREAM_BLOCK_MAXN) : TB_GSTREAM_BLOCK_MAXN;

		// read data
		tb_long_t real = tb_gstream_aread(istream, data, need);
		if (real > 0)
		{
			// writ data
			if (!tb_gstream_bwrit(ostream, data, real)) break;

			// save writ
			writ += real;

			// has func or limit rate?
			if (func || lrate) 
			{
				// the time
				time = tb_mclock();

				// save size1s
				size1s += real;

				// < 1s?
				if (time < basc + 1000)
				{
					// save current rate if < 1s from base
					if (time < base + 1000) crate = size1s;
				
					// compute the delay for limit rate
					if (lrate) delay = size1s >= lrate? basc + 1000 - time : 0;
				}
				else
				{
					// save current rate
					crate = size1s;

					// update basc
					basc = time;

					// reset size1s
					size1s = 0;

					// reset delay
					delay = 0;
				}

				// done func
				if (func) func(TB_GSTREAM_STATE_OK, real, crate, priv);

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
		tb_size_t trate = (writ && (time > base))? (tb_size_t)((writ * 1000) / (time - base)) : writ;
	
		// done func
		func(TB_GSTREAM_STATE_CLOSED, 0, trate, priv);
	}

	// ok?
	return writ;
}
tb_hong_t tb_tstream_save_uu(tb_char_t const* iurl, tb_char_t const* ourl, tb_size_t lrate, tb_tstream_save_func_t func, tb_pointer_t priv)
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
		size = tb_tstream_save_gg(istream, ostream, lrate, func, priv);

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
tb_handle_t tb_tstream_init_aa(tb_astream_t* istream, tb_astream_t* ostream, tb_tstream_save_func_t func, tb_pointer_t priv )
{
	// done
	tb_tstream_aa_t* tstream = tb_null;
	do
	{
		// check
		tb_assert_and_check_break(istream && ostream);	

		// make tstream
		tstream = tb_malloc0(sizeof(tb_tstream_aa_t));
		tb_assert_and_check_break(tstream);

		// init tstream
		tstream->base.type 		= TB_TSTREAM_TYPE_AA;
		tstream->base.istream 	= istream;
		tstream->base.iowner 	= tb_false;
		tstream->base.func 		= func;
		tstream->base.priv 		= priv;
		tstream->ostream 		= ostream;
		tstream->oowner 		= tb_false;

	} while (0);

	// ok?
	return (tb_handle_t)tstream;
}
tb_handle_t tb_tstream_init_ag(tb_astream_t* istream, tb_gstream_t* ostream, tb_tstream_save_func_t func, tb_pointer_t priv )
{
	// done
	tb_tstream_ag_t* tstream = tb_null;
	do
	{
		// check
		tb_assert_and_check_break(istream && ostream);	

		// make tstream
		tstream = tb_malloc0(sizeof(tb_tstream_ag_t));
		tb_assert_and_check_break(tstream);

		// init tstream
		tstream->base.type 		= TB_TSTREAM_TYPE_AG;
		tstream->base.istream 	= istream;
		tstream->base.iowner 	= tb_false;
		tstream->base.func 		= func;
		tstream->base.priv 		= priv;
		tstream->ostream 		= ostream;

	} while (0);

	// ok?
	return (tb_handle_t)tstream;
}
tb_handle_t tb_tstream_init_uu(tb_aicp_t* aicp, tb_char_t const* iurl, tb_char_t const* ourl, tb_tstream_save_func_t func, tb_pointer_t priv )
{
	// done
	tb_astream_t* 		istream = tb_null;
	tb_astream_t* 		ostream = tb_null;
	tb_tstream_aa_t* 	tstream = tb_null;
	do
	{
		// check
		tb_assert_and_check_break(aicp && iurl && ourl);

		// init istream
		istream = tb_astream_init_from_url(aicp, iurl);
		tb_assert_and_check_break(istream);

		// init ostream
		ostream = tb_astream_init_from_url(aicp, ourl);
		tb_assert_and_check_break(ostream);

		// make tstream
		tstream = tb_malloc0(sizeof(tb_tstream_aa_t));
		tb_assert_and_check_break(tstream);

		// init tstream
		tstream->base.type 		= TB_TSTREAM_TYPE_AA;
		tstream->base.istream 	= istream;
		tstream->base.iowner 	= tb_true;
		tstream->base.func 		= func;
		tstream->base.priv 		= priv;
		tstream->ostream 		= ostream;
		tstream->oowner 		= tb_true;
		
		// ok
		istream = tb_null;
		ostream = tb_null;

	} while (0);

	// exit istream
	if (istream) tb_astream_exit(istream);
	istream = tb_null;

	// exit ostream
	if (ostream) tb_astream_exit(ostream);
	ostream = tb_null;

	// ok?
	return (tb_handle_t)tstream;
}
tb_bool_t tb_tstream_start(tb_handle_t handle, tb_hong_t offset)
{
	// check
	tb_tstream_t* tstream = (tb_tstream_t*)handle;
	tb_assert_and_check_return_val(tstream, tb_false);

	// done
	tb_bool_t ok = tb_false;
	do
	{
		// stoped?
		tb_assert_and_check_break(!tb_atomic_get(&tstream->stoped));

		// check
		tb_assert_and_check_break(tstream->istream);

		// save offset
		tstream->offset = offset;

		// resume it
		tb_atomic_set0(&tstream->paused);

		// init some rate info
		tstream->base 	= tb_aicp_time(tb_astream_aicp(tstream->istream));
		tstream->basc 	= tstream->base;
		tstream->size 	= 0;
		tstream->size1s = 0;
		tstream->crate 	= 0;

		// open it first if has been opened
		tb_bool_t opened = tb_false;
		if (tstream->type == TB_TSTREAM_TYPE_AA)
		{
			// the ostream
			tb_astream_t* ostream = ((tb_tstream_aa_t*)tstream)->ostream;
			tb_assert_and_check_break(ostream);

			// open it first if ostream have been not opened
			if (!tb_astream_ctrl(ostream, TB_ASTREAM_CTRL_IS_OPENED, &opened)) break;
			if (!opened)
			{
				// ctrl file
				if (tb_astream_type(ostream) == TB_ASTREAM_TYPE_FILE) 
				{
					if (!tb_astream_ctrl(ostream, TB_ASTREAM_CTRL_FILE_SET_MODE, TB_FILE_MODE_WO | TB_FILE_MODE_CREAT | TB_FILE_MODE_TRUNC)) return -1;
				}

				// open it
				if (!tb_astream_open(ostream, tb_tstream_ostream_open_func, tstream)) break;
			}
		}
		else if (tstream->type == TB_TSTREAM_TYPE_AG)
		{
			// the ostream
			tb_gstream_t* ostream = ((tb_tstream_ag_t*)tstream)->ostream;
			tb_assert_and_check_break(ostream);

			// open it first if ostream have been not opened
			if (!tb_gstream_ctrl(ostream, TB_GSTREAM_CTRL_IS_OPENED, &opened)) break;
			if (!opened)
			{
				// ctrl file
				if (tb_gstream_type(ostream) == TB_GSTREAM_TYPE_FILE) 
				{
					if (!tb_gstream_ctrl(ostream, TB_GSTREAM_CTRL_FILE_SET_MODE, TB_FILE_MODE_WO | TB_FILE_MODE_CREAT | TB_FILE_MODE_TRUNC)) return -1;
				}

				// open it
				if (!tb_gstream_bopen(ostream)) break;

				// open ok
				opened = tb_true;
			}
		}

		// opened? seek or read it now
		if (opened)
		{
			// need seek?
			if (offset >= 0)
			{
				// open and seek it
				if (!tb_astream_oseek(tstream->istream, offset, tb_tstream_istream_seek_func, tstream)) break;
			}
			else
			{
				// open and read it
				if (!tb_astream_oread(tstream->istream, tb_tstream_istream_read_func, tstream)) break;
			}
		}

		// ok
		ok = tb_true;

	} while (0);

	// ok?
	return ok;
}
tb_void_t tb_tstream_pause(tb_handle_t handle)
{
	// check
	tb_tstream_t* tstream = (tb_tstream_t*)handle;
	tb_assert_and_check_return(tstream);

	// pause it
	tb_atomic_set(&tstream->paused, 1);
}
tb_void_t tb_tstream_limit(tb_handle_t handle, tb_size_t rate)
{
	// check
	tb_tstream_t* tstream = (tb_tstream_t*)handle;
	tb_assert_and_check_return(tstream);

	// set the limit rate
	tb_atomic_set(&tstream->lrate, rate);
}
tb_void_t tb_tstream_stop(tb_handle_t handle)
{
	// check
	tb_tstream_t* tstream = (tb_tstream_t*)handle;
	tb_assert_and_check_return(tstream);

	// stop it
	if (tb_atomic_fetch_and_set(&tstream->stoped, 1))
	{
		// kill istream
		if (tstream->istream) tb_astream_kill(tstream->istream);

		// kill ostream
		if (tstream->type == TB_TSTREAM_TYPE_AA) 
		{
			if (((tb_tstream_aa_t*)tstream)->ostream) 
				tb_astream_kill(((tb_tstream_aa_t*)tstream)->ostream);
		}
	}
}
tb_void_t tb_tstream_exit(tb_handle_t handle)
{
	// check
	tb_tstream_t* tstream = (tb_tstream_t*)handle;
	tb_assert_and_check_return(tstream);

	// trace
	tb_trace_impl("exit: ..");

	// stop it first if not stoped
	if (!tb_atomic_get(&tstream->stoped)) 
	{
		// stop it
		tb_tstream_stop(handle);

	}

	// exit istream
	if (tstream->istream)
	{
		// wait it
		tb_size_t tryn = 10;
		while (tb_astream_pending(tstream->istream) && tryn--) tb_msleep(200);
		if (tb_astream_pending(tstream->istream))
		{
			// trace
			tb_trace("[tstream]: the istream is pending for func: %s, line: %lu, file: %s", tstream->istream->func, tstream->istream->line, tstream->istream->file);
		}

		// exit it
		if (tstream->iowner) tb_astream_exit(tstream->istream);
		tstream->istream = tb_null;
	}

	// exit ostream
	if (tstream->type == TB_TSTREAM_TYPE_AA) 
	{
		tb_astream_t* ostream = ((tb_tstream_aa_t*)tstream)->ostream;
		if (ostream)
		{
			// wait it
			tb_size_t tryn = 10;
			while (tb_astream_pending(ostream) && tryn--) tb_msleep(200);
			if (tb_astream_pending(ostream))
			{
				// trace
				tb_trace("[tstream]: the ostream is pending for func: %s, line: %lu, file: %s", ostream->func, ostream->line, ostream->file);
			}

			// exit it
			if (((tb_tstream_aa_t*)tstream)->oowner) tb_astream_exit(ostream);
			((tb_tstream_aa_t*)tstream)->ostream = tb_null;
		}
	}
	else if (tstream->type == TB_TSTREAM_TYPE_AG)
		((tb_tstream_ag_t*)tstream)->ostream = tb_null;

	// exit tstream
	tb_free(tstream);

	// trace
	tb_trace_impl("exit: ok");

}
