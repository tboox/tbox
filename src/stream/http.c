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
 * \http		http.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../http.h"

/* /////////////////////////////////////////////////////////
 * details
 */
static tb_bool_t tb_http_stream_seek(tb_stream_t* st, tb_int_t offset, tb_stream_seek_t flag)
{
	if (st && !(st->flag & TB_STREAM_FLAG_ZLIB))
	{
		// get http
		tb_http_t* http = st->pdata;
		TB_ASSERT(http);
		if (!http) return 0;



		return TB_TRUE;
	}
	else return TB_FALSE;
}

static tb_int_t tb_http_stream_read(tb_stream_t* st, tb_byte_t* data, tb_size_t size)
{
	TB_ASSERT(data && size);
	if (st && st->pdata) return tb_http_recv_data((tb_http_t*)st->pdata, data, size, TB_FALSE);
	else return -1;
}
static void tb_http_stream_close(tb_stream_t* st)
{
	if (st)
	{
		if (st->pdata) tb_http_destroy((tb_http_t*)st->pdata);
		st->pdata = TB_NULL;
	}
}
static tb_size_t tb_http_stream_size(tb_stream_t* st)
{
	if (st && st->pdata && !(st->flag & TB_STREAM_FLAG_ZLIB)) return tb_http_size((tb_http_t*)st->pdata);
	else return 0;
}
/* /////////////////////////////////////////////////////////
 * interfaces
 */
tb_stream_t* tb_stream_open_from_http(tb_http_stream_t* st, tb_char_t const* url, tb_stream_flag_t flag)
{
	TB_ASSERT(st && url);
	if (!st || !url) return TB_NULL;
	
	// create http
	tb_http_t* http = tb_http_create();
	if (!http) return TB_NULL;

	// init stream
	memset(st, 0, sizeof(tb_http_stream_t));
	st->base.flag = flag;
	st->base.head = st->base.data;
	st->base.size = 0;
	st->base.offset = 0;
	st->base.pdata = http;

	// init http stream
	st->base.read = tb_http_stream_read;
	st->base.close = tb_http_stream_close;
	st->base.ssize = tb_http_stream_size;

	// connect to host
	if (TB_FALSE == tb_http_open(http, url, TB_NULL, TB_HTTP_METHOD_GET)) goto fail;
	TB_DBG("connect ok!");

	// save url
	tb_string_init(&st->base.url);
	tb_http_url(http, &st->base.url);

#if 0
	// is able to seek?
	if (TB_TRUE == tb_http_stream(http)) st->base.seek = TB_NULL;
	else st->base.seek = tb_http_stream_seek;
#endif

#ifdef TB_CONFIG_ZLIB
	// is hzlib?
	if (flag & TB_STREAM_FLAG_ZLIB)
	{
		st->base.hzlib = tb_zlib_create();
		if (st->base.hzlib == TB_INVALID_HANDLE) goto fail;
	}
#endif
	return ((tb_stream_t*)st);

fail:
	if (http) tb_http_destroy(http);
	return TB_NULL;
	// }
}
