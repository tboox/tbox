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
 * @file		filter.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
//#define TB_TRACE_IMPL_TAG 			"filter"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "filter.h"
#include "../../../memory/memory.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_long_t tb_gstream_filter_afill(tb_gstream_filter_t* filter, tb_byte_t* data, tb_size_t size, tb_bool_t sync)
{
	// fill data
	tb_long_t fill = 0;
	if (!sync) 
	{
		// read data
		fill = tb_gstream_aread(filter->gst, data, size);
		tb_trace_impl("read: %ld <? %lu", fill, size);
		tb_check_goto(fill < 0, end);

		// end? sync data
		fill = tb_gstream_afread(filter->gst, data, size);
		tb_trace_impl("sync: %ld <? %lu", fill, size);
	}
	// sync data
	else 
	{
		// sync data
		fill = tb_gstream_afread(filter->gst, data, size);
		tb_trace_impl("sync: %ld <? %lu", fill, size);
	}

end:
	// ok?
	return fill;
}
/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_gstream_filter_t* tb_gstream_filter_cast(tb_gstream_t* gst)
{
	tb_assert_and_check_return_val(gst && gst->type == TB_GSTREAM_TYPE_FLTR, tb_null);
	return (tb_gstream_filter_t*)gst;
}
tb_bool_t tb_gstream_filter_ctrl(tb_gstream_t* gst, tb_size_t cmd, tb_va_list_t args)
{
	tb_gstream_filter_t* filter = tb_gstream_filter_cast(gst);
	tb_assert_and_check_return_val(filter, tb_false);

	switch (cmd)
	{
	case TB_GSTREAM_CTRL_FLTR_GET_GSTREAM:
		{
			tb_gstream_t** pgst = (tb_gstream_t**)tb_va_arg(args, tb_gstream_t**);
			tb_assert_and_check_return_val(pgst, tb_false);
			*pgst = filter->gst;
			return tb_true;
		}
	case TB_GSTREAM_CTRL_FLTR_SET_GSTREAM:
		{
			filter->gst = (tb_gstream_t*)tb_va_arg(args, tb_gstream_t*);
			tb_assert_and_check_return_val(filter->gst, tb_false);
			return tb_true;
		}
	default:
		break;
	}
	return tb_false;
}
tb_long_t tb_gstream_filter_aopen(tb_gstream_t* gst)
{
	tb_gstream_filter_t* filter = tb_gstream_filter_cast(gst);
	tb_assert_and_check_return_val(filter && filter->gst, -1);

	// init input
	filter->ip = filter->ib;
	filter->in = 0;

	// init output
	filter->op = filter->ob;
	filter->on = 0;

	// init read & writ
	filter->read = 0;
	filter->writ = 0;

	// ok
	return 1;
}
tb_long_t tb_gstream_filter_aclose(tb_gstream_t* gst)
{
	tb_gstream_filter_t* filter = tb_gstream_filter_cast(gst);
	tb_assert_and_check_return_val(filter && filter->gst, -1);

	// init input
	filter->ip = filter->ib;
	filter->in = 0;

	// init output
	filter->op = filter->ob;
	filter->on = 0;

	// init read & writ
	filter->read = 0;
	filter->writ = 0;

	// ok
	return 1;
}
tb_long_t tb_gstream_filter_aread(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size, tb_bool_t sync)
{
	tb_gstream_filter_t* filter = tb_gstream_filter_cast(gst);
	tb_assert_and_check_return_val(filter && filter->gst && filter->spak, -1);

	// no data?
	if (!data)
	{
		// flush data?
		if (sync)
		{
			tb_long_t r = tb_gstream_afread(filter->gst, tb_null, 0);
			tb_check_return_val(r < 0, r);
		}

		// init input
		filter->ip = filter->ib;
		filter->in = 0;

		// init output
		filter->op = filter->ob;
		filter->on = 0;

		// init read & writ
		filter->read = 0;
		filter->writ = 0;

		// end
		return -1;
	}

	// check
	tb_check_return_val(size, 0);

	// read the output data first
	tb_long_t read = 0;
	if (filter->on > 0)
	{
		// enough?
		tb_assert_and_check_return_val(filter->op, -1);
		if (filter->on > size)
		{
			read = size;
			tb_memcpy(data, filter->op, read);
			filter->op += read;
			filter->on -= read;
		}
		else
		{
			read = filter->on;
			tb_memcpy(data, filter->op, read);
			filter->op = filter->ob;
			filter->on = 0;
		}
	}

	// enough?
	tb_check_goto(read < size, end);

	// move the input data for the more space
	if (filter->ip != filter->ib) 
	{
		if (filter->in) tb_memmov(filter->ib, filter->ip, filter->in);
		filter->ip = filter->ib;
	}

	// fill the input data now
	tb_long_t ln = filter->ib + TB_GSTREAM_FLTR_CACHE_MAXN - filter->ip;
	if (filter->in < ln && filter->read >= 0)
	{
		// read data
		filter->read = tb_gstream_filter_afill(filter, filter->ip + filter->in, ln - filter->in, sync); 

		// update the input size
		if (filter->read > 0) filter->in += filter->read;
		// no data?
		else if (!filter->read) 
		{
			// has size?
			tb_hize_t n = tb_gstream_size(filter->gst);
			tb_hize_t o = tb_gstream_offset(filter->gst);

			// is end?
			if (n && o >= n) 
			{
				filter->read = -1;
				tb_trace_impl("end");
			}
		}

		// trace
		tb_trace_impl("fill: %ld <? %lu", filter->in, ln);		
	}

	// input enough or end? spak it
	if (filter->in >= ln || filter->read < 0)
	{
		// spak it
		tb_assert_and_check_return_val(!filter->on && filter->op == filter->ob, -1);
		if (filter->spak(gst, (tb_long_t)((filter->read < 0)? -1 : (sync? 1 : 0))) < 0) return -1;
		tb_trace_impl("spak: %lu, left: %lu", filter->on, filter->in);
	}

	// continue to read the output data
	if (filter->on > 0)
	{
		// enough?
		tb_size_t need = size - read;
		if (filter->on > need)
		{
			tb_memcpy(data + read, filter->op, need);
			read += need;
			filter->op += need;
			filter->on -= need;
		}
		else
		{
			tb_memcpy(data + read, filter->op, filter->on);
			read += filter->on;
			filter->op = filter->ob;
			filter->on = 0;
		}
	}
	// no output? end?
	else if (!filter->in && !read && filter->read < 0) 
	{
		tb_trace_impl("end");
		return -1;
	}
end:

	// ok?
	tb_trace_impl("read: %u, left: %lu", read, filter->in);
	return read;
}
tb_long_t tb_gstream_filter_wait(tb_gstream_t* gst, tb_size_t wait, tb_long_t timeout)
{
	// check
	tb_gstream_filter_t* filter = tb_gstream_filter_cast(gst);
	tb_assert_and_check_return_val(filter, -1);

	// has read
	if (filter->read > 0) return wait;
	// no read? wait it
	else if (!filter->read) 
	{
		tb_trace_impl("wait: %u, timeout: %d", wait, timeout);
		return tb_gstream_wait(filter->gst, wait, timeout);
	}
	// has left input? abort it if read empty next
	else if (filter->in) return wait;
	// end
	else return -1;
}
