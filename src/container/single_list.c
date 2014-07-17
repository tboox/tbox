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
 * @file        single_list.c
 * @ingroup     container
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "single_list"
#define TB_TRACE_MODULE_DEBUG               (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "single_list.h"
#include "../libc/libc.h"
#include "../utils/utils.h"
#include "../memory/memory.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the single impl item type
typedef struct __tb_single_list_item_t
{
    // the item next
    tb_size_t           next;

}tb_single_list_item_t;

// the single list impl type
typedef struct __tb_single_list_impl_t
{
    // the itor
    tb_iterator_t           itor;

    // the pool
    tb_fixed_pool_ref_t     pool;

    // the head item 
    tb_size_t               head;

    // the last item
    tb_size_t               last;

    // the item func
    tb_item_func_t          func;

}tb_single_list_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * iterator
 */
static tb_size_t tb_single_list_itor_size(tb_iterator_ref_t iterator)
{
    // check
    tb_single_list_impl_t* impl = (tb_single_list_impl_t*)iterator;
    tb_assert_and_check_return_val(impl && impl->pool, 0);

    // the size
    return tb_fixed_pool_size(impl->pool);
}
static tb_size_t tb_single_list_itor_head(tb_iterator_ref_t iterator)
{
    // check
    tb_single_list_impl_t* impl = (tb_single_list_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);

    // head
    return impl->head;
}
static tb_size_t tb_single_list_itor_tail(tb_iterator_ref_t iterator)
{
    // check
    tb_single_list_impl_t* impl = (tb_single_list_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);

    // tail
    return 0;
}
static tb_size_t tb_single_list_itor_next(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_single_list_impl_t* impl = (tb_single_list_impl_t*)iterator;
    tb_assert_and_check_return_val(impl && itor, 0);

    // next
    return ((tb_single_list_item_t const*)itor)->next;
}
static tb_size_t tb_single_list_itor_prev(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_single_list_impl_t* impl = (tb_single_list_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);

    // is tail?
    tb_size_t prev = 0;
    if (!itor) prev = impl->last;
    // is head?
    else if (itor == impl->head) prev = 0;
    // find it
    else
    {
        tb_size_t node = tb_single_list_itor_head(iterator);
        tb_size_t tail = tb_single_list_itor_tail(iterator);
        for (prev = node; node != tail && node != itor; prev = node, node = tb_single_list_itor_next(iterator, node)) ;
    }

    return prev;
}
static tb_pointer_t tb_single_list_itor_item(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_single_list_impl_t* impl = (tb_single_list_impl_t*)iterator;
    tb_assert_and_check_return_val(impl && itor, tb_null);

    // data
    return impl->func.data(&impl->func, &((tb_single_list_item_t const*)itor)[1]);
}
static tb_void_t tb_single_list_itor_copy(tb_iterator_ref_t iterator, tb_size_t itor, tb_cpointer_t item)
{
    // check
    tb_single_list_impl_t* impl = (tb_single_list_impl_t*)iterator;
    tb_assert_and_check_return(impl && itor);

    // copy
    impl->func.copy(&impl->func, (tb_pointer_t)&((tb_single_list_item_t const*)itor)[1], item);
}
static tb_long_t tb_single_list_itor_comp(tb_iterator_ref_t iterator, tb_cpointer_t ltem, tb_cpointer_t rtem)
{
    // check
    tb_single_list_impl_t* impl = (tb_single_list_impl_t*)iterator;
    tb_assert_and_check_return_val(impl && impl->func.comp, 0);

    // comp
    return impl->func.comp(&impl->func, ltem, rtem);
}
static tb_void_t tb_single_list_itor_remove(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // remove it
    tb_single_list_remove((tb_single_list_ref_t)iterator, itor);
}
static tb_void_t tb_single_list_itor_remove_range(tb_iterator_ref_t iterator, tb_size_t prev, tb_size_t next, tb_size_t size)
{
    // no size?
    tb_check_return(size);

    // remove the body items
    if (prev) tb_single_list_nremove_next((tb_single_list_ref_t)iterator, prev, size);
    // remove the head items
    else tb_single_list_nremove_head((tb_single_list_ref_t)iterator, size);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_size_t tb_single_list_attach_next(tb_single_list_impl_t* impl, tb_size_t itor, tb_size_t node)
{
    // check
    tb_assert_and_check_return_val(impl && impl->pool && node, 0);

    // the prev node
    tb_size_t prev = itor;

    // init node, inode => 0
    tb_single_list_item_t* pnode = (tb_single_list_item_t*)node;
    pnode->next = 0;

    // non-empty?
    if (impl->head)
    {
        // is head?
        if (!prev)
        {
            // node => head
            pnode->next = impl->head;

            // update head
            impl->head = node;
        }
        // is last?
        else if (prev == impl->last)
        {
            // the prev data
            tb_single_list_item_t* pprev = (tb_single_list_item_t*)prev;
            tb_assert_and_check_return_val(pprev, 0);

            // last => node => null
            pprev->next = node;

            // update last
            impl->last = node;
        }
        // is body?
        else
        {
            // the prev data
            tb_single_list_item_t* pprev = (tb_single_list_item_t*)prev;
            tb_assert_and_check_return_val(pprev, 0);

            // node => next
            pnode->next = pprev->next;

            // prev => node
            pprev->next = node;
        }
    }
    // empty?
    else
    {
        // must be zero
        tb_assert_and_check_return_val(!prev, 0);

        // update head
        impl->head = node;

        // update last
        impl->last = node;
    }

    // return the new node
    return node;
}
static tb_size_t tb_single_list_detach_next(tb_single_list_impl_t* impl, tb_size_t itor)
{
    // check
    tb_assert_and_check_return_val(impl && impl->pool, 0);

    // non-empty?
    tb_check_return_val(impl->head, 0);

    // the prev node
    tb_size_t prev = itor;

    // the next node
    tb_size_t next = 0;

    // the midd node
    tb_size_t midd = 0;
    
    // remove head?
    if (!prev)
    {
        // the midd node
        midd = impl->head;

        // only one item?
        if (midd == impl->last)
        {
            impl->head = 0;
            impl->last = 0;
        }
        // update head
        else impl->head = next = tb_iterator_next((tb_iterator_ref_t)impl, midd);
    }
    // remove body?
    else
    {
        // the midd node
        midd = tb_iterator_next((tb_iterator_ref_t)impl, prev);

        // get the prev data
        tb_single_list_item_t* pprev = (tb_single_list_item_t*)prev;
        tb_assert_and_check_return_val(pprev, 0);

        // the next node
        next = tb_iterator_next((tb_iterator_ref_t)impl, midd);

        // prev => next
        pprev->next = next;

        // update last if midd is last
        if (midd == impl->last) impl->last = next = prev;
    }

    // ok?
    return midd;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_single_list_ref_t tb_single_list_init(tb_size_t grow, tb_item_func_t func)
{
    // check
    tb_assert_and_check_return_val(grow, tb_null);
    tb_assert_and_check_return_val(func.size && func.data && func.dupl && func.repl, tb_null);

    // done
    tb_bool_t               ok = tb_false;
    tb_single_list_impl_t*  impl = tb_null;
    do
    {
        // make impl
        impl = tb_malloc0_type(tb_single_list_impl_t);
        tb_assert_and_check_break(impl);

        // init impl
        impl->head = 0;
        impl->last = 0;
        impl->func = func;

        // init iterator
        impl->itor.mode         = TB_ITERATOR_MODE_FORWARD | TB_ITERATOR_MODE_REVERSE;
        impl->itor.priv         = tb_null;
        impl->itor.step         = func.size;
        impl->itor.size         = tb_single_list_itor_size;
        impl->itor.head         = tb_single_list_itor_head;
        impl->itor.tail         = tb_single_list_itor_tail;
        impl->itor.prev         = tb_single_list_itor_prev;
        impl->itor.next         = tb_single_list_itor_next;
        impl->itor.item         = tb_single_list_itor_item;
        impl->itor.copy         = tb_single_list_itor_copy;
        impl->itor.comp         = tb_single_list_itor_comp;
        impl->itor.remove       = tb_single_list_itor_remove;
        impl->itor.remove_range = tb_single_list_itor_remove_range;

        // init pool, step = next + data
        impl->pool = tb_fixed_pool_init(tb_null, grow, sizeof(tb_single_list_item_t) + func.size, tb_null, tb_null, tb_null);
        tb_assert_and_check_break(impl->pool);

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (impl) tb_single_list_exit((tb_single_list_ref_t)impl);
        impl = tb_null;
    }

    // ok?
    return (tb_single_list_ref_t)impl;
}
tb_void_t tb_single_list_exit(tb_single_list_ref_t list)
{
    // check
    tb_single_list_impl_t* impl = (tb_single_list_impl_t*)list;
    tb_assert_and_check_return(impl);

    // clear data
    tb_single_list_clear(list);

    // free pool
    if (impl->pool) tb_fixed_pool_exit(impl->pool);

    // free it
    tb_free(impl);
}
tb_void_t tb_single_list_clear(tb_single_list_ref_t list)
{
    // check
    tb_single_list_impl_t* impl = (tb_single_list_impl_t*)list;
    tb_assert_and_check_return(impl);

    // free items
    if (impl->func.free)
    {
        tb_size_t itor = impl->head;
        while (itor)
        {
            // item
            tb_single_list_item_t* item = (tb_single_list_item_t*)itor;

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
tb_pointer_t tb_single_list_head(tb_single_list_ref_t list)
{
    return tb_iterator_item((tb_iterator_ref_t)list, tb_iterator_head((tb_iterator_ref_t)list));
}
tb_pointer_t tb_single_list_last(tb_single_list_ref_t list)
{
    return tb_iterator_item((tb_iterator_ref_t)list, tb_iterator_last((tb_iterator_ref_t)list));
}
tb_size_t tb_single_list_size(tb_single_list_ref_t list)
{
    // check
    tb_single_list_impl_t const* impl = (tb_single_list_impl_t const*)list;
    tb_assert_and_check_return_val(impl && impl->pool, 0);

    // the size
    return tb_fixed_pool_size(impl->pool);
}
tb_size_t tb_single_list_maxn(tb_single_list_ref_t list)
{
    // check
    tb_single_list_impl_t const* impl = (tb_single_list_impl_t const*)list;
    tb_assert_and_check_return_val(impl, 0);

    // the maxn
    return TB_MAXU32;
}
/* insert node:
 *
 * itor == a: 
 * before: [a] => ... => [b] => [0]
 *         head         last   tail
 *
 * after:  [node] => [a] => ... => [b] => [0]
 *          head                  last    tail
 *
 * itor == b: 
 * before: [a] => ... => [b] => [c] => [0]
 *         head                last   tail
 *
 * after:  [a] => ... => [node] => [b] => [c] => [0]
 *         head                          last   tail
 *
 * itor == 0: 
 * before: [a] => ... => [b] => [0]
 *         head         last   tail
 *
 * after:  [a] => ... => [b] => [node] => [0]
 *         head                  last    tail
 *
 */
tb_size_t tb_single_list_insert_prev(tb_single_list_ref_t list, tb_size_t itor, tb_cpointer_t data)
{
    return tb_single_list_insert_next(list, tb_iterator_prev(list, itor), data);
}
tb_size_t tb_single_list_insert_next(tb_single_list_ref_t list, tb_size_t itor, tb_cpointer_t data)
{
    // check
    tb_single_list_impl_t* impl = (tb_single_list_impl_t*)list;
    tb_assert_and_check_return_val(impl && impl->pool, 0);

    // make the node data
    tb_single_list_item_t* pnode = (tb_single_list_item_t*)tb_fixed_pool_malloc(impl->pool);
    tb_assert_and_check_return_val(pnode, 0);

    // init node, inode => 0
    pnode->next = 0;

    // init data
    impl->func.dupl(&impl->func, &pnode[1], data);

    // attach next
    return tb_single_list_attach_next(impl, itor, (tb_size_t)pnode);
}
tb_size_t tb_single_list_insert_head(tb_single_list_ref_t list, tb_cpointer_t data)
{
    return tb_single_list_insert_prev(list, tb_iterator_head(list), data);
}
tb_size_t tb_single_list_insert_tail(tb_single_list_ref_t list, tb_cpointer_t data)
{
    return tb_single_list_insert_prev(list, tb_iterator_tail(list), data);
}
tb_size_t tb_single_list_ninsert_prev(tb_single_list_ref_t list, tb_size_t itor, tb_cpointer_t data, tb_size_t size)
{
    return tb_single_list_ninsert_next(list, tb_iterator_prev(list, itor), data, size);
}
tb_size_t tb_single_list_ninsert_next(tb_single_list_ref_t list, tb_size_t itor, tb_cpointer_t data, tb_size_t size)
{
    // check
    tb_single_list_impl_t* impl = (tb_single_list_impl_t*)list;
    tb_assert_and_check_return_val(impl && size, 0);

    // insert items
    tb_size_t node = itor;
    while (size--) node = tb_single_list_insert_next(list, itor, data);

    // return the first itor
    return node;
}
tb_size_t tb_single_list_ninsert_head(tb_single_list_ref_t list, tb_cpointer_t data, tb_size_t size)
{
    return tb_single_list_ninsert_prev(list, tb_iterator_head(list), data, size);
}
tb_size_t tb_single_list_ninsert_tail(tb_single_list_ref_t list, tb_cpointer_t data, tb_size_t size)
{
    return tb_single_list_ninsert_prev(list, tb_iterator_tail(list), data, size);
}
tb_size_t tb_single_list_replace(tb_single_list_ref_t list, tb_size_t itor, tb_cpointer_t data)
{
    // check
    tb_single_list_impl_t* impl = (tb_single_list_impl_t*)list;
    tb_assert_and_check_return_val(impl && itor, itor);

    // the item
    tb_single_list_item_t* item = (tb_single_list_item_t*)itor;

    // replace data
    impl->func.repl(&impl->func, &item[1], data);

    // ok
    return itor;
}
tb_size_t tb_single_list_replace_head(tb_single_list_ref_t list, tb_cpointer_t data)
{
    return tb_single_list_replace(list, tb_iterator_head(list), data);
}
tb_size_t tb_single_list_replace_last(tb_single_list_ref_t list, tb_cpointer_t data)
{
    return tb_single_list_replace(list, tb_iterator_last(list), data);
}
tb_size_t tb_single_list_nreplace(tb_single_list_ref_t list, tb_size_t itor, tb_cpointer_t data, tb_size_t size)
{
    // check
    tb_single_list_impl_t* impl = (tb_single_list_impl_t*)list;
    tb_assert_and_check_return_val(impl && size, itor);

    tb_size_t head = itor;
    tb_size_t tail = tb_iterator_tail((tb_iterator_ref_t)impl);
    for (; size-- && itor != tail; itor = tb_iterator_next((tb_iterator_ref_t)impl, itor)) 
        tb_single_list_replace(list, itor, data);
    return head;
}
tb_size_t tb_single_list_nreplace_head(tb_single_list_ref_t list, tb_cpointer_t data, tb_size_t size)
{
    return tb_single_list_nreplace(list, tb_iterator_head(list), data, size);
}
tb_size_t tb_single_list_nreplace_last(tb_single_list_ref_t list, tb_cpointer_t data, tb_size_t size)
{
    // compute offset
    tb_size_t n = tb_single_list_size(list);
    tb_size_t o = n > size? n - size : 0;
    
    // seek
    tb_size_t itor = tb_iterator_head(list);
    tb_size_t tail = tb_iterator_tail(list);
    for (; o-- && itor != tail; itor = tb_iterator_next(list, itor)) ;

    // replace
    return tb_single_list_nreplace(list, itor, data, size);
}
/* remove node:
 *
 * itor == node: 
 * before: [node] => [a] => ... => [b] => [0]
 *          head                  last    tail

 * after:  [a] => ... => [b] => [0]
 *         head         last   tail
 *
 *
 * itor == node: 
 * before: [a] => ... => [node] => [b] => [c] => [0]
 *         head                          last   tail
 * after:  [a] => ... => [b] => [c] => [0]
 *         head                last   tail
 *
 * itor == node: 
 * before: [a] => ... => [b] => [node] => [0]
 *         head                  last    tail
 * after:  [a] => ... => [b] => [0]
 *         head         last   tail
 *
 *
 */
tb_size_t tb_single_list_remove(tb_single_list_ref_t list, tb_size_t itor)
{   
    return tb_single_list_remove_next(list, tb_iterator_prev(list, itor));
}
tb_size_t tb_single_list_remove_next(tb_single_list_ref_t list, tb_size_t itor)
{
    // check
    tb_single_list_impl_t* impl = (tb_single_list_impl_t*)list;
    tb_assert_and_check_return_val(impl && impl->pool, itor);

    // detach next
    tb_size_t node = tb_single_list_detach_next(impl, itor);
    tb_assert_and_check_return_val(node, itor);

    // next item
    tb_size_t next = tb_iterator_next((tb_iterator_ref_t)impl, node);

    // free item
    if (impl->func.free)
        impl->func.free(&impl->func, &((tb_single_list_item_t*)node)[1]);

    // free node
    tb_fixed_pool_free(impl->pool, (tb_pointer_t)node);

    // return next node
    return next;
}
tb_size_t tb_single_list_remove_head(tb_single_list_ref_t list)
{
    return tb_single_list_remove(list, tb_iterator_head(list));
}
tb_size_t tb_single_list_remove_last(tb_single_list_ref_t list)
{
    return tb_single_list_remove(list, tb_iterator_last(list));
}
tb_size_t tb_single_list_nremove(tb_single_list_ref_t list, tb_size_t itor, tb_size_t size)
{
    return tb_single_list_nremove_next(list, tb_iterator_prev(list, itor), size);
}
tb_size_t tb_single_list_nremove_next(tb_single_list_ref_t list, tb_size_t itor, tb_size_t size)
{
    // check
    tb_single_list_impl_t* impl = (tb_single_list_impl_t*)list;
    tb_assert_and_check_return_val(impl && size, itor);

    tb_size_t next = itor;
    while (size--) next = tb_single_list_remove_next(list, itor);
    return next;
}
tb_size_t tb_single_list_nremove_head(tb_single_list_ref_t list, tb_size_t size)
{
    tb_single_list_nremove(list, tb_iterator_head(list), size);
    return tb_iterator_head(list);
}
tb_size_t tb_single_list_nremove_last(tb_single_list_ref_t list, tb_size_t size)
{
    // compute offset
    tb_size_t n = tb_single_list_size(list);
    tb_size_t o = n > size? n - size : 0;
    
    // seek
    tb_size_t prev = 0;
    tb_size_t itor = tb_iterator_head(list);
    tb_size_t tail = tb_iterator_tail(list);
    for (; o-- && itor != tail; prev = itor, itor = tb_iterator_next(list, itor)) ;

    // remove
    tb_single_list_nremove_next(list, prev, size);

    // ok?
    return tb_iterator_last(list);
}
tb_size_t tb_single_list_moveto_prev(tb_single_list_ref_t list, tb_size_t itor, tb_size_t move)
{
    return tb_single_list_moveto_next(list, tb_iterator_prev(list, itor), move);
}
tb_size_t tb_single_list_moveto_next(tb_single_list_ref_t list, tb_size_t itor, tb_size_t move)
{
    // check
    tb_single_list_impl_t* impl = (tb_single_list_impl_t*)list;
    tb_assert_and_check_return_val(impl && impl->pool && move, move);

    // detach move
    tb_size_t node = tb_single_list_detach_next(impl, tb_iterator_prev((tb_iterator_ref_t)impl, move));
    tb_assert_and_check_return_val(node && node == move, move);

    // attach move to next
    return tb_single_list_attach_next(impl, itor, node);
}
tb_size_t tb_single_list_moveto_head(tb_single_list_ref_t list, tb_size_t move)
{
    return tb_single_list_moveto_prev(list, tb_iterator_head(list), move);
}
tb_size_t tb_single_list_moveto_tail(tb_single_list_ref_t list, tb_size_t move)
{
    return tb_single_list_moveto_prev(list, tb_iterator_tail(list), move);
}

