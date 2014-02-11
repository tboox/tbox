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
 * @file		filter.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
//#define TB_TRACE_IMPL_TAG 				"afilter"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../filter/filter.h"
#include "../../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the filter stream type
typedef struct __tb_astream_filter_t
{
	// the base
	tb_astream_t 				base;

	// the filter handle
	tb_filter_t* 				filter;

	// the filter handle is referenced? need not exit it
	tb_bool_t 					bref;

	// the astream
	tb_astream_t* 				astream;

	// the func
	union
	{
		tb_astream_open_func_t 	open;
		tb_astream_read_func_t 	read;
		tb_astream_writ_func_t 	writ;
		tb_astream_sync_func_t 	sync;

	} 							func;

	// the priv
	tb_pointer_t 				priv;

}tb_astream_filter_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_astream_filter_t* tb_astream_filter_cast(tb_astream_t* astream)
{
	tb_assert_and_check_return_val(astream && astream->type == TB_ASTREAM_TYPE_FLTR, tb_null);
	return (tb_astream_filter_t*)astream;
}
static tb_bool_t tb_astream_filter_open_func(tb_astream_t* astream, tb_size_t state, tb_pointer_t priv)
{
	// check
	tb_assert_and_check_return_val(astream, tb_false);

	// the stream
	tb_astream_filter_t* fstream = (tb_astream_filter_t*)priv;
	tb_assert_and_check_return_val(fstream && fstream->func.open, tb_false);

	// opened
	tb_atomic_set(&fstream->base.opened, 1);

	// done func
	return fstream->func.open((tb_astream_t*)fstream, state, fstream->priv);
}
static tb_bool_t tb_astream_filter_open(tb_astream_t* astream, tb_astream_open_func_t func, tb_pointer_t priv)
{
	// check
	tb_astream_filter_t* fstream = tb_astream_filter_cast(astream);
	tb_assert_and_check_return_val(fstream && fstream->astream && func, tb_false);

	// save func and priv
	fstream->priv 		= priv;
	fstream->func.open 	= func;

	// post open
	return tb_astream_open(fstream->astream, tb_astream_filter_open_func, astream);
}
static tb_bool_t tb_astream_filter_read_func(tb_astream_t* astream, tb_size_t state, tb_byte_t const* data, tb_size_t real, tb_size_t size, tb_pointer_t priv)
{
	// check
	tb_assert_and_check_return_val(astream && data, tb_false);

	// the stream
	tb_astream_filter_t* fstream = (tb_astream_filter_t*)priv;
	tb_assert_and_check_return_val(fstream && fstream->func.read, tb_false);

	// done filter
	tb_bool_t ok = tb_false;
	if (fstream->filter)
	{
		// spak the filter
		tb_long_t spak = tb_filter_spak(fstream->filter, data, real, &data, size, state == TB_ASTREAM_STATE_CLOSED? -1 : 0);
		
		// has data?
		if (spak > 0 && data)
		{
			// done data
			ok = fstream->func.read((tb_astream_t*)fstream, TB_ASTREAM_STATE_OK, data, spak, size, fstream->priv);

			// continue and done it if closed or failed
			if (ok && state != TB_ASTREAM_STATE_OK)
			{
				// done closed or failed
				fstream->func.read((tb_astream_t*)fstream, state, tb_null, 0, size, fstream->priv);

				// break it
				// ok = tb_false;
			}
		}
		// no data? continue it
		else if (!spak) ok = tb_true;
		// end or error?
		else
		{
			// done closed or failed
			fstream->func.read((tb_astream_t*)fstream, state == TB_ASTREAM_STATE_OK? TB_ASTREAM_STATE_CLOSED : state, tb_null, 0, size, fstream->priv);

			// break it
			// ok = tb_false;
		}
	}
	// done func
	else ok = fstream->func.read((tb_astream_t*)fstream, state, data, real, size, fstream->priv);
 
	// ok?
	return ok;
}
static tb_bool_t tb_astream_filter_read(tb_astream_t* astream, tb_size_t delay, tb_size_t maxn, tb_astream_read_func_t func, tb_pointer_t priv)
{
	// check
	tb_astream_filter_t* fstream = tb_astream_filter_cast(astream);
	tb_assert_and_check_return_val(fstream && fstream->astream && func, tb_false);

	// save func and priv
	fstream->priv 		= priv;
	fstream->func.read 	= func;

	// post read
	return tb_astream_read_after(fstream->astream, delay, maxn, tb_astream_filter_read_func, astream);
}
static tb_bool_t tb_astream_filter_writ_func(tb_astream_t* astream, tb_size_t state, tb_byte_t const* data, tb_size_t real, tb_size_t size, tb_pointer_t priv)
{
	// check
	tb_assert_and_check_return_val(astream && data && size, tb_false);

	// the stream
	tb_astream_filter_t* fstream = (tb_astream_filter_t*)priv;
	tb_assert_and_check_return_val(fstream && fstream->func.writ, tb_false);

	// done func
	return fstream->func.writ((tb_astream_t*)fstream, state, data, real, size, fstream->priv);
}
static tb_bool_t tb_astream_filter_writ(tb_astream_t* astream, tb_size_t delay, tb_byte_t const* data, tb_size_t size, tb_astream_writ_func_t func, tb_pointer_t priv)
{
	// check
	tb_astream_filter_t* fstream = tb_astream_filter_cast(astream);
	tb_assert_and_check_return_val(fstream && fstream->astream && data && size && func, tb_false);

	// save func and priv
	fstream->priv 		= priv;
	fstream->func.writ 	= func;

	// done filter
	tb_bool_t ok = tb_true;
	if (fstream->filter)
	{
		// spak the filter
		tb_long_t real = tb_filter_spak(fstream->filter, data, size, &data, 0, 0);
		
		// has data? post writ
		if (real > 0 && data)
			ok = tb_astream_writ_after(fstream->astream, delay, data, real, tb_astream_filter_writ_func, astream);
	}
	// post writ
	else ok = tb_astream_writ_after(fstream->astream, delay, data, size, tb_astream_filter_writ_func, astream);

	// ok?
	return ok;
}
static tb_bool_t tb_astream_filter_sync_func(tb_astream_t* astream, tb_size_t state, tb_pointer_t priv)
{
	// check
	tb_assert_and_check_return_val(astream, tb_false);

	// the stream
	tb_astream_filter_t* fstream = (tb_astream_filter_t*)priv;
	tb_assert_and_check_return_val(fstream && fstream->func.sync, tb_false);

	// done func
	return fstream->func.sync((tb_astream_t*)fstream, state, fstream->priv);
}
static tb_bool_t tb_astream_filter_writ_sync_func(tb_astream_t* astream, tb_size_t state, tb_byte_t const* data, tb_size_t real, tb_size_t size, tb_pointer_t priv)
{
	// check
	tb_assert_and_check_return_val(astream && data && size, tb_false);

	// the stream
	tb_astream_filter_t* fstream = (tb_astream_filter_t*)priv;
	tb_assert_and_check_return_val(fstream && fstream->func.sync, tb_false);

	// post sync
	tb_bool_t ok = tb_astream_sync(fstream->astream, tb_true, tb_astream_filter_sync_func, astream);

	// failed? done func
	if (!ok) ok = fstream->func.sync((tb_astream_t*)fstream, TB_ASTREAM_STATE_UNKNOWN_ERROR, fstream->priv);

	// ok?
	return ok;
}
static tb_bool_t tb_astream_filter_sync(tb_astream_t* astream, tb_bool_t bclosing, tb_astream_sync_func_t func, tb_pointer_t priv)
{
	// check
	tb_astream_filter_t* fstream = tb_astream_filter_cast(astream);
	tb_assert_and_check_return_val(fstream && fstream->astream && func, tb_false);

	// save func and priv
	fstream->priv 		= priv;
	fstream->func.sync 	= func;

	// done filter
	tb_bool_t ok = tb_true;
	if (fstream->filter)
	{
		// spak the filter
		tb_byte_t const* 	data = tb_null;
		tb_long_t 			real = tb_filter_spak(fstream->filter, tb_null, 0, &data, 0, bclosing? -1 : 1);
		
		// has data? post writ and sync
		if (real > 0 && data)
			ok = tb_astream_writ(fstream->astream, data, real, tb_astream_filter_writ_sync_func, astream);
	}
	// post sync
	else ok = tb_astream_sync(fstream->astream, bclosing, tb_astream_filter_sync_func, astream);

	// ok?
	return ok;
}
static tb_void_t tb_astream_filter_kill(tb_astream_t* astream)
{	
	// check
	tb_astream_filter_t* fstream = tb_astream_filter_cast(astream);
	tb_assert_and_check_return(fstream);

	// kill it
	if (fstream->astream) tb_astream_kill(fstream->astream);
}
static tb_void_t tb_astream_filter_clos(tb_astream_t* astream, tb_bool_t bcalling)
{	
	// check
	tb_astream_filter_t* fstream = tb_astream_filter_cast(astream);
	tb_assert_and_check_return(fstream);

	// clear the filter
	if (fstream->filter) tb_filter_cler(fstream->filter);
}
static tb_void_t tb_astream_filter_exit(tb_astream_t* astream, tb_bool_t bcalling)
{	
	// check
	tb_astream_filter_t* fstream = tb_astream_filter_cast(astream);
	tb_assert_and_check_return(fstream);

	// exit it
	if (!fstream->bref && fstream->filter) tb_filter_exit(fstream->filter);
	fstream->filter = tb_null;
	fstream->bref = tb_false;
}
static tb_bool_t tb_astream_filter_ctrl(tb_astream_t* astream, tb_size_t ctrl, tb_va_list_t args)
{
	// check
	tb_astream_filter_t* fstream = tb_astream_filter_cast(astream);
	tb_assert_and_check_return_val(fstream, tb_false);

	// ctrl
	switch (ctrl)
	{
	case TB_ASTREAM_CTRL_FLTR_SET_ASTREAM:
		{
			// check
			tb_assert_and_check_return_val(!tb_atomic_get(&astream->opened), tb_false);

			// set astream
			tb_astream_t* astream = (tb_astream_t*)tb_va_arg(args, tb_astream_t*);
			fstream->astream = astream;
			return tb_true;
		}
	case TB_ASTREAM_CTRL_FLTR_GET_ASTREAM:
		{
			// get astream
			tb_astream_t** pastream = (tb_astream_t**)tb_va_arg(args, tb_astream_t**);
			tb_assert_and_check_return_val(pastream, tb_false);
			*pastream = fstream->astream;
			return tb_true;
		}
	case TB_ASTREAM_CTRL_FLTR_SET_FILTER:
		{
			// check
			tb_assert_and_check_return_val(!tb_atomic_get(&astream->opened), tb_false);

			// TODO: exit filter first if exists
//			if (!fstream->bref && fstream->filter) tb_filter_exit(fstream->filter);

			// set filter
			tb_filter_t* filter = (tb_filter_t*)tb_va_arg(args, tb_filter_t*);
			fstream->filter = filter;
			fstream->bref = filter? tb_true : tb_false;
			return tb_true;
		}
	case TB_ASTREAM_CTRL_FLTR_GET_FILTER:
		{
			// get filter
			tb_filter_t** phandle = (tb_filter_t**)tb_va_arg(args, tb_filter_t**);
			tb_assert_and_check_return_val(phandle, tb_false);
			*phandle = fstream->filter;
			return tb_true;
		}
	default:
		break;
	}
	return tb_false;
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_astream_t* tb_astream_init_filter(tb_aicp_t* aicp)
{
	// check
	tb_assert_and_check_return_val(aicp, tb_null);

	// make stream
	tb_astream_filter_t* fstream = (tb_astream_filter_t*)tb_malloc0(sizeof(tb_astream_filter_t));
	tb_assert_and_check_return_val(fstream, tb_null);

	// init stream
	if (!tb_astream_init((tb_astream_t*)fstream, aicp, TB_ASTREAM_TYPE_FLTR)) goto fail;
	fstream->base.open 		= tb_astream_filter_open;
	fstream->base.read 		= tb_astream_filter_read;
	fstream->base.writ 		= tb_astream_filter_writ;
	fstream->base.sync 		= tb_astream_filter_sync;
	fstream->base.kill 		= tb_astream_filter_kill;
	fstream->base.clos 		= tb_astream_filter_clos;
	fstream->base.exit 		= tb_astream_filter_exit;
	fstream->base.ctrl 		= tb_astream_filter_ctrl;

	// ok
	return (tb_astream_t*)fstream;

fail:
	if (fstream) tb_astream_exit((tb_astream_t*)fstream, tb_false);
	return tb_null;
}
tb_astream_t* tb_astream_init_filter_from_null(tb_astream_t* astream)
{
	// check
	tb_assert_and_check_return_val(astream, tb_null);

	// the aicp
	tb_aicp_t* aicp = tb_astream_aicp(astream);
	tb_assert_and_check_return_val(aicp, tb_null);

	// init filter stream
	tb_astream_t* fstream = tb_astream_init_filter(aicp);
	tb_assert_and_check_return_val(fstream, tb_null);

	// set astream
	if (!tb_astream_ctrl(fstream, TB_ASTREAM_CTRL_FLTR_SET_ASTREAM, astream)) goto fail;

	// ok
	return fstream;
fail:
	if (fstream) tb_astream_exit(fstream, tb_false);
	return tb_null;
}

tb_astream_t* tb_astream_init_filter_from_zip(tb_astream_t* astream, tb_size_t algo, tb_size_t action)
{
	// check
	tb_assert_and_check_return_val(astream, tb_null);

	// the aicp
	tb_aicp_t* aicp = tb_astream_aicp(astream);
	tb_assert_and_check_return_val(aicp, tb_null);

	// init filter stream
	tb_astream_t* fstream = tb_astream_init_filter(aicp);
	tb_assert_and_check_return_val(fstream, tb_null);

	// set astream
	if (!tb_astream_ctrl(fstream, TB_ASTREAM_CTRL_FLTR_SET_ASTREAM, astream)) goto fail;

	// set filter
	((tb_astream_filter_t*)fstream)->bref = tb_false;
	((tb_astream_filter_t*)fstream)->filter = tb_filter_init_from_zip(algo, action);
	tb_assert_and_check_goto(((tb_astream_filter_t*)fstream)->filter, fail);
	
	// ok
	return fstream;
fail:
	if (fstream) tb_astream_exit(fstream, tb_false);
	return tb_null;
}
tb_astream_t* tb_astream_init_filter_from_charset(tb_astream_t* astream, tb_size_t fr, tb_size_t to)
{
	// check
	tb_assert_and_check_return_val(astream, tb_null);

	// the aicp
	tb_aicp_t* aicp = tb_astream_aicp(astream);
	tb_assert_and_check_return_val(aicp, tb_null);

	// init filter stream
	tb_astream_t* fstream = tb_astream_init_filter(aicp);
	tb_assert_and_check_return_val(fstream, tb_null);

	// set astream
	if (!tb_astream_ctrl(fstream, TB_ASTREAM_CTRL_FLTR_SET_ASTREAM, astream)) goto fail;

	// set filter
	((tb_astream_filter_t*)fstream)->bref = tb_false;
	((tb_astream_filter_t*)fstream)->filter = tb_filter_init_from_charset(fr, to);
	tb_assert_and_check_goto(((tb_astream_filter_t*)fstream)->filter, fail);
	
	// ok
	return fstream;
fail:
	if (fstream) tb_astream_exit(fstream, tb_false);
	return tb_null;
}
tb_astream_t* tb_astream_init_filter_from_chunked(tb_astream_t* astream, tb_bool_t dechunked)
{
	// check
	tb_assert_and_check_return_val(astream, tb_null);

	// the aicp
	tb_aicp_t* aicp = tb_astream_aicp(astream);
	tb_assert_and_check_return_val(aicp, tb_null);

	// init filter stream
	tb_astream_t* fstream = tb_astream_init_filter(aicp);
	tb_assert_and_check_return_val(fstream, tb_null);

	// set astream
	if (!tb_astream_ctrl(fstream, TB_ASTREAM_CTRL_FLTR_SET_ASTREAM, astream)) goto fail;

	// set filter
	((tb_astream_filter_t*)fstream)->bref = tb_false;
	((tb_astream_filter_t*)fstream)->filter = tb_filter_init_from_chunked(dechunked);
	tb_assert_and_check_goto(((tb_astream_filter_t*)fstream)->filter, fail);
	
	// ok
	return fstream;
fail:
	if (fstream) tb_astream_exit(fstream, tb_false);
	return tb_null;
}
