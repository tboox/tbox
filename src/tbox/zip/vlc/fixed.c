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
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author      ruki
 * @file        fixed.c
 *
 */
/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

static tb_void_t tb_zip_vlc_fixed_set(tb_zip_vlc_t* vlc, tb_uint32_t val, tb_static_stream_ref_t sstream)
{
    tb_assert(vlc);
    tb_assert(val && val <= (0x1 << ((tb_zip_vlc_fixed_t*)vlc)->nbits));
    tb_static_stream_writ_ubits32(sstream, val - 1, ((tb_zip_vlc_fixed_t*)vlc)->nbits);
}
static tb_uint32_t tb_zip_vlc_fixed_get(tb_zip_vlc_t* vlc, tb_static_stream_ref_t sstream)
{
    tb_assert(vlc);
    return (tb_static_stream_read_ubits32((tb_static_stream_ref_t)sstream, ((tb_zip_vlc_fixed_t*)vlc)->nbits) + 1);
}
/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_zip_vlc_t* tb_zip_vlc_fixed_open(tb_zip_vlc_fixed_t* fixed, tb_byte_t nbits)
{
    // init
    tb_memset(fixed, 0, sizeof(tb_zip_vlc_fixed_t));
    ((tb_zip_vlc_t*)fixed)->type = TB_ZIP_VLC_TYPE_FIXED;
    ((tb_zip_vlc_t*)fixed)->set = tb_zip_vlc_fixed_set;
    ((tb_zip_vlc_t*)fixed)->get = tb_zip_vlc_fixed_get;
    ((tb_zip_vlc_t*)fixed)->clos = tb_null;
    fixed->nbits = nbits;

    // check
    tb_assert_and_check_return_val(nbits <= 32, tb_null);

    return (tb_zip_vlc_t*)fixed;
}
