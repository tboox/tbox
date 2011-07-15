/*!The Tiny Box Library
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

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../memory/memory.h"

/* /////////////////////////////////////////////////////////
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


/* /////////////////////////////////////////////////////////
 * details
 */
static __tb_inline__ tb_dstream_t* tb_dstream_cast(tb_gstream_t* gst)
{
	TB_ASSERT_RETURN_VAL(gst && gst->type == TB_GSTREAM_TYPE_DATA, TB_NULL);
	return (tb_dstream_t*)gst;
}
static tb_bool_t tb_dstream_open(tb_gstream_t* gst)
{
	tb_dstream_t* dst = tb_dstream_cast(gst);
	TB_ASSERT_RETURN_VAL(dst && dst->data && dst->size, TB_FALSE);

	dst->head = dst->data;
	return TB_TRUE;
}
static void tb_dstream_close(tb_gstream_t* gst)
{
	tb_dstream_t* dst = tb_dstream_cast(gst);
	if (dst) dst->head = TB_NULL;
}
static tb_int_t tb_dstream_read(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size)
{
	tb_dstream_t* dst = tb_dstream_cast(gst);
	TB_ASSERT_RETURN_VAL(dst && dst->data && dst->head && data, -1);
	TB_IF_FAIL_RETURN_VAL(size, 0);

	// adjust size
	tb_size_t left = dst->data + dst->size - dst->head;
	if (size > left) size = left;

	// read data
	tb_memcpy(data, dst->head, size);
	dst->head += size;
	return (tb_int_t)(size);
}
static tb_int_t tb_dstream_write(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size)
{
	tb_dstream_t* dst = tb_dstream_cast(gst);
	TB_ASSERT_RETURN_VAL(dst && dst->data && dst->head && data, -1);
	TB_IF_FAIL_RETURN_VAL(size, 0);

	// adjust size
	tb_size_t left = dst->data + dst->size - dst->head;
	if (size > left) size = left;

	// write data
	tb_memcpy(dst->head, data, size);
	dst->head += size;
	return (tb_int_t)(size);
}
static tb_byte_t* tb_dstream_need(tb_gstream_t* gst, tb_size_t size)
{
	tb_dstream_t* dst = tb_dstream_cast(gst);
	TB_ASSERT_RETURN_VAL(dst && dst->head && dst->data, TB_NULL);
	if (dst->head + size > dst->data + dst->size) return TB_NULL;

	return dst->head;
}
static tb_size_t tb_dstream_size(tb_gstream_t const* gst)
{
	tb_dstream_t* dst = tb_dstream_cast(gst);
	TB_ASSERT_RETURN_VAL(dst, 0);

	return dst->size;
}

static tb_size_t tb_dstream_offset(tb_gstream_t const* gst)
{
	tb_dstream_t* dst = tb_dstream_cast(gst);
	TB_ASSERT_RETURN_VAL(dst, 0);

	return (dst->head - dst->data);
}
static tb_bool_t tb_dstream_seek(tb_gstream_t* gst, tb_int_t offset, tb_gstream_seek_t flag)
{
	TB_NOT_IMPLEMENT();
	TB_ABORT();
	return TB_FALSE;
}

static tb_bool_t tb_dstream_ioctl2(tb_gstream_t* gst, tb_size_t cmd, void* arg1, void* arg2)
{
	tb_dstream_t* dst = tb_dstream_cast(gst);
	TB_ASSERT_RETURN_VAL(dst, TB_FALSE);

	switch (cmd)
	{
	case TB_DSTREAM_CMD_SET_DATA:
		{
			TB_ASSERT_RETURN_VAL(arg1 && arg2, TB_FALSE);
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
/* /////////////////////////////////////////////////////////
 * interface implemention
 */
tb_gstream_t* tb_gstream_create_data()
{
	tb_gstream_t* gst = (tb_gstream_t*)tb_calloc(1, sizeof(tb_dstream_t));
	TB_ASSERT_RETURN_VAL(gst, TB_NULL);

	// init stream
	gst->type 	= TB_GSTREAM_TYPE_DATA;
	gst->open 	= tb_dstream_open;
	gst->close 	= tb_dstream_close;
	gst->read 	= tb_dstream_read;
	gst->write 	= tb_dstream_write;
	gst->bread 	= tb_dstream_read;
	gst->bwrite = tb_dstream_write;
	gst->need 	= tb_dstream_need;
	gst->size 	= tb_dstream_size;
	gst->offset	= tb_dstream_offset;
	gst->seek 	= tb_dstream_seek;
	gst->ioctl2 = tb_dstream_ioctl2;

	return gst;
}
tb_gstream_t* tb_gstream_create_from_data(tb_byte_t const* data, tb_size_t size)
{
	TB_ASSERT_RETURN_VAL(data && size, TB_NULL);

	// create data stream
	tb_gstream_t* gst = tb_gstream_create_data();
	TB_ASSERT_RETURN_VAL(gst, TB_NULL);

	// set data & size
	if (TB_FALSE == tb_gstream_ioctl2(gst, TB_DSTREAM_CMD_SET_DATA, (void*)data, (void*)size)) goto fail;
	
	return gst;

fail:
	if (gst) tb_gstream_destroy(gst);
	return TB_NULL;
}
