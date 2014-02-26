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
#include "../../utils/utils.h"
#include "../../memory/memory.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the data stream type
typedef struct __tb_gstream_data_t
{
	// the base
	tb_gstream_t 		base;

	// the data
	tb_byte_t* 			data;

	// the head
	tb_byte_t* 			head;

	// the size
	tb_size_t 			size;

	// the data is referenced?
	tb_bool_t 			bref;

}tb_gstream_data_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_gstream_data_t* tb_gstream_data_cast(tb_handle_t stream)
{
	tb_gstream_t* gstream = (tb_gstream_t*)stream;
	tb_assert_and_check_return_val(gstream && gstream->base.type == TB_STREAM_TYPE_DATA, tb_null);
	return (tb_gstream_data_t*)gstream;
}
static tb_long_t tb_gstream_data_open(tb_handle_t gstream)
{
	// check
	tb_gstream_data_t* dstream = tb_gstream_data_cast(gstream);
	tb_assert_and_check_return_val(dstream && dstream->data && dstream->size, -1);

	// init head
	dstream->head = dstream->data;

	// ok
	return 1;
}
static tb_long_t tb_gstream_data_clos(tb_handle_t gstream)
{
	// check
	tb_gstream_data_t* dstream = tb_gstream_data_cast(gstream);
	tb_assert_and_check_return_val(dstream, -1);
	
	// clear head
	dstream->head = tb_null;

	// ok
	return 1;
}
static tb_void_t tb_gstream_data_exit(tb_handle_t gstream)
{
	// check
	tb_gstream_data_t* dstream = tb_gstream_data_cast(gstream);
	tb_assert_and_check_return(dstream);
	
	// clear head
	dstream->head = tb_null;

	// exit data
	if (dstream->data && !dstream->bref) tb_free(dstream->data);
	dstream->data = tb_null;
	dstream->size = 0;
}
static tb_long_t tb_gstream_data_read(tb_handle_t gstream, tb_byte_t* data, tb_size_t size, tb_bool_t sync)
{
	// check
	tb_gstream_data_t* dstream = tb_gstream_data_cast(gstream);
	tb_assert_and_check_return_val(dstream && dstream->data && dstream->head, -1);

	// check
	tb_check_return_val(data, -1);
	tb_check_return_val(size, 0);

	// the left
	tb_size_t left = dstream->data + dstream->size - dstream->head;

	// the need
	if (size > left) size = left;

	// read data
	if (size) tb_memcpy(data, dstream->head, size);

	// save head
	dstream->head += size;

	// ok?
	return (tb_long_t)(size);
}
static tb_long_t tb_gstream_data_writ(tb_handle_t gstream, tb_byte_t const* data, tb_size_t size, tb_bool_t sync)
{
	// check
	tb_gstream_data_t* dstream = tb_gstream_data_cast(gstream);
	tb_assert_and_check_return_val(dstream && dstream->data && dstream->head, -1);

	// check
	tb_check_return_val(data, -1);
	tb_check_return_val(size, 0);

	// the left
	tb_size_t left = dstream->data + dstream->size - dstream->head;

	// the need
	if (size > left) size = left;

	// writ data
	if (size) tb_memcpy(dstream->head, data, size);

	// save head
	dstream->head += size;

	// ok?
	return left? (tb_long_t)(size) : -1; // force end if full
}
static tb_long_t tb_gstream_data_seek(tb_handle_t gstream, tb_hize_t offset)
{
	// check
	tb_gstream_data_t* dstream = tb_gstream_data_cast(gstream);
	tb_assert_and_check_return_val(dstream && offset <= dstream->size, -1);

	// seek 
	dstream->head = dstream->data + offset;

	// ok
	return 1;
}
static tb_long_t tb_gstream_data_wait(tb_handle_t gstream, tb_size_t wait, tb_long_t timeout)
{
	// check
	tb_gstream_data_t* dstream = tb_gstream_data_cast(gstream);
	tb_assert_and_check_return_val(dstream && dstream->head <= dstream->data + dstream->size, -1);

	// wait 
	tb_long_t aioe = 0;
	if (!tb_gstream_beof(gstream))
	{
		if (wait & TB_GSTREAM_WAIT_READ) aioe |= TB_GSTREAM_WAIT_READ;
		if (wait & TB_GSTREAM_WAIT_WRIT) aioe |= TB_GSTREAM_WAIT_WRIT;
	}

	// ok?
	return aioe;
}
static tb_bool_t tb_gstream_data_ctrl(tb_handle_t gstream, tb_size_t ctrl, tb_va_list_t args)
{
	// check
	tb_gstream_data_t* dstream = tb_gstream_data_cast(gstream);
	tb_assert_and_check_return_val(dstream, tb_false);

	// ctrl
	switch (ctrl)
	{
	case TB_STREAM_CTRL_GET_SIZE:
		{
			// the psize
			tb_hong_t* psize = (tb_hong_t*)tb_va_arg(args, tb_hong_t*);
			tb_assert_and_check_return_val(psize, tb_false);

			// get size
			*psize = dstream->size;
			return tb_true;
		}	
	case TB_STREAM_CTRL_GET_OFFSET:
		{
			// the poffset
			tb_hize_t* poffset = (tb_hize_t*)tb_va_arg(args, tb_hize_t*);
			tb_assert_and_check_return_val(poffset, tb_false);

			// get offset
			*poffset = dstream->base.offset;
			return tb_true;
		}
	case TB_STREAM_CTRL_DATA_SET_DATA:
		{
			// exit data first if exists
			if (dstream->data && !dstream->bref) tb_free(dstream->data);

			// save data
			dstream->data = (tb_byte_t*)tb_va_arg(args, tb_byte_t*);
			dstream->size = (tb_size_t)tb_va_arg(args, tb_size_t);
			dstream->head = tb_null;
			dstream->bref = tb_true;

			// check
			tb_assert_and_check_return_val(dstream->data && dstream->size, tb_false);
			return tb_true;
		}
	case TB_STREAM_CTRL_SET_URL:
		{
			// check
			tb_assert_and_check_return_val(!tb_stream_is_opened(gstream), tb_false);

			// set url
			tb_char_t const* url = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
			tb_assert_and_check_return_val(url, tb_false); 
			
			// the url size
			tb_size_t url_size = tb_strlen(url);
			tb_assert_and_check_return_val(url_size > 7, tb_false);

			// the base64 data and size
			tb_char_t const* 	base64_data = url + 7;
			tb_size_t 			base64_size = url_size - 7;

			// make data
			tb_size_t 	maxn = base64_size;
			tb_byte_t* 	data = tb_malloc(maxn); 
			tb_assert_and_check_return_val(data, tb_false);

			// decode base64 data
			tb_size_t 	size = tb_base64_decode(base64_data, base64_size, data, maxn);
			tb_assert_and_check_return_val(size, tb_false);

			// exit data first if exists
			if (dstream->data && !dstream->bref) tb_free(dstream->data);

			// save data
			dstream->data = data;
			dstream->size = size;
			dstream->bref = tb_false;
			dstream->head = tb_null;

			// ok
			return tb_true;
		}
		break;
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
	tb_gstream_t* gstream = (tb_gstream_t*)tb_malloc0(sizeof(tb_gstream_data_t));
	tb_assert_and_check_return_val(gstream, tb_null);

	// init stream
	if (!tb_gstream_init(gstream, TB_STREAM_TYPE_DATA, 0)) goto fail;

	// init func
	gstream->open 		= tb_gstream_data_open;
	gstream->clos 		= tb_gstream_data_clos;
	gstream->exit 		= tb_gstream_data_exit;
	gstream->read 		= tb_gstream_data_read;
	gstream->writ 		= tb_gstream_data_writ;
	gstream->seek 		= tb_gstream_data_seek;
	gstream->wait 		= tb_gstream_data_wait;
	gstream->base.ctrl 	= tb_gstream_data_ctrl;

	// ok
	return gstream;

fail:
	if (gstream) tb_gstream_exit(gstream);
	return tb_null;
}
tb_gstream_t* tb_gstream_init_from_data(tb_byte_t const* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(data && size, tb_null);

	// init data stream
	tb_gstream_t* gstream = tb_gstream_init_data();
	tb_assert_and_check_return_val(gstream, tb_null);

	// set data & size
	if (!tb_stream_ctrl(gstream, TB_STREAM_CTRL_DATA_SET_DATA, data, size)) goto fail;
	
	// ok
	return gstream;

fail:
	if (gstream) tb_gstream_exit(gstream);
	return tb_null;
}
