/*!The Treasure Box Library
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * Copyright (C) 2009 - 2018, TBOOX Open Source Group.
 *
 * @author      ruki
 * @file        assert.h
 *
 */
#ifndef TB_PREFIX_ASSERT_H
#define TB_PREFIX_ASSERT_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "config.h"
#include "trace.h"
#include "abort.h"
#include "check.h"
#include "keyword.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the assert backtrace prefix
#define TB_ASSERT_BACKTRACE_PREFIX                          "    "

// the assert backtrace nframe
#define TB_ASSERT_BACKTRACE_NFRAME                          (16)

// the assert backtrace dump
#define tb_assert_backtrace_dump()                          tb_backtrace_dump(TB_ASSERT_BACKTRACE_PREFIX, tb_null, TB_ASSERT_BACKTRACE_NFRAME)

// assert
#ifdef __tb_debug__
#   if defined(TB_COMPILER_IS_GCC) || defined(TB_COMPILER_IS_TINYC)
#       define tb_assertf(x, fmt, arg...)                                   do { if (!(x)) {tb_trace_a("expr[%s]: " fmt, #x, ##arg); tb_assert_backtrace_dump(); tb_trace_sync(); tb_abort(); } } while(0)
#       define tb_assertf_and_check_abort(x, fmt, arg...)                   tb_assertf(x, fmt, ##arg)
#       define tb_assertf_and_check_return(x, fmt, arg...)                  tb_assertf(x, fmt, ##arg)
#       define tb_assertf_and_check_return_val(x, v, fmt, arg...)           tb_assertf(x, fmt, ##arg)
#       define tb_assertf_and_check_goto(x, b, fmt, arg...)                 tb_assertf(x, fmt, ##arg)
#       define tb_assertf_and_check_break(x, fmt, arg...)                   tb_assertf(x, fmt, ##arg)
#       define tb_assertf_and_check_continue(x, fmt, arg...)                tb_assertf(x, fmt, ##arg)
#   elif defined(TB_COMPILER_IS_MSVC) && TB_COMPILER_VERSION_BE(13, 0)
#       define tb_assertf(x, fmt, ...)                                      do { if (!(x)) {tb_trace_a("expr[%s]: " fmt, #x, __VA_ARGS__); tb_assert_backtrace_dump(); tb_trace_sync(); tb_abort(); } } while(0)
#       define tb_assertf_and_check_abort(x, fmt, ...)                      tb_assertf(x, fmt, __VA_ARGS__)
#       define tb_assertf_and_check_return(x, fmt, ...)                     tb_assertf(x, fmt, __VA_ARGS__)
#       define tb_assertf_and_check_return_val(x, v, fmt, ...)              tb_assertf(x, fmt, __VA_ARGS__)
#       define tb_assertf_and_check_goto(x, b, fmt, ...)                    tb_assertf(x, fmt, __VA_ARGS__)
#       define tb_assertf_and_check_break(x, fmt, ...)                      tb_assertf(x, fmt, __VA_ARGS__)
#       define tb_assertf_and_check_continue(x, fmt, ...)                   tb_assertf(x, fmt, __VA_ARGS__)
#   else
#       define tb_assertf                                   
#       define tb_assertf_and_check_abort                                 
#       define tb_assertf_and_check_return                                
#       define tb_assertf_and_check_return_val                           
#       define tb_assertf_and_check_goto                                 
#       define tb_assertf_and_check_break                                  
#       define tb_assertf_and_check_continue                               
#   endif
#else
#   if defined(TB_COMPILER_IS_GCC) || defined(TB_COMPILER_IS_TINYC)
#       define tb_assertf(x, fmt, arg...)                             
#       define tb_assertf_and_check_abort(x, fmt, arg...)                   tb_check_abort(x)
#       define tb_assertf_and_check_return(x, fmt, arg...)                  tb_check_return(x)
#       define tb_assertf_and_check_return_val(x, v, fmt, arg...)           tb_check_return_val(x, v)
#       define tb_assertf_and_check_goto(x, b, fmt, arg...)                 tb_check_goto(x, b)
#       define tb_assertf_and_check_break(x, fmt, arg...)                   tb_check_break(x)
#       define tb_assertf_and_check_continue(x, fmt, arg...)                tb_check_continue(x)
#   elif defined(TB_COMPILER_IS_MSVC) && TB_COMPILER_VERSION_BE(13, 0)
#       define tb_assertf(x, fmt, ...)                             
#       define tb_assertf_and_check_abort(x, fmt, ...)                      tb_check_abort(x)
#       define tb_assertf_and_check_return(x, fmt, ...)                     tb_check_return(x)
#       define tb_assertf_and_check_return_val(x, v, fmt, ...)              tb_check_return_val(x, v)
#       define tb_assertf_and_check_goto(x, b, fmt, ...)                    tb_check_goto(x, b)
#       define tb_assertf_and_check_break(x, fmt, ...)                      tb_check_break(x)
#       define tb_assertf_and_check_continue(x, fmt, ...)                   tb_check_continue(x)
#   else
#       define tb_assertf                                              
#       define tb_assertf_and_check_abort                                 
#       define tb_assertf_and_check_return                                
#       define tb_assertf_and_check_return_val                            
#       define tb_assertf_and_check_goto                                 
#       define tb_assertf_and_check_break                                   
#       define tb_assertf_and_check_continue                                
#   endif
#endif

