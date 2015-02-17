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
 * @file        queue.c
 * @ingroup     container
 *
 */
/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "queue.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_queue_ref_t tb_queue_init(tb_size_t grow, tb_item_func_t func)
{  
    return (tb_queue_ref_t)tb_single_list_init(grow, func);
}
tb_void_t tb_queue_exit(tb_queue_ref_t queue)
{   
    tb_single_list_exit((tb_single_list_ref_t)queue);
}
tb_void_t tb_queue_clear(tb_queue_ref_t queue)
{
    tb_single_list_clear((tb_single_list_ref_t)queue);
}
tb_void_t tb_queue_put(tb_queue_ref_t queue, tb_cpointer_t data)
{   
    tb_single_list_insert_tail((tb_single_list_ref_t)queue, data);
}
tb_void_t tb_queue_pop(tb_queue_ref_t queue)
{   
    tb_single_list_remove_head((tb_single_list_ref_t)queue);
}
tb_pointer_t tb_queue_get(tb_queue_ref_t queue)
{
    return tb_queue_head(queue);
}
tb_pointer_t tb_queue_head(tb_queue_ref_t queue)
{
    return tb_single_list_head((tb_single_list_ref_t)queue);
}
tb_pointer_t tb_queue_last(tb_queue_ref_t queue)
{
    return tb_single_list_last((tb_single_list_ref_t)queue);
}
tb_size_t tb_queue_size(tb_queue_ref_t queue)
{   
    return tb_single_list_size((tb_single_list_ref_t)queue);
}
tb_size_t tb_queue_maxn(tb_queue_ref_t queue)
{   
    return tb_single_list_maxn((tb_single_list_ref_t)queue);
}
tb_bool_t tb_queue_full(tb_queue_ref_t queue)
{   
    return (tb_single_list_size((tb_single_list_ref_t)queue) < tb_single_list_maxn((tb_single_list_ref_t)queue))? tb_false : tb_true;
}
tb_bool_t tb_queue_null(tb_queue_ref_t queue)
{   
    return tb_single_list_size((tb_single_list_ref_t)queue)? tb_false : tb_true;
}
#ifdef __tb_debug__
tb_void_t tb_queue_dump(tb_queue_ref_t queue)
{
    tb_single_list_dump((tb_single_list_ref_t)queue);
}
#endif
