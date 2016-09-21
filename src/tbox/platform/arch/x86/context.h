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
 * @file        context.h
 *
 */
#ifndef TB_PLATFORM_ARCH_x86_CONTEXT_H
#define TB_PLATFORM_ARCH_x86_CONTEXT_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// enable arch context implementation
#define TB_CONTEXT_ARCH_IMPL 

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the context type
typedef struct __tb_context_t
{
    // the registers
    tb_uint32_t             edi;
    tb_uint32_t             esi;
    tb_uint32_t             ebx;
    tb_uint32_t             ebp;
    tb_uint32_t             eip;

}tb_context_t;

#endif