#ifdef __tb_debug__
#   define tb_assert(x)                                     tb_assert_impl(!!(x), #x __tb_debug_vals__)
#   define tb_assert_and_check_abort(x)                     tb_assert(x)
#   define tb_assert_and_check_return(x)                    tb_assert(x)
#   define tb_assert_and_check_return_val(x, v)             tb_assert(x)
#   define tb_assert_and_check_goto(x, b)                   tb_assert(x)
#   define tb_assert_and_check_break(x)                     tb_assert(x)
#   define tb_assert_and_check_continue(x)                  tb_assert(x)
#   define tb_assert_and_check_break_state(x, s, v)         tb_assert(x)
#else
#   define tb_assert(x)                                     tb_assert_empty_impl()
#   define tb_assert_and_check_abort(x)                     tb_check_abort(x)
#   define tb_assert_and_check_return(x)                    tb_check_return(x)
#   define tb_assert_and_check_return_val(x, v)             tb_check_return_val(x, v)
#   define tb_assert_and_check_goto(x, b)                   tb_check_goto(x, b)
#   define tb_assert_and_check_break(x)                     tb_check_break(x)
#   define tb_assert_and_check_continue(x)                  tb_check_continue(x)
#   define tb_assert_and_check_break_state(x, s, v)         tb_check_break_state(x, s, v)
#endif

