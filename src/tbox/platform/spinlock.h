/*!The Treasure Box Library
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (C) 2009-present, TBOOX Open Source Group.
 *
 * @author      ruki
 * @file        spinlock.h
 * @ingroup     platform
 *
 */
#ifndef TB_PLATFORM_SPINLOCK_H
#define TB_PLATFORM_SPINLOCK_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "cpu.h"
#include "sched.h"
#include "atomic.h"
#include "../utils/lock_profiler.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the initial value
#define TB_SPINLOCK_INIT            TB_ATOMIC_FLAG_INIT

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init spinlock
 *
 * @param lock      the lock
 *
 * @return          tb_true or tb_false
 */
static __tb_inline_force__ tb_bool_t tb_spinlock_init(tb_spinlock_ref_t lock)
{
    // check
    tb_assert(lock);
    tb_atomic_flag_clear_explicit(lock, TB_ATOMIC_RELAXED);
    return tb_true;
}

/*! exit spinlock
 *
 * @param lock      the lock
 */
static __tb_inline_force__ tb_void_t tb_spinlock_exit(tb_spinlock_ref_t lock)
{
    // check
    tb_assert(lock);
    tb_atomic_flag_clear_explicit(lock, TB_ATOMIC_RELAXED);
}

/*! enter spinlock
 *
 * @param lock      the lock
 */
static __tb_inline_force__ tb_void_t tb_spinlock_enter(tb_spinlock_ref_t lock)
{
    // check
    tb_assert(lock);

    // init occupied
#ifdef TB_LOCK_PROFILER_ENABLE
    tb_bool_t occupied = tb_false;
#endif

    // get cpu count
#if defined(tb_cpu_pause) && !defined(TB_CONFIG_MICRO_ENABLE)
    tb_size_t ncpu = tb_cpu_count();
#endif

    // lock it
    while (1)
    {
        /* try non-atomic directly reading to reduce the performance loss of atomic synchronization,
         * this maybe read some dirty data, but only leads to enter wait state fastly,
         * but does not affect to acquire lock.
         */
        if (!tb_atomic_flag_test_noatomic(lock) && !tb_atomic_flag_test_and_set(lock))
            return ;

#ifdef TB_LOCK_PROFILER_ENABLE
        // occupied
        if (!occupied)
        {
            // occupied++
            occupied = tb_true;
            tb_lock_profiler_occupied(tb_lock_profiler(), (tb_pointer_t)lock);

            // dump backtrace
#if 0//def __tb_debug__
            tb_backtrace_dump("spinlock", tb_null, 10);
#endif
        }
#endif

#if defined(tb_cpu_pause) && !defined(TB_CONFIG_MICRO_ENABLE)
        if (ncpu > 1)
        {
            tb_size_t i, n;
            for (n = 1; n < 2048; n <<= 1)
            {
                /* spin_Lock:
                 *    cmp lockvar, 0   ; check if lock is free
                 *    je get_Lock
                 *    pause            ; wait for memory pipeline to become empty
                 *    jmp spin_Lock
                 * get_Lock:
                 *
                 * The PAUSE instruction will "de-pipeline" the memory reads,
                 * so that the pipeline is not filled with speculative CMP (2) instructions like in the first example.
                 * (I.e. it could block the pipeline until all older memory instructions are committed.)
                 * Because the CMP instructions (2) execute sequentially it is unlikely (i.e. the time window is much shorter)
                 * that an external write occurs after the CMP instruction (2) read lockvar but before the CMP is committed.
                 */
                for (i = 0; i < n; i++)
                    tb_cpu_pause();

                if (!tb_atomic_flag_test_noatomic(lock) && !tb_atomic_flag_test_and_set(lock))
                    return ;
            }
        }
#endif
        tb_sched_yield();
    }
}

/*! enter spinlock without the lock profiler
 *
 * @param lock      the lock
 */
static __tb_inline_force__ tb_void_t tb_spinlock_enter_without_profiler(tb_spinlock_ref_t lock)
{
    // check
    tb_assert(lock);

    // lock it
#if defined(tb_cpu_pause) && !defined(TB_CONFIG_MICRO_ENABLE)
    tb_size_t ncpu = tb_cpu_count();
#endif
    while (1)
    {
        if (!tb_atomic_flag_test_noatomic(lock) && !tb_atomic_flag_test_and_set(lock))
            return ;

#if defined(tb_cpu_pause) && !defined(TB_CONFIG_MICRO_ENABLE)
        if (ncpu > 1)
        {
            tb_size_t i, n;
            for (n = 1; n < 2048; n <<= 1)
            {
                for (i = 0; i < n; i++)
                    tb_cpu_pause();

                if (!tb_atomic_flag_test_noatomic(lock) && !tb_atomic_flag_test_and_set(lock))
                    return ;
            }
        }
#endif
        tb_sched_yield();
    }
}

/*! try to enter spinlock
 *
 * @param lock      the lock
 *
 * @return          tb_true or tb_false
 */
static __tb_inline_force__ tb_bool_t tb_spinlock_enter_try(tb_spinlock_ref_t lock)
{
    // check
    tb_assert(lock);

#ifndef TB_LOCK_PROFILER_ENABLE
    // try locking it
    return !tb_atomic_flag_test_and_set(lock);
#else
    // try locking it
    tb_bool_t ok = !tb_atomic_flag_test_and_set(lock);

    // occupied?
    if (!ok) tb_lock_profiler_occupied(tb_lock_profiler(), (tb_pointer_t)lock);

    // ok?
    return ok;
#endif
}

/*! try to enter spinlock without the lock profiler
 *
 * @param lock      the lock
 *
 * @return          tb_true or tb_false
 */
static __tb_inline_force__ tb_bool_t tb_spinlock_enter_try_without_profiler(tb_spinlock_ref_t lock)
{
    // check
    tb_assert(lock);

    // try locking it
    return !tb_atomic_flag_test_and_set(lock);
}

/*! leave spinlock
 *
 * @param lock      the lock
 */
static __tb_inline_force__ tb_void_t tb_spinlock_leave(tb_spinlock_ref_t lock)
{
    // check
    tb_assert(lock);

    // leave
    tb_atomic_flag_clear(lock);
}

#endif
