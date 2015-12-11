/*The Treasure Box Library
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
 * @file        native_large_pool.h
 *
 */
#ifndef TB_MEMORY_IMPL_NATIVE_LARGE_POOL_H
#define TB_MEMORY_IMPL_NATIVE_LARGE_POOL_H

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

/* init the native large pool and the allocated data will be aligned by the page size
 * 
 * @return              the pool 
 */
tb_large_pool_ref_t     tb_native_large_pool_init(tb_noarg_t);

/* exit the pool
 *
 * @param pool          the pool 
 */
tb_void_t               tb_native_large_pool_exit(tb_large_pool_ref_t pool);

/* clear the pool
 *
 * @param pool          the pool 
 */
tb_void_t               tb_native_large_pool_clear(tb_large_pool_ref_t pool);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
