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
 * @file		event.h
 * @ingroup 	platform
 *
 */
#ifndef TB_PLATFORM_EVENT_H
#define TB_PLATFORM_EVENT_H


/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init event
 * 
 * @return 			the event handle
 */
tb_handle_t 		tb_event_init(tb_noarg_t);

/*! exit event
 * 
 * @param handle 	the event handle
 */
tb_void_t 			tb_event_exit(tb_handle_t handle);

/*! post event
 * 
 * @param handle 	the event handle
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_event_post(tb_handle_t handle);

/*! wait event
 * 
 * @param handle 	the event handle
 * @param timeout 	the timeout
 *
 * @return 			ok: 1, timeout: 0, fail: -1
 */
tb_long_t 			tb_event_wait(tb_handle_t handle, tb_long_t timeout);

	
#endif
