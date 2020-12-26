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
 * @file        exception.h
 * @ingroup     platform
 *
 */
#ifndef TB_PLATFORM_LIBC_EXCEPTION_H
#define TB_PLATFORM_LIBC_EXCEPTION_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../thread_local.h"
#include "../../container/container.h"
#if defined(TB_CONFIG_LIBC_HAVE_SETJMP) || defined(TB_CONFIG_LIBC_HAVE_SIGSETJMP)
#   include <setjmp.h>
#   include <unistd.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */
extern tb_thread_local_t g_exception_local;

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

#if defined(TB_CONFIG_LIBC_HAVE_SIGSETJMP)

    // try
#   define __tb_try \
    do \
    { \
        /* init exception stack */ \
        tb_stack_ref_t __stack = tb_null; \
        if (!(__stack = (tb_stack_ref_t)tb_thread_local_get(&g_exception_local))) \
        { \
            tb_stack_ref_t __stack_new = tb_stack_init(16, tb_element_mem(sizeof(sigjmp_buf), tb_null, tb_null)); \
            if (__stack_new && tb_thread_local_set(&g_exception_local, __stack_new)) \
                __stack = __stack_new; \
            else if (__stack_new) \
                tb_stack_exit(__stack_new); \
        } \
        \
        /* push jmpbuf */ \
        sigjmp_buf* __top = tb_null; \
        if (__stack) \
        { \
            sigjmp_buf __buf; \
            tb_stack_put(__stack, &__buf); \
            __top = (sigjmp_buf*)tb_stack_top(__stack); \
        } \
        \
        /* init jmpbuf and save sigmask */ \
        __tb_volatile__ tb_int_t __j = __top? sigsetjmp(*__top, 1) : 0; \
        /* done try */ \
        if (!__j) \
        {

    // except
#   define __tb_except(x) \
        } \
        \
        /* check */ \
        tb_assert(x >= 0); \
        /* pop the jmpbuf */ \
        if (__stack) tb_stack_pop(__stack); \
        /* do not this catch? */ \
        if (__j && !(x)) \
        { \
            /* goto the top exception stack */ \
            if (__stack && tb_stack_size(__stack)) \
            { \
                sigjmp_buf* jmpbuf = (sigjmp_buf*)tb_stack_top(__stack); \
                if (jmpbuf) siglongjmp(*jmpbuf, 1); \
            } \
            else \
            { \
                /* no exception handler */ \
                tb_assert_and_check_break(0); \
            } \
        } \
        /* exception been catched? */ \
        if (__j)

#elif defined(TB_CONFIG_LIBC_HAVE_SETJMP)

    // try
#   define __tb_try \
    do \
    { \
        /* init exception stack */ \
        tb_stack_ref_t __stack = tb_null; \
        if (!(__stack = (tb_stack_ref_t)tb_thread_local_get(&g_exception_local))) \
        { \
            tb_stack_ref_t __stack_new = tb_stack_init(16, tb_element_mem(sizeof(jmpbuf), tb_null, tb_null)); \
            if (__stack_new && tb_thread_local_set(&g_exception_local, __stack_new)) \
                __stack = __stack_new; \
            else if (__stack_new) \
                tb_stack_exit(__stack_new); \
        } \
        \
        /* push jmpbuf */ \
        jmpbuf* __top = tb_null; \
        if (__stack) \
        { \
            jmpbuf __buf; \
            tb_stack_put(__stack, &__buf); \
            __top = (jmpbuf*)tb_stack_top(__stack); \
        } \
        \
        /* init jmpbuf */ \
        __tb_volatile__ tb_int_t __j = __top? setjmp(*__top) : 0; \
        /* done try */ \
        if (!__j) \
        {

    // except
#   define __tb_except(x) \
        } \
        \
        /* check */ \
        tb_assert(x >= 0); \
        /* pop the jmpbuf */ \
        if (__stack) tb_stack_pop(__stack); \
        /* do not this catch? */ \
        if (__j && !(x)) \
        { \
            /* goto the top exception stack */ \
            if (__stack && tb_stack_size(__stack)) \
            { \
                jmpbuf* jmpbuf = (jmpbuf*)tb_stack_top(__stack); \
                if (jmpbuf) longjmp(*jmpbuf, 1); \
            } \
            else \
            { \
                /* no exception handler */ \
                tb_assert_and_check_break(0); \
            } \
        } \
        /* exception been catched? */ \
        if (__j)

#endif

    // end
#define __tb_end \
    } while (0);

    // leave
#define __tb_leave   break


#endif


