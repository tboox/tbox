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
 * @file        semaphore.h
 * @ingroup     platform
 *
 */
#ifndef TB_PLATFORM_SEMAPHORE_H
#define TB_PLATFORM_SEMAPHORE_H


/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init semaphore
 *
 * @param init      the init semaphore value
 * 
 * @return          the semaphore handle
 */
tb_handle_t         tb_semaphore_init(tb_size_t init);

/*! exit semaphore
 * 
 * @param handle    the semaphore handle
 */
tb_void_t           tb_semaphore_exit(tb_handle_t handle);

/*! post semaphore
 * 
 * @param handle    the semaphore handle
 * @param post      the post semaphore value
 *
 * @return          tb_true or tb_false
 */
tb_bool_t           tb_semaphore_post(tb_handle_t handle, tb_size_t post);

/*! the semaphore value
 * 
 * @param handle    the semaphore handle
 *
 * @return          >= 0: the semaphore value, -1: failed
 */
tb_long_t           tb_semaphore_value(tb_handle_t handle);

/*! wait semaphore
 * 
 * @param handle    the semaphore handle
 * @param timeout   the timeout
 *
 * @return          ok: 1, timeout: 0, fail: -1
 */
tb_long_t           tb_semaphore_wait(tb_handle_t handle, tb_long_t timeout);

    
#endif
