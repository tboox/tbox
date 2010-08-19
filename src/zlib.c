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
 * along with TGraphic; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		zlib.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "zlib.h"
#include "external/external.h"

/* /////////////////////////////////////////////////////////
 * types
 */

// the zlib type 
typedef struct __tb_zlib_t
{
	// zlib stream
	z_stream 			st;
	
}tb_zlib_t;
/* /////////////////////////////////////////////////////////
 * interface
 */


tb_handle_t tb_zlib_create()
{
	// create zlib
	tb_zlib_t* z = tb_malloc(sizeof(tb_zlib_t));
	if (!z) return TB_INVALID_HANDLE;

	// init zlib
	memset(z, 0, sizeof(tb_zlib_t));

	// inflate init
    if (inflateInit(&z->st) != Z_OK)
        goto fail;

	return (tb_handle_t)z;

fail:
	if (z) tb_free(z);
	return TB_INVALID_HANDLE;
}
void tb_zlib_attach(tb_handle_t hz, tb_byte_t const* data, tb_size_t size)
{
	TB_ASSERT(hz != TB_INVALID_HANDLE);
	tb_zlib_t* pz = (tb_zlib_t*)hz;
	TB_ASSERT(pz && data);

	// attach input
	pz->st.next_in = (Bytef*)data;
	pz->st.avail_in = (uInt)size;
}
tb_bool_t tb_zlib_inflate_partial(tb_handle_t hz, tb_byte_t* data, tb_size_t* size)
{
	TB_ASSERT(hz != TB_INVALID_HANDLE);
	tb_zlib_t* pz = (tb_zlib_t*)hz;
	TB_ASSERT(pz && data && size);

	// init
	pz->st.next_out = (Bytef*)data;
	pz->st.avail_out = (uInt)*size;
	*size = 0;

	// check input
	if (!pz->st.next_in || !pz->st.avail_in) 
		return TB_FALSE;

	// save old total out_n
	tb_size_t out_n = (tb_size_t)pz->st.total_out;

	// inflate partial data
	tb_int_t ret = inflate(&pz->st, Z_PARTIAL_FLUSH);
	if (ret != Z_OK && ret != Z_STREAM_END) return TB_FALSE;

	// return the current out_n
	*size = pz->st.total_out - out_n;
	return TB_TRUE;
}
tb_size_t tb_zlib_left(tb_handle_t hz)
{
	TB_ASSERT(hz != TB_INVALID_HANDLE);
	tb_zlib_t* pz = (tb_zlib_t*)hz;
	TB_ASSERT(pz);

	return (tb_size_t)pz->st.avail_in;
}
void tb_zlib_destroy(tb_handle_t hz)
{
	TB_ASSERT(hz != TB_INVALID_HANDLE);
	tb_zlib_t* pz = (tb_zlib_t*)hz;
	if (pz)
	{
		inflateEnd(&(pz->st));
		tb_free(pz);
	}
}

