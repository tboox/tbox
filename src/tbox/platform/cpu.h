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
 * @file        cpu.h
 * @ingroup     platform
 *
 */
#ifndef TB_PLATFORM_CPU_H
#define TB_PLATFORM_CPU_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#if defined(TB_CONFIG_OS_WINDOWS) && defined(TB_COMPILER_IS_MSVC)
#   include "windows/prefix.h"
#   include <intrin.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

/*! notifies the CPU that this is a spinlock wait loop so memory and cache accesses may be optimized.
 * pause may actually stop CPU for some time to save power. Older CPUs decode it as REP NOP, so you don't have to check if its supported. Older CPUs will simply do nothing (NOP) as fast as possible.
 */
#if defined(TB_CONFIG_OS_WINDOWS) && defined(TB_COMPILER_IS_MSVC)
#   if defined(_M_AMD64) || defined(_M_IX86)
#       pragma intrinsic(_mm_pause)
#       define tb_cpu_pause()       do { _mm_pause(); } while (0)
#   elif defined(_M_IA64)
#       pragma intrinsic(__yield)
#       define tb_cpu_pause()       do { __yield(); } while (0)
#   else
#       define tb_cpu_pause()       do { YieldProcessor(); } while (0)
#   endif
#elif defined(TB_ASSEMBLER_IS_GAS)
#   if defined(TB_COMPILER_IS_GCC) && defined(TB_ARCH_x86)
        // old "as" does not support "pause" opcode
#       define tb_cpu_pause()       do { __tb_asm__ __tb_volatile__ (".byte 0xf3, 0x90"); } while (0)
#   elif (defined(TB_ARCH_x86) || defined(TB_ARCH_x64))
#       if defined(TB_COMPILER_IS_SUNC)
            // Sun Studio 12 exits with segmentation fault on '__asm ("pause")'
#           define tb_cpu_pause()   do { __tb_asm__ __tb_volatile__ ("rep;nop" ::: "memory"); } while (0)
#       else
#           define tb_cpu_pause()   do { __tb_asm__ __tb_volatile__ ("pause"); } while (0)
#       endif
#   elif 0 // defined(TB_ARCH_ARM), TODO some cross-toolchains may be not supported
#       ifdef __CC_ARM
#           define tb_cpu_pause()   __yield()
#       else
#           define tb_cpu_pause()   do { __tb_asm__ __volatile__ ("yield"); } while (0)
#       endif
#   endif
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! the cpu count
 *
 * @return              the cpu count
 */
tb_size_t               tb_cpu_count(tb_noarg_t);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