// assert and pass code, not abort it
#ifdef __tb_debug__
#   if defined(TB_COMPILER_IS_GCC) || defined(TB_COMPILER_IS_TINYC)
#       define tb_assertf_pass_return(x, fmt, arg...)                           do { if (!(x)) {tb_trace_a("expr[%s]: " fmt, #x, ##arg); tb_assert_backtrace_dump(); tb_trace_sync(); return ; } } while(0)
#       define tb_assertf_pass_return_val(x, v, fmt, arg...)                    do { if (!(x)) {tb_trace_a("expr[%s]: " fmt, #x, ##arg); tb_assert_backtrace_dump(); tb_trace_sync(); return (v); } } while(0)
#       define tb_assertf_pass_goto(x, b, fmt, arg...)                          do { if (!(x)) {tb_trace_a("expr[%s]: " fmt, #x, ##arg); tb_assert_backtrace_dump(); tb_trace_sync(); goto b; } } while(0)
#       define tb_assertf_pass_break(x, fmt, arg...)                            { if (!(x)) {tb_trace_a("expr[%s]: " fmt, #x, ##arg); tb_assert_backtrace_dump(); tb_trace_sync(); break ; } }
#       define tb_assertf_pass_continue(x, fmt, arg...)                         { if (!(x)) {tb_trace_a("expr[%s]: " fmt, #x, ##arg); tb_assert_backtrace_dump(); tb_trace_sync(); continue ; } }
#       define tb_assertf_pass_and_check_abort(x, fmt, arg...)                  tb_assertf_pass_abort(x, fmt, ##arg)
#       define tb_assertf_pass_and_check_return(x, fmt, arg...)                 tb_assertf_pass_return(x, fmt, ##arg)
#       define tb_assertf_pass_and_check_return_val(x, v, fmt, arg...)          tb_assertf_pass_return_val(x, v, fmt, ##arg)
#       define tb_assertf_pass_and_check_goto(x, b, fmt, arg...)                tb_assertf_pass_goto(x, b, fmt, ##arg)
#       define tb_assertf_pass_and_check_break(x, fmt, arg...)                  tb_assertf_pass_break(x, fmt, ##arg)
#       define tb_assertf_pass_and_check_continue(x, fmt, arg...)               tb_assertf_pass_continue(x, fmt, ##arg)
#   elif defined(TB_COMPILER_IS_MSVC) && TB_COMPILER_VERSION_BE(13, 0)
#       define tb_assertf_pass_return(x, fmt, ...)                              do { if (!(x)) {tb_trace_a("expr[%s]: " fmt, #x, __VA_ARGS__); tb_assert_backtrace_dump(); tb_trace_sync(); return ; } } while(0)
#       define tb_assertf_pass_return_val(x, v, fmt, ...)                       do { if (!(x)) {tb_trace_a("expr[%s]: " fmt, #x, __VA_ARGS__); tb_assert_backtrace_dump(); tb_trace_sync(); return (v); } } while(0)
#       define tb_assertf_pass_goto(x, b, fmt, ...)                             do { if (!(x)) {tb_trace_a("expr[%s]: " fmt, #x, __VA_ARGS__); tb_assert_backtrace_dump(); tb_trace_sync(); goto b; } } while(0)
#       define tb_assertf_pass_break(x, fmt, ...)                               { if (!(x)) {tb_trace_a("expr[%s]: " fmt, #x, __VA_ARGS__); tb_assert_backtrace_dump(); tb_trace_sync(); break ; } }
#       define tb_assertf_pass_continue(x, fmt, ...)                            { if (!(x)) {tb_trace_a("expr[%s]: " fmt, #x, __VA_ARGS__); tb_assert_backtrace_dump(); tb_trace_sync(); continue ; } }
#       define tb_assertf_pass_and_check_abort(x, fmt, ...)                     tb_assertf_pass_abort(x, fmt, __VA_ARGS__)
#       define tb_assertf_pass_and_check_return(x, fmt, ...)                    tb_assertf_pass_return(x, fmt, __VA_ARGS__)
#       define tb_assertf_pass_and_check_return_val(x, v, fmt, ...)             tb_assertf_pass_return_val(x, v, fmt, __VA_ARGS__)
#       define tb_assertf_pass_and_check_goto(x, b, fmt, ...)                   tb_assertf_pass_goto(x, b, fmt, __VA_ARGS__)
#       define tb_assertf_pass_and_check_break(x, fmt, ...)                     tb_assertf_pass_break(x, fmt, __VA_ARGS__)
#       define tb_assertf_pass_and_check_continue(x, fmt, ...)                  tb_assertf_pass_continue(x, fmt, __VA_ARGS__)
#   else
#       define tb_assertf_pass_return                            
#       define tb_assertf_pass_return_val              
#       define tb_assertf_pass_goto                       
#       define tb_assertf_pass_break                             
#       define tb_assertf_pass_continue                          
#       define tb_assertf_pass_and_check_abort                            
#       define tb_assertf_pass_and_check_return                             
#       define tb_assertf_pass_and_check_return_val                     
#       define tb_assertf_pass_and_check_goto                                  
#       define tb_assertf_pass_and_check_break                              
#       define tb_assertf_pass_and_check_continue                                
#   endif
#else
#   if defined(TB_COMPILER_IS_GCC) || defined(TB_COMPILER_IS_TINYC)
#       define tb_assertf_pass_return(x, fmt, arg...)                            
#       define tb_assertf_pass_return_val(x, v, fmt, arg...)                     
#       define tb_assertf_pass_goto(x, b, fmt, arg...)                           
#       define tb_assertf_pass_break(x, fmt, arg...)                             
#       define tb_assertf_pass_continue(x, fmt, arg...)                          
#       define tb_assertf_pass_and_check_abort(x, fmt, arg...)                  tb_check_abort(x)
#       define tb_assertf_pass_and_check_return(x, fmt, arg...)                 tb_check_return(x)
#       define tb_assertf_pass_and_check_return_val(x, v, fmt, arg...)          tb_check_return_val(x, v)
#       define tb_assertf_pass_and_check_goto(x, b, fmt, arg...)                tb_check_goto(x, b)
#       define tb_assertf_pass_and_check_break(x, fmt, arg...)                  tb_check_break(x)
#       define tb_assertf_pass_and_check_continue(x, fmt, arg...)               tb_check_continue(x)
#   elif defined(TB_COMPILER_IS_MSVC) && TB_COMPILER_VERSION_BE(13, 0)
#       define tb_assertf_pass_return(x, fmt, ...)                            
#       define tb_assertf_pass_return_val(x, v, fmt, ...)                     
#       define tb_assertf_pass_goto(x, b, fmt, ...)                           
#       define tb_assertf_pass_break(x, fmt, ...)                             
#       define tb_assertf_pass_continue(x, fmt, ...)                          
#       define tb_assertf_pass_and_check_abort(x, fmt, ...)                     tb_check_abort(x)
#       define tb_assertf_pass_and_check_return(x, fmt, ...)                    tb_check_return(x)
#       define tb_assertf_pass_and_check_return_val(x, v, fmt, ...)             tb_check_return_val(x, v)
#       define tb_assertf_pass_and_check_goto(x, b, fmt, ...)                   tb_check_goto(x, b)
#       define tb_assertf_pass_and_check_break(x, fmt, ...)                     tb_check_break(x)
#       define tb_assertf_pass_and_check_continue(x, fmt, ...)                  tb_check_continue(x)
#   else
#       define tb_assertf_pass_return                            
#       define tb_assertf_pass_return_val               
#       define tb_assertf_pass_goto                       
#       define tb_assertf_pass_break                             
#       define tb_assertf_pass_continue                          
#       define tb_assertf_pass_and_check_abort                                 
#       define tb_assertf_pass_and_check_return                                 
#       define tb_assertf_pass_and_check_return_val                            
#       define tb_assertf_pass_and_check_goto                                   
#       define tb_assertf_pass_and_check_break                                 
#       define tb_assertf_pass_and_check_continue                               
#   endif
#endif

