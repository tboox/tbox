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
 * @file		zstream.c
 *
 */
/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "filter.h"
#include "../../bstream.h"
#include "../../../zip/zip.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the encoding stream type
typedef struct __tb_gstream_filter_zip_t
{
	// the stream base
	tb_gstream_filter_t 			base;

	// the zip algorithm
	tb_size_t 				algo;

	// the zip action
	tb_size_t 				action;

	// the zip 
	tb_zip_t* 				zip;

}tb_gstream_filter_zip_t;

/* ///////////////////////////////////////////////////////////////////////
 * implements
 */

static __tb_inline__ tb_gstream_filter_zip_t* tb_gstream_filter_zip_cast(tb_gstream_t* gstream)
{
	// check
	tb_gstream_filter_t* filter = tb_gstream_filter_cast(gstream);
	tb_assert_and_check_return_val(filter && filter->type == TB_GSTREAM_FLTR_TYPE_ZIP, tb_null);
	return (tb_gstream_filter_zip_t*)filter;
}
static tb_long_t tb_gstream_filter_zip_open(tb_gstream_t* gstream)
{
	// check
	tb_gstream_filter_zip_t* zstream = tb_gstream_filter_zip_cast(gstream);
	tb_assert_and_check_return_val(zstream && !zstream->zip, -1);

	// open zip
	zstream->zip = tb_zip_init(zstream->algo, zstream->action);
	tb_assert_and_check_return_val(zstream->zip, -1);

	// open filter
	return tb_gstream_filter_open(gstream);
}
static tb_long_t tb_gstream_filter_zip_clos(tb_gstream_t* gstream)
{
	// check
	tb_gstream_filter_zip_t* zstream = tb_gstream_filter_zip_cast(gstream);
	tb_assert_and_check_return_val(zstream, -1);

	// close zip
	if (zstream->zip) 
	{
		tb_zip_exit(zstream->zip);
		zstream->zip = tb_null;
	}

	// close filter
	return tb_gstream_filter_clos(gstream);
}
static tb_bool_t tb_gstream_filter_zip_ctrl(tb_gstream_t* gstream, tb_size_t ctrl, tb_va_list_t args)
{
	// check
	tb_gstream_filter_zip_t* zstream = tb_gstream_filter_zip_cast(gstream);
	tb_assert_and_check_return_val(zstream, tb_false);

	// ctrl
	switch (ctrl)
	{
	case TB_GSTREAM_CTRL_FLTR_ZIP_GET_ALGO:
		{
			tb_size_t* pa = (tb_size_t*)tb_va_arg(args, tb_size_t*);
			tb_assert_and_check_return_val(pa, tb_false);
			*pa = zstream->algo;
			return tb_true;
		}
	case TB_GSTREAM_CTRL_FLTR_ZIP_GET_ACTION:
		{
			tb_size_t* pa = (tb_size_t*)tb_va_arg(args, tb_size_t*);
			tb_assert_and_check_return_val(pa, tb_false);
			*pa = zstream->action;
			return tb_true;
		}
	case TB_GSTREAM_CTRL_FLTR_ZIP_SET_ALGO:
		{
			zstream->algo = (tb_size_t)tb_va_arg(args, tb_size_t);
			return tb_true;
		}
	case TB_GSTREAM_CTRL_FLTR_ZIP_SET_ACTION:
		{
			zstream->action = (tb_size_t)tb_va_arg(args, tb_size_t);
			return tb_true;
		}
	default:
		break;
	}

	// routine to filter 
	return tb_gstream_filter_ctrl(gstream, ctrl, args);
}
static tb_long_t tb_gstream_filter_zip_spak(tb_gstream_t* gstream, tb_long_t sync)
{
	// check
	tb_gstream_filter_zip_t* zstream = tb_gstream_filter_zip_cast(gstream);
	tb_gstream_filter_t* filter = tb_gstream_filter_cast(gstream);
	tb_assert_and_check_return_val(zstream && zstream->zip && filter, -1);

	// the input
	tb_assert_and_check_return_val(filter->ip, -1);
	tb_byte_t const* 	ib = filter->ip;
	tb_byte_t const* 	ip = filter->ip;
	tb_byte_t const* 	ie = ip + filter->in;
	tb_check_return_val(ip < ie || sync, 0);

	// the output
	tb_assert_and_check_return_val(filter->op, -1);
	tb_byte_t* 			ob = filter->op;
	tb_byte_t* 			op = filter->op;
	tb_byte_t const* 	oe = filter->ob + TB_GSTREAM_FLTR_CACHE_MAXN;
	tb_check_return_val(op < oe, 0);

	// attach bstream
	tb_bstream_t ist, ost;
	tb_bstream_init(&ist, ip, ie - ip);
	tb_bstream_init(&ost, op, oe - op);

	// spak it
	tb_long_t r = tb_zip_spak(zstream->zip, &ist, &ost, sync);
	tb_check_return_val(r >= 0, -1);

	// update pointer
	ip = ist.p;
	op = ost.p;

	// check
	tb_assert_and_check_return_val(ip >= ib && ip <= ie, -1);
	tb_assert_and_check_return_val(op >= ob && op <= oe, -1);

	// update input
	filter->in -= ip - ib;
	filter->ip = ip;

	// update output
	filter->on += op - ob;

	// ok
	return (op - ob);
}
/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_gstream_t* tb_gstream_init_filter_zip()
{
	// make stream
	tb_gstream_t* gstream = (tb_gstream_t*)tb_malloc0(sizeof(tb_gstream_filter_zip_t));
	tb_assert_and_check_return_val(gstream, tb_null);

	// init stream
	if (!tb_gstream_init(gstream, TB_GSTREAM_TYPE_FLTR)) goto fail;

	// init func
	gstream->open 	= tb_gstream_filter_zip_open;
	gstream->read 	= tb_gstream_filter_read;
	gstream->clos	= tb_gstream_filter_zip_clos;
	gstream->wait	= tb_gstream_filter_wait;
	gstream->ctrl 	= tb_gstream_filter_zip_ctrl;

	// init filter
	((tb_gstream_filter_t*)gstream)->type 	= TB_GSTREAM_FLTR_TYPE_ZIP;
	((tb_gstream_filter_t*)gstream)->spak = tb_gstream_filter_zip_spak;

	// ok
	return gstream;

fail:
	if (gstream) tb_gstream_exit(gstream);
	return tb_null;
}
tb_gstream_t* tb_gstream_init_filter_from_zip(tb_gstream_t* gstream, tb_size_t algo, tb_size_t action)
{
	// check
	tb_assert_and_check_return_val(gstream, tb_null);

	// create encoding stream
	tb_gstream_t* zstream = tb_gstream_init_filter_zip();
	tb_assert_and_check_return_val(zstream, tb_null);

	// set gstream
	if (!tb_gstream_ctrl(zstream, TB_GSTREAM_CTRL_FLTR_SET_GSTREAM, gstream)) goto fail;
		
	// set zip algorithm
	if (!tb_gstream_ctrl(zstream, TB_GSTREAM_CTRL_FLTR_ZIP_SET_ALGO, algo)) goto fail;
		
	// set zip action
	if (!tb_gstream_ctrl(zstream, TB_GSTREAM_CTRL_FLTR_ZIP_SET_ACTION, action)) goto fail;
	
	// ok
	return zstream;

fail:
	if (zstream) tb_gstream_exit(zstream);
	return tb_null;
}

