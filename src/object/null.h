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
 * @file		null.h
 * @ingroup 	object
 *
 */
#ifndef TB_OBJECT_NULL_H
#define TB_OBJECT_NULL_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init null reader
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_null_init_reader();

/*! init null writer
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_null_init_writer();

/*! init null
 *
 * @return 			the null object
 */
tb_object_t const* 	tb_null_init();


#endif

