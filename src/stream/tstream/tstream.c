/*!The Tiny Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		tstream.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "tstream.h"
#include "../../memops.h"

/* /////////////////////////////////////////////////////////
 * interfaces
 */
tb_tstream_t* tb_tstream_cast(tb_gstream_t* gst)
{
	TB_ASSERT_RETURN_VAL(gst && gst->type == TB_GSTREAM_TYPE_TRAN, TB_NULL);
	return (tb_tstream_t*)gst;
}
tb_bool_t tb_tstream_ioctl0(tb_gstream_t* gst, tb_size_t cmd)
{
	return TB_FALSE;
}
tb_bool_t tb_tstream_ioctl1(tb_gstream_t* gst, tb_size_t cmd, void* arg1)
{
	tb_tstream_t* tst = tb_tstream_cast(gst);
	TB_ASSERT_RETURN_VAL(tst, TB_FALSE);

	switch (cmd)
	{
	case TB_TSTREAM_CMD_GET_GSTREAM:
		{
			tb_gstream_t** pgst = (tb_gstream_t*)arg1;
			TB_ASSERT_RETURN_VAL(pgst, TB_FALSE);
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
tb_bool_t tb_tstream_ioctl2(tb_gstream_t* gst, tb_size_t cmd, void* arg1, void* arg2)
{
	return TB_FALSE;
}
tb_bool_t tb_tstream_open(tb_gstream_t* gst)
{
	tb_tstream_t* tst = tb_tstream_cast(gst);
	TB_ASSERT_RETURN_VAL(tst && tst->gst, TB_FALSE);

	// init input
	tst->ip = tst->ib;
	tst->in = 0;

	// init output
	tst->op = tst->ob;
	tst->on = 0;

	// init status
	tst->status = TB_TSTREAM_STATUS_OK;

	return TB_TRUE;
}
void tb_tstream_close(tb_gstream_t* gst)
{
}
tb_int_t tb_tstream_read(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size)
{
	tb_tstream_t* tst = tb_tstream_cast(gst);
	//TB_DBG("=====================================");
	//TB_DBG("need: %d", size);

	// check
	TB_ASSERT_RETURN_VAL(tst && data, -1);
	TB_IF_FAIL_RETURN_VAL(size, 0);

	// read data from the output data first
	tb_int_t read = 0;
	if (tst->on > 0)
	{
		// if enough?
		TB_ASSERT_RETURN_VAL(tst->op, -1);
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

	//TB_DBG("have: %d", read);
	// is enough?
	if (read == size) return read;

	// read data from gstream if the input data is not full
	tb_int_t ln = tst->ib + TB_GSTREAM_BLOCK_SIZE - tst->ip;
	tb_int_t ret = -1;
	if (tst->in < ln)
	{
		// read data
		TB_ASSERT_RETURN_VAL(tst->gst, -1);
		ret = tb_gstream_read(tst->gst, tst->ip + tst->in, ln - tst->in);
		if (ret > 0) tst->in += ret;
		// handle the left data
		else if (tst->in) ;
		// return the left data
		else if (read) return read;
		else if (!ret && tst->status != TB_TSTREAM_STATUS_FAIL) 
		{
			// has size?
			tb_size_t gsize = tb_gstream_size(tst->gst);

			// is end?
			if (gsize && tb_gstream_offset(tst->gst) >= gsize) return -1;
			else if (tst->status == TB_TSTREAM_STATUS_END) return -1;
			// no data, wait for timeout?
			else return 0;
		}
		// error? end?
		else return -1;
	}

	//TB_DBG("spank[i]: %d", tst->in);
	// spank it for transform
	TB_ASSERT_RETURN_VAL(tst->spank && !tst->on && tst->op == tst->ob, -1);
	if (TB_FALSE == tst->spank(gst)) 
	{
		tst->status = TB_TSTREAM_STATUS_FAIL;
		return read? read : -1;
	}

	//TB_DBG("spank[o]: %d, left: %d", tst->on, tst->in);
	// read data from the output data
	if (tst->on > 0)
	{
		// if enough?
		TB_ASSERT_RETURN_VAL(read < size, -1);
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

	//TB_DBG("move: %d", tst->in);
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
			tb_size_t gsize = tb_gstream_size(tst->gst);
			if (gsize && tb_gstream_offset(tst->gst) >= gsize) return -1;
			else if (tst->status == TB_TSTREAM_STATUS_END) return -1;
		}
	}

	//TB_DBG("read: %d", read);
	return read;
}

