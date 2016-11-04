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
 * @file        context.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "context.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#if !defined(TB_ARCH_x86) && \
    !defined(TB_ARCH_x64) && \
    !defined(TB_ARCH_ARM) && \
    !defined(TB_ARCH_MIPS) 
tb_context_ref_t tb_context_make(tb_byte_t* stackdata, tb_size_t stacksize, tb_context_func_t func)
{
    tb_trace_noimpl();
    return tb_null;
}
tb_context_from_t tb_context_jump(tb_context_ref_t context, tb_cpointer_t priv)
{
    // noimpl
    tb_trace_noimpl();

    // return emtry context
    tb_context_from_t from = {0};
    return from;
}
#endif

