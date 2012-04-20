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
 * @file		zip.h
 *
 */
#ifndef TB_ZIP_H
#define TB_ZIP_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "rlc.h"
#include "gzip.h"
#include "zlib.h"
#include "zlibraw.h"

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

// open & close
tb_zip_t* 			tb_zip_init(tb_size_t algo, tb_size_t action);
tb_void_t 			tb_zip_exit(tb_zip_t* zip);

// spak
tb_long_t 			tb_zip_spak(tb_zip_t* zip, tb_bstream_t* ist, tb_bstream_t* ost, tb_bool_t sync);

#endif
