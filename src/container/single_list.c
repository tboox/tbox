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

// the single list item type
typedef struct __tb_single_list_item_t
{
    // the item next
    tb_size_t           next;

}tb_single_list_item_t;

/*! the single list type
 *
 * <pre>
 * list:  |-----| => |-------------------------------------------------=> |------| => |------| => 0
 *         head                                                                         last      tail
 *
 * head: => the first item
 * last: => the last item
 * tail: => behind the last item, no item
 
 * performance: 
 *
 * insert:
 * insert midd: slow
 * insert head: fast
 * insert tail: fast
 * insert next: fast
 * 
 * ninsert:
 * ninsert midd: slow
 * ninsert head: fast
 * ninsert tail: fast
 * ninsert next: fast
 *
 * remove:
 * remove midd: slow
 * remove head: fast
 * remove last: slow
 * remove next: fast
 *
 * nremove:
 * nremove midd: slow
 * nremove head: fast
 * nremove last: slow
 * nremove next: fast
 *
 * iterator:
 * next: fast
 * prev: slow
 * </pre>
 *
 */
typedef struct __tb_single_list_impl_t
{
    /// the itor
    tb_iterator_t           itor;

    /// the rpool
    tb_handle_t             pool;

    /// the head item 
    tb_size_t               head;

    /// the last item
    tb_size_t               last;

    /// the item func
    tb_item_func_t          func;

}tb_single_list_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * iterator
 */
static tb_size_t tb_single_list_iterator_size(tb_iterator_t* iterator)
{
    // check
    tb_single_list_impl_t* list = (tb_single_list_impl_t*)iterator;
    tb_assert_and_check_return_val(list && list->pool, 0);

    // the size
    return tb_fixed_pool_size(list->pool);
}
static tb_size_t tb_single_list_iterator_head(tb_iterator_t* iterator)
{
    // check
    tb_single_list_impl_t* list = (tb_single_list_impl_t*)iterator;
    tb_assert_and_check_return_val(list, 0);

    // head
    return list->head;
}
static tb_size_t tb_single_list_iterator_tail(tb_iterator_t* iterator)
{
    // check
    tb_single_list_impl_t* list = (tb_single_list_impl_t*)iterator;
    tb_assert_and_check_return_val(list, 0);

    // tail
    return 0;
}
static tb_size_t tb_single_list_iterator_next(tb_iterator_t* iterator, tb_size_t itor)
{
    // check
    tb_single_list_impl_t* list = (tb_single_list_impl_t*)iterator;
    tb_assert_and_check_return_val(list && itor, 0);

    // next
    return ((tb_single_list_item_t const*)itor)->next;
}
static tb_size_t tb_single_list_iterator_prev(tb_iterator_t* iterator, tb_size_t itor)
{
    // check
    tb_single_list_impl_t* list = (tb_single_list_impl_t*)iterator;
    tb_assert_and_check_return_val(list, 0);

    // is tail?
    tb_size_t prev = 0;
    if (!itor) prev = list->last;
    // is head?
    else if (itor == list->head) prev = 0;
    // find it
    else
    {
        tb_size_t node = tb_single_list_iterator_head(iterator);
        tb_size_t tail = tb_single_list_iterator_tail(iterator);
        for (prev = node; node != tail && node != itor; prev = node, node = tb_single_list_iterator_next(iterator, node)) ;
    }

    return prev;
}
static tb_pointer_t tb_single_list_iterator_item(tb_iterator_t* iterator, tb_size_t itor)
{
    // check
    tb_single_list_impl_t* list = (tb_single_list_impl_t*)iterator;
    tb_assert_and_check_return_val(list && itor, tb_null);

    // data
    return list->func.data(&list->func, &((tb_single_list_item_t const*)itor)[1]);
}
static tb_void_t tb_single_list_iterator_copy(tb_iterator_t* iterator, tb_size_t itor, tb_cpointer_t item)
{
    // check
    tb_single_list_impl_t* list = (tb_single_list_impl_t*)iterator;
    tb_assert_and_check_return(list && itor);

    // copy
    list->func.copy(&list->func, (tb_pointer_t)&((tb_single_list_item_t const*)itor)[1], item);
}
static tb_long_t tb_single_list_iterator_comp(tb_iterator_t* iterator, tb_cpointer_t ltem, tb_cpointer_t rtem)
{
    // check
    tb_single_list_impl_t* list = (tb_single_list_impl_t*)iterator;
    tb_assert_and_check_return_val(list && list->func.comp, 0);

    // comp
    return list->func.comp(&list->func, ltem, rtem);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_size_t tb_single_list_attach_next(tb_single_list_impl_t* list, tb_size_t itor, tb_size_t node)
{
    // check
    tb_assert_and_check_return_val(list && list->pool && node, 0);

    // the prev node
    tb_size_t prev = itor;

    // init node, inode => 0
    tb_single_list_item_t* pnode = (tb_single_list_item_t*)node;
    pnode->next = 0;

    // non-empty?
    if (list->head)
    {
        // is head?
        if (!prev)
        {
            // node => head
            pnode->next = list->head;

            // update head
            list->head = node;
        }
        // is last?
        else if (prev == list->last)
        {
            // the prev data
            tb_single_list_item_t* pprev = (tb_single_list_item_t*)prev;
            tb_assert_and_check_return_val(pprev, 0);

            // last => node => null
            pprev->next = node;

            // update last
            list->last = node;
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
        list->head = node;

        // update last
        list->last = node;
    }

    // return the new node
    return node;
}
static tb_size_t tb_single_list_detach_next(tb_single_list_impl_t* list, tb_size_t itor)
{
    // check
    tb_assert_and_check_return_val(list && list->pool, 0);

    // non-empty?
    tb_check_return_val(list->head, 0);

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
        midd = list->head;

        // only one item?
        if (midd == list->last)
        {
            list->head = 0;
            list->last = 0;
        }
        // update head
        else list->head = next = tb_iterator_next((tb_iterator_t*)list, midd);
    }
    // remove body?
    else
    {
        // the midd node
        midd = tb_iterator_next((tb_iterator_t*)list, prev);

        // get the prev data
        tb_single_list_item_t* pprev = (tb_single_list_item_t*)prev;
        tb_assert_and_check_return_val(pprev, 0);

        // the next node
        next = tb_iterator_next((tb_iterator_t*)list, midd);

        // prev => next
        pprev->next = next;

        // update last if midd is last
        if (midd == list->last) list->last = next = prev;
    }

    // ok?
    return midd;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_single_list_t* tb_single_list_init(tb_size_t grow, tb_item_func_t func)
{
    // check
    tb_assert_and_check_return_val(grow, tb_null);
    tb_assert_and_check_return_val(func.size && func.data && func.dupl && func.repl, tb_null);

    // alloc list
    tb_single_list_impl_t* list = (tb_single_list_impl_t*)tb_malloc0(sizeof(tb_single_list_impl_t));
    tb_assert_and_check_return_val(list, tb_null);

    // init list
    list->head = 0;
    list->last = 0;
    list->func = func;

    // init iterator
    list->itor.mode = TB_ITERATOR_MODE_FORWARD | TB_ITERATOR_MODE_REVERSE;
    list->itor.priv = tb_null;
    list->itor.step = func.size;
    list->itor.size = tb_single_list_iterator_size;
    list->itor.head = tb_single_list_iterator_head;
    list->itor.tail = tb_single_list_iterator_tail;
    list->itor.prev = tb_single_list_iterator_prev;
    list->itor.next = tb_single_list_iterator_next;
    list->itor.item = tb_single_list_iterator_item;
    list->itor.copy = tb_single_list_iterator_copy;
    list->itor.comp = tb_single_list_iterator_comp;

    // init pool, step = next + data
    list->pool = tb_fixed_pool_init(grow, sizeof(tb_single_list_item_t) + func.size, 0);
    tb_assert_and_check_goto(list->pool, fail);

    // ok
    return list;
fail:
    if (list) tb_single_list_exit(list);
    return tb_null;
}

tb_void_t tb_single_list_exit(tb_single_list_t* handle)
{
    tb_single_list_impl_t* list = (tb_single_list_impl_t*)handle;
    if (list)
    {
        // clear data
        tb_single_list_clear(list);

        // free pool
        if (list->pool) tb_fixed_pool_exit(list->pool);

        // free it
        tb_free(list);
    }
}
tb_void_t tb_single_list_clear(tb_single_list_t* handle)
{
    tb_single_list_impl_t* list = (tb_single_list_impl_t*)handle;
    if (list) 
    {
        // free items
        if (list->func.free)
        {
            tb_size_t itor = list->head;
            while (itor)
            {
                // item
                tb_single_list_item_t* item = (tb_single_list_item_t*)itor;

                // free 
                list->func.free(&list->func, &item[1]);
        
                // next
                itor = item->next;
            }
        }

        // clear pool
        if (list->pool) tb_fixed_pool_clear(list->pool);

        // reset it
        list->head = 0;
        list->last = 0;
    }
}
tb_pointer_t tb_single_list_head(tb_single_list_t const* handle)
{
    return tb_iterator_item((tb_iterator_t*)handle, tb_iterator_head((tb_iterator_t*)handle));
}
tb_pointer_t tb_single_list_last(tb_single_list_t const* handle)
{
    return tb_iterator_item((tb_iterator_t*)handle, tb_iterator_last((tb_iterator_t*)handle));
}
tb_size_t tb_single_list_size(tb_single_list_t const* handle)
{
    // check
    tb_single_list_impl_t const* list = (tb_single_list_impl_t const*)handle;
    tb_assert_and_check_return_val(list && list->pool, 0);

    // the size
    return tb_fixed_pool_size(list->pool);
}
tb_size_t tb_single_list_maxn(tb_single_list_t const* handle)
{
    // check
    tb_single_list_impl_t const* list = (tb_single_list_impl_t const*)handle;
    tb_assert_and_check_return_val(list, 0);

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
tb_size_t tb_single_list_insert_prev(tb_single_list_t* handle, tb_size_t itor, tb_cpointer_t data)
{
    return tb_single_list_insert_next(handle, tb_iterator_prev(handle, itor), data);
}
tb_size_t tb_single_list_insert_next(tb_single_list_t* handle, tb_size_t itor, tb_cpointer_t data)
{
    // check
    tb_single_list_impl_t* list = (tb_single_list_impl_t*)handle;
    tb_assert_and_check_return_val(list && list->pool, 0);

    // make the node data
    tb_single_list_item_t* pnode = tb_fixed_pool_malloc(list->pool);
    tb_assert_and_check_return_val(pnode, 0);

    // init node, inode => 0
    pnode->next = 0;

    // init data
    list->func.dupl(&list->func, &pnode[1], data);

    // attach next
    return tb_single_list_attach_next(list, itor, (tb_size_t)pnode);
}
tb_size_t tb_single_list_insert_head(tb_single_list_t* handle, tb_cpointer_t data)
{
    return tb_single_list_insert_prev(handle, tb_iterator_head(handle), data);
}
tb_size_t tb_single_list_insert_tail(tb_single_list_t* handle, tb_cpointer_t data)
{
    return tb_single_list_insert_prev(handle, tb_iterator_tail(handle), data);
}
tb_size_t tb_single_list_ninsert_prev(tb_single_list_t* handle, tb_size_t itor, tb_cpointer_t data, tb_size_t size)
{
    return tb_single_list_ninsert_next(handle, tb_iterator_prev(handle, itor), data, size);
}
tb_size_t tb_single_list_ninsert_next(tb_single_list_t* handle, tb_size_t itor, tb_cpointer_t data, tb_size_t size)
{
    // check
    tb_single_list_impl_t* list = (tb_single_list_impl_t*)handle;
    tb_assert_and_check_return_val(list && size, 0);

    // insert items
    tb_size_t node = itor;
    while (size--) node = tb_single_list_insert_next(list, itor, data);

    // return the first itor
    return node;
}
tb_size_t tb_single_list_ninsert_head(tb_single_list_t* handle, tb_cpointer_t data, tb_size_t size)
{
    return tb_single_list_ninsert_prev(handle, tb_iterator_head(handle), data, size);
}
tb_size_t tb_single_list_ninsert_tail(tb_single_list_t* handle, tb_cpointer_t data, tb_size_t size)
{
    return tb_single_list_ninsert_prev(handle, tb_iterator_tail(handle), data, size);
}
tb_size_t tb_single_list_replace(tb_single_list_t* handle, tb_size_t itor, tb_cpointer_t data)
{
    // check
    tb_single_list_impl_t* list = (tb_single_list_impl_t*)handle;
    tb_assert_and_check_return_val(list && itor, itor);

    // the item
    tb_single_list_item_t* item = (tb_single_list_item_t*)itor;

    // replace data
    list->func.repl(&list->func, &item[1], data);

    // ok
    return itor;
}
tb_size_t tb_single_list_replace_head(tb_single_list_t* handle, tb_cpointer_t data)
{
    return tb_single_list_replace(handle, tb_iterator_head(handle), data);
}
tb_size_t tb_single_list_replace_last(tb_single_list_t* handle, tb_cpointer_t data)
{
    return tb_single_list_replace(handle, tb_iterator_last(handle), data);
}
tb_size_t tb_single_list_nreplace(tb_single_list_t* handle, tb_size_t itor, tb_cpointer_t data, tb_size_t size)
{
    // check
    tb_single_list_impl_t* list = (tb_single_list_impl_t*)handle;
    tb_assert_and_check_return_val(list && size, itor);

    tb_size_t head = itor;
    tb_size_t tail = tb_iterator_tail((tb_iterator_t*)list);
    for (; size-- && itor != tail; itor = tb_iterator_next((tb_iterator_t*)list, itor)) 
        tb_single_list_replace(list, itor, data);
    return head;
}
tb_size_t tb_single_list_nreplace_head(tb_single_list_t* handle, tb_cpointer_t data, tb_size_t size)
{
    return tb_single_list_nreplace(handle, tb_iterator_head(handle), data, size);
}
tb_size_t tb_single_list_nreplace_last(tb_single_list_t* handle, tb_cpointer_t data, tb_size_t size)
{
    // compute offset
    tb_size_t n = tb_single_list_size(handle);
    tb_size_t o = n > size? n - size : 0;
    
    // seek
    tb_size_t itor = tb_iterator_head(handle);
    tb_size_t tail = tb_iterator_tail(handle);
    for (; o-- && itor != tail; itor = tb_iterator_next(handle, itor)) ;

    // replace
    return tb_single_list_nreplace(handle, itor, data, size);
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
tb_size_t tb_single_list_remove(tb_single_list_t* handle, tb_size_t itor)
{   
    return tb_single_list_remove_next(handle, tb_iterator_prev(handle, itor));
}
tb_size_t tb_single_list_remove_next(tb_single_list_t* handle, tb_size_t itor)
{
    // check
    tb_single_list_impl_t* list = (tb_single_list_impl_t*)handle;
    tb_assert_and_check_return_val(list && list->pool, itor);

    // detach next
    tb_size_t node = tb_single_list_detach_next(list, itor);
    tb_assert_and_check_return_val(node, itor);

    // next item
    tb_size_t next = tb_iterator_next((tb_iterator_t*)list, node);

    // free item
    if (list->func.free)
        list->func.free(&list->func, &((tb_single_list_item_t*)node)[1]);

    // free node
    tb_fixed_pool_free(list->pool, (tb_pointer_t)node);

    // return next node
    return next;
}
tb_size_t tb_single_list_remove_head(tb_single_list_t* handle)
{
    return tb_single_list_remove(handle, tb_iterator_head(handle));
}
tb_size_t tb_single_list_remove_last(tb_single_list_t* handle)
{
    return tb_single_list_remove(handle, tb_iterator_last(handle));
}
tb_size_t tb_single_list_nremove(tb_single_list_t* handle, tb_size_t itor, tb_size_t size)
{
    return tb_single_list_nremove_next(handle, tb_iterator_prev(handle, itor), size);
}
tb_size_t tb_single_list_nremove_next(tb_single_list_t* handle, tb_size_t itor, tb_size_t size)
{
    // check
    tb_single_list_impl_t* list = (tb_single_list_impl_t*)handle;
    tb_assert_and_check_return_val(list && size, itor);

    tb_size_t next = itor;
    while (size--) next = tb_single_list_remove_next(list, itor);
    return next;
}
tb_size_t tb_single_list_nremove_head(tb_single_list_t* handle, tb_size_t size)
{
    tb_single_list_nremove(handle, tb_iterator_head(handle), size);
    return tb_iterator_head(handle);
}
tb_size_t tb_single_list_nremove_last(tb_single_list_t* handle, tb_size_t size)
{
    // compute offset
    tb_size_t n = tb_single_list_size(handle);
    tb_size_t o = n > size? n - size : 0;
    
    // seek
    tb_size_t prev = 0;
    tb_size_t itor = tb_iterator_head(handle);
    tb_size_t tail = tb_iterator_tail(handle);
    for (; o-- && itor != tail; prev = itor, itor = tb_iterator_next(handle, itor)) ;

    // remove
    tb_single_list_nremove_next(handle, prev, size);

    // ok?
    return tb_iterator_last(handle);
}
tb_size_t tb_single_list_moveto_prev(tb_single_list_t* handle, tb_size_t itor, tb_size_t move)
{
    return tb_single_list_moveto_next(handle, tb_iterator_prev(handle, itor), move);
}
tb_size_t tb_single_list_moveto_next(tb_single_list_t* handle, tb_size_t itor, tb_size_t move)
{
    // check
    tb_single_list_impl_t* list = (tb_single_list_impl_t*)handle;
    tb_assert_and_check_return_val(list && list->pool && move, move);

    // detach move
    tb_size_t node = tb_single_list_detach_next(list, tb_iterator_prev((tb_iterator_t*)list, move));
    tb_assert_and_check_return_val(node && node == move, move);

    // attach move to next
    return tb_single_list_attach_next(list, itor, node);
}
tb_size_t tb_single_list_moveto_head(tb_single_list_t* handle, tb_size_t move)
{
    return tb_single_list_moveto_prev(handle, tb_iterator_head(handle), move);
}
tb_size_t tb_single_list_moveto_tail(tb_single_list_t* handle, tb_size_t move)
{
    return tb_single_list_moveto_prev(handle, tb_iterator_tail(handle), move);
}
tb_void_t tb_single_list_walk(tb_single_list_t* handle, tb_bool_t (*func)(tb_single_list_t* list, tb_pointer_t item, tb_bool_t* bdel, tb_cpointer_t priv), tb_cpointer_t priv)
{
    // check
    tb_single_list_impl_t* list = (tb_single_list_impl_t*)handle;
    tb_assert_and_check_return(list && list->pool && func);

    // pool
    tb_handle_t pool = list->pool;

    // step
    tb_size_t   step = list->func.size;
    tb_assert_and_check_return(step);

    // check
    tb_assert_and_check_return((tb_fixed_pool_size(pool) && list->head) || !tb_fixed_pool_size(pool));

    // walk
    tb_size_t   base = -1;
    tb_size_t   prev = 0;
    tb_bool_t   bdel = tb_false;
    tb_size_t   itor = list->head;
    tb_bool_t   stop = tb_false;
    tb_assert_abort(itor);
    while (itor)
    {
        // node
        tb_single_list_item_t* node = (tb_single_list_item_t*)itor;

        // item
        tb_pointer_t item = list->func.data(&list->func, &node[1]);

        // next
        tb_size_t next = node->next;
    
        // bdel
        bdel = tb_false;

        // callback: item
        if (!func(list, item, &bdel, priv)) stop = tb_true;

        // free it?
        if (bdel)
        {
            // save
            if (base == -1) base = prev;

            // free item
            if (list->func.free)
                list->func.free(&list->func, &((tb_single_list_item_t*)itor)[1]);

            // free item
            tb_fixed_pool_free(pool, (tb_pointer_t)itor);
        }
        
        // trace
        tb_trace_d("prev: %p, itor: %p, next: %p, base: %p, head: %p, last: %p, bdel: %u", prev, itor, next, base, list->head, list->last, bdel);

        // remove items?
        if (!bdel || !next || stop)
        {
            // have removed items?
            if (base != -1)
            {
                // the body
                tb_size_t body = bdel? next : itor;

                // remove body
                if (base)
                {
                    // base => body
                    ((tb_single_list_item_t*)base)->next = body;

                    // update last
                    if (!body) list->last = base;
                }
                // remove head
                else
                {
                    // head => body
                    list->head = body;

                    // update last
                    if (!body) list->last = 0;
                }
            }

            // reset
            base = -1;

            // stop?
            tb_check_goto(!stop, end);
        }
    
        // next
        prev = itor;
        itor = next;
    }

end:
    return ;
}
