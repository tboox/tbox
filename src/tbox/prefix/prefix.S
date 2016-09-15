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
 * Copyright (C) 2009 - 2017, ruki All rights reserved.
 *
 * @author      ruki
 * @file        prefix.S
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "config.h"
#include "arch.h"
#include "cpu.h"
#include "endian.h"
#include "compiler.h"
#include "cache.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#ifdef TB_ARCH_ELF
#   define EXTERN_ASM
#else
#   define EXTERN_ASM _
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * arch
 */

#if defined(TB_ARCH_ARM_v7A)
        .arch armv7-a
#elif defined(TB_ARCH_ARM_v7)
        .arch armv7
#elif defined(TB_ARCH_ARM_v6) 
        .arch armv6
#elif defined(TB_ARCH_ARM_v5te) 
        .arch armv5te
#elif defined(TB_ARCH_ARM_v5) 
        .arch armv5
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * fpu
 */

#if defined(TB_ARCH_ARM_NEON) && !defined(TB_ARCH_ARM64)
        .fpu neon
#elif defined(TB_ARCH_VFP)
        .fpu vfp
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * syntax
 */
#if defined(TB_ARCH_ARM) && !defined(TB_ARCH_ARM64)
        .syntax unified
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * eabi
 */
#if defined(TB_ARCH_ELF) && defined(TB_ARCH_ARM) && !defined(TB_ARCH_ARM64)
        .eabi_attribute 25, 1 
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

/*! function
 * 
 * @code
    function func_xxxx, export = 1
        ...
    endfunc
   @endcode
 */
.macro function name, export = 0
    .macro endfunc
#ifdef TB_ARCH_ELF
        .size \name, . - \name
#endif
        .purgem endfunc
    .endm

        .text
        .align TB_CPU_BITBYTE
    .if \export
        .global EXTERN_ASM\name
#ifdef TB_ARCH_ELF
        .hidden EXTERN_ASM\name
#endif
EXTERN_ASM\name:
    .endif
#ifdef TB_ARCH_ELF
        .type \name, %function
#endif
\name:
.endm

/*! label
 * 
 * @code
    label name
        ...
   @endcode
 */
.macro label name
        .align TB_CPU_BITBYTE
\name:
.endm