#ifdef __tb_debug__
#   define tb_assert_pass_return(x)                             do { if (!(x)) {tb_trace_a("expr[%s]", #x); tb_assert_backtrace_dump(); tb_trace_sync(); return ; } } while(0)
#   define tb_assert_pass_return_val(x, v)                      do { if (!(x)) {tb_trace_a("expr[%s]", #x); tb_assert_backtrace_dump(); tb_trace_sync(); return (v); } } while(0)
#   define tb_assert_pass_goto(x, b)                            do { if (!(x)) {tb_trace_a("expr[%s]", #x); tb_assert_backtrace_dump(); tb_trace_sync(); goto b; } } while(0)
#   define tb_assert_pass_break(x)                              { if (!(x)) {tb_trace_a("expr[%s]", #x); tb_assert_backtrace_dump(); tb_trace_sync(); break ; } }
#   define tb_assert_pass_continue(x)                           { if (!(x)) {tb_trace_a("expr[%s]", #x); tb_assert_backtrace_dump(); tb_trace_sync(); continue ; } }
#   define tb_assert_pass_break_state(x, s, v)                  { if (!(x)) { (s) = (v); tb_trace_a("expr[%s]", #x); tb_assert_backtrace_dump(); tb_trace_sync(); break ; } }

#   define tb_assert_pass_and_check_abort(x)                    tb_assert_pass_abort(x)
#   define tb_assert_pass_and_check_return(x)                   tb_assert_pass_return(x)
#   define tb_assert_pass_and_check_return_val(x, v)            tb_assert_pass_return_val(x, v)
#   define tb_assert_pass_and_check_goto(x, b)                  tb_assert_pass_goto(x, b)
#   define tb_assert_pass_and_check_break(x)                    tb_assert_pass_break(x)
#   define tb_assert_pass_and_check_continue(x)                 tb_assert_pass_continue(x)
#   define tb_assert_pass_and_check_break_state(x, s, v)        tb_assert_pass_break_state(x, s, v)
#else
#   define tb_assert_pass_return(x)
#   define tb_assert_pass_return_val(x, v)
#   define tb_assert_pass_goto(x, b)
#   define tb_assert_pass_break(x)
#   define tb_assert_pass_continue(x)
#   define tb_assert_pass_break_state(x, s, v)

