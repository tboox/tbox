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
 * @author		ruki
 * @file		data.h
 * @ingroup 	object
 *
 */
#ifndef TB_OBJECT_DATA_H
#define TB_OBJECT_DATA_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init data from url
 *
 * @param data 		the data
 * @param size 		the size
 *
 * @return 			the data object
 */
tb_object_t* 		tb_data_init_from_url(tb_char_t const* url);

/*! init data from data
 *
 * @param data 		the data
 * @param size 		the size
 *
 * @return 			the data object
 */
tb_object_t* 		tb_data_init_from_data(tb_pointer_t data, tb_size_t size);

/*! init data from pbuffer
 *
 * @param pbuf 		the pbuffer
 *
 * @return 			the data object
 */
tb_object_t* 		tb_data_init_from_pbuf(tb_scoped_buffer_t* pbuf);

/*! get the data 
 *
 * @param data 		the data object
 *
 * @return 			the data address
 */
tb_pointer_t 		tb_data_getp(tb_object_t* data);

/*! set the data 
 *
 * @param data 		the data object
 * @param addr 		the data address
 * @param size 		the data size
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_data_setp(tb_object_t* data, tb_pointer_t addr, tb_size_t size);

/*! the data size
 *
 * @param data 		the data object
 *
 * @return 			the data size
 */
tb_size_t 			tb_data_size(tb_object_t* data);

/*! the data buffer
 *
 * @param data 		the data object
 *
 * @return 			the data buffer
 */
tb_scoped_buffer_t* 		tb_data_buff(tb_object_t* data);

/*! writ data to url
 *
 * @param data 		the data object
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_data_writ_to_url(tb_object_t* data, tb_char_t const* url);

#endif

