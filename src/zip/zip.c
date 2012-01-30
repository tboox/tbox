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
 * \file		zip.c
 *
 */
/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "zip.h"

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_zip_t* tb_zip_open(tb_zip_package_t* package, tb_zip_algo_t algo, tb_zip_action_t action)
{
	tb_assert_and_check_return_val(package, TB_NULL);
	switch (algo)
	{
	case TB_ZIP_ALGO_RLC:
		return tb_zip_rlc_open(&package->rlc, action);
	case TB_ZIP_ALGO_ZLIBRAW:
		return tb_zip_zlibraw_open(&package->zlibraw, action);
	case TB_ZIP_ALGO_ZLIB:
		return tb_zip_zlib_open(&package->zlib, action);
	case TB_ZIP_ALGO_GZIP:
		return tb_zip_gzip_open(&package->gzip, action);
	default:
		break;
	}
	return TB_NULL;
}
tb_void_t tb_zip_close(tb_zip_t* zip)
{
	if (zip && zip->close) zip->close(zip);
}
tb_zip_status_t tb_zip_spank(tb_zip_t* zip, tb_bstream_t* ist, tb_bstream_t* ost)
{
	tb_assert_and_check_return_val(zip && zip->spank && ist && ost, TB_ZIP_STATUS_FAIL);
	return zip->spank(zip, ist, ost);
}