#   define tb_assert_pass_and_check_abort(x)                    tb_check_abort(x)
#   define tb_assert_pass_and_check_return(x)                   tb_check_return(x)
#   define tb_assert_pass_and_check_return_val(x, v)            tb_check_return_val(x, v)
#   define tb_assert_pass_and_check_goto(x, b)                  tb_check_goto(x, b)
#   define tb_assert_pass_and_check_break(x)                    tb_check_break(x)
#   define tb_assert_pass_and_check_continue(x)                 tb_check_continue(x)
#   define tb_assert_pass_and_check_break_state(x, s, v)        tb_check_break_state(x, s, v)
#endif

/// assert: noimpl
#define tb_assert_noimpl()                                      tb_assertf(0, "noimpl")

/*! the static assert
 *
 * @code
 *
 * tb_assert_static(sizeof(tb_uint32_t) == 4);
 *
 * @endcode
 */
#if __tb_has_feature__(c_static_assert)
#   define tb_assert_static(x)      _Static_assert(x, "")
#elif defined(TB_COMPILER_IS_GCC) && TB_COMPILER_VERSION_BE(4, 6)
#   define tb_assert_static(x)      _Static_assert(x, "")
#else
#   define tb_assert_static(x)      do { typedef int __tb_static_assert__[(x)? 1 : -1]; __tb_volatile__ __tb_static_assert__ __a; tb_used_ptr((tb_cpointer_t)(tb_size_t)__a); } while(0)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */

/*! dump backtrace 
 *
 * @param prefix    the prefix tag
 * @pragm frames    the frame list
 * @param nframe    the frame count
 */
tb_void_t           tb_backtrace_dump(tb_char_t const* prefix, tb_pointer_t* frames, tb_size_t nframe);

/*! this variable have been used
 *
 * @param variable  the variable
 */
tb_void_t           tb_used_ptr(tb_cpointer_t variable);

/* //////////////////////////////////////////////////////////////////////////////////////
 * inlines
 */

#ifdef __tb_debug__
/* the assert implementation
 *
 * @code
 * tb_size_t v = (tb_assert(x), value);
 * tb_assert(v);
 * @endcode
 *
 * @param x                     the boolean value of the expression 
 * @param expr                  the expression string 
 *
 * @return                      the boolean value of the expression 
 */
static __tb_inline__ tb_bool_t  tb_assert_impl(tb_bool_t x, tb_char_t const* expr __tb_debug_decl__) 
{
    // failed?
    if (!x)
    {
        // trace
        tb_trace_a("expr[%s] at %s(): %d, %s", expr __tb_debug_args__); 

        // dump backtrace
        tb_assert_backtrace_dump();

        // sync trace
        tb_trace_sync(); 

        // abort it
        tb_abort();
    }

    // ok?
    return x;
}
#else
/* the assert empty implementation
 *
 * @return                      only tb_false
 */
static __tb_inline__ tb_bool_t  tb_assert_empty_impl(tb_noarg_t) 
{
    return tb_false;
}
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif


