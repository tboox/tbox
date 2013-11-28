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
 * @file		spinlock.h
 * @ingroup 	platform
 *
 */
#ifndef TB_PLATFORM_SPINLOCK_H
#define TB_PLATFORM_SPINLOCK_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "sched.h"
#include "atomic.h"

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init spinlock
 *
 * @return 			the spinlock handle
 */
tb_handle_t 		tb_spinlock_init(tb_noarg_t);

/* exit spinlock
 *
 * @param handle 	the spinlock handle
 */
tb_void_t 			tb_spinlock_exit(tb_handle_t handle);

#ifndef TB_CONFIG_SPINLOCK_ATOMIC_ENABLE

/* enter spinlock
 *
 * @param handle 	the spinlock handle
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_spinlock_enter(tb_handle_t handle);

/* try to enter spinlock
 *
 * @param handle 	the spinlock handle
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_spinlock_enter_try(tb_handle_t handle);

/* leave spinlock
 *
 * @param handle 	the spinlock handle
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_spinlock_leave(tb_handle_t handle);

#else

static __tb_inline_force__ tb_bool_t tb_spinlock_enter(tb_handle_t handle)
{
	// check
	tb_assert_and_check_return_val(handle, tb_false);

	// init tryn
	__tb_volatile__ tb_size_t tryn = 5;

	// spin lock
#if defined(TB_COMPILER_IS_GCC) \
	&& !defined(TB_CONFIG_COMPILER_NOT_SUPPORT_BUILTIN_FUNCTIONS) \
	&&	TB_COMPILER_VERSION_BE(4, 1)
	while (*((tb_atomic_t*)handle) && !__sync_bool_compare_and_swap((tb_atomic_t*)handle, 0, 1))
#else
	while (*((tb_atomic_t*)handle) && tb_atomic_fetch_and_pset((tb_atomic_t*)handle, 0, 1))
#endif
	{
		if (!tryn--)
		{
			// yield the processor
			tb_sched_yield();
//			tb_usleep(1);

			// reset tryn
			tryn = 5;
		}
	}

	// ok
	return tb_true;
}

static __tb_inline_force__ tb_bool_t tb_spinlock_enter_try(tb_handle_t handle)
{
	// check
	tb_assert_and_check_return_val(handle, tb_false);

#if defined(TB_COMPILER_IS_GCC) \
	&& !defined(TB_CONFIG_COMPILER_NOT_SUPPORT_BUILTIN_FUNCTIONS) \
	&&	TB_COMPILER_VERSION_BE(4, 1)
	return (!*((tb_atomic_t*)handle) && __sync_bool_compare_and_swap((tb_atomic_t*)handle, 0, 1))? tb_true : tb_false;
#else
	return (!*((tb_atomic_t*)handle) && !tb_atomic_fetch_and_pset((tb_atomic_t*)handle, 0, 1))? tb_true : tb_false;
#endif
}
static __tb_inline_force__ tb_bool_t tb_spinlock_leave(tb_handle_t handle)
{
	// check
	tb_assert_and_check_return_val(handle, tb_false);

    *((tb_atomic_t*)handle) = 0;
	return tb_true;
}

#endif

#endif
