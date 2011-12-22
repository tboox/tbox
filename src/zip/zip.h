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
 * \file		zip.h
 *
 */
#ifndef TB_ZIP_H
#define TB_ZIP_H

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "rlc.h"
#include "gzip.h"
#include "zlib.h"
#include "zlibraw.h"

/* /////////////////////////////////////////////////////////
 * types
 */

// the zip package type
typedef union __tb_zip_package_t
{
	tb_zip_rlc_t 		rlc;
	tb_zip_gzip_t 		gzip;
	tb_zip_zlib_t 		zlib;
	tb_zip_zlibraw_t 	zlibraw;

}tb_zip_package_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */

// open & close
tb_zip_t* 			tb_zip_open(tb_zip_package_t* package, tb_zip_algo_t algo, tb_zip_action_t action);
tb_void_t 				tb_zip_close(tb_zip_t* zip);

// spank
tb_zip_status_t 	tb_zip_spank(tb_zip_t* zip, tb_bstream_t* ist, tb_bstream_t* ost);

#endif
