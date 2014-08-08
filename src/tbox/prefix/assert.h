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
#   if defined(TB_COMPILER_IS_GCC)
#       define tb_assertf(x, fmt, arg...)                                   do { if (!(x)) {tb_trace_a("expr[%s]: " fmt, #x, ##arg); tb_assert_backtrace_dump(); } } while(0)
#       define tb_assertf_abort(x, fmt, arg...)                             do { if (!(x)) {tb_trace_a("expr[%s]: " fmt, #x, ##arg); tb_assert_backtrace_dump(); tb_abort(); } } while(0)
#       define tb_assertf_return(x, fmt, arg...)                            do { if (!(x)) {tb_trace_a("expr[%s]: " fmt, #x, ##arg); tb_assert_backtrace_dump(); return ; } } while(0)
#       define tb_assertf_return_val(x, v, fmt, arg...)                     do { if (!(x)) {tb_trace_a("expr[%s]: " fmt, #x, ##arg); tb_assert_backtrace_dump(); return (v); } } while(0)
#       define tb_assertf_goto(x, b, fmt, arg...)                           do { if (!(x)) {tb_trace_a("expr[%s]: " fmt, #x, ##arg); tb_assert_backtrace_dump(); goto b; } } while(0)
#       define tb_assertf_break(x, fmt, arg...)                             { if (!(x)) {tb_trace_a("expr[%s]: " fmt, #x, ##arg); tb_assert_backtrace_dump(); break ; } }
#       define tb_assertf_continue(x, fmt, arg...)                          { if (!(x)) {tb_trace_a("expr[%s]: " fmt, #x, ##arg); tb_assert_backtrace_dump(); continue ; } }
#       define tb_assertf_and_check_abort(x, fmt, arg...)                   tb_assertf_abort(x, fmt, ##arg)
#       define tb_assertf_and_check_return(x, fmt, arg...)                  tb_assertf_return(x, fmt, ##arg)
#       define tb_assertf_and_check_return_val(x, v, fmt, arg...)           tb_assertf_return_val(x, v, fmt, ##arg)
#       define tb_assertf_and_check_goto(x, b, fmt, arg...)                 tb_assertf_goto(x, b, fmt, ##arg)
#       define tb_assertf_and_check_break(x, fmt, arg...)                   tb_assertf_break(x, fmt, ##arg)
#       define tb_assertf_and_check_continue(x, fmt, arg...)                tb_assertf_continue(x, fmt, ##arg)
#   elif defined(TB_COMPILER_IS_MSVC) && TB_COMPILER_VERSION_BE(13, 0)
#       define tb_assertf(x, fmt, ...)                                      do { if (!(x)) {tb_trace_a("expr[%s]: " fmt, #x, __VA_ARGS__); tb_assert_backtrace_dump(); } } while(0)
#       define tb_assertf_abort(x, fmt, ...)                                do { if (!(x)) {tb_trace_a("expr[%s]: " fmt, #x, __VA_ARGS__); tb_assert_backtrace_dump(); tb_abort(); } } while(0)
#       define tb_assertf_return(x, fmt, ...)                               do { if (!(x)) {tb_trace_a("expr[%s]: " fmt, #x, __VA_ARGS__); tb_assert_backtrace_dump(); return ; } } while(0)
#       define tb_assertf_return_val(x, v, fmt, ...)                        do { if (!(x)) {tb_trace_a("expr[%s]: " fmt, #x, __VA_ARGS__); tb_assert_backtrace_dump(); return (v); } } while(0)
#       define tb_assertf_goto(x, b, fmt, ...)                              do { if (!(x)) {tb_trace_a("expr[%s]: " fmt, #x, __VA_ARGS__); tb_assert_backtrace_dump(); goto b; } } while(0)
#       define tb_assertf_break(x, fmt, ...)                                { if (!(x)) {tb_trace_a("expr[%s]: " fmt, #x, __VA_ARGS__); tb_assert_backtrace_dump(); break ; } }
#       define tb_assertf_continue(x, fmt, ...)                             { if (!(x)) {tb_trace_a("expr[%s]: " fmt, #x, __VA_ARGS__); tb_assert_backtrace_dump(); continue ; } }
#       define tb_assertf_and_check_abort(x, fmt, ...)                      tb_assertf_abort(x, fmt, __VA_ARGS__)
#       define tb_assertf_and_check_return(x, fmt, ...)                     tb_assertf_return(x, fmt, __VA_ARGS__)
#       define tb_assertf_and_check_return_val(x, v, fmt, ...)              tb_assertf_return_val(x, v, fmt, __VA_ARGS__)
#       define tb_assertf_and_check_goto(x, b, fmt, ...)                    tb_assertf_goto(x, b, fmt, __VA_ARGS__)
#       define tb_assertf_and_check_break(x, fmt, ...)                      tb_assertf_break(x, fmt, __VA_ARGS__)
#       define tb_assertf_and_check_continue(x, fmt, ...)                   tb_assertf_continue(x, fmt, __VA_ARGS__)
#   else
#       define tb_assertf                                   
#       define tb_assertf_abort                             
#       define tb_assertf_return                            
#       define tb_assertf_return_val              
#       define tb_assertf_goto                       
#       define tb_assertf_break                             
#       define tb_assertf_continue                          
#       define tb_assertf_and_check_abort                                   tb_abort();
#       define tb_assertf_and_check_return                                  tb_abort();
#       define tb_assertf_and_check_return_val                              tb_abort();
#       define tb_assertf_and_check_goto                                    tb_abort();
#       define tb_assertf_and_check_break                                   tb_abort();
#       define tb_assertf_and_check_continue                                tb_abort();
#   endif
#else
#   if defined(TB_COMPILER_IS_GCC)
#       define tb_assertf(x, fmt, arg...)                                   
#       define tb_assertf_abort(x, fmt, arg...)                             
#       define tb_assertf_return(x, fmt, arg...)                            
#       define tb_assertf_return_val(x, v, fmt, arg...)                     
#       define tb_assertf_goto(x, b, fmt, arg...)                           
#       define tb_assertf_break(x, fmt, arg...)                             
#       define tb_assertf_continue(x, fmt, arg...)                          
#       define tb_assertf_and_check_abort(x, fmt, arg...)                   tb_check_abort(x)
#       define tb_assertf_and_check_return(x, fmt, arg...)                  tb_check_return(x)
#       define tb_assertf_and_check_return_val(x, v, fmt, arg...)           tb_check_return_val(x, v)
#       define tb_assertf_and_check_goto(x, b, fmt, arg...)                 tb_check_goto(x, b)
#       define tb_assertf_and_check_break(x, fmt, arg...)                   tb_check_break(x)
#       define tb_assertf_and_check_continue(x, fmt, arg...)                tb_check_continue(x)
#   elif defined(TB_COMPILER_IS_MSVC) && TB_COMPILER_VERSION_BE(13, 0)
#       define tb_assertf(x, fmt, ...)                                   
#       define tb_assertf_abort(x, fmt, ...)                             
#       define tb_assertf_return(x, fmt, ...)                            
#       define tb_assertf_return_val(x, v, fmt, ...)                     
#       define tb_assertf_goto(x, b, fmt, ...)                           
#       define tb_assertf_break(x, fmt, ...)                             
#       define tb_assertf_continue(x, fmt, ...)                          
#       define tb_assertf_and_check_abort(x, fmt, ...)                      tb_check_abort(x)
#       define tb_assertf_and_check_return(x, fmt, ...)                     tb_check_return(x)
#       define tb_assertf_and_check_return_val(x, v, fmt, ...)              tb_check_return_val(x, v)
#       define tb_assertf_and_check_goto(x, b, fmt, ...)                    tb_check_goto(x, b)
#       define tb_assertf_and_check_break(x, fmt, ...)                      tb_check_break(x)
#       define tb_assertf_and_check_continue(x, fmt, ...)                   tb_check_continue(x)
#   else
#       error
#       define tb_assertf                                   
#       define tb_assertf_abort                             
#       define tb_assertf_return                            
#       define tb_assertf_return_val               
#       define tb_assertf_goto                       
#       define tb_assertf_break                             
#       define tb_assertf_continue                          
#       define tb_assertf_and_check_abort                                   tb_abort();
#       define tb_assertf_and_check_return                                  tb_abort();
#       define tb_assertf_and_check_return_val                              tb_abort();
#       define tb_assertf_and_check_goto                                    tb_abort();
#       define tb_assertf_and_check_break                                   tb_abort();
#       define tb_assertf_and_check_continue                                tb_abort();
#   endif
#endif

