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
#include "../libc/libc.h"
#include "../math/math.h"
#include "../memory/memory.h"
#include "../stream/stream.h"
#include "../platform/platform.h"
#include "../algorithm/algorithm.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the list impl item type
typedef struct __tb_list_item_t
{
    // the item next
    tb_size_t               next;

    // the item prev
    tb_size_t               prev;

}tb_list_item_t;

// the list impl type
typedef struct __tb_list_impl_t
{
    /// the itor
    tb_iterator_t           itor;

    // the pool
    tb_fixed_pool_ref_t     pool;

    // the head item
    tb_size_t               head;

    // the last item
    tb_size_t               last;

    // the func
    tb_item_func_t          func;

}tb_list_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * iterator
 */
static tb_size_t tb_list_itor_size(tb_iterator_ref_t iterator)
{
    // check
    tb_list_impl_t* impl = (tb_list_impl_t*)iterator;
    tb_assert_and_check_return_val(impl && impl->pool, 0);

    // the size
    return tb_fixed_pool_size(impl->pool);
}
static tb_size_t tb_list_itor_head(tb_iterator_ref_t iterator)
{
    // check
    tb_list_impl_t* impl = (tb_list_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);

    // head
    return impl->head;
}
static tb_size_t tb_list_itor_tail(tb_iterator_ref_t iterator)
{
    // check
    tb_list_impl_t* impl = (tb_list_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);

    // tail
    return 0;
}
static tb_size_t tb_list_itor_next(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_list_impl_t* impl = (tb_list_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);

    // next
    if (!itor) return impl->head;
    else return ((tb_list_item_t const*)itor)->next;
}
static tb_size_t tb_list_itor_prev(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_list_impl_t* impl = (tb_list_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);

    // prev
    if (!itor) return impl->last;
    else return ((tb_list_item_t const*)itor)->prev;
}
static tb_pointer_t tb_list_itor_item(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_list_impl_t* impl = (tb_list_impl_t*)iterator;
    tb_assert_and_check_return_val(impl && itor, tb_null);

    // data
    return impl->func.data(&impl->func, &((tb_list_item_t const*)itor)[1]);
}
static tb_void_t tb_list_itor_copy(tb_iterator_ref_t iterator, tb_size_t itor, tb_cpointer_t item)
{
    // check
    tb_list_impl_t* impl = (tb_list_impl_t*)iterator;
    tb_assert_and_check_return(impl && itor);

    // copy
    impl->func.copy(&impl->func, (tb_pointer_t)&((tb_list_item_t const*)itor)[1], item);
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

    // remove the body items
    if (prev) tb_list_nremove((tb_list_ref_t)iterator, tb_list_itor_next(iterator, prev), size);
    // remove the head items
    else tb_list_nremove_head((tb_list_ref_t)iterator, size);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_size_t tb_list_attach_prev(tb_list_impl_t* impl, tb_size_t itor, tb_size_t node)
{
    // check
    tb_list_item_t* pnode = (tb_list_item_t*)node;
    tb_assert_and_check_return_val(impl && impl->pool && pnode, 0);

    // init node
    pnode->prev = 0;
    pnode->next = 0;

    // is null?
    if (!impl->head && !impl->last)
    {
        /* impl: 0 => node => 0
         *       tail  head   tail
         *             last
         */
        impl->head = node;
        impl->last = node;
    }
    else
    {
        // check
        tb_assert_and_check_return_val(impl->head && impl->last, 0);

        // insert to tail
        if (!itor)
        {
            // the last node
            tb_size_t last = impl->last;
        
            // the last data
            tb_list_item_t* plast = (tb_list_item_t*)last;
            tb_assert_and_check_return_val(plast, 0);

            // last <=> node <=> 0
            plast->next = node;
            pnode->prev = last;

            // update the last node
            impl->last = node;
        }
        // insert to head
        else if (itor == impl->head)
        {
            // the head node
            tb_size_t head = impl->head;
        
            // the head data
            tb_list_item_t* phead = (tb_list_item_t*)head;
            tb_assert_and_check_return_val(phead, 0);

            // 0 <=> node <=> head
            phead->prev = node;
            pnode->next = head;

            // update the head node
            impl->head = node;
        }
        // insert to body
        else
        {
            // the body node
            tb_size_t body = itor;
        
            // the body data
            tb_list_item_t* pbody = (tb_list_item_t*)body;
            tb_assert_and_check_return_val(pbody, 0);

            // the prev node 
            tb_size_t prev = pbody->prev;

            // the prev data
            tb_list_item_t* pprev = (tb_list_item_t*)prev;
            tb_assert_and_check_return_val(pprev, 0);

            /* 0 <=> ... <=> prev <=> body <=> ... <=> 0
             * 0 <=> ... <=> prev <=> node <=> body <=> ... <=> 0
             */
            pnode->next = body;
            pnode->prev = prev;
            pprev->next = node;
            pbody->prev = node;
        }
    }

    // return the new node
    return node;
}
static tb_size_t tb_list_detach_item(tb_list_impl_t* impl, tb_size_t itor)
{
    // check
    tb_assert_and_check_return_val(impl && impl->pool && itor, 0);

    // not empty?
    tb_check_return_val(impl->head && impl->last, 0);

    // only one?
    if (impl->head == impl->last)
    {
        tb_assert_and_check_return_val(impl->head == itor, 0);
        impl->head = 0;
        impl->last = 0;
    }
    else
    {
        // remove head?
        if (itor == impl->head)
        {
            // the next node
            tb_size_t next = tb_iterator_next((tb_iterator_ref_t)impl, itor);

            // the next data
            tb_list_item_t* pnext = (tb_list_item_t*)next;
            tb_assert_and_check_return_val(pnext, 0);

            /* 0 <=> node <=> next <=> ... <=> 0
             * 0 <=> next <=> ... <=> 0
             */
            impl->head = next;
            pnext->prev = 0;
        }
        // remove last?
        else if (itor == impl->last)
        {
            // the prev node
            tb_size_t prev = tb_iterator_prev((tb_iterator_ref_t)impl, itor);

            // the prev data
            tb_list_item_t* pprev = (tb_list_item_t*)prev;
            tb_assert_and_check_return_val(pprev, 0);

            /* 0 <=> ... <=> prev <=> node <=> 0
             * 0 <=> ... <=> prev <=> 0
             */
            pprev->next = 0;
            impl->last = prev;
        }
        // remove body?
        else
        {
            // the body node
            tb_size_t body = itor;

            // the body data
            tb_list_item_t* pbody = (tb_list_item_t*)body;
            tb_assert_and_check_return_val(pbody, 0);

            // the next node
            tb_size_t next = pbody->next;

            // the next data
            tb_list_item_t* pnext = (tb_list_item_t*)next;
            tb_assert_and_check_return_val(pnext, 0);

            // the prev node
            tb_size_t prev = pbody->prev;

            // the prev data
            tb_list_item_t* pprev = (tb_list_item_t*)prev;
            tb_assert_and_check_return_val(pprev, 0);

            /* 0 <=> ... <=> prev <=> body <=> next <=> ... <=> 0
             * 0 <=> ... <=> prev <=> next <=> ... <=> 0
             */
            pprev->next = next;
            pnext->prev = prev;
        }
    }

    // ok?
    return itor;
}
/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_list_ref_t tb_list_init(tb_size_t grow, tb_item_func_t func)
{
    // check
    tb_assert_and_check_return_val(grow, tb_null);
    tb_assert_and_check_return_val(func.size && func.data && func.dupl && func.repl, tb_null);

    // done
    tb_bool_t       ok = tb_false;
    tb_list_impl_t* impl = tb_null;
    do
    {
        // make impl
        impl = tb_malloc0_type(tb_list_impl_t);
        tb_assert_and_check_break(impl);

        // init impl
        impl->head = 0;
        impl->last = 0;
        impl->func = func;

        // init iterator
        impl->itor.mode         = TB_ITERATOR_MODE_FORWARD | TB_ITERATOR_MODE_REVERSE;
        impl->itor.priv         = tb_null;
        impl->itor.step         = func.size;
        impl->itor.size         = tb_list_itor_size;
        impl->itor.head         = tb_list_itor_head;
        impl->itor.tail         = tb_list_itor_tail;
        impl->itor.prev         = tb_list_itor_prev;
        impl->itor.next         = tb_list_itor_next;
        impl->itor.item         = tb_list_itor_item;
        impl->itor.copy         = tb_list_itor_copy;
        impl->itor.comp         = tb_list_itor_comp;
        impl->itor.remove       = tb_list_itor_remove;
        impl->itor.remove_range = tb_list_itor_remove_range;

        // init pool, step = next + prev + data
        impl->pool = tb_fixed_pool_init(tb_null, grow, sizeof(tb_list_item_t) + func.size, tb_null, tb_null, tb_null);
        tb_assert_and_check_break(impl->pool);

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
    
    // free items
    if (impl->func.free)
    {
        tb_size_t itor = impl->head;
        while (itor)
        {
            // item
            tb_list_item_t* item = (tb_list_item_t*)itor;

            // free 
            impl->func.free(&impl->func, &item[1]);
    
            // next
            itor = item->next;
        }
    }

    // clear pool
    if (impl->pool) tb_fixed_pool_clear(impl->pool);

    // reset it
    impl->head = 0;
    impl->last = 0;
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

    // the size
    return tb_fixed_pool_size(impl->pool);
}
tb_size_t tb_list_maxn(tb_list_ref_t list)
{
    // check
    tb_assert_and_check_return_val(list, 0);
    return TB_MAXU32;
}
tb_size_t tb_list_insert_prev(tb_list_ref_t list, tb_size_t itor, tb_cpointer_t data)
{
    // check
    tb_list_impl_t* impl = (tb_list_impl_t*)list;
    tb_assert_and_check_return_val(impl && impl->pool, 0);

    // make the node data
    tb_list_item_t* pnode = (tb_list_item_t*)tb_fixed_pool_malloc(impl->pool);
    tb_assert_and_check_return_val(pnode, 0);

    // init node
    pnode->prev = 0;
    pnode->next = 0;
    impl->func.dupl(&impl->func, &pnode[1], data);

    // attach node
    return tb_list_attach_prev(impl, itor, (tb_size_t)pnode);
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
tb_size_t tb_list_ninsert_prev(tb_list_ref_t list, tb_size_t itor, tb_cpointer_t data, tb_size_t size)
{
    // check
    tb_list_impl_t* impl = (tb_list_impl_t*)list;
    tb_assert_and_check_return_val(impl && size, 0);

    // insert items
    tb_size_t node = itor;
    while (size--) node = tb_list_insert_prev(list, node, data);

    // return the first itor
    return node;
}
tb_size_t tb_list_ninsert_next(tb_list_ref_t list, tb_size_t itor, tb_cpointer_t data, tb_size_t size)
{
    return tb_list_ninsert_prev(list, tb_iterator_next(list, itor), data, size);
}
tb_size_t tb_list_ninsert_head(tb_list_ref_t list, tb_cpointer_t data, tb_size_t size)
{
    return tb_list_ninsert_prev(list, tb_iterator_head(list), data, size);
}
tb_size_t tb_list_ninsert_tail(tb_list_ref_t list, tb_cpointer_t data, tb_size_t size)
{
    return tb_list_ninsert_prev(list, tb_iterator_tail(list), data, size);
}
tb_size_t tb_list_replace(tb_list_ref_t list, tb_size_t itor, tb_cpointer_t data)
{
    // check
    tb_list_impl_t* impl = (tb_list_impl_t*)list;
    tb_assert_and_check_return_val(impl && itor, itor);

    // the item
    tb_list_item_t* item = (tb_list_item_t*)itor;

    // replace data
    impl->func.repl(&impl->func, &item[1], data);

    // ok
    return itor;
}
tb_size_t tb_list_replace_head(tb_list_ref_t list, tb_cpointer_t data)
{
    return tb_list_replace(list, tb_iterator_head(list), data);
}
tb_size_t tb_list_replace_last(tb_list_ref_t list, tb_cpointer_t data)
{
    return tb_list_replace(list, tb_iterator_last(list), data);
}
tb_size_t tb_list_nreplace(tb_list_ref_t list, tb_size_t itor, tb_cpointer_t data, tb_size_t size)
{
    // check
    tb_list_impl_t* impl = (tb_list_impl_t*)list;
    tb_assert_and_check_return_val(impl && data && size, itor);

    tb_size_t head = itor;
    tb_size_t tail = tb_iterator_tail((tb_iterator_ref_t)impl);
    for (; size-- && itor != tail; itor = tb_iterator_next((tb_iterator_ref_t)impl, itor)) 
        tb_list_replace(list, itor, data);
    return head;
}
tb_size_t tb_list_nreplace_head(tb_list_ref_t list, tb_cpointer_t data, tb_size_t size)
{
    return tb_list_nreplace(list, tb_iterator_head(list), data, size);
}
tb_size_t tb_list_nreplace_last(tb_list_ref_t list, tb_cpointer_t data, tb_size_t size)
{
    tb_size_t node = 0;
    tb_size_t itor = tb_iterator_last(list);
    tb_size_t tail = tb_iterator_tail(list);
    for (; size-- && itor != tail; itor = tb_iterator_prev(list, itor)) 
        node = tb_list_replace(list, itor, data);

    return node;
}
tb_size_t tb_list_remove(tb_list_ref_t list, tb_size_t itor)
{
    // check
    tb_list_impl_t* impl = (tb_list_impl_t*)list;
    tb_assert_and_check_return_val(impl && impl->pool && itor, itor);

    // detach item
    tb_size_t node = tb_list_detach_item(impl, itor);
    tb_assert_and_check_return_val(node && node == itor, itor);

    // next item
    tb_size_t next = tb_iterator_next((tb_iterator_ref_t)impl, node);

    // free item
    if (impl->func.free)
        impl->func.free(&impl->func, &((tb_list_item_t*)node)[1]);

    // free node
    tb_fixed_pool_free(impl->pool, (tb_pointer_t)node);

    // return next node
    return next;
}
tb_size_t tb_list_remove_next(tb_list_ref_t list, tb_size_t itor)
{
    return tb_list_remove(list, tb_iterator_next(list, itor));
}
tb_size_t tb_list_remove_head(tb_list_ref_t list)
{
    return tb_list_remove(list, tb_iterator_head(list));
}
tb_size_t tb_list_remove_last(tb_list_ref_t list)
{
    return tb_list_remove(list, tb_iterator_last(list));
}
tb_size_t tb_list_nremove(tb_list_ref_t list, tb_size_t itor, tb_size_t size)
{
    // check
    tb_list_impl_t* impl = (tb_list_impl_t*)list;
    tb_assert_and_check_return_val(impl && size, itor);

    tb_size_t next = itor;
    while (size--) next = tb_list_remove(list, next);
    return next;
}
tb_size_t tb_list_nremove_head(tb_list_ref_t list, tb_size_t size)
{
    while (size-- && tb_list_size(list)) tb_list_remove_head(list);
    return tb_iterator_head(list);
}
tb_size_t tb_list_nremove_last(tb_list_ref_t list, tb_size_t size)
{
    while (size-- && tb_list_size(list)) tb_list_remove_last(list);
    return tb_iterator_last(list);
}
tb_size_t tb_list_moveto_prev(tb_list_ref_t list, tb_size_t itor, tb_size_t move)
{
    // check
    tb_list_impl_t* impl = (tb_list_impl_t*)list;
    tb_assert_and_check_return_val(impl && impl->pool && move, move);

    // detach move
    tb_size_t node = tb_list_detach_item(impl, move);
    tb_assert_and_check_return_val(node && node == move, move);

    // attach move to prev
    return tb_list_attach_prev(impl, itor, node);
}
tb_size_t tb_list_moveto_next(tb_list_ref_t list, tb_size_t itor, tb_size_t move)
{
    return tb_list_moveto_prev(list, tb_iterator_next(list, itor), move);
}
tb_size_t tb_list_moveto_head(tb_list_ref_t list, tb_size_t move)
{
    return tb_list_moveto_prev(list, tb_iterator_head(list), move);
}
tb_size_t tb_list_moveto_tail(tb_list_ref_t list, tb_size_t move)
{
    return tb_list_moveto_prev(list, tb_iterator_tail(list), move);
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
