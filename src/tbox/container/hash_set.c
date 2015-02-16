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
 * @file        hash_set.c
 * @ingroup     container
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "hash_set"
#define TB_TRACE_MODULE_DEBUG               (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "hash_set.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */
// the hash map itor item func type
typedef tb_pointer_t (*gb_hash_map_item_func_t)(tb_iterator_ref_t, tb_size_t);

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_pointer_t tb_hash_set_itor_item(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_assert_abort(iterator && iterator->priv);

    // the item func for the hash map
    gb_hash_map_item_func_t func = (gb_hash_map_item_func_t)iterator->priv;

    // get the item of the hash map
    tb_hash_map_item_ref_t item = func(iterator, itor);
    
    // get the item of the hash set
    return item? item->name : tb_null;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_hash_set_ref_t tb_hash_set_init(tb_size_t bucket_size, tb_item_func_t func)
{
    // init hash set
    tb_iterator_ref_t hash_set = (tb_iterator_ref_t)tb_hash_map_init(bucket_size, func, tb_item_func_true());

    // @note the private data of the hash map iterator cannot be used
    tb_assert_abort(!hash_set->priv);

    // hacking hash_map and hook the item func
    hash_set->priv = (tb_pointer_t)hash_set->item;
    hash_set->item = tb_hash_set_itor_item;

    // ok?
    return (tb_hash_set_ref_t)hash_set;
}
tb_void_t tb_hash_set_exit(tb_hash_set_ref_t hash_set)
{
    tb_hash_map_exit((tb_hash_map_ref_t)hash_set);
}
tb_void_t tb_hash_set_clear(tb_hash_set_ref_t hash_set)
{
    tb_hash_map_clear((tb_hash_map_ref_t)hash_set);
}
tb_bool_t tb_hash_set_get(tb_hash_set_ref_t hash_set, tb_cpointer_t data)
{
    return (tb_bool_t)tb_hash_map_get((tb_hash_map_ref_t)hash_set, data);
}
tb_size_t tb_hash_set_find(tb_hash_set_ref_t hash_set, tb_cpointer_t data)
{
    return tb_hash_map_find((tb_hash_map_ref_t)hash_set, data);
}
tb_size_t tb_hash_set_insert(tb_hash_set_ref_t hash_set, tb_cpointer_t data)
{
    return tb_hash_map_insert((tb_hash_map_ref_t)hash_set, data, tb_b2p(tb_true));
}
tb_void_t tb_hash_set_remove(tb_hash_set_ref_t hash_set, tb_cpointer_t data)
{
    tb_hash_map_remove((tb_hash_map_ref_t)hash_set, data);
}
tb_size_t tb_hash_set_size(tb_hash_set_ref_t hash_set)
{
    return tb_hash_map_size((tb_hash_map_ref_t)hash_set);
}
tb_size_t tb_hash_set_maxn(tb_hash_set_ref_t hash_set)
{
    return tb_hash_map_maxn((tb_hash_map_ref_t)hash_set);
}
#ifdef __tb_debug__
tb_void_t tb_hash_set_dump(tb_hash_set_ref_t hash_set)
{
    tb_hash_map_dump((tb_hash_map_ref_t)hash_set);
}
#endif

