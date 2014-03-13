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
 * @file		opool.h
 * @ingroup 	object
 *
 */
#ifndef TB_MEMORY_OPOOL_H
#define TB_MEMORY_OPOOL_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
#define tb_opool_get(size, flag, type) 			tb_opool_get_impl(size, flag, type __tb_debug_vals__)
#define tb_opool_del(object) 					tb_opool_del_impl(object __tb_debug_vals__)

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init object pool
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_opool_init(tb_noarg_t);

/// exit opool
tb_void_t 			tb_opool_exit(tb_noarg_t);

/// clear opool
tb_void_t 			tb_opool_clear(tb_noarg_t);

/*! get object from opool
 *
 * @param size 		the object size
 * @param flag 		the object flag
 * @param type 		the object type
 *
 * @return 			the object pointer
 */
tb_object_t* 		tb_opool_get_impl(tb_size_t size, tb_size_t flag, tb_size_t type __tb_debug_decl__);

/*! del object from opool
 *
 * @param object 	the object pointer
 */
tb_void_t 			tb_opool_del_impl(tb_object_t* object __tb_debug_decl__);

/// dump opool
tb_void_t 			tb_opool_dump(tb_noarg_t);

#endif
