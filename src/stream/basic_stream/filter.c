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

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../stream.h"
#include "../../filter/filter.h"
#include "../../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the filter stream type
typedef struct __tb_basic_stream_filter_t
{
	// the base
	tb_basic_stream_t 		base;

	// the filter 
	tb_filter_t* 		filter;

	// the filter is referenced? need not exit it
	tb_bool_t 			bref;

	// is eof?
	tb_bool_t 			beof;

	// is wait?
	tb_bool_t 			wait;

	// the last
	tb_long_t 			last;

	// the mode, none: 0, read: 1, writ: -1
	tb_long_t 			mode;

	// the bstream
	tb_basic_stream_t* 		bstream;

}tb_basic_stream_filter_t;
 
/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_basic_stream_filter_t* tb_basic_stream_filter_cast(tb_handle_t stream)
{
	tb_basic_stream_t* bstream = (tb_basic_stream_t*)stream;
	tb_assert_and_check_return_val(bstream && bstream->base.type == TB_STREAM_TYPE_FLTR, tb_null);
	return (tb_basic_stream_filter_t*)bstream;
}
static tb_bool_t tb_basic_stream_filter_open(tb_handle_t bstream)
{
	// check
	tb_basic_stream_filter_t* fstream = tb_basic_stream_filter_cast(bstream);
	tb_assert_and_check_return_val(fstream && fstream->bstream, tb_false);

	// clear mode
	fstream->mode = 0;

	// clear last
	fstream->last = 0;

	// clear wait
	fstream->wait = tb_false;

	// clear eof
	fstream->beof = tb_false;

	// ok
	return tb_basic_stream_open(fstream->bstream);
}
static tb_bool_t tb_basic_stream_filter_clos(tb_handle_t bstream)
{
	// check
	tb_basic_stream_filter_t* fstream = tb_basic_stream_filter_cast(bstream);
	tb_assert_and_check_return_val(fstream && fstream->bstream, tb_false);
	
	// sync the end filter data
	if (fstream->filter && fstream->mode == -1)
	{
		// spak data
		tb_byte_t const* 	data = tb_null;
		tb_long_t 			size = tb_filter_spak(fstream->filter, tb_null, 0, &data, 0, -1);
		if (size > 0 && data)
		{
			// writ data
			if (!tb_basic_stream_bwrit(fstream->bstream, data, size)) return tb_false;
		}
	}

	// done
	tb_bool_t ok = tb_basic_stream_clos(fstream->bstream);

	// ok?
	if (ok) 
	{
		// clear mode
		fstream->mode = 0;

		// clear last
		fstream->last = 0;

		// clear wait
		fstream->wait = tb_false;

		// clear eof
		fstream->beof = tb_false;

		// clear the filter
		if (fstream->filter) tb_filter_cler(fstream->filter);
	}

	// ok?
	return ok;
}
static tb_void_t tb_basic_stream_filter_exit(tb_handle_t bstream)
{	
	// check
	tb_basic_stream_filter_t* fstream = tb_basic_stream_filter_cast(bstream);
	tb_assert_and_check_return(fstream);

	// exit it
	if (!fstream->bref && fstream->filter) tb_filter_exit(fstream->filter);
	fstream->filter = tb_null;
	fstream->bref = tb_false;
}
static tb_void_t tb_basic_stream_filter_kill(tb_handle_t bstream)
{	
	// check
	tb_basic_stream_filter_t* fstream = tb_basic_stream_filter_cast(bstream);
	tb_assert_and_check_return(fstream);

	// kill it
	if (fstream->bstream) tb_stream_kill(fstream->bstream);
}
static tb_long_t tb_basic_stream_filter_read(tb_handle_t bstream, tb_byte_t* data, tb_size_t size)
{
	// check
	tb_basic_stream_filter_t* fstream = tb_basic_stream_filter_cast(bstream);
	tb_assert_and_check_return_val(fstream && fstream->bstream, -1);

	// read 
	tb_long_t real = tb_basic_stream_aread(fstream->bstream, data, size);

	// done filter
	if (fstream->filter)
	{
		// save mode: read
		if (!fstream->mode) fstream->mode = 1;

		// check mode
		tb_assert_and_check_return_val(fstream->mode == 1, -1);

		// save last
		fstream->last = real;

		// eof?
		if (real < 0 || (!real && fstream->wait) || tb_filter_beof(fstream->filter))
			fstream->beof = tb_true;
		// clear wait
		else if (real > 0) fstream->wait = tb_false;

		// spak data
		tb_byte_t const* odata = tb_null;
		if (real) real = tb_filter_spak(fstream->filter, data, real < 0? 0 : real, &odata, size, fstream->beof? -1 : 0);
		// no data? try to sync it
		if (!real) real = tb_filter_spak(fstream->filter, tb_null, 0, &odata, size, fstream->beof? -1 : 1);

		// has data? save it
		if (real > 0 && odata) tb_memcpy(data, odata, real);

		// eof?
		if (fstream->beof && !real) real = -1;
	}

	// ok? 
	return real;
}
static tb_long_t tb_basic_stream_filter_writ(tb_handle_t bstream, tb_byte_t const* data, tb_size_t size)
{
	// check
	tb_basic_stream_filter_t* fstream = tb_basic_stream_filter_cast(bstream);
	tb_assert_and_check_return_val(fstream && fstream->bstream, -1);

	// done filter
	if (fstream->filter && data && size)
	{
		// save mode: writ
		if (!fstream->mode) fstream->mode = -1;

		// check mode
		tb_assert_and_check_return_val(fstream->mode == -1, -1);

		// spak data
		tb_long_t real = tb_filter_spak(fstream->filter, data, size, &data, size, 0);
		tb_assert_and_check_return_val(real >= 0, -1);

		// no data?
		tb_check_return_val(real, 0);

		// save size
		size = real;
	}

	// writ 
	return tb_basic_stream_awrit(fstream->bstream, data, size);
}
static tb_bool_t tb_basic_stream_filter_sync(tb_handle_t bstream, tb_bool_t bclosing)
{
	// check
	tb_basic_stream_filter_t* fstream = tb_basic_stream_filter_cast(bstream);
	tb_assert_and_check_return_val(fstream && fstream->bstream, tb_false);

	// done filter
	if (fstream->filter)
	{
		// save mode: writ
		if (!fstream->mode) fstream->mode = -1;

		// check mode
		tb_assert_and_check_return_val(fstream->mode == -1, tb_false);

		// spak data
		tb_byte_t const* 	data = tb_null;
		tb_long_t 			real = -1;
		while ( !tb_atomic_get(&fstream->base.base.bstoped)
			&& 	(real = tb_filter_spak(fstream->filter, tb_null, 0, &data, 0, bclosing? -1 : 1)) > 0
			&& 	data)
		{
			if (!tb_basic_stream_bwrit(fstream->bstream, data, real)) return tb_false;
		}
	}

	// writ 
	return tb_basic_stream_sync(fstream->bstream, bclosing);
}
static tb_long_t tb_basic_stream_filter_wait(tb_handle_t bstream, tb_size_t wait, tb_long_t timeout)
{
	// check
	tb_basic_stream_filter_t* fstream = tb_basic_stream_filter_cast(bstream);
	tb_assert_and_check_return_val(fstream && fstream->bstream, -1);

	// done
	tb_long_t ok = -1;
	if (fstream->filter && fstream->mode == 1)
	{
		// wait ok
		if (fstream->last > 0) ok = wait;
		// need wait
		else if (!fstream->last && !fstream->beof && !tb_filter_beof(fstream->filter))
		{
			// wait
			ok = tb_basic_stream_wait(fstream->bstream, wait, timeout);

			// eof?
			if (!ok) 
			{
				// wait ok and continue to read or writ
				ok = wait;

				// set eof
				fstream->beof = tb_true;
			}
			// wait ok
			else fstream->wait = tb_true;
		}
		// eof
		else 
		{	
			// wait ok and continue to read or writ
			ok = wait;

			// set eof
			fstream->beof = tb_true;
		}
	}
	else ok = tb_basic_stream_wait(fstream->bstream, wait, timeout);

	// ok?
	return ok;
}
static tb_bool_t tb_basic_stream_filter_ctrl(tb_handle_t bstream, tb_size_t ctrl, tb_va_list_t args)
{
	// check
	tb_basic_stream_filter_t* fstream = tb_basic_stream_filter_cast(bstream);
	tb_assert_and_check_return_val(fstream, tb_false);

	// ctrl
	switch (ctrl)
	{
	case TB_STREAM_CTRL_GET_OFFSET:
		{
			// the poffset
			tb_hize_t* poffset = (tb_hize_t*)tb_va_arg(args, tb_hize_t*);
			tb_assert_and_check_return_val(poffset, tb_false);

			// get offset
			*poffset = fstream->base.offset;
			return tb_true;
		}
	case TB_STREAM_CTRL_FLTR_SET_STREAM:
		{
			// check
			tb_assert_and_check_return_val(!tb_stream_is_opened(bstream), tb_false);

			// set bstream
			tb_basic_stream_t* bstream = (tb_basic_stream_t*)tb_va_arg(args, tb_basic_stream_t*);
			fstream->bstream = bstream;
			return tb_true;
		}
	case TB_STREAM_CTRL_FLTR_GET_STREAM:
		{
			// get bstream
			tb_basic_stream_t** pbstream = (tb_basic_stream_t**)tb_va_arg(args, tb_basic_stream_t**);
			tb_assert_and_check_return_val(pbstream, tb_false);
			*pbstream = fstream->bstream;
			return tb_true;
		}
	case TB_STREAM_CTRL_FLTR_SET_FILTER:
		{
			// check
			tb_assert_and_check_return_val(!tb_stream_is_opened(bstream), tb_false);

			//  exit filter first if exists
			if (!fstream->bref && fstream->filter) tb_filter_exit(fstream->filter);

			// set filter
			tb_filter_t* filter = (tb_filter_t*)tb_va_arg(args, tb_filter_t*);
			fstream->filter = filter;
			fstream->bref = filter? tb_true : tb_false;
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
/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_basic_stream_t* tb_basic_stream_init_filter()
{
	// make stream
	tb_basic_stream_filter_t* bstream = (tb_basic_stream_filter_t*)tb_malloc0(sizeof(tb_basic_stream_filter_t));
	tb_assert_and_check_return_val(bstream, tb_null);

	// init base
	if (!tb_basic_stream_init((tb_basic_stream_t*)bstream, TB_STREAM_TYPE_FLTR, 0)) goto fail;

	// init stream
	bstream->base.open		= tb_basic_stream_filter_open;
	bstream->base.clos 		= tb_basic_stream_filter_clos;
	bstream->base.exit 		= tb_basic_stream_filter_exit;
	bstream->base.read 		= tb_basic_stream_filter_read;
	bstream->base.writ 		= tb_basic_stream_filter_writ;
	bstream->base.sync 		= tb_basic_stream_filter_sync;
	bstream->base.wait 		= tb_basic_stream_filter_wait;
	bstream->base.base.ctrl = tb_basic_stream_filter_ctrl;
	bstream->base.base.kill = tb_basic_stream_filter_kill;

	// ok
	return (tb_basic_stream_t*)bstream;

fail:
	if (bstream) tb_basic_stream_exit((tb_basic_stream_t*)bstream);
	return tb_null;
}
tb_basic_stream_t* tb_basic_stream_init_filter_from_null(tb_basic_stream_t* bstream)
{
	// check
	tb_assert_and_check_return_val(bstream, tb_null);

	// init filter stream
	tb_basic_stream_t* fstream = tb_basic_stream_init_filter();
	tb_assert_and_check_return_val(fstream, tb_null);

	// set bstream
	if (!tb_stream_ctrl(fstream, TB_STREAM_CTRL_FLTR_SET_STREAM, bstream)) goto fail;

	// ok
	return fstream;
fail:
	if (fstream) tb_basic_stream_exit(fstream);
	return tb_null;
}
tb_basic_stream_t* tb_basic_stream_init_filter_from_zip(tb_basic_stream_t* bstream, tb_size_t algo, tb_size_t action)
{
	// check
	tb_assert_and_check_return_val(bstream, tb_null);

	// init filter stream
	tb_basic_stream_t* fstream = tb_basic_stream_init_filter();
	tb_assert_and_check_return_val(fstream, tb_null);

	// set bstream
	if (!tb_stream_ctrl(fstream, TB_STREAM_CTRL_FLTR_SET_STREAM, bstream)) goto fail;

	// set filter
	((tb_basic_stream_filter_t*)fstream)->bref = tb_false;
	((tb_basic_stream_filter_t*)fstream)->filter = tb_filter_init_from_zip(algo, action);
	tb_assert_and_check_goto(((tb_basic_stream_filter_t*)fstream)->filter, fail);
	
	// ok
	return fstream;
fail:
	if (fstream) tb_basic_stream_exit(fstream);
	return tb_null;
}
tb_basic_stream_t* tb_basic_stream_init_filter_from_cache(tb_basic_stream_t* bstream, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(bstream, tb_null);

	// init filter stream
	tb_basic_stream_t* fstream = tb_basic_stream_init_filter();
	tb_assert_and_check_return_val(fstream, tb_null);

	// set bstream
	if (!tb_stream_ctrl(fstream, TB_STREAM_CTRL_FLTR_SET_STREAM, bstream)) goto fail;

	// set filter
	((tb_basic_stream_filter_t*)fstream)->bref = tb_false;
	((tb_basic_stream_filter_t*)fstream)->filter = tb_filter_init_from_cache(size);
	tb_assert_and_check_goto(((tb_basic_stream_filter_t*)fstream)->filter, fail);
	
	// ok
	return fstream;
fail:
	if (fstream) tb_basic_stream_exit(fstream);
	return tb_null;
}
tb_basic_stream_t* tb_basic_stream_init_filter_from_charset(tb_basic_stream_t* bstream, tb_size_t fr, tb_size_t to)
{
	// check
	tb_assert_and_check_return_val(bstream, tb_null);

	// init filter stream
	tb_basic_stream_t* fstream = tb_basic_stream_init_filter();
	tb_assert_and_check_return_val(fstream, tb_null);

	// set bstream
	if (!tb_stream_ctrl(fstream, TB_STREAM_CTRL_FLTR_SET_STREAM, bstream)) goto fail;

	// set filter
	((tb_basic_stream_filter_t*)fstream)->bref = tb_false;
	((tb_basic_stream_filter_t*)fstream)->filter = tb_filter_init_from_charset(fr, to);
	tb_assert_and_check_goto(((tb_basic_stream_filter_t*)fstream)->filter, fail);
	
	// ok
	return fstream;
fail:
	if (fstream) tb_basic_stream_exit(fstream);
	return tb_null;
}
tb_basic_stream_t* tb_basic_stream_init_filter_from_chunked(tb_basic_stream_t* bstream, tb_bool_t dechunked)
{
	// check
	tb_assert_and_check_return_val(bstream, tb_null);

	// init filter stream
	tb_basic_stream_t* fstream = tb_basic_stream_init_filter();
	tb_assert_and_check_return_val(fstream, tb_null);
 
	// set bstream
	if (!tb_stream_ctrl(fstream, TB_STREAM_CTRL_FLTR_SET_STREAM, bstream)) goto fail;

	// set filter
	((tb_basic_stream_filter_t*)fstream)->bref = tb_false;
	((tb_basic_stream_filter_t*)fstream)->filter = tb_filter_init_from_chunked(dechunked);
	tb_assert_and_check_goto(((tb_basic_stream_filter_t*)fstream)->filter, fail);
	
	// ok
	return fstream;
fail:
	if (fstream) tb_basic_stream_exit(fstream);
	return tb_null;
}
