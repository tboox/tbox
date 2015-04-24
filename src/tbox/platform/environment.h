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
 * @file        environment.h
 * @ingroup     platform
 *
 */
#ifndef TB_PLATFORM_ENVIRONMENT_H
#define TB_PLATFORM_ENVIRONMENT_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! get the environment variable
 *
 * @param name      the variable name
 * @param value     the variable value 
 * @param maxn      the variable value maximum size
 *
 * @return          the variable value size 
 */
tb_size_t           tb_environment_get(tb_char_t const* name, tb_char_t* value, tb_size_t maxn);

/*! set the environment variable and will overwrite it
 *
 * @param name      the variable name
 * @param value     the variable value
 *
 * @return          tb_true or tb_false
 */
tb_bool_t           tb_environment_set(tb_char_t const* name, tb_char_t const* value);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
