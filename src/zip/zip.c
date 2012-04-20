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
 * @file		zip.c
 * @ingroup 	zip
 *
 */
/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "zip.h"

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_zip_t* tb_zip_init(tb_size_t algo, tb_size_t action)
{
	// table
	static tb_zip_t* (*s_init[])(tb_size_t action) =
	{
		TB_NULL
	, 	tb_zip_rlc_init
	, 	TB_NULL
	, 	TB_NULL
	, 	TB_NULL
	, 	tb_zip_zlibraw_init
	, 	tb_zip_zlib_init
	, 	tb_zip_gzip_init
	, 	TB_NULL
	};
	tb_assert_and_check_return_val(algo < tb_arrayn(s_init) && s_init[algo], TB_NULL);

	// init
	return s_init[algo](action);
}
tb_void_t tb_zip_exit(tb_zip_t* zip)
{
	// check
	tb_assert_and_check_return(zip);

	// table
	static tb_void_t (*s_exit[])(tb_zip_t* zip) =
	{
		TB_NULL
	, 	tb_zip_rlc_exit
	, 	TB_NULL
	, 	TB_NULL
	, 	TB_NULL
	, 	tb_zip_zlibraw_exit
	, 	tb_zip_zlib_exit
	, 	tb_zip_gzip_exit
	, 	TB_NULL
	};
	tb_assert_and_check_return_val(zip->algo < tb_arrayn(s_exit) && s_exit[zip->algo], TB_NULL);

	// exit
	s_exit[zip->algo](zip);
}
tb_long_t tb_zip_spak(tb_zip_t* zip, tb_bstream_t* ist, tb_bstream_t* ost, tb_bool_t sync)
{
	tb_assert_and_check_return_val(zip && zip->spak && ist && ost, -1);
	return zip->spak(zip, ist, ost, sync);
}

