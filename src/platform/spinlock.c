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
 * @file		spinlock.c
 * @ingroup 	platform
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "spinlock.h"
#ifndef TB_CONFIG_SPINLOCK_ATOMIC_ENABLE
# 	if defined(TB_CONFIG_OS_WINDOWS)
# 		include "windows/spinlock.c"
# 	else
# 		include "unix/spinlock.c"
# 	endif 
#endif

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */

#ifdef TB_CONFIG_SPINLOCK_ATOMIC_ENABLE
tb_handle_t tb_spinlock_init()
{
	return (tb_handle_t)tb_malloc0(sizeof(tb_atomic_t));
}
tb_void_t tb_spinlock_exit(tb_handle_t handle)
{
	if (handle) tb_free(handle);
}
#endif

