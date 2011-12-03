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
 * \sock		sock.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../string/string.h"
#include "../../platform/platform.h"

/* /////////////////////////////////////////////////////////
 * macros
 */


/* /////////////////////////////////////////////////////////
 * types
 */

// the sock stream type
typedef struct __tb_sstream_t
{
	// the base
	tb_gstream_t 		base;

	// the sock handle
	tb_handle_t 		sock;

}tb_sstream_t;


/* /////////////////////////////////////////////////////////
 * details
 */
static __tb_inline__ tb_sstream_t* tb_sstream_cast(tb_gstream_t* gst)
{
	tb_assert_and_check_return_val(gst && gst->type == TB_GSTREAM_TYPE_SOCK, TB_NULL);
	return (tb_sstream_t*)gst;
}
static tb_bool_t tb_sstream_open(tb_gstream_t* gst)
{
	tb_sstream_t* sst = tb_sstream_cast(gst);
	tb_assert_and_check_return_val(sst && !sst->sock && gst->url, TB_FALSE);

	return TB_TRUE;
}
static tb_void_t tb_sstream_close(tb_gstream_t* gst)
{
	tb_sstream_t* sst = tb_sstream_cast(gst);
	if (sst && sst->sock)
	{
	}
}
static tb_long_t tb_sstream_read(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size)
{
	tb_sstream_t* sst = tb_sstream_cast(gst);
	tb_assert_and_check_return_val(sst && sst->sock && data, -1);
	tb_check_return_val(size, 0);

	return 0;
}
static tb_long_t tb_sstream_writ(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size)
{
	tb_sstream_t* sst = tb_sstream_cast(gst);
	tb_assert_and_check_return_val(sst && sst->sock && data, -1);
	tb_check_return_val(size, 0);

	return 0;
}
static tb_bool_t tb_sstream_ioctl1(tb_gstream_t* gst, tb_size_t cmd, tb_pointer_t arg1)
{
	tb_sstream_t* sst = tb_sstream_cast(gst);
	tb_assert_and_check_return_val(sst, TB_FALSE);

	switch (cmd)
	{
	default:
		break;
	}
	return TB_FALSE;
}
/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_gstream_t* tb_gstream_init_sock()
{
	tb_gstream_t* gst = (tb_gstream_t*)tb_calloc(1, sizeof(tb_sstream_t));
	tb_assert_and_check_return_val(gst, TB_NULL);

	// init stream
	tb_sstream_t* sst = (tb_sstream_t*)gst;
	gst->type 	= TB_GSTREAM_TYPE_FILE;
	gst->open 	= tb_sstream_open;
	gst->close 	= tb_sstream_close;
	gst->read 	= tb_sstream_read;
	gst->writ 	= tb_sstream_writ;
	gst->ioctl1 = tb_sstream_ioctl1;
	sst->sock 	= TB_NULL;

	return gst;
}

