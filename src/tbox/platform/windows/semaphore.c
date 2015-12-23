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
 * @file        semaphore.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_SEMAPHORE_VALUE_MAXN             (65536)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the semaphore type
typedef struct __tb_semaphore_impl_t
{
    // the semaphore semaphore
    HANDLE          semaphore;

    // the semaphore value
    tb_atomic_t     value;

}tb_semaphore_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_semaphore_ref_t tb_semaphore_init(tb_size_t init)
{
    // check
    tb_assert_and_check_return_val(init <= TB_SEMAPHORE_VALUE_MAXN, tb_null);

    // done
    tb_bool_t               ok = tb_false;
    tb_semaphore_impl_t*    impl = tb_null;
    do
    {
        // make semaphore
        impl = tb_malloc0_type(tb_semaphore_impl_t);
        tb_assert_and_check_break(impl);

        // init semaphore 
        impl->semaphore = CreateSemaphoreA(tb_null, (DWORD)init, TB_SEMAPHORE_VALUE_MAXN, tb_null);
        tb_assert_and_check_break(impl->semaphore && impl->semaphore != INVALID_HANDLE_VALUE);

        // init value
        impl->value = init;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (impl) tb_semaphore_exit((tb_semaphore_ref_t)impl);
        impl = tb_null;
    }

    // ok?
    return (tb_semaphore_ref_t)impl;
}
tb_void_t tb_semaphore_exit(tb_semaphore_ref_t semaphore)
{
    tb_semaphore_impl_t* impl = (tb_semaphore_impl_t*)semaphore;
    if (semaphore) 
    {
        // exit semaphore
        if (impl->semaphore && impl->semaphore != INVALID_HANDLE_VALUE) CloseHandle(impl->semaphore);
        impl->semaphore = INVALID_HANDLE_VALUE;

        // exit it
        tb_free(semaphore);
    }
}
tb_bool_t tb_semaphore_post(tb_semaphore_ref_t semaphore, tb_size_t post)
{
    // check
    tb_semaphore_impl_t* impl = (tb_semaphore_impl_t*)semaphore;
    tb_assert_and_check_return_val(semaphore && impl->semaphore && impl->semaphore != INVALID_HANDLE_VALUE && post, tb_false);
    
    // += post
    tb_atomic_fetch_and_add(&impl->value, post);
    
    // post
    LONG prev = 0;
    if (!ReleaseSemaphore(impl->semaphore, (LONG)post, &prev) && prev >= 0) 
    {
        // restore
        tb_atomic_fetch_and_sub(&impl->value, (tb_long_t)post);
        return tb_false;
    }

    // check
    tb_assert_and_check_return_val(prev + post <= TB_SEMAPHORE_VALUE_MAXN, tb_false);
    
    // save value
    tb_atomic_set(&impl->value, prev + post);
    
    // ok
    return tb_true;
}
tb_long_t tb_semaphore_value(tb_semaphore_ref_t semaphore)
{
    // check
    tb_semaphore_impl_t* impl = (tb_semaphore_impl_t*)semaphore;
    tb_assert_and_check_return_val(semaphore, -1);

    // get value
    return (tb_long_t)tb_atomic_get(&impl->value);
}
tb_long_t tb_semaphore_wait(tb_semaphore_ref_t semaphore, tb_long_t timeout)
{
    // check
    tb_semaphore_impl_t* impl = (tb_semaphore_impl_t*)semaphore;
    tb_assert_and_check_return_val(semaphore && impl->semaphore && impl->semaphore != INVALID_HANDLE_VALUE, -1);

    // wait
    tb_long_t r = WaitForSingleObject(impl->semaphore, (DWORD)(timeout >= 0? timeout : INFINITE));
    tb_assert_and_check_return_val(r != WAIT_FAILED, -1);

    // timeout?
    tb_check_return_val(r != WAIT_TIMEOUT, 0);

    // error?
    tb_check_return_val(r >= WAIT_OBJECT_0, -1);

    // check value
    tb_assert_and_check_return_val((tb_long_t)tb_atomic_get(&impl->value) > 0, -1);
    
    // value--
    tb_atomic_fetch_and_dec(&impl->value);
    
    // ok
    return 1;
}


