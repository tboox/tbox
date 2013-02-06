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
 * @file		data.h
 * @ingroup 	object
 *
 */
#ifndef TB_OBJECT_DATA_H
#define TB_OBJECT_DATA_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init data 
 *
 * @param data 		the data
 * @param size 		the size
 *
 * @return 			the data object
 */
tb_object_t* 	tb_data_init(tb_pointer_t data, tb_size_t size);

/*! the data address
 *
 * @param data 		the data object
 *
 * @return 			the data address
 */
tb_pointer_t 	tb_data_addr(tb_object_t* data);

/*! the data size
 *
 * @param data 		the data object
 *
 * @return 			the data size
 */
tb_size_t 		tb_data_size(tb_object_t* data);

/*! the data buffer
 *
 * @param data 		the data object
 *
 * @return 			the data buffer
 */
tb_pbuffer_t* 	tb_data_buff(tb_object_t* data);

#endif

