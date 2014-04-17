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
 * @file		spinlock.h
 * @ingroup 	platform
 *
 */
#ifndef TB_PLATFORM_SPINLOCK_H
#define TB_PLATFORM_SPINLOCK_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "sched.h"
#include "atomic.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the initial value
#define TB_SPINLOCK_INIT 			(0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init spinlock
 *
 * @param lock 		the lock
 *
 * @return 			tb_true or tb_false
 */
static __tb_inline_force__ tb_bool_t tb_spinlock_init(tb_spinlock_t* lock)
{
	// check
	tb_assert_and_check_return_val(lock, tb_false);

	// init 
	*lock = 0;

	// ok
	return tb_true;
}

/*! exit spinlock
 *
 * @param lock 		the lock
 */
static __tb_inline_force__ tb_void_t tb_spinlock_exit(tb_spinlock_t* lock)
{
	// check
	tb_assert_and_check_return(lock);

	// exit 
	*lock = 0;
}

/*! enter spinlock
 *
 * @param lock 		the lock
 */
static __tb_inline_force__ tb_void_t tb_spinlock_enter(tb_spinlock_t* lock)
{
	// check
	tb_assert_and_check_return(lock);

	// init tryn
	__tb_volatile__ tb_size_t tryn = 5;

	// lock it
	while (tb_atomic_fetch_and_pset((tb_atomic_t*)lock, 0, 1))
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
}

/*! try to enter spinlock
 *
 * @param lock 		the lock
 *
 * @return 			tb_true or tb_false
 */
static __tb_inline_force__ tb_bool_t tb_spinlock_enter_try(tb_spinlock_t* lock)
{
	// check
	tb_assert_and_check_return_val(lock, tb_false);

	// try lock it
	return tb_atomic_fetch_and_pset((tb_atomic_t*)lock, 0, 1)? tb_false : tb_true;
}

/*! leave spinlock
 *
 * @param lock 		the lock
 */
static __tb_inline_force__ tb_void_t tb_spinlock_leave(tb_spinlock_t* lock)
{
	// check
	tb_assert_and_check_return(lock);

	// leave
    *((tb_atomic_t*)lock) = 0;
}

#endif
