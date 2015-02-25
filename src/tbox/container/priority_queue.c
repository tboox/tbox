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
 * @file        priority_queue.c
 * @ingroup     container
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "priority_queue.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_priority_queue_ref_t tb_priority_queue_init(tb_size_t grow, tb_element_t element)
{
    return (tb_priority_queue_ref_t)tb_heap_init(grow, element);
}
tb_void_t tb_priority_queue_exit(tb_priority_queue_ref_t queue)
{
    tb_heap_exit((tb_heap_ref_t)queue);
}
tb_void_t tb_priority_queue_clear(tb_priority_queue_ref_t queue)
{
    tb_heap_clear((tb_heap_ref_t)queue);
}
tb_size_t tb_priority_queue_size(tb_priority_queue_ref_t queue)
{
    return tb_heap_size((tb_heap_ref_t)queue);
}
tb_size_t tb_priority_queue_maxn(tb_priority_queue_ref_t queue)
{
    return tb_heap_maxn((tb_heap_ref_t)queue);
}
tb_pointer_t tb_priority_queue_get(tb_priority_queue_ref_t queue)
{
    return tb_heap_top((tb_heap_ref_t)queue);
}
tb_void_t tb_priority_queue_put(tb_priority_queue_ref_t queue, tb_cpointer_t data)
{
    tb_heap_put((tb_heap_ref_t)queue, data);
}
tb_void_t tb_priority_queue_pop(tb_priority_queue_ref_t queue)
{
    tb_heap_pop((tb_heap_ref_t)queue);
}
tb_void_t tb_priority_queue_remove(tb_priority_queue_ref_t queue, tb_size_t itor)
{
    tb_heap_remove((tb_heap_ref_t)queue, itor);
}
#ifdef __tb_debug__
tb_void_t tb_priority_queue_dump(tb_priority_queue_ref_t queue)
{
    tb_heap_dump((tb_heap_ref_t)queue);
}
#endif
