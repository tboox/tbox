/*!The Treasure Box dynamic
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
 * @file        mutex.h
 * @ingroup     dynamic
 *
 */
#ifndef TB_PLATFORM_DYNAMIC_H
#define TB_PLATFORM_DYNAMIC_H


/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init dynamic library
 * 
 * @param name      the library name
 *
 * @return          the library handle
 */
tb_handle_t         tb_dynamic_init(tb_char_t const* name);

/*! exit dynamic library
 * 
 * @param dynamic   the library handle
 */
tb_void_t           tb_dynamic_exit(tb_handle_t handle);

/*! the dynamic library function
 * 
 * @param handle    the library handle
 * @param name      the function name
 *
 * @return          the function address
 */
tb_pointer_t        tb_dynamic_func(tb_handle_t handle, tb_char_t const* name);

/*! the dynamic library variable
 * 
 * @param handle    the library handle
 * @param name      the variable name
 *
 * @return          the variable address
 */
tb_pointer_t        tb_dynamic_pvar(tb_handle_t handle, tb_char_t const* name);

#endif
