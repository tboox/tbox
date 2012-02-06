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
 * \author		ruki
 * \file		estream.c
 *
 */
/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "tstream.h"
#include "../../encoding/encoding.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the encoding stream type
typedef struct __tb_estream_t
{
	// the stream base
	tb_tstream_t 			base;

	// the encoder
	tb_encoder_t const* 	ic;
	tb_encoder_t const* 	oc;

}tb_estream_t;

/* ///////////////////////////////////////////////////////////////////////
 * implements
 */

static __tb_inline__ tb_estream_t* tb_estream_cast(tb_gstream_t* gst)
{
	tb_tstream_t* tst = tb_tstream_cast(gst);
	tb_assert_and_check_return_val(tst && tst->type == TB_TSTREAM_TYPE_ENCODING, TB_NULL);
	return (tb_estream_t*)tst;
}
static tb_long_t tb_estream_aopen(tb_gstream_t* gst)
{
	tb_estream_t* est = tb_estream_cast(gst);
	tb_assert_and_check_return_val(est && est->ic && est->oc, TB_FALSE);

	return tb_tstream_aopen(gst);
}
static tb_bool_t tb_estream_ctrl1(tb_gstream_t* gst, tb_size_t cmd, tb_pointer_t arg1)
{
	tb_estream_t* est = tb_estream_cast(gst);
	tb_assert_and_check_return_val(est, TB_FALSE);

	// handle it
	switch (cmd)
	{
	case TB_ESTREAM_CMD_GET_IE:
		{
			tb_encoding_t* pe = (tb_encoding_t*)arg1;
			tb_assert_and_check_return_val(pe && est->ic, TB_FALSE);
			*pe = est->ic->encoding;
			return TB_TRUE;
		}
	case TB_ESTREAM_CMD_GET_OE:
		{
			tb_encoding_t* pe = (tb_encoding_t*)arg1;
			tb_assert_and_check_return_val(pe && est->oc, TB_FALSE);
			*pe = est->oc->encoding;
			return TB_TRUE;
		}
	case TB_ESTREAM_CMD_SET_IE:
		{
			est->ic = (tb_encoder_t const*)tb_encoding_get_encoder((tb_encoding_t)arg1);
			return est->ic? TB_TRUE : TB_FALSE;
		}
	case TB_ESTREAM_CMD_SET_OE:
		{
			est->oc = (tb_encoder_t const*)tb_encoding_get_encoder((tb_encoding_t)arg1);
			return est->oc? TB_TRUE : TB_FALSE;
		}
	default:
		break;
	}

	// routine to tstream 
	return tb_tstream_ctrl1(gst, cmd, arg1);
}
static tb_bool_t tb_estream_spank(tb_gstream_t* gst)
{
	tb_estream_t* est = tb_estream_cast(gst);
	tb_tstream_t* tst = tb_tstream_cast(gst);
	tb_assert_and_check_return_val(est && tst, TB_FALSE);

	// get convecter
	tb_assert_and_check_return_val(est->ic && est->oc, TB_FALSE);
	tb_encoder_t const* ic = est->ic;
	tb_encoder_t const* oc = est->oc;

	// get input
	tb_assert_and_check_return_val(tst->ip && tst->in, TB_FALSE);
	tb_byte_t const* ip = tst->ip;
	tb_byte_t const* ie = ip + tst->in;

	// get output
	tb_assert_and_check_return_val(tst->op, TB_FALSE);
	tb_byte_t* op = tst->op;
	// FIXME
//	tb_byte_t* oe = tst->ob + TB_GSTREAM_CACHE_MAXN;
	tb_byte_t* oe = tst->ob + 1;

	// spank it
	tb_uint32_t ch;
	while (ip < ie && op < oe)
	{
		if (!ic->get(&ch, &ip, ie - ip)) break;
		if (!oc->set(ch, &op, oe - op)) break;
	}
	//while (ip < ie && op < oe) *op++ = *ip++;

	// check
	tb_assert_and_check_return_val(ip >= tst->ip && ip <= ie, TB_FALSE);
	tb_assert_and_check_return_val(op >= tst->op && op <= oe, TB_FALSE);

	// update input
	tst->in -= ip - tst->ip;
	tst->ip = ip;

	// update output
	tst->on += op - tst->op;

	return TB_TRUE;
}
/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_gstream_t* tb_gstream_init_encoding()
{
	// create stream
	tb_gstream_t* gst = (tb_gstream_t*)tb_calloc(1, sizeof(tb_estream_t));
	tb_tstream_t* tst = (tb_tstream_t*)gst;
	tb_assert_and_check_return_val(gst, TB_NULL);

	// init gstream
	gst->type 	= TB_GSTREAM_TYPE_TRAN;
	gst->aopen 	= tb_estream_aopen;
	gst->aread 	= tb_tstream_aread;
	gst->aclose	= tb_tstream_aclose;
	gst->bare	= tb_tstream_bare;
	gst->wait	= tb_tstream_wait;
	gst->ctrl1 	= tb_estream_ctrl1;

	// init tstream
	tst->type 	= TB_TSTREAM_TYPE_ENCODING;
	tst->spank 	= tb_estream_spank;

	return gst;
}

tb_gstream_t* tb_gstream_init_from_encoding(tb_gstream_t* gst, tb_size_t ie, tb_size_t oe)
{
	tb_assert_and_check_return_val(gst, TB_NULL);

	// create encoding stream
	tb_gstream_t* est = tb_gstream_init_encoding();
	tb_assert_and_check_return_val(est, TB_NULL);

	// set gstream
	if (!tb_gstream_ctrl1(est, TB_TSTREAM_CMD_SET_GSTREAM, (tb_pointer_t)gst)) goto fail;
		
	// set input encoding
	if (!tb_gstream_ctrl1(est, TB_ESTREAM_CMD_SET_IE, (tb_pointer_t)ie)) goto fail;
		
	// set output encoding
	if (!tb_gstream_ctrl1(est, TB_ESTREAM_CMD_SET_OE, (tb_pointer_t)oe)) goto fail;
	
	return est;

fail:
	if (est) tb_gstream_exit(est);
	return TB_NULL;
}
