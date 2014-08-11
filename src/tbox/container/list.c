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
 * @file        list.c
 * @ingroup     container
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "list"
#define TB_TRACE_MODULE_DEBUG               (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "list.h"
#include "list_entry.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../memory/memory.h"
#include "../stream/stream.h"
#include "../platform/platform.h"
#include "../algorithm/algorithm.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the list grow
#ifdef __tb_small__ 
#   define TB_LIST_GROW             (128)
#else
#   define TB_LIST_GROW             (256)
#endif

// the list maxn
#ifdef __tb_small__
#   define TB_LIST_MAXN             (1 << 16)
#else
#   define TB_LIST_MAXN             (1 << 30)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the list impl type
typedef struct __tb_list_impl_t
{
    // the itor
    tb_iterator_t               itor;

    // the pool
    tb_fixed_pool_ref_t         pool;

    // the head
    tb_list_entry_head_t        head;

    // the func
    tb_item_func_t              func;

}tb_list_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_size_t tb_list_itor_size(tb_iterator_ref_t iterator)
{
    // the size
    return tb_list_size((tb_list_ref_t)iterator);
}
static tb_size_t tb_list_itor_head(tb_iterator_ref_t iterator)
{
    // check
    tb_list_impl_t* impl = (tb_list_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);

    // head
    return (tb_size_t)tb_list_entry_head(&impl->head);
}
static tb_size_t tb_list_itor_last(tb_iterator_ref_t iterator)
{
    // check
    tb_list_impl_t* impl = (tb_list_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);

    // last
    return (tb_size_t)tb_list_entry_last(&impl->head);
}
static tb_size_t tb_list_itor_tail(tb_iterator_ref_t iterator)
{
    // check
    tb_list_impl_t* impl = (tb_list_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);

    // tail
    return (tb_size_t)tb_list_entry_tail(&impl->head);
}
static tb_size_t tb_list_itor_next(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_list_impl_t* impl = (tb_list_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);
    tb_assert_and_check_return_val(itor, tb_list_itor_tail(iterator));

    // next
    return (tb_size_t)tb_list_entry_next(&impl->head, (tb_list_entry_t*)itor);
}
static tb_size_t tb_list_itor_prev(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_list_impl_t* impl = (tb_list_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);
    tb_assert_and_check_return_val(itor, tb_list_itor_tail(iterator));

    // prev
    return (tb_size_t)tb_list_entry_prev(&impl->head, (tb_list_entry_t*)itor);
}
static tb_pointer_t tb_list_itor_item(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_list_impl_t* impl = (tb_list_impl_t*)iterator;
    tb_assert_and_check_return_val(impl && itor, tb_null);

    // data
    return impl->func.data(&impl->func, (tb_cpointer_t)(((tb_list_entry_t*)itor) + 1));
}
static tb_void_t tb_list_itor_copy(tb_iterator_ref_t iterator, tb_size_t itor, tb_cpointer_t item)
{
    // check
    tb_list_impl_t* impl = (tb_list_impl_t*)iterator;
    tb_assert_and_check_return(impl && itor);

    // copy
    impl->func.copy(&impl->func, (tb_pointer_t)(((tb_list_entry_t*)itor) + 1), item);
}
static tb_long_t tb_list_itor_comp(tb_iterator_ref_t iterator, tb_cpointer_t ltem, tb_cpointer_t rtem)
{
    // check
    tb_list_impl_t* impl = (tb_list_impl_t*)iterator;
    tb_assert_and_check_return_val(impl && impl->func.comp, 0);

    // comp
    return impl->func.comp(&impl->func, ltem, rtem);
}
static tb_void_t tb_list_itor_remove(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // remove it
    tb_list_remove((tb_list_ref_t)iterator, itor);
}
static tb_void_t tb_list_itor_remove_range(tb_iterator_ref_t iterator, tb_size_t prev, tb_size_t next, tb_size_t size)
{
    // no size?
    tb_check_return(size);

    // the list size
    tb_size_t list_size = tb_list_size((tb_list_ref_t)iterator);
    tb_check_return(list_size);

    // limit size
    if (size > list_size) size = list_size;

    // remove the body items
    if (prev) 
    {
        tb_size_t itor = tb_iterator_next((tb_list_ref_t)iterator, prev);
        while (itor != next && size--) itor = tb_list_remove((tb_list_ref_t)iterator, itor);
    }
    // remove the head items
    else 
    {
        while (size--) tb_list_remove_head((tb_list_ref_t)iterator);
    }
}
static tb_void_t tb_list_item_exit(tb_pointer_t data, tb_cpointer_t priv)
{
    // check
    tb_list_impl_t* impl = (tb_list_impl_t*)priv;
    tb_assert_and_check_return(impl);

    // free data
    if (impl->func.free) impl->func.free(&impl->func, (tb_pointer_t)(((tb_list_entry_t*)data) + 1));
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_list_ref_t tb_list_init(tb_size_t grow, tb_item_func_t func)
{
    // check
    tb_assert_and_check_return_val(func.size && func.data && func.dupl && func.repl, tb_null);

    // done
    tb_bool_t       ok = tb_false;
    tb_list_impl_t* impl = tb_null;
    do
    {
        // using the default grow
        if (!grow) grow = TB_LIST_GROW;

        // make list
        impl = tb_malloc0_type(tb_list_impl_t);
        tb_assert_and_check_break(impl);

        // init func
        impl->func = func;

        // init iterator
        impl->itor.mode         = TB_ITERATOR_MODE_FORWARD | TB_ITERATOR_MODE_REVERSE;
        impl->itor.priv         = tb_null;
        impl->itor.step         = func.size;
        impl->itor.size         = tb_list_itor_size;
        impl->itor.head         = tb_list_itor_head;
        impl->itor.last         = tb_list_itor_last;
        impl->itor.tail         = tb_list_itor_tail;
        impl->itor.prev         = tb_list_itor_prev;
        impl->itor.next         = tb_list_itor_next;
        impl->itor.item         = tb_list_itor_item;
        impl->itor.copy         = tb_list_itor_copy;
        impl->itor.comp         = tb_list_itor_comp;
        impl->itor.remove       = tb_list_itor_remove;
        impl->itor.remove_range = tb_list_itor_remove_range;

        // init pool, item = entry + data
        impl->pool = tb_fixed_pool_init(tb_null, grow, sizeof(tb_list_entry_t) + func.size, tb_null, tb_list_item_exit, (tb_cpointer_t)impl);
        tb_assert_and_check_break(impl->pool);

        // init head
        tb_list_entry_init_(&impl->head, 0, sizeof(tb_list_entry_t) + func.size, tb_null);

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (impl) tb_list_exit((tb_list_ref_t)impl);
        impl = tb_null;
    }

    // ok?
    return (tb_list_ref_t)impl;
}
tb_void_t tb_list_exit(tb_list_ref_t list)
{
    // check
    tb_list_impl_t* impl = (tb_list_impl_t*)list;
    tb_assert_and_check_return(impl);
    
    // clear data
    tb_list_clear((tb_list_ref_t)impl);

    // free pool
    if (impl->pool) tb_fixed_pool_exit(impl->pool);

    // free it
    tb_free(impl);
}
tb_void_t tb_list_clear(tb_list_ref_t list)
{
    // check
    tb_list_impl_t* impl = (tb_list_impl_t*)list;
    tb_assert_and_check_return(impl);

    // clear pool
    if (impl->pool) tb_fixed_pool_clear(impl->pool);

    // clear head
    tb_list_entry_clear(&impl->head);
}
tb_pointer_t tb_list_head(tb_list_ref_t list)
{
    return tb_iterator_item(list, tb_iterator_head(list));
}
tb_pointer_t tb_list_last(tb_list_ref_t list)
{
    return tb_iterator_item(list, tb_iterator_last(list));
}
tb_size_t tb_list_size(tb_list_ref_t list)
{
    // check
    tb_list_impl_t* impl = (tb_list_impl_t*)list;
    tb_assert_and_check_return_val(impl && impl->pool, 0);
    tb_assert_abort(tb_list_entry_size(&impl->head) == tb_fixed_pool_size(impl->pool));

    // the size
    return tb_list_entry_size(&impl->head);
}
tb_size_t tb_list_maxn(tb_list_ref_t list)
{
    // the item maxn
    return TB_LIST_MAXN;
}
tb_size_t tb_list_insert_prev(tb_list_ref_t list, tb_size_t itor, tb_cpointer_t data)
{
    // check
    tb_list_impl_t* impl = (tb_list_impl_t*)list;
    tb_assert_and_check_return_val(impl && impl->func.dupl && impl->pool, 0);

    // full?
    tb_assert_and_check_return_val(tb_list_size(list) < tb_list_maxn(list), tb_iterator_tail(list));

    // the node
    tb_list_entry_ref_t node = (tb_list_entry_ref_t)itor;
    tb_assert_and_check_return_val(node, tb_iterator_tail(list));

    // make entry
    tb_list_entry_ref_t entry = (tb_list_entry_ref_t)tb_fixed_pool_malloc(impl->pool);
    tb_assert_and_check_return_val(entry, tb_iterator_tail(list));

    // init entry data
    impl->func.dupl(&impl->func, (tb_pointer_t)(((tb_list_entry_t*)entry) + 1), data);

    // insert it
    tb_list_entry_insert_prev(&impl->head, node, entry);

    // ok
    return (tb_size_t)entry;
}
tb_size_t tb_list_insert_next(tb_list_ref_t list, tb_size_t itor, tb_cpointer_t data)
{
    return tb_list_insert_prev(list, tb_iterator_next(list, itor), data);
}
tb_size_t tb_list_insert_head(tb_list_ref_t list, tb_cpointer_t data)
{
    return tb_list_insert_prev(list, tb_iterator_head(list), data);
}
tb_size_t tb_list_insert_tail(tb_list_ref_t list, tb_cpointer_t data)
{
    return tb_list_insert_prev(list, tb_iterator_tail(list), data);
}
tb_void_t tb_list_replace(tb_list_ref_t list, tb_size_t itor, tb_cpointer_t data)
{
    // check
    tb_list_impl_t* impl = (tb_list_impl_t*)list;
    tb_assert_and_check_return(impl && impl->func.repl && itor);

    // the node
    tb_list_entry_ref_t node = (tb_list_entry_ref_t)itor;
    tb_assert_and_check_return(node);

    // replace data
    impl->func.repl(&impl->func, (tb_pointer_t)(((tb_list_entry_t*)node) + 1), data);
}
tb_void_t tb_list_replace_head(tb_list_ref_t list, tb_cpointer_t data)
{
    tb_list_replace(list, tb_iterator_head(list), data);
}
tb_void_t tb_list_replace_last(tb_list_ref_t list, tb_cpointer_t data)
{
    tb_list_replace(list, tb_iterator_last(list), data);
}
tb_size_t tb_list_remove(tb_list_ref_t list, tb_size_t itor)
{
    // check
    tb_list_impl_t* impl = (tb_list_impl_t*)list;
    tb_assert_and_check_return_val(impl && impl->pool && itor, 0);

    // the node
    tb_list_entry_ref_t node = (tb_list_entry_ref_t)itor;
    tb_assert_and_check_return_val(node, tb_iterator_tail(list));

    // the next node
    tb_list_entry_ref_t next = tb_list_entry_next(&impl->head, node);

    // remove node
    tb_list_entry_remove(&impl->head, node);

    // free node
    tb_fixed_pool_free(impl->pool, node);
    
    // the next node
    return (tb_size_t)next;
}
tb_void_t tb_list_remove_head(tb_list_ref_t list)
{
    tb_list_remove(list, tb_iterator_head(list));
}
tb_void_t tb_list_remove_last(tb_list_ref_t list)
{
    tb_list_remove(list, tb_iterator_last(list));
}
tb_void_t tb_list_moveto_prev(tb_list_ref_t list, tb_size_t itor, tb_size_t move)
{
    // check
    tb_list_impl_t* impl = (tb_list_impl_t*)list;
    tb_assert_and_check_return(impl && impl->pool && move);

    // the node
    tb_list_entry_ref_t node = (tb_list_entry_ref_t)itor;
    tb_assert_and_check_return(node);

    // the entry
    tb_list_entry_ref_t entry = (tb_list_entry_ref_t)move;
    tb_assert_and_check_return(entry);

    // move to the prev node
    tb_list_entry_moveto_prev(&impl->head, node, entry);
}
tb_void_t tb_list_moveto_next(tb_list_ref_t list, tb_size_t itor, tb_size_t move)
{
    tb_list_moveto_prev(list, tb_iterator_next(list, itor), move);
}
tb_void_t tb_list_moveto_head(tb_list_ref_t list, tb_size_t move)
{
    tb_list_moveto_prev(list, tb_iterator_head(list), move);
}
tb_void_t tb_list_moveto_tail(tb_list_ref_t list, tb_size_t move)
{
    tb_list_moveto_prev(list, tb_iterator_tail(list), move);
}
tb_bool_t tb_list_load(tb_list_ref_t list, tb_stream_ref_t stream)
{
    // check
    tb_list_impl_t* impl = (tb_list_impl_t*)list;
    tb_assert_and_check_return_val(impl && stream, tb_false);
    tb_assert_and_check_return_val(impl->func.hash && impl->func.load && impl->func.free, tb_false);

    // clear the list first
    tb_list_clear(list);
  
    // the offset
    tb_hize_t offset = tb_stream_offset(stream);

    // done
    tb_bool_t       ok = tb_false;
    tb_uint32_t     crc32 = 0;
    tb_pointer_t    buff = tb_null;
    do
    {
        // calc type
        crc32 = tb_crc_encode_cstr(TB_CRC_MODE_32_IEEE_LE, crc32, "list");

        // calc item type
        crc32 = tb_crc_encode_value(TB_CRC_MODE_32_IEEE_LE, crc32, impl->func.type);

        // calc item size
        crc32 = tb_crc_encode_value(TB_CRC_MODE_32_IEEE_LE, crc32, impl->func.size);

        // load the head crc32
        tb_uint32_t crc32_head = tb_stream_bread_u32_be(stream);
        tb_assert_and_check_break(crc32_head == crc32);

        // make item buffer
        buff = impl->func.size? tb_malloc(impl->func.size) : tb_null;

        // load size
        tb_uint32_t size = tb_stream_bread_u32_be(stream);

        // load list
        tb_uint32_t load = 0;
        for (load = 0; load < size; load++)
        {
            // load item
            if (!impl->func.load(&impl->func, buff, stream)) break;

            // the item data
            tb_cpointer_t data = impl->func.data(&impl->func, buff);

            // hash item
            tb_size_t hash = impl->func.hash(&impl->func, data, -1, 0);

            // calc item
            crc32 = tb_crc_encode_value(TB_CRC_MODE_32_IEEE_LE, crc32, hash);

            // save item
            tb_list_insert_tail(list, data);

            // free name
            impl->func.free(&impl->func, buff);
        }

        // check
        tb_assert_and_check_break(load == size);

        // load the body crc32
        tb_uint32_t crc32_body = tb_stream_bread_u32_be(stream);
        tb_assert_and_check_break(crc32_body == crc32);

        // ok
        ok = tb_true;

    } while (0);

    // failed? 
    if (!ok) 
    {
        // restore it
        tb_stream_seek(stream, offset);

        // clear it
        tb_list_clear(list);
    }

    // exit buffer
    if (buff) tb_free(buff);
    buff = tb_null;

    // ok?
    return ok;
}
tb_bool_t tb_list_save(tb_list_ref_t list, tb_stream_ref_t stream)
{
    // check
    tb_list_impl_t* impl = (tb_list_impl_t*)list;
    tb_assert_and_check_return_val(impl && stream, tb_false);
    tb_assert_and_check_return_val(impl->func.hash && impl->func.save, tb_false);
    
    // the offset
    tb_hize_t offset = tb_stream_offset(stream);

    // done
    tb_bool_t   ok = tb_false;
    tb_uint32_t crc32 = 0;
    do
    {
        // calc type
        crc32 = tb_crc_encode_cstr(TB_CRC_MODE_32_IEEE_LE, crc32, "list");

        // calc item type
        crc32 = tb_crc_encode_value(TB_CRC_MODE_32_IEEE_LE, crc32, impl->func.type);

        // calc item size
        crc32 = tb_crc_encode_value(TB_CRC_MODE_32_IEEE_LE, crc32, impl->func.size);

        // save the head crc32
        if (!tb_stream_bwrit_u32_be(stream, crc32)) break;

        // the size
        tb_size_t size = tb_list_size(list);

        // save size
        if (!tb_stream_bwrit_u32_be(stream, (tb_uint32_t)size)) break;
        
        // save list
        tb_size_t save = 0;
        tb_for_all (tb_cpointer_t, item, list)
        {
            // save item
            if (!impl->func.save(&impl->func, item, stream)) break;

            // hash item
            tb_size_t hash = impl->func.hash(&impl->func, item, -1, 0);

            // calc item
            crc32 = tb_crc_encode_value(TB_CRC_MODE_32_IEEE_LE, crc32, hash);

            // update the save count
            save++;
        }

        // check
        tb_assert_and_check_break(save == size); 
 
        // save the body crc32
        if (!tb_stream_bwrit_u32_be(stream, crc32)) break;
        
        // ok
        ok = tb_true;

    } while (0);

    // failed? restore it
    if (!ok) tb_stream_seek(stream, offset);

    // ok?
    return ok;
}