#ifdef __tb_debug__
#   define tb_assert(x)                                     do { if (!(x)) {tb_trace_a("expr[%s]", #x); tb_assert_backtrace_dump(); } } while(0)
#   define tb_assert_abort(x)                               do { if (!(x)) {tb_trace_a("expr[%s]", #x); tb_assert_backtrace_dump(); tb_abort(); } } while(0)
#   define tb_assert_return(x)                              do { if (!(x)) {tb_trace_a("expr[%s]", #x); tb_assert_backtrace_dump(); return ; } } while(0)
#   define tb_assert_return_val(x, v)                       do { if (!(x)) {tb_trace_a("expr[%s]", #x); tb_assert_backtrace_dump(); return (v); } } while(0)
#   define tb_assert_goto(x, b)                             do { if (!(x)) {tb_trace_a("expr[%s]", #x); tb_assert_backtrace_dump(); goto b; } } while(0)
#   define tb_assert_break(x)                               { if (!(x)) {tb_trace_a("expr[%s]", #x); tb_assert_backtrace_dump(); break ; } }
#   define tb_assert_continue(x)                            { if (!(x)) {tb_trace_a("expr[%s]", #x); tb_assert_backtrace_dump(); continue ; } }
#   define tb_assert_break_state(x, s, v)                   { if (!(x)) { (s) = (v); tb_trace_a("expr[%s]", #x); tb_assert_backtrace_dump(); break ; } }
#   define tb_assert_and_check_abort(x)                     tb_assert_abort(x)
#   define tb_assert_and_check_return(x)                    tb_assert_return(x)
#   define tb_assert_and_check_return_val(x, v)             tb_assert_return_val(x, v)
#   define tb_assert_and_check_goto(x, b)                   tb_assert_goto(x, b)
#   define tb_assert_and_check_break(x)                     tb_assert_break(x)
#   define tb_assert_and_check_continue(x)                  tb_assert_continue(x)
#   define tb_assert_and_check_break_state(x, s, v)         tb_assert_break_state(x, s, v)
#else
#   define tb_assert(x)
#   define tb_assert_abort(x)
#   define tb_assert_return(x)
#   define tb_assert_return_val(x, v)
#   define tb_assert_goto(x, b)
#   define tb_assert_break(x)
#   define tb_assert_continue(x)
#   define tb_assert_break_state(x, s, v)
#   define tb_assert_and_check_abort(x)                     tb_check_abort(x)
#   define tb_assert_and_check_return(x)                    tb_check_return(x)
#   define tb_assert_and_check_return_val(x, v)             tb_check_return_val(x, v)
#   define tb_assert_and_check_goto(x, b)                   tb_check_goto(x, b)
#   define tb_assert_and_check_break(x)                     tb_check_break(x)
#   define tb_assert_and_check_continue(x)                  tb_check_continue(x)
#   define tb_assert_and_check_break_state(x, s, v)         tb_check_break_state(x, s, v)
#endif

#define tb_assert_static(x)                                 do { typedef int __tb_static_assert__[(x)? 1 : -1]; __tb_volatile__ __tb_static_assert__ __a; tb_used(__a); } while(0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */
tb_void_t tb_backtrace_dump(tb_char_t const* prefix, tb_pointer_t* frames, tb_size_t nframe);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif


