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
 * @file		tstream.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
//#define TB_TRACE_IMPL_TAG 			"tst"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tstream.h"
#include "../../../memory/memory.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_long_t tb_tstream_afill(tb_tstream_t* tst, tb_byte_t* data, tb_size_t size, tb_bool_t sync)
{
	// fill data
	tb_long_t fill = 0;
	if (!sync) 
	{
		// read data
		fill = tb_gstream_aread(tst->gst, data, size);
		tb_check_goto(fill < 0, end);

		// end? sync data
		fill = tb_gstream_afread(tst->gst, data, size);
	}
	// sync data
	else fill = tb_gstream_afread(tst->gst, data, size);

end:
	// ok?
	return fill;
}
/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_tstream_t* tb_tstream_cast(tb_gstream_t* gst)
{
	tb_assert_and_check_return_val(gst && gst->type == TB_GSTREAM_TYPE_TRAN, tb_null);
	return (tb_tstream_t*)gst;
}
tb_bool_t tb_tstream_ctrl(tb_gstream_t* gst, tb_size_t cmd, tb_va_list_t args)
{
	tb_tstream_t* tst = tb_tstream_cast(gst);
	tb_assert_and_check_return_val(tst, tb_false);

	switch (cmd)
	{
	case TB_TSTREAM_CTRL_GET_GSTREAM:
		{
			tb_gstream_t** pgst = (tb_gstream_t**)tb_va_arg(args, tb_gstream_t**);
			tb_assert_and_check_return_val(pgst, tb_false);
			*pgst = tst->gst;
			return tb_true;
		}
	case TB_TSTREAM_CTRL_SET_GSTREAM:
		{
			tst->gst = (tb_gstream_t*)tb_va_arg(args, tb_gstream_t*);
			tb_assert_and_check_return_val(tst->gst, tb_false);
			return tb_true;
		}
	default:
		break;
	}
	return tb_false;
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
tb_long_t tb_tstream_aread(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size, tb_bool_t sync)
{
	tb_tstream_t* tst = tb_tstream_cast(gst);
	tb_assert_and_check_return_val(tst && tst->gst && tst->spak, -1);

	// no data?
	if (!data)
	{
		// flush data?
		if (sync)
		{
			tb_long_t r = tb_gstream_afread(tst->gst, tb_null, 0);
			tb_check_return_val(r < 0, r);
		}

		// init input
		tst->ip = tst->ib;
		tst->in = 0;

		// init output
		tst->op = tst->ob;
		tst->on = 0;

		// init read & writ
		tst->read = 0;
		tst->writ = 0;

		// end
		return -1;
	}

	// check
	tb_check_return_val(size, 0);

	// read the output data first
	tb_long_t read = 0;
	if (tst->on > 0)
	{
		// enough?
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
	tb_check_goto(read < size, end);

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
		tst->read = tb_tstream_afill(tst, tst->ip + tst->in, ln - tst->in, sync); 

		// update the input size
		if (tst->read > 0) tst->in += tst->read;
		// no data?
		else if (!tst->read) 
		{
			// has size?
			tb_hize_t n = tb_gstream_size(tst->gst);
			tb_hize_t o = tb_gstream_offset(tst->gst);

			// is end?
			if (n && o >= n) tst->read = -1;
		}
		tb_trace_impl("fill: %d", tst->read);
	}

	// spak it
	tb_long_t r = 0;
	tb_assert_and_check_return_val(!tst->on && tst->op == tst->ob, -1);
	while ((r = tst->spak(gst, tst->read < 0? tb_true : sync)) > 0) ;
	tb_trace_impl("spak: %u", tst->on);

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

end:
	// ok?
	tb_trace_impl("read: %u", read);
	return read;
}
tb_long_t tb_tstream_wait(tb_gstream_t* gst, tb_size_t etype, tb_long_t timeout)
{
	tb_tstream_t* tst = tb_tstream_cast(gst);
	tb_assert_and_check_return_val(tst, tb_null);

	if (tst->read > 0) return etype;
	else if (!tst->read) 
	{
		tb_trace_impl("wait: %u, timeout: %d", etype, timeout);
		return tb_gstream_wait(tst->gst, etype, timeout);
	}
	else return -1;
}
