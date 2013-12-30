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
 * @file		cstream.c
 *
 */
/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "filter.h"
#include "../../../charset/charset.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the charset stream type
typedef struct __tb_gstream_filter_charset_t
{
	// the stream base
	tb_gstream_filter_t 			base;

	// the from type
	tb_size_t 				ftype;

	// the to type
	tb_size_t 				ttype;

}tb_gstream_filter_charset_t;

/* ///////////////////////////////////////////////////////////////////////
 * implements
 */

static __tb_inline__ tb_gstream_filter_charset_t* tb_gstream_filter_charset_cast(tb_gstream_t* gst)
{
	tb_gstream_filter_t* filter = tb_gstream_filter_cast(gst);
	tb_assert_and_check_return_val(filter && filter->type == TB_GSTREAM_FLTR_TYPE_CHARSET, tb_null);
	return (tb_gstream_filter_charset_t*)filter;
}
static tb_long_t tb_gstream_filter_charset_aopen(tb_gstream_t* gst)
{
	tb_gstream_filter_charset_t* cst = tb_gstream_filter_charset_cast(gst);
	tb_assert_and_check_return_val(cst && cst->ftype && cst->ttype, -1);

	return tb_gstream_filter_aopen(gst);
}
static tb_bool_t tb_gstream_filter_charset_ctrl(tb_gstream_t* gst, tb_size_t cmd, tb_va_list_t args)
{
	tb_gstream_filter_charset_t* cst = tb_gstream_filter_charset_cast(gst);
	tb_assert_and_check_return_val(cst, tb_false);

	switch (cmd)
	{
	case TB_GSTREAM_CTRL_FLTR_CHARSET_GET_FTYPE:
		{
			tb_size_t* pe = (tb_size_t*)tb_va_arg(args, tb_size_t*);
			tb_assert_and_check_return_val(pe, tb_false);
			*pe = cst->ftype;
			return tb_true;
		}
	case TB_GSTREAM_CTRL_FLTR_CHARSET_GET_TTYPE:
		{
			tb_size_t* pe = (tb_size_t*)tb_va_arg(args, tb_size_t*);
			tb_assert_and_check_return_val(pe, tb_false);
			*pe = cst->ttype;
			return tb_true;
		}
	case TB_GSTREAM_CTRL_FLTR_CHARSET_SET_FTYPE:
		{
			cst->ftype = (tb_size_t)tb_va_arg(args, tb_size_t);
			return TB_CHARSET_TYPE_OK(cst->ftype)? tb_true : tb_false;
		}
	case TB_GSTREAM_CTRL_FLTR_CHARSET_SET_TTYPE:
		{
			cst->ttype = (tb_size_t)tb_va_arg(args, tb_size_t);
			return TB_CHARSET_TYPE_OK(cst->ttype)? tb_true : tb_false;
		}
	default:
		break;
	}

	// routine to filter 
	return tb_gstream_filter_ctrl(gst, cmd, args);
}
static tb_long_t tb_gstream_filter_charset_spak(tb_gstream_t* gst, tb_long_t sync)
{
	tb_gstream_filter_charset_t* cst = tb_gstream_filter_charset_cast(gst);
	tb_gstream_filter_t* filter = tb_gstream_filter_cast(gst);
	tb_assert_and_check_return_val(cst && filter, -1);

	// the convecter
	tb_assert_and_check_return_val(TB_CHARSET_TYPE_OK(cst->ftype) && TB_CHARSET_TYPE_OK(cst->ttype), -1);

	// the input
	tb_assert_and_check_return_val(filter->ip, -1);
	tb_byte_t const* 	ib = filter->ip;
	tb_byte_t const* 	ip = filter->ip;
	tb_byte_t const* 	ie = ip + filter->in;
	tb_check_return_val(ip < ie, 0);

	// the output
	tb_assert_and_check_return_val(filter->op, -1);
	tb_byte_t* 			ob = filter->op;
	tb_byte_t* 			op = filter->op;
	tb_byte_t const* 	oe = filter->ob + TB_GSTREAM_FLTR_CACHE_MAXN;
	tb_check_return_val(op < oe, 0);

	// spak it
	tb_bstream_t ist;
	tb_bstream_t ost;
	tb_bstream_init(&ist, ib, ie - ib);
	tb_bstream_init(&ost, ob, oe - ob);
	if (tb_charset_conv_bst(cst->ftype, cst->ttype, &ist, &ost) < 0) return -1;
	ip = tb_bstream_pos(&ist);
	op = tb_bstream_pos(&ost);

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
tb_gstream_t* tb_gstream_init_filter_charset()
{
	tb_gstream_t* gst = (tb_gstream_t*)tb_malloc0(sizeof(tb_gstream_filter_charset_t));
	tb_assert_and_check_return_val(gst, tb_null);

	// init base
	if (!tb_gstream_init(gst)) goto fail;

	// init gstream
	gst->type 	= TB_GSTREAM_TYPE_FLTR;
	gst->aopen 	= tb_gstream_filter_charset_aopen;
	gst->aread 	= tb_gstream_filter_aread;
	gst->aclose	= tb_gstream_filter_aclose;
	gst->wait	= tb_gstream_filter_wait;
	gst->ctrl 	= tb_gstream_filter_charset_ctrl;

	// init filter
	((tb_gstream_filter_t*)gst)->type 	= TB_GSTREAM_FLTR_TYPE_CHARSET;
	((tb_gstream_filter_t*)gst)->spak = tb_gstream_filter_charset_spak;

	// ok
	return gst;

fail:
	if (gst) tb_gstream_exit(gst);
	return tb_null;
}

tb_gstream_t* tb_gstream_init_filter_from_charset(tb_gstream_t* gst, tb_size_t ftype, tb_size_t ttype)
{
	tb_assert_and_check_return_val(gst, tb_null);

	// create charset stream
	tb_gstream_t* cst = tb_gstream_init_filter_charset();
	tb_assert_and_check_return_val(cst, tb_null);

	// set gstream
	if (!tb_gstream_ctrl(cst, TB_GSTREAM_CTRL_FLTR_SET_GSTREAM, gst)) goto fail;
		
	// set from charset type
	if (!tb_gstream_ctrl(cst, TB_GSTREAM_CTRL_FLTR_CHARSET_SET_FTYPE, ftype)) goto fail;
		
	// set to charset type
	if (!tb_gstream_ctrl(cst, TB_GSTREAM_CTRL_FLTR_CHARSET_SET_TTYPE, ttype)) goto fail;
	
	return cst;

fail:
	if (cst) tb_gstream_exit(cst);
	return tb_null;
}
