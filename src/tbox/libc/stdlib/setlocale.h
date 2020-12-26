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
 * @author      OpportunityLiu
 * @file        setlocale.h
 * @ingroup     libc
 *
 */

#ifndef TB_LIBC_STDLIB_SETLOCALE_H
#define TB_LIBC_STDLIB_SETLOCALE_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#ifdef TB_CONFIG_LIBC_HAVE_SETLOCALE
#   include <locale.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#ifdef TB_CONFIG_LIBC_HAVE_SETLOCALE
#   ifdef TB_CONFIG_FORCE_UTF8
#       if defined(TB_CONFIG_OS_WINDOWS)
#           define tb_setlocale()                                              \
                do                                                             \
                {                                                              \
                    if (!setlocale(LC_ALL, ".65001"))                          \
                    {                                                          \
                        tb_trace_w("failed to setlocale to utf-8");            \
                        setlocale(LC_ALL, "");                                 \
                    }                                                          \
                } while (0)
#       else
#           define tb_setlocale()                                              \
                do                                                             \
                {                                                              \
                    if (!(setlocale(LC_ALL, "C.UTF-8") ||                      \
                          setlocale(LC_ALL, "en_US.UTF-8") ||                  \
                          setlocale(LC_ALL, "zh_CN.UTF-8")))                   \
                    {                                                          \
                        tb_trace_w("failed to setlocale to utf-8");            \
                        setlocale(LC_ALL, "");                                 \
                    }                                                          \
                } while (0)
#       endif
#   else
#       define tb_setlocale() setlocale(LC_ALL, "")
#   endif
#   define tb_resetlocale() setlocale(LC_ALL, "C")
#else
#   define tb_setlocale() ((void)0)
#   define tb_resetlocale() ((void)0)
#endif

#endif