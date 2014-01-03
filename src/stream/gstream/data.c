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
 * @file		data.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../asio/asio.h"
#include "../../memory/memory.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the data stream type
typedef struct __tb_gstream_data_t
{
	// the base
	tb_gstream_t 		base;

	// the data & size
	tb_byte_t* 			data;
	tb_byte_t* 			head;
	tb_size_t 			size;

}tb_gstream_data_t;


/* ///////////////////////////////////////////////////////////////////////
 * details
 */
static __tb_inline__ tb_gstream_data_t* tb_gstream_data_cast(tb_gstream_t* gst)
{
	tb_assert_and_check_return_val(gst && gst->type == TB_GSTREAM_TYPE_DATA, tb_null);
	return (tb_gstream_data_t*)gst;
}
static tb_long_t tb_gstream_data_open(tb_gstream_t* gst)
{
	tb_gstream_data_t* dst = tb_gstream_data_cast(gst);
	tb_assert_and_check_return_val(dst && dst->data && dst->size, -1);

	// attach data
	dst->head = dst->data;

	// ok
	return 1;
}
static tb_long_t tb_gstream_data_close(tb_gstream_t* gst)
{
	tb_gstream_data_t* dst = tb_gstream_data_cast(gst);
	tb_assert_and_check_return_val(dst, -1);
	
	// reset head
	dst->head = tb_null;

	// ok
	return 1;
}
static tb_long_t tb_gstream_data_read(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size, tb_bool_t sync)
{
	tb_gstream_data_t* dst = tb_gstream_data_cast(gst);
	tb_assert_and_check_return_val(dst && dst->data && dst->head, -1);

	// check
	tb_check_return_val(data, -1);
	tb_check_return_val(size, 0);

	// adjust size
	tb_size_t left = dst->data + dst->size - dst->head;
	if (size > left) size = left;

	// read data
	tb_memcpy(data, dst->head, size);
	dst->head += size;
	return (tb_long_t)(size);
}
static tb_long_t tb_gstream_data_writ(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size, tb_bool_t sync)
{
	tb_gstream_data_t* dst = tb_gstream_data_cast(gst);
	tb_assert_and_check_return_val(dst && dst->data && dst->head, -1);

	// check
	tb_check_return_val(data, -1);
	tb_check_return_val(size, 0);

	// adjust size
	tb_size_t left = dst->data + dst->size - dst->head;
	if (size > left) size = left;

	// writ data
	tb_memcpy(dst->head, data, size);
	dst->head += size;

	return left? (tb_long_t)(size) : -1; // force end if full
}
static tb_hize_t tb_gstream_data_size(tb_gstream_t* gst)
{
	tb_gstream_data_t* dst = tb_gstream_data_cast(gst);
	tb_assert_and_check_return_val(dst, 0);

	return dst->size;
}
static tb_long_t tb_gstream_data_seek(tb_gstream_t* gst, tb_hize_t offset)
{
	tb_gstream_data_t* dst = tb_gstream_data_cast(gst);
	tb_assert_and_check_return_val(dst, -1);

	// seek 
	dst->head = dst->data + offset;

	// ok
	return 1;
}
static tb_long_t tb_gstream_data_wait(tb_gstream_t* gst, tb_size_t wait, tb_long_t timeout)
{
	tb_gstream_data_t* dst = tb_gstream_data_cast(gst);
	tb_assert_and_check_return_val(dst && dst->head <= dst->data + dst->size, -1);

	// wait 
	return wait;
}
static tb_bool_t tb_gstream_data_ctrl(tb_gstream_t* gst, tb_size_t ctrl, tb_va_list_t args)
{
	tb_gstream_data_t* dst = tb_gstream_data_cast(gst);
	tb_assert_and_check_return_val(dst, tb_false);

	switch (ctrl)
	{
	case TB_GSTREAM_CTRL_DATA_SET_DATA:
		{
			dst->data = (tb_byte_t*)tb_va_arg(args, tb_byte_t*);
			dst->size = (tb_size_t)tb_va_arg(args, tb_size_t);
			dst->head = tb_null;
			tb_assert_and_check_return_val(dst->data && dst->size, tb_false);
		}
		return tb_true;
	default:
		break;
	}
	return tb_false;
}
/* ///////////////////////////////////////////////////////////////////////
 * interface implementation
 */
tb_gstream_t* tb_gstream_init_data()
{
	// make stream
	tb_gstream_t* gst = (tb_gstream_t*)tb_malloc0(sizeof(tb_gstream_data_t));
	tb_assert_and_check_return_val(gst, tb_null);

	// init stream
	if (!tb_gstream_init(gst, TB_GSTREAM_TYPE_DATA)) goto fail;

	// init func
	gst->open 	= tb_gstream_data_open;
	gst->close 	= tb_gstream_data_close;
	gst->read 	= tb_gstream_data_read;
	gst->writ 	= tb_gstream_data_writ;
	gst->seek 	= tb_gstream_data_seek;
	gst->size 	= tb_gstream_data_size;
	gst->wait 	= tb_gstream_data_wait;
	gst->ctrl 	= tb_gstream_data_ctrl;

	// ok
	return gst;

fail:
	if (gst) tb_gstream_exit(gst);
	return tb_null;
}
tb_gstream_t* tb_gstream_init_from_data(tb_byte_t const* data, tb_size_t size)
{
	tb_assert_and_check_return_val(data && size, tb_null);

	// init data stream
	tb_gstream_t* gst = tb_gstream_init_data();
	tb_assert_and_check_return_val(gst, tb_null);

	// set data & size
	if (!tb_gstream_ctrl(gst, TB_GSTREAM_CTRL_DATA_SET_DATA, data, size)) goto fail;
	
	return gst;

fail:
	if (gst) tb_gstream_exit(gst);
	return tb_null;
}
