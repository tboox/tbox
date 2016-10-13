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
 * @file        stack.c
 * @ingroup     container
 *
 */
/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "stack.h"
#include "../libc/libc.h"
#include "../utils/utils.h"
#include "../memory/memory.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_stack_ref_t tb_stack_init(tb_size_t grow, tb_element_t element)
{
    return (tb_stack_ref_t)tb_vector_init(grow, element);
}
tb_void_t tb_stack_exit(tb_stack_ref_t self)
{
    tb_vector_exit((tb_vector_ref_t)self);
}
tb_void_t tb_stack_clear(tb_stack_ref_t self)
{
    tb_vector_clear((tb_vector_ref_t)self);
}
tb_void_t tb_stack_copy(tb_stack_ref_t self, tb_stack_ref_t copy)
{
    tb_vector_copy((tb_vector_ref_t)self, copy);
}
tb_void_t tb_stack_put(tb_stack_ref_t self, tb_cpointer_t data)
{
    tb_vector_insert_tail((tb_vector_ref_t)self, data);
}
tb_void_t tb_stack_pop(tb_stack_ref_t self)
{
    tb_vector_remove_last((tb_vector_ref_t)self);
}
tb_pointer_t tb_stack_top(tb_stack_ref_t self)
{
    return tb_vector_last((tb_vector_ref_t)self);
}
tb_pointer_t tb_stack_head(tb_stack_ref_t self)
{
    return tb_vector_head((tb_vector_ref_t)self);
}
tb_pointer_t tb_stack_last(tb_stack_ref_t self)
{
    return tb_vector_last((tb_vector_ref_t)self);
}
tb_size_t tb_stack_size(tb_stack_ref_t self)
{
    return tb_vector_size((tb_vector_ref_t)self);
}
tb_size_t tb_stack_maxn(tb_stack_ref_t self)
{
    return tb_vector_maxn((tb_vector_ref_t)self);
}
#ifdef __tb_debug__
tb_void_t tb_stack_dump(tb_stack_ref_t self)
{
    tb_vector_dump((tb_vector_ref_t)self);
}
#endif
