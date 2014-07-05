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
#include "../platform/platform.h"

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
        impl->itor.mode = TB_ITERATOR_MODE_FORWARD | TB_ITERATOR_MODE_REVERSE;
        impl->itor.priv = tb_null;
        impl->itor.step = func.size;
        impl->itor.size = tb_list_itor_size;
        impl->itor.head = tb_list_itor_head;
        impl->itor.tail = tb_list_itor_tail;
        impl->itor.prev = tb_list_itor_prev;
        impl->itor.next = tb_list_itor_next;
        impl->itor.item = tb_list_itor_item;
        impl->itor.copy = tb_list_itor_copy;
        impl->itor.comp = tb_list_itor_comp;

        // init pool, step = next + prev + data
        impl->pool = tb_fixed_pool_init(grow, sizeof(tb_list_item_t) + func.size, 0);
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
tb_void_t tb_list_walk(tb_list_ref_t list, tb_bool_t (*func)(tb_list_ref_t impl, tb_pointer_t item, tb_bool_t* bdel, tb_cpointer_t priv), tb_cpointer_t priv)
{
    // check
    tb_list_impl_t* impl = (tb_list_impl_t*)list;
    tb_assert_and_check_return(impl && func);

    // pool
    tb_fixed_pool_ref_t pool = impl->pool;
    tb_assert_and_check_return(pool);

    // step
    tb_size_t step = impl->func.size;
    tb_assert_and_check_return(step);

    // check
    tb_assert_and_check_return((tb_fixed_pool_size(pool) && impl->head) || !tb_fixed_pool_size(pool));

    // walk
    tb_size_t   base = -1;
    tb_size_t   prev = 0;
    tb_bool_t   bdel = tb_false;
    tb_size_t   itor = impl->head;
    tb_bool_t   stop = tb_false;
    while (itor)
    {
        // node
        tb_list_item_t* node = (tb_list_item_t*)itor;

        // item
        tb_pointer_t item = impl->func.data(&impl->func, &node[1]);

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
            if (impl->func.free)
                impl->func.free(&impl->func, &((tb_list_item_t*)itor)[1]);

            // free item
            tb_fixed_pool_free(pool, (tb_pointer_t)itor);
        }
            
        // trace
        tb_trace_d("prev: %p, itor: %p, next: %p, base: %p, head: %p, last: %p, bdel: %u", prev, itor, next, base, impl->head, impl->last, bdel);

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
                    // the base data
                    tb_list_item_t* pbase = (tb_list_item_t*)base;

                    // base => body
                    pbase->next = body;

                    // prev => base
                    if (body) ((tb_list_item_t*)body)->prev = base;
                    // update last
                    else impl->last = base;
                }
                // remove head
                else
                {
                    // head => body
                    impl->head = body;

                    // prev => 0
                    if (body) ((tb_list_item_t*)body)->prev = 0;
                    // update last
                    else impl->last = 0;
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
