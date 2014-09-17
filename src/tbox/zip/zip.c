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
 * @file        zip.c
 * @ingroup     zip
 *
 */
/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "zip.h"
#include "rlc.h"
#include "gzip.h"
#include "zlib.h"
#include "zlibraw.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_zip_ref_t tb_zip_init(tb_size_t algo, tb_size_t action)
{
    // table
    static tb_zip_ref_t (*s_init[])(tb_size_t action) =
    {
        tb_null
    ,   tb_zip_rlc_init
    ,   tb_null
    ,   tb_null
    ,   tb_null
#ifdef TB_CONFIG_THIRD_HAVE_ZLIB
    ,   tb_zip_zlibraw_init
    ,   tb_zip_zlib_init
    ,   tb_zip_gzip_init
#else
    ,   tb_null
    ,   tb_null
    ,   tb_null
#endif
    ,   tb_null
    };
    tb_assert_and_check_return_val(algo < tb_arrayn(s_init) && s_init[algo], tb_null);

    // init
    return s_init[algo](action);
}
tb_void_t tb_zip_exit(tb_zip_ref_t zip)
{
    // check
    tb_assert_and_check_return(zip);

    // table
    static tb_void_t (*s_exit[])(tb_zip_ref_t zip) =
    {
        tb_null
    ,   tb_zip_rlc_exit
    ,   tb_null
    ,   tb_null
    ,   tb_null
#ifdef TB_CONFIG_THIRD_HAVE_ZLIB
    ,   tb_zip_zlibraw_exit
    ,   tb_zip_zlib_exit
    ,   tb_zip_gzip_exit
#else
    ,   tb_null
    ,   tb_null
    ,   tb_null
#endif
    ,   tb_null
    };
    tb_assert_and_check_return(zip->algo < tb_arrayn(s_exit) && s_exit[zip->algo]);

    // exit
    s_exit[zip->algo](zip);
}
tb_long_t tb_zip_spak(tb_zip_ref_t zip, tb_static_stream_ref_t ist, tb_static_stream_ref_t ost, tb_long_t sync)
{
    tb_assert_and_check_return_val(zip && zip->spak && ist && ost, -1);
    return zip->spak(zip, ist, ost, sync);
}

