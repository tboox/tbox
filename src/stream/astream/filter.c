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
 * @file		filter.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
//#define TB_TRACE_MODULE_NAME 				"afilter"

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

	// the filter 
	tb_filter_t* 				filter;

	// the filter is referenced? need not exit it
	tb_uint32_t 				bref 	: 1;

	// is reading now?
	tb_uint32_t 				bread 	: 1;

	// the astream
	tb_astream_t* 				astream;

	// the read maxn
	tb_size_t 					maxn;

	// the offset
	tb_atomic64_t 				offset;

	// the func
	union
	{
		tb_astream_open_func_t 	open;
		tb_astream_read_func_t 	read;
		tb_astream_writ_func_t 	writ;
		tb_astream_sync_func_t 	sync;
		tb_astream_task_func_t 	task;

	} 							func;

	// the priv
	tb_pointer_t 				priv;

}tb_astream_filter_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_astream_filter_t* tb_astream_filter_cast(tb_handle_t stream)
{
	tb_astream_t* astream = (tb_astream_t*)stream;
	tb_assert_and_check_return_val(astream && astream->base.type == TB_STREAM_TYPE_FLTR, tb_null);
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
	tb_atomic_set(&fstream->base.base.bopened, 1);

	// done func
	return fstream->func.open((tb_astream_t*)fstream, state, fstream->priv);
}
static tb_bool_t tb_astream_filter_open(tb_handle_t astream, tb_astream_open_func_t func, tb_pointer_t priv)
{
	// check
	tb_astream_filter_t* fstream = tb_astream_filter_cast(astream);
	tb_assert_and_check_return_val(fstream && fstream->astream, tb_false);

	// clear the mode
	fstream->bread = 0;

	// clear the offset
	tb_atomic64_set0(&fstream->offset);

	// have been opened?
	if (tb_stream_is_opened(fstream->astream)) 
	{
		// opened
		tb_atomic_set(&fstream->base.base.bopened, 1);

		// done func
		return func? func(astream, TB_STREAM_STATE_OK, fstream->priv) : tb_true;
	}

	// check
	tb_assert_and_check_return_val(func, tb_false);

	// save func and priv
	fstream->priv 		= priv;
	fstream->func.open 	= func;

	// post open
	return tb_astream_open(fstream->astream, tb_astream_filter_open_func, astream);
}
static tb_bool_t tb_astream_filter_sync_read_func(tb_astream_t* astream, tb_size_t state, tb_pointer_t priv)
{
	// check
	tb_assert_and_check_return_val(astream, tb_false);

	// the stream
	tb_astream_filter_t* fstream = (tb_astream_filter_t*)priv;
	tb_assert_and_check_return_val(fstream && fstream->func.read, tb_false);

	// spak the filter
	tb_byte_t const* 	data = tb_null;
	tb_long_t 			spak = tb_filter_spak(fstream->filter, tb_null, 0, &data, fstream->maxn, -1);
	
	// has output data?
	tb_bool_t ok = tb_false;
	if (spak > 0 && data)
	{	
		// save offset
		tb_atomic64_fetch_and_add(&fstream->offset, spak);

		// done data
		ok = fstream->func.read((tb_astream_t*)fstream, TB_STREAM_STATE_OK, data, spak, fstream->maxn, fstream->priv);
	}
	// closed?
	else
	{
		// done closed
		fstream->func.read((tb_astream_t*)fstream, TB_STREAM_STATE_CLOSED, tb_null, 0, fstream->maxn, fstream->priv);

		// break it
		// ok = tb_false;
	}

	// ok?
	return ok;
}
static tb_bool_t tb_astream_filter_read_func(tb_astream_t* astream, tb_size_t state, tb_byte_t const* data, tb_size_t real, tb_size_t size, tb_pointer_t priv)
{
	// check
	tb_assert_and_check_return_val(astream, tb_false);

	// the stream
	tb_astream_filter_t* fstream = (tb_astream_filter_t*)priv;
	tb_assert_and_check_return_val(fstream && fstream->func.read, tb_false);

	// done filter
	tb_bool_t ok = tb_false;
	if (fstream->filter)
	{
		// done filter
		switch (state)
		{
		case TB_STREAM_STATE_OK:
			{
				// spak the filter
				tb_long_t spak = tb_filter_spak(fstream->filter, data, real, &data, size, 0);
				
				// has output data?
				if (spak > 0 && data)
				{
					// save offset
					tb_atomic64_fetch_and_add(&fstream->offset, spak);

					// done data
					ok = fstream->func.read((tb_astream_t*)fstream, TB_STREAM_STATE_OK, data, spak, size, fstream->priv);
				}
				// no data? continue it
				else if (!spak) ok = tb_true;
				// closed?
				else
				{
					// done closed
					fstream->func.read((tb_astream_t*)fstream, TB_STREAM_STATE_CLOSED, tb_null, 0, size, fstream->priv);

					// break it
					// ok = tb_false;
				}

				// eof and continue?
				if (tb_filter_beof(fstream->filter) && ok)
				{
					// done sync for reading
					ok = tb_astream_task(fstream->astream, 0, tb_astream_filter_sync_read_func, fstream);
					
					// error? done error
					if (!ok) fstream->func.read((tb_astream_t*)fstream, TB_STREAM_STATE_UNKNOWN_ERROR, tb_null, 0, size, fstream->priv);

					// need not read data, break it
					ok = tb_false;
				}
			}
			break;
		case TB_STREAM_STATE_CLOSED:
			{
				// done sync for reading
				ok = tb_astream_task(fstream->astream, 0, tb_astream_filter_sync_read_func, fstream);
				
				// error? done error
				if (!ok) fstream->func.read((tb_astream_t*)fstream, TB_STREAM_STATE_UNKNOWN_ERROR, tb_null, 0, size, fstream->priv);
			}
			break;
		default:
			{
				// done closed or failed
				fstream->func.read((tb_astream_t*)fstream, state, tb_null, 0, size, fstream->priv);

				// break it
				// ok = tb_false;
			}
			break;
		}
	}
	// done func
	else ok = fstream->func.read((tb_astream_t*)fstream, state, data, real, size, fstream->priv);
 
	// ok?
	return ok;
}
static tb_bool_t tb_astream_filter_read(tb_handle_t astream, tb_size_t delay, tb_size_t maxn, tb_astream_read_func_t func, tb_pointer_t priv)
{
	// check
	tb_astream_filter_t* fstream = tb_astream_filter_cast(astream);
	tb_assert_and_check_return_val(fstream && fstream->astream && func, tb_false);

	// clear the offset if be writ mode now
	if (!fstream->bread) tb_atomic64_set0(&fstream->offset);

	// set read mode
	fstream->bread = 1;

	// save func and priv
	fstream->priv 		= priv;
	fstream->func.read 	= func;
	fstream->maxn 		= maxn;

	// filter eof? flush the left data
	if (fstream->filter && tb_filter_beof(fstream->filter))
		return tb_astream_task(fstream->astream, 0, tb_astream_filter_sync_read_func, fstream);

	// post read
	return tb_astream_read_after(fstream->astream, delay, maxn, tb_astream_filter_read_func, astream);
}
static tb_bool_t tb_astream_filter_writ_func(tb_astream_t* astream, tb_size_t state, tb_byte_t const* data, tb_size_t real, tb_size_t size, tb_pointer_t priv)
{
	// check
	tb_assert_and_check_return_val(astream, tb_false);

	// the stream
	tb_astream_filter_t* fstream = (tb_astream_filter_t*)priv;
	tb_assert_and_check_return_val(fstream && fstream->func.writ, tb_false);

	// save offset
	if (real) tb_atomic64_fetch_and_add(&fstream->offset, real);

	// done func
	return fstream->func.writ((tb_astream_t*)fstream, state, data, real, size, fstream->priv);
}
static tb_bool_t tb_astream_filter_writ(tb_handle_t astream, tb_size_t delay, tb_byte_t const* data, tb_size_t size, tb_astream_writ_func_t func, tb_pointer_t priv)
{
	// check
	tb_astream_filter_t* fstream = tb_astream_filter_cast(astream);
	tb_assert_and_check_return_val(fstream && fstream->astream && data && size && func, tb_false);

	// clear the offset if be read mode now
	if (fstream->bread) tb_atomic64_set0(&fstream->offset);

	// set writ mode
	fstream->bread = 0;

	// save func and priv
	fstream->priv 		= priv;
	fstream->func.writ 	= func;

	// done filter
	tb_bool_t ok = tb_true;
	if (fstream->filter)
	{
		// spak the filter
		tb_long_t real = tb_filter_spak(fstream->filter, data, size, &data, size, 0);
		
		// has data? 
		if (real > 0 && data)
		{
			// post writ
			ok = tb_astream_writ_after(fstream->astream, delay, data, real, tb_astream_filter_writ_func, astream);
		}
		// no data or end? continue to writ or sync
		else
		{
			// done func, no data and finished
			ok = func((tb_astream_t*)fstream, TB_STREAM_STATE_OK, tb_null, 0, 0, fstream->priv);
		}
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

	// not finished? continue it
	if (state == TB_STREAM_STATE_OK && real < size) return tb_true;

	// save offset
	if (real) tb_atomic64_fetch_and_add(&fstream->offset, real);

	// post sync
	tb_bool_t ok = tb_astream_sync(fstream->astream, tb_true, tb_astream_filter_sync_func, fstream);

	// failed? done func
	if (!ok) ok = fstream->func.sync((tb_astream_t*)fstream, TB_STREAM_STATE_UNKNOWN_ERROR, fstream->priv);

	// ok?
	return ok;
}
static tb_bool_t tb_astream_filter_sync(tb_handle_t astream, tb_bool_t bclosing, tb_astream_sync_func_t func, tb_pointer_t priv)
{
	// check
	tb_astream_filter_t* fstream = tb_astream_filter_cast(astream);
	tb_assert_and_check_return_val(fstream && fstream->astream && func, tb_false);

	// clear the offset if be read mode now
	if (fstream->bread) tb_atomic64_set0(&fstream->offset);

	// set writ mode
	fstream->bread = 0;

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
static tb_bool_t tb_astream_filter_task_func(tb_astream_t* astream, tb_size_t state, tb_pointer_t priv)
{
	// check
	tb_assert_and_check_return_val(astream, tb_false);

	// the stream
	tb_astream_filter_t* fstream = (tb_astream_filter_t*)priv;
	tb_assert_and_check_return_val(fstream && fstream->func.task, tb_false);

	// done func
	return fstream->func.task((tb_astream_t*)fstream, state, fstream->priv);
}
static tb_bool_t tb_astream_filter_task(tb_handle_t astream, tb_size_t delay, tb_astream_task_func_t func, tb_pointer_t priv)
{
	// check
	tb_astream_filter_t* fstream = tb_astream_filter_cast(astream);
	tb_assert_and_check_return_val(fstream && fstream->astream && func, tb_false);

	// save func and priv
	fstream->priv 		= priv;
	fstream->func.task 	= func;

	// post task
	return tb_astream_task(fstream->astream, delay, tb_astream_filter_task_func, astream);
}
static tb_void_t tb_astream_filter_kill(tb_handle_t astream)
{	
	// check
	tb_astream_filter_t* fstream = tb_astream_filter_cast(astream);
	tb_assert_and_check_return(fstream);

	// kill stream
	if (fstream->astream) tb_stream_kill(fstream->astream);
}
static tb_void_t tb_astream_filter_clos(tb_handle_t astream, tb_bool_t bcalling)
{	
	// check
	tb_astream_filter_t* fstream = tb_astream_filter_cast(astream);
	tb_assert_and_check_return(fstream);

	// close stream
	if (fstream->astream) tb_astream_clos(fstream->astream, bcalling);

	// clear the filter
	if (fstream->filter) tb_filter_cler(fstream->filter);

	// clear the mode
	fstream->bread = 0;

	// clear the offset
	tb_atomic64_set0(&fstream->offset);
}
static tb_void_t tb_astream_filter_exit(tb_handle_t astream, tb_bool_t bcalling)
{	
	// check
	tb_astream_filter_t* fstream = tb_astream_filter_cast(astream);
	tb_assert_and_check_return(fstream);

	// exit it
	if (!fstream->bref && fstream->filter) tb_filter_exit(fstream->filter);
	fstream->filter = tb_null;
	fstream->bref = 0;
}
static tb_bool_t tb_astream_filter_ctrl(tb_handle_t astream, tb_size_t ctrl, tb_va_list_t args)
{
	// check
	tb_astream_filter_t* fstream = tb_astream_filter_cast(astream);
	tb_assert_and_check_return_val(fstream, tb_false);

	// ctrl
	switch (ctrl)
	{
	case TB_STREAM_CTRL_GET_OFFSET:
		{
			// check
			tb_assert_and_check_return_val(tb_stream_is_opened(astream), tb_false);

			// get offset
			tb_hize_t* poffset = (tb_hize_t*)tb_va_arg(args, tb_hize_t*);
			tb_assert_and_check_return_val(poffset, tb_false);
			*poffset = (tb_hize_t)tb_atomic64_get(&fstream->offset);
			return tb_true;
		}
	case TB_STREAM_CTRL_FLTR_SET_STREAM:
		{
			// check
			tb_assert_and_check_return_val(!tb_stream_is_opened(astream), tb_false);

			// set astream
			tb_handle_t astream = (tb_astream_t*)tb_va_arg(args, tb_astream_t*);
			fstream->astream = astream;
			return tb_true;
		}
	case TB_STREAM_CTRL_FLTR_GET_STREAM:
		{
			// get astream
			tb_astream_t** pastream = (tb_astream_t**)tb_va_arg(args, tb_astream_t**);
			tb_assert_and_check_return_val(pastream, tb_false);
			*pastream = fstream->astream;
			return tb_true;
		}
	case TB_STREAM_CTRL_FLTR_SET_FILTER:
		{
			// check
			tb_assert_and_check_return_val(!tb_stream_is_opened(astream), tb_false);

			//  exit filter first if exists
			if (!fstream->bref && fstream->filter) tb_filter_exit(fstream->filter);

			// set filter
			tb_filter_t* filter = (tb_filter_t*)tb_va_arg(args, tb_filter_t*);
			fstream->filter = filter;
			fstream->bref = filter? 1 : 0;
			return tb_true;
		}
	case TB_STREAM_CTRL_FLTR_GET_FILTER:
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
	if (!tb_astream_init((tb_astream_t*)fstream, aicp, TB_STREAM_TYPE_FLTR, 0)) goto fail;
	fstream->base.open 		= tb_astream_filter_open;
	fstream->base.read 		= tb_astream_filter_read;
	fstream->base.writ 		= tb_astream_filter_writ;
	fstream->base.sync 		= tb_astream_filter_sync;
	fstream->base.task 		= tb_astream_filter_task;
	fstream->base.kill 		= tb_astream_filter_kill;
	fstream->base.clos 		= tb_astream_filter_clos;
	fstream->base.exit 		= tb_astream_filter_exit;
	fstream->base.base.ctrl = tb_astream_filter_ctrl;

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
	if (!tb_stream_ctrl(fstream, TB_STREAM_CTRL_FLTR_SET_STREAM, astream)) goto fail;

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
	if (!tb_stream_ctrl(fstream, TB_STREAM_CTRL_FLTR_SET_STREAM, astream)) goto fail;

	// set filter
	((tb_astream_filter_t*)fstream)->bref = 0;
	((tb_astream_filter_t*)fstream)->filter = tb_filter_init_from_zip(algo, action);
	tb_assert_and_check_goto(((tb_astream_filter_t*)fstream)->filter, fail);
	
	// ok
	return fstream;
fail:
	if (fstream) tb_astream_exit(fstream, tb_false);
	return tb_null;
}
tb_astream_t* tb_astream_init_filter_from_cache(tb_astream_t* astream, tb_size_t size)
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
	if (!tb_stream_ctrl(fstream, TB_STREAM_CTRL_FLTR_SET_STREAM, astream)) goto fail;

	// set filter
	((tb_astream_filter_t*)fstream)->bref = 0;
	((tb_astream_filter_t*)fstream)->filter = tb_filter_init_from_cache(size);
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
	if (!tb_stream_ctrl(fstream, TB_STREAM_CTRL_FLTR_SET_STREAM, astream)) goto fail;

	// set filter
	((tb_astream_filter_t*)fstream)->bref = 0;
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
	if (!tb_stream_ctrl(fstream, TB_STREAM_CTRL_FLTR_SET_STREAM, astream)) goto fail;

	// set filter
	((tb_astream_filter_t*)fstream)->bref = 0;
	((tb_astream_filter_t*)fstream)->filter = tb_filter_init_from_chunked(dechunked);
	tb_assert_and_check_goto(((tb_astream_filter_t*)fstream)->filter, fail);
	
	// ok
	return fstream;
fail:
	if (fstream) tb_astream_exit(fstream, tb_false);
	return tb_null;
}
