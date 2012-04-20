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
 * @file		event.h
 * @ingroup 	platform
 *
 */
#ifndef TB_PLATFORM_EVENT_H
#define TB_PLATFORM_EVENT_H


/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_handle_t 	tb_event_init(tb_char_t const* name);
tb_void_t 		tb_event_exit(tb_handle_t handle);

tb_void_t 		tb_event_post(tb_handle_t handle);
tb_long_t 		tb_event_wait(tb_handle_t handle, tb_long_t timeout);

	
#endif
