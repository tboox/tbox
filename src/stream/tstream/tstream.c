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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		tstream.c
 *
 */

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
tb_bool_t tb_tstream_ctrl0(tb_gstream_t* gst, tb_size_t cmd)
{
	return TB_FALSE;
}
tb_bool_t tb_tstream_ctrl1(tb_gstream_t* gst, tb_size_t cmd, tb_pointer_t arg1)
{
	tb_tstream_t* tst = tb_tstream_cast(gst);
	tb_assert_and_check_return_val(tst, TB_FALSE);

	switch (cmd)
	{
	case TB_TSTREAM_CMD_GET_GSTREAM:
		{
			tb_gstream_t** pgst = (tb_gstream_t*)arg1;
			tb_assert_and_check_return_val(pgst, TB_FALSE);
			*pgst = tst->gst;
			return TB_TRUE;
		}
	case TB_TSTREAM_CMD_SET_GSTREAM:
		{
			tst->gst = (tb_gstream_t*)arg1;
			return TB_TRUE;
		}
	default:
		break;
	}
	return TB_FALSE;
}
tb_bool_t tb_tstream_ctrl2(tb_gstream_t* gst, tb_size_t cmd, tb_pointer_t arg1, tb_pointer_t arg2)
{
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

	// init status
	tst->status = TB_TSTREAM_STATUS_OK;

	// ok
	return 1;
}
tb_long_t tb_tstream_aclose(tb_gstream_t* gst)
{
	// ok
	return 1;
}
tb_long_t tb_tstream_aread(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size)
{
	tb_tstream_t* tst = tb_tstream_cast(gst);
	//tb_trace("=====================================");
	//tb_trace("need: %d", size);

	// check
	tb_assert_and_check_return_val(tst && data, -1);
	tb_check_return_val(size, 0);

	// read data from the output data first
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

	//tb_trace("have: %d", read);
	// is enough?
	if (read == size) return read;

	// FIXME
	// read data from gstream if the input data is not full
//	tb_long_t ln = tst->ib + TB_GSTREAM_CACHE_MAXN - tst->ip;
	tb_long_t ln = tst->ib + 1 - tst->ip;
	tb_long_t ret = -1;
	if (tst->in < ln)
	{
		// read data
		tb_assert_and_check_return_val(tst->gst, -1);
		ret = tb_gstream_aread(tst->gst, tst->ip + tst->in, ln - tst->in);
		if (ret > 0) tst->in += ret;
		// handle the left data
		else if (tst->in) ;
		// return the left data
		else if (read) return read;
		else if (!ret && tst->status != TB_TSTREAM_STATUS_FAIL) 
		{
			// has size?
			tb_uint64_t gsize = tb_gstream_size(tst->gst);

			// is end?
			if (gsize && tb_gstream_offset(tst->gst) >= gsize) return -1;
			else if (tst->status == TB_TSTREAM_STATUS_END) return -1;
			// no data, wait for timeout?
			else return 0;
		}
		// error? end?
		else return -1;
	}

	//tb_trace("spank[i]: %d", tst->in);
	// spank it for transform
	tb_assert_and_check_return_val(tst->spank && !tst->on && tst->op == tst->ob, -1);
	if (!tst->spank(gst)) 
	{
		tst->status = TB_TSTREAM_STATUS_FAIL;
		return read? read : -1;
	}

	//tb_trace("spank[o]: %d, left: %d", tst->on, tst->in);
	// read data from the output data
	if (tst->on > 0)
	{
		// if enough?
		tb_assert_and_check_return_val(read < size, -1);
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
	// have not enough input data for transform?
	else ;

	//tb_trace("move: %d", tst->in);
	// move the left input data to the begin for getting more data
	if (tst->ib != tst->ip)
	{
		if (tst->in) tb_memmov(tst->ib, tst->ip, tst->in);
		tst->ip = tst->ib;
	}
	// break it if no enough input? tst->in > 0
	else if (!tst->on && !read)
	{
		// error?
		if (ret < 0) return -1;
		// is end?
		else
		{
			tb_uint64_t gsize = tb_gstream_size(tst->gst);
			if (gsize && tb_gstream_offset(tst->gst) >= gsize) return -1;
			else if (tst->status == TB_TSTREAM_STATUS_END) return -1;
		}
	}

	//tb_trace("read: %d", read);
	return read;
}

tb_handle_t tb_tstream_bare(tb_gstream_t* gst)
{
	tb_trace_noimpl();
	return TB_NULL;
}
tb_long_t tb_tstream_wait(tb_gstream_t* gst, tb_size_t etype, tb_long_t timeout)
{
	tb_trace_noimpl();
	return -1;
}
