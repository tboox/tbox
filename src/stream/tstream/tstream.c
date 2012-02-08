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
 * \file		tstream.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_IMPL_TAG 			"tst"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tstream.h"
#include "../../memory/memory.h"

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_tstream_t* tb_tstream_cast(tb_gstream_t* gst)
{
	tb_assert_and_check_return_val(gst && gst->type == TB_GSTREAM_TYPE_TRAN, TB_NULL);
	return (tb_tstream_t*)gst;
}
tb_bool_t tb_tstream_ctrl(tb_gstream_t* gst, tb_size_t cmd, tb_va_list_t args)
{
	tb_tstream_t* tst = tb_tstream_cast(gst);
	tb_assert_and_check_return_val(tst, TB_FALSE);

	switch (cmd)
	{
	case TB_TSTREAM_CMD_GET_GSTREAM:
		{
			tb_gstream_t** pgst = (tb_gstream_t**)tb_va_arg(args, tb_gstream_t**);
			tb_assert_and_check_return_val(pgst, TB_FALSE);
			*pgst = tst->gst;
			return TB_TRUE;
		}
	case TB_TSTREAM_CMD_SET_GSTREAM:
		{
			tst->gst = (tb_gstream_t*)tb_va_arg(args, tb_gstream_t*);
			tb_assert_and_check_return_val(tst->gst, TB_FALSE);
			return TB_TRUE;
		}
	default:
		break;
	}
	return TB_FALSE;
}
tb_long_t tb_tstream_aopen(tb_gstream_t* gst)
{
	tb_tstream_t* tst = tb_tstream_cast(gst);
	tb_assert_and_check_return_val(tst && tst->gst, -1);

	// init input
	tst->ip = tst->ib;
	tst->in = 0;

	// init output
	tst->op = tst->ob;
	tst->on = 0;

	// init read & writ
	tst->read = 0;
	tst->writ = 0;

	// ok
	return 1;
}
tb_long_t tb_tstream_aclose(tb_gstream_t* gst)
{
	tb_tstream_t* tst = tb_tstream_cast(gst);
	tb_assert_and_check_return_val(tst && tst->gst, -1);

	// init input
	tst->ip = tst->ib;
	tst->in = 0;

	// init output
	tst->op = tst->ob;
	tst->on = 0;

	// init read & writ
	tst->read = 0;
	tst->writ = 0;

	// ok
	return 1;
}
tb_long_t tb_tstream_aread(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size)
{
	tb_tstream_t* tst = tb_tstream_cast(gst);
	tb_assert_and_check_return_val(tst && tst->gst && tst->spank && data, -1);
	tb_check_return_val(size, 0);

	// read the output data first
	tb_long_t read = 0;
	if (tst->on > 0)
	{
		// if enough?
		tb_assert_and_check_return_val(tst->op, -1);
		if (tst->on > size)
		{
			read = size;
			tb_memcpy(data, tst->op, read);
			tst->op += read;
			tst->on -= read;
		}
		else
		{
			read = tst->on;
			tb_memcpy(data, tst->op, read);
			tst->op = tst->ob;
			tst->on = 0;
		}
	}

	// enough?
	tb_check_return_val(read < size, read);

	// move the input data for the more space
	if (tst->ip != tst->ib) 
	{
		if (tst->in) tb_memmov(tst->ib, tst->ip, tst->in);
		tst->ip = tst->ib;
	}

	// fill the input data now
	tb_long_t ln = tst->ib + TB_TSTREAM_CACHE_MAXN - tst->ip;
	if (tst->in < ln && tst->read >= 0)
	{
		// read data
		tst->read = tb_gstream_aread(tst->gst, tst->ip + tst->in, ln - tst->in);

		// update the input size
		if (tst->read > 0) tst->in += tst->read;
		// no data?
		else if (!tst->read) 
		{
			// has size?
			tb_uint64_t n = tb_gstream_size(tst->gst);
			tb_uint64_t o = tb_gstream_offset(tst->gst);

			// is end?
			if (n && o >= n) tst->read = -1;
		}
	}

	// spank it
	tb_long_t r = 0;
	tb_assert_and_check_return_val(!tst->on && tst->op == tst->ob, -1);
	while ((r = tst->spank(gst)) > 0) ;

	// continue to read the output data
	if (tst->on > 0)
	{
		// enough?
		tb_size_t need = size - read;
		if (tst->on > need)
		{
			tb_memcpy(data + read, tst->op, need);
			read += need;
			tst->op += need;
			tst->on -= need;
		}
		else
		{
			tb_memcpy(data + read, tst->op, tst->on);
			read += tst->on;
			tst->op = tst->ob;
			tst->on = 0;
		}
	}
	// no output? end?
	else if (tst->read < 0) return -1;

	// ok?
	return read;
}

tb_long_t tb_tstream_awrit(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size)
{
	tb_trace_noimpl();
	return -1;
}
tb_handle_t tb_tstream_bare(tb_gstream_t* gst)
{
	tb_tstream_t* tst = tb_tstream_cast(gst);
	tb_assert_and_check_return_val(tst, TB_NULL);

	return tb_gstream_bare(tst->gst);
}
tb_long_t tb_tstream_wait(tb_gstream_t* gst, tb_size_t etype, tb_long_t timeout)
{
	tb_tstream_t* tst = tb_tstream_cast(gst);
	tb_assert_and_check_return_val(tst, TB_NULL);

	if (tst->read > 0) return etype;
	else if (!tst->read) return tb_gstream_wait(tst->gst, etype, timeout);
	else return -1;
}
