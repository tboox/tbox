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
 * \file		data.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../aio/aio.h"
#include "../../memory/memory.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the data stream type
typedef struct __tb_dstream_t
{
	// the base
	tb_gstream_t 		base;

	// the data & size
	tb_byte_t* 			data;
	tb_byte_t* 			head;
	tb_size_t 			size;

}tb_dstream_t;


/* ///////////////////////////////////////////////////////////////////////
 * details
 */
static __tb_inline__ tb_dstream_t* tb_dstream_cast(tb_gstream_t* gst)
{
	tb_assert_and_check_return_val(gst && gst->type == TB_GSTREAM_TYPE_DATA, TB_NULL);
	return (tb_dstream_t*)gst;
}
static tb_long_t tb_dstream_aopen(tb_gstream_t* gst)
{
	tb_dstream_t* dst = tb_dstream_cast(gst);
	tb_assert_and_check_return_val(dst && dst->data && dst->size, -1);

	// attach data
	dst->head = dst->data;

	// ok
	return 1;
}
static tb_long_t tb_dstream_aclose(tb_gstream_t* gst)
{
	tb_dstream_t* dst = tb_dstream_cast(gst);
	tb_assert_and_check_return_val(dst, -1);
	
	// reset head
	dst->head = TB_NULL;

	// ok
	return 1;
}
static tb_long_t tb_dstream_aread(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size)
{
	tb_dstream_t* dst = tb_dstream_cast(gst);
	tb_assert_and_check_return_val(dst && dst->data && dst->head && data, -1);
	tb_check_return_val(size, 0);

	// adjust size
	tb_size_t left = dst->data + dst->size - dst->head;
	if (size > left) size = left;

	// read data
	tb_memcpy(data, dst->head, size);
	dst->head += size;
	return (tb_long_t)(size);
}
static tb_long_t tb_dstream_awrit(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size)
{
	tb_dstream_t* dst = tb_dstream_cast(gst);
	tb_assert_and_check_return_val(dst && dst->data && dst->head && data, -1);
	tb_check_return_val(size, 0);

	// adjust size
	tb_size_t left = dst->data + dst->size - dst->head;
	if (size > left) size = left;

	// writ data
	tb_memcpy(dst->head, data, size);
	dst->head += size;
	return (tb_long_t)(size);
}
static tb_uint64_t tb_dstream_size(tb_gstream_t const* gst)
{
	tb_dstream_t* dst = tb_dstream_cast(gst);
	tb_assert_and_check_return_val(dst, 0);

	return dst->size;
}
static tb_bool_t tb_dstream_seek(tb_gstream_t* gst, tb_int64_t offset)
{
	tb_dstream_t* dst = tb_dstream_cast(gst);
	tb_assert_and_check_return_val(dst, TB_FALSE);

	// seek 
	dst->head = dst->data + offset;

	// ok
	return TB_TRUE;
}
static tb_long_t tb_dstream_wait(tb_gstream_t* gst, tb_size_t etype, tb_long_t timeout)
{
	tb_dstream_t* dst = tb_dstream_cast(gst);
	tb_assert_and_check_return_val(dst && dst->head <= dst->data + dst->size, -1);

	tb_long_t 	e = 0;
	if (etype & TB_AIOO_ETYPE_READ) e |= TB_AIOO_ETYPE_READ;
	if (etype & TB_AIOO_ETYPE_WRIT) e |= TB_AIOO_ETYPE_WRIT;
	return e;
}
static tb_bool_t tb_dstream_ctrl2(tb_gstream_t* gst, tb_size_t cmd, tb_pointer_t arg1, tb_pointer_t arg2)
{
	tb_dstream_t* dst = tb_dstream_cast(gst);
	tb_assert_and_check_return_val(dst, TB_FALSE);

	switch (cmd)
	{
	case TB_DSTREAM_CMD_SET_DATA:
		{
			tb_assert_and_check_return_val(arg1 && arg2, TB_FALSE);
			dst->data = (tb_byte_t*)arg1;
			dst->size = (tb_size_t)arg2;
			dst->head = TB_NULL;
		}
		return TB_TRUE;
	default:
		break;
	}
	return TB_FALSE;
}
/* ///////////////////////////////////////////////////////////////////////
 * interface implemention
 */
tb_gstream_t* tb_gstream_init_data()
{
	tb_gstream_t* gst = (tb_gstream_t*)tb_calloc(1, sizeof(tb_dstream_t));
	tb_assert_and_check_return_val(gst, TB_NULL);

	// init base
	if (!tb_gstream_init(gst)) goto fail;

	// init stream
	gst->type 	= TB_GSTREAM_TYPE_DATA;
	gst->aopen 	= tb_dstream_aopen;
	gst->aclose = tb_dstream_aclose;
	gst->aread 	= tb_dstream_aread;
	gst->awrit 	= tb_dstream_awrit;
	gst->size 	= tb_dstream_size;
	gst->seek 	= tb_dstream_seek;
	gst->wait 	= tb_dstream_wait;
	gst->ctrl2 = tb_dstream_ctrl2;

	// ok
	return gst;

fail:
	if (gst) tb_free(gst);
	return TB_NULL;
}
tb_gstream_t* tb_gstream_init_from_data(tb_byte_t const* data, tb_size_t size)
{
	tb_assert_and_check_return_val(data && size, TB_NULL);

	// init data stream
	tb_gstream_t* gst = tb_gstream_init_data();
	tb_assert_and_check_return_val(gst, TB_NULL);

	// set data & size
	if (!tb_gstream_ctrl2(gst, TB_DSTREAM_CMD_SET_DATA, (tb_pointer_t)data, (tb_pointer_t)size)) goto fail;
	
	return gst;

fail:
	if (gst) tb_gstream_exit(gst);
	return TB_NULL;
}
