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
 * \file		fixed.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include "fixed.h"

/* /////////////////////////////////////////////////////////
 * details
 */

static void tb_zstream_vlc_fixed_set(tb_zstream_vlc_t* vlc, tb_uint32_t val, tb_bstream_t* bst)
{
	TB_ASSERT(vlc);
	TB_ASSERT(val && val <= (0x1 << ((tb_zstream_vlc_fixed_t*)vlc)->nbits));
	tb_bstream_set_ubits(bst, val - 1, ((tb_zstream_vlc_fixed_t*)vlc)->nbits);
}
static tb_uint32_t tb_zstream_vlc_fixed_get(tb_zstream_vlc_t* vlc, tb_bstream_t const* bst)
{
	TB_ASSERT(vlc);
	return (tb_bstream_get_ubits(bst, ((tb_zstream_vlc_fixed_t*)vlc)->nbits) + 1);
}
/* /////////////////////////////////////////////////////////
 * interfaces
 */
tb_zstream_vlc_t* tb_zstream_vlc_fixed_open(tb_zstream_vlc_fixed_t* fixed, tb_byte_t nbits)
{
	// init
	memset(fixed, 0, sizeof(tb_zstream_vlc_fixed_t));
	((tb_zstream_vlc_t*)fixed)->type = TB_ZSTREAM_VLC_TYPE_FIXED;
	((tb_zstream_vlc_t*)fixed)->set = tb_zstream_vlc_fixed_set;
	((tb_zstream_vlc_t*)fixed)->get = tb_zstream_vlc_fixed_get;
	((tb_zstream_vlc_t*)fixed)->close = TB_NULL;
	fixed->nbits = nbits;

	// check
	TB_ASSERT(nbits <= 32);
	if (nbits > 32) return TB_NULL;

	return (tb_zstream_vlc_t*)fixed;
}
