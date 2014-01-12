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

static __tb_inline__ tb_gstream_filter_charset_t* tb_gstream_filter_charset_cast(tb_gstream_t* gstream)
{
	tb_gstream_filter_t* filter = tb_gstream_filter_cast(gstream);
	tb_assert_and_check_return_val(filter && filter->type == TB_GSTREAM_FLTR_TYPE_CHARSET, tb_null);
	return (tb_gstream_filter_charset_t*)filter;
}
static tb_long_t tb_gstream_filter_charset_open(tb_gstream_t* gstream)
{
	tb_gstream_filter_charset_t* cstream = tb_gstream_filter_charset_cast(gstream);
	tb_assert_and_check_return_val(cstream && cstream->ftype && cstream->ttype, -1);

	return tb_gstream_filter_open(gstream);
}
static tb_bool_t tb_gstream_filter_charset_ctrl(tb_gstream_t* gstream, tb_size_t cmd, tb_va_list_t args)
{
	tb_gstream_filter_charset_t* cstream = tb_gstream_filter_charset_cast(gstream);
	tb_assert_and_check_return_val(cstream, tb_false);

	switch (cmd)
	{
	case TB_GSTREAM_CTRL_FLTR_CHARSET_GET_FTYPE:
		{
			tb_size_t* pe = (tb_size_t*)tb_va_arg(args, tb_size_t*);
			tb_assert_and_check_return_val(pe, tb_false);
			*pe = cstream->ftype;
			return tb_true;
		}
	case TB_GSTREAM_CTRL_FLTR_CHARSET_GET_TTYPE:
		{
			tb_size_t* pe = (tb_size_t*)tb_va_arg(args, tb_size_t*);
			tb_assert_and_check_return_val(pe, tb_false);
			*pe = cstream->ttype;
			return tb_true;
		}
	case TB_GSTREAM_CTRL_FLTR_CHARSET_SET_FTYPE:
		{
			cstream->ftype = (tb_size_t)tb_va_arg(args, tb_size_t);
			return TB_CHARSET_TYPE_OK(cstream->ftype)? tb_true : tb_false;
		}
	case TB_GSTREAM_CTRL_FLTR_CHARSET_SET_TTYPE:
		{
			cstream->ttype = (tb_size_t)tb_va_arg(args, tb_size_t);
			return TB_CHARSET_TYPE_OK(cstream->ttype)? tb_true : tb_false;
		}
	default:
		break;
	}

	// routine to filter 
	return tb_gstream_filter_ctrl(gstream, cmd, args);
}
static tb_long_t tb_gstream_filter_charset_spak(tb_gstream_t* gstream, tb_long_t sync)
{
	tb_gstream_filter_charset_t* cstream = tb_gstream_filter_charset_cast(gstream);
	tb_gstream_filter_t* filter = tb_gstream_filter_cast(gstream);
	tb_assert_and_check_return_val(cstream && filter, -1);

	// the convecter
	tb_assert_and_check_return_val(TB_CHARSET_TYPE_OK(cstream->ftype) && TB_CHARSET_TYPE_OK(cstream->ttype), -1);

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
	if (tb_charset_conv_bst(cstream->ftype, cstream->ttype, &ist, &ost) < 0) return -1;
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
	// make stream
	tb_gstream_t* gstream = (tb_gstream_t*)tb_malloc0(sizeof(tb_gstream_filter_charset_t));
	tb_assert_and_check_return_val(gstream, tb_null);

	// init stream
	if (!tb_gstream_init(gstream, TB_GSTREAM_TYPE_FLTR)) goto fail;

	// init func
	gstream->open 	= tb_gstream_filter_charset_open;
	gstream->read 	= tb_gstream_filter_read;
	gstream->close	= tb_gstream_filter_close;
	gstream->wait	= tb_gstream_filter_wait;
	gstream->ctrl 	= tb_gstream_filter_charset_ctrl;

	// init filter
	((tb_gstream_filter_t*)gstream)->type 	= TB_GSTREAM_FLTR_TYPE_CHARSET;
	((tb_gstream_filter_t*)gstream)->spak = tb_gstream_filter_charset_spak;

	// ok
	return gstream;

fail:
	if (gstream) tb_gstream_exit(gstream);
	return tb_null;
}

tb_gstream_t* tb_gstream_init_filter_from_charset(tb_gstream_t* gstream, tb_size_t ftype, tb_size_t ttype)
{
	// check
	tb_assert_and_check_return_val(gstream, tb_null);

	// create charset stream
	tb_gstream_t* cstream = tb_gstream_init_filter_charset();
	tb_assert_and_check_return_val(cstream, tb_null);

	// set gstream
	if (!tb_gstream_ctrl(cstream, TB_GSTREAM_CTRL_FLTR_SET_GSTREAM, gstream)) goto fail;
		
	// set from charset type
	if (!tb_gstream_ctrl(cstream, TB_GSTREAM_CTRL_FLTR_CHARSET_SET_FTYPE, ftype)) goto fail;
		
	// set to charset type
	if (!tb_gstream_ctrl(cstream, TB_GSTREAM_CTRL_FLTR_CHARSET_SET_TTYPE, ttype)) goto fail;
	
	return cstream;

fail:
	if (cstream) tb_gstream_exit(cstream);
	return tb_null;
}
