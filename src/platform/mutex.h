/*!The Treasure Platform Library
 * 
 * TPlat is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TPlat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TPlat; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		mutex.h
 *
 */
#ifndef TB_PLATFORM_MUTEX_H
#define TB_PLATFORM_MUTEX_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif


/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_handle_t 	tb_mutex_init(tb_char_t const* name);
tb_void_t 		tb_mutex_exit(tb_handle_t hmutex);

tb_bool_t 		tb_mutex_enter(tb_handle_t hmutex);
tb_bool_t 		tb_mutex_leave(tb_handle_t hmutex);

tb_bool_t 		tb_mutex_tryenter(tb_handle_t hmutex);
	
// c plus plus
#ifdef __cplusplus
}
#endif

#endif
