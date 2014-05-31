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

// the list item type
typedef struct __tb_list_item_t
{
    // the item next
    tb_size_t           next;

    // the item prev
    tb_size_t           prev;

}tb_list_item_t;

/*!the double list type
 *
 *
 * <pre>
 * list:  0 => |-----| => |-------------------------------------------------=> |------| => |------| => 0
 *       tail   head                                                                         last     tail
 *        |                                                                                            |
 *        ---------------------------------------------------------------------------------------------
 *
 * head: => the first item
 * last: => the last item
 * tail: => behind the last item, no item
 *
 * performance: 
 *
 * insert:
 * insert midd: fast
 * insert head: fast
 * insert tail: fast
 * insert next: fast
 * 
 * ninsert:
 * ninsert midd: fast
 * ninsert head: fast
 * ninsert tail: fast
 * ninsert next: fast
 *
 * remove:
 * remove midd: fast
 * remove head: fast
 * remove last: fast
 * remove next: fast
 *
 * nremove:
 * nremove midd: fast
 * nremove head: fast
 * nremove last: fast
 * nremove next: fast
 *
 * iterator:
 * next: fast
 * prev: fast
 * </pre>
 *
 */
typedef struct __tb_list_impl_t
{
    /// the itor
    tb_iterator_t           itor;

    // the pool
    tb_handle_t             pool;

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
static tb_size_t tb_list_iterator_size(tb_iterator_t* iterator)
{
    // check
    tb_list_impl_t* list = (tb_list_impl_t*)iterator;
    tb_assert_and_check_return_val(list && list->pool, 0);

    // the size
    return tb_fixed_pool_size(list->pool);
}
static tb_size_t tb_list_iterator_head(tb_iterator_t* iterator)
{
    // check
    tb_list_impl_t* list = (tb_list_impl_t*)iterator;
    tb_assert_and_check_return_val(list, 0);

    // head
    return list->head;
}
static tb_size_t tb_list_iterator_tail(tb_iterator_t* iterator)
{
    // check
    tb_list_impl_t* list = (tb_list_impl_t*)iterator;
    tb_assert_and_check_return_val(list, 0);

    // tail
    return 0;
}
static tb_size_t tb_list_iterator_next(tb_iterator_t* iterator, tb_size_t itor)
{
    // check
    tb_list_impl_t* list = (tb_list_impl_t*)iterator;
    tb_assert_and_check_return_val(list, 0);

    // next
    if (!itor) return list->head;
    else return ((tb_list_item_t const*)itor)->next;
}
static tb_size_t tb_list_iterator_prev(tb_iterator_t* iterator, tb_size_t itor)
{
    // check
    tb_list_impl_t* list = (tb_list_impl_t*)iterator;
    tb_assert_and_check_return_val(list, 0);

    // prev
    if (!itor) return list->last;
    else return ((tb_list_item_t const*)itor)->prev;
}
static tb_pointer_t tb_list_iterator_item(tb_iterator_t* iterator, tb_size_t itor)
{
    // check
    tb_list_impl_t* list = (tb_list_impl_t*)iterator;
    tb_assert_and_check_return_val(list && itor, tb_null);

    // data
    return list->func.data(&list->func, &((tb_list_item_t const*)itor)[1]);
}
static tb_void_t tb_list_iterator_copy(tb_iterator_t* iterator, tb_size_t itor, tb_cpointer_t item)
{
    // check
    tb_list_impl_t* list = (tb_list_impl_t*)iterator;
    tb_assert_and_check_return(list && itor);

    // copy
    list->func.copy(&list->func, (tb_pointer_t)&((tb_list_item_t const*)itor)[1], item);
}
static tb_long_t tb_list_iterator_comp(tb_iterator_t* iterator, tb_cpointer_t ltem, tb_cpointer_t rtem)
{
    // check
    tb_list_impl_t* list = (tb_list_impl_t*)iterator;
    tb_assert_and_check_return_val(list && list->func.comp, 0);

    // comp
    return list->func.comp(&list->func, ltem, rtem);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_size_t tb_list_attach_prev(tb_list_impl_t* list, tb_size_t itor, tb_size_t node)
{
    // check
    tb_list_item_t* pnode = (tb_list_item_t*)node;
    tb_assert_and_check_return_val(list && list->pool && pnode, 0);

    // init node
    pnode->prev = 0;
    pnode->next = 0;

    // is null?
    if (!list->head && !list->last)
    {
        /* list: 0 => node => 0
         *       tail  head   tail
         *             last
         */
        list->head = node;
        list->last = node;
    }
    else
    {
        // check
        tb_assert_and_check_return_val(list->head && list->last, 0);

        // insert to tail
        if (!itor)
        {
            // the last node
            tb_size_t last = list->last;
        
            // the last data
            tb_list_item_t* plast = (tb_list_item_t*)last;
            tb_assert_and_check_return_val(plast, 0);

            // last <=> node <=> 0
            plast->next = node;
            pnode->prev = last;

            // update the last node
            list->last = node;
        }
        // insert to head
        else if (itor == list->head)
        {
            // the head node
            tb_size_t head = list->head;
        
            // the head data
            tb_list_item_t* phead = (tb_list_item_t*)head;
            tb_assert_and_check_return_val(phead, 0);

            // 0 <=> node <=> head
            phead->prev = node;
            pnode->next = head;

            // update the head node
            list->head = node;
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
static tb_size_t tb_list_detach_item(tb_list_impl_t* list, tb_size_t itor)
{
    // check
    tb_assert_and_check_return_val(list && list->pool && itor, 0);

    // not empty?
    tb_check_return_val(list->head && list->last, 0);

    // only one?
    if (list->head == list->last)
    {
        tb_assert_and_check_return_val(list->head == itor, 0);
        list->head = 0;
        list->last = 0;
    }
    else
    {
        // remove head?
        if (itor == list->head)
        {
            // the next node
            tb_size_t next = tb_iterator_next((tb_iterator_t*)list, itor);

            // the next data
            tb_list_item_t* pnext = (tb_list_item_t*)next;
            tb_assert_and_check_return_val(pnext, 0);

            /* 0 <=> node <=> next <=> ... <=> 0
             * 0 <=> next <=> ... <=> 0
             */
            list->head = next;
            pnext->prev = 0;
        }
        // remove last?
        else if (itor == list->last)
        {
            // the prev node
            tb_size_t prev = tb_iterator_prev((tb_iterator_t*)list, itor);

            // the prev data
            tb_list_item_t* pprev = (tb_list_item_t*)prev;
            tb_assert_and_check_return_val(pprev, 0);

            /* 0 <=> ... <=> prev <=> node <=> 0
             * 0 <=> ... <=> prev <=> 0
             */
            pprev->next = 0;
            list->last = prev;
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

tb_list_t* tb_list_init(tb_size_t grow, tb_item_func_t func)
{
    // check
    tb_assert_and_check_return_val(grow, tb_null);
    tb_assert_and_check_return_val(func.size && func.data && func.dupl && func.repl, tb_null);

    // alloc list
    tb_list_impl_t* list = (tb_list_impl_t*)tb_malloc0(sizeof(tb_list_impl_t));
    tb_assert_and_check_return_val(list, tb_null);

    // init list
    list->head = 0;
    list->last = 0;
    list->func = func;

    // init iterator
    list->itor.mode = TB_ITERATOR_MODE_FORWARD | TB_ITERATOR_MODE_REVERSE;
    list->itor.priv = tb_null;
    list->itor.step = func.size;
    list->itor.size = tb_list_iterator_size;
    list->itor.head = tb_list_iterator_head;
    list->itor.tail = tb_list_iterator_tail;
    list->itor.prev = tb_list_iterator_prev;
    list->itor.next = tb_list_iterator_next;
    list->itor.item = tb_list_iterator_item;
    list->itor.copy = tb_list_iterator_copy;
    list->itor.comp = tb_list_iterator_comp;

    // init pool, step = next + prev + data
    list->pool = tb_fixed_pool_init(grow, sizeof(tb_list_item_t) + func.size, 0);
    tb_assert_and_check_goto(list->pool, fail);

    // ok
    return list;
fail:
    if (list) tb_list_exit(list);
    return tb_null;
}

tb_void_t tb_list_exit(tb_list_t* handle)
{
    tb_list_impl_t* list = (tb_list_impl_t*)handle;
    if (list)
    {
        // clear data
        tb_list_clear(list);

        // free pool
        if (list->pool) tb_fixed_pool_exit(list->pool);

        // free it
        tb_free(list);
    }
}
tb_void_t tb_list_clear(tb_list_t* handle)
{
    tb_list_impl_t* list = (tb_list_impl_t*)handle;
    if (list) 
    {
        // free items
        if (list->func.free)
        {
            tb_size_t itor = list->head;
            while (itor)
            {
                // item
                tb_list_item_t* item = (tb_list_item_t*)itor;

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
tb_pointer_t tb_list_head(tb_list_t const* handle)
{
    return tb_iterator_item((tb_iterator_t*)handle, tb_iterator_head((tb_iterator_t*)handle));
}
tb_pointer_t tb_list_last(tb_list_t const* handle)
{
    return tb_iterator_item((tb_iterator_t*)handle, tb_iterator_last((tb_iterator_t*)handle));
}
tb_size_t tb_list_size(tb_list_t const* handle)
{
    // check
    tb_list_impl_t* list = (tb_list_impl_t*)handle;
    tb_assert_and_check_return_val(list && list->pool, 0);

    // the size
    return tb_fixed_pool_size(list->pool);
}
tb_size_t tb_list_maxn(tb_list_t const* handle)
{
    // check
    tb_assert_and_check_return_val(handle, 0);
    return TB_MAXU32;
}
tb_size_t tb_list_insert_prev(tb_list_t* handle, tb_size_t itor, tb_cpointer_t data)
{
    // check
    tb_list_impl_t* list = (tb_list_impl_t*)handle;
    tb_assert_and_check_return_val(list && list->pool, 0);

    // make the node data
    tb_list_item_t* pnode = tb_fixed_pool_malloc(list->pool);
    tb_assert_and_check_return_val(pnode, 0);

    // init node
    pnode->prev = 0;
    pnode->next = 0;
    list->func.dupl(&list->func, &pnode[1], data);

    // attach node
    return tb_list_attach_prev(list, itor, (tb_size_t)pnode);
}
tb_size_t tb_list_insert_next(tb_list_t* handle, tb_size_t itor, tb_cpointer_t data)
{
    return tb_list_insert_prev(handle, tb_iterator_next(handle, itor), data);
}
tb_size_t tb_list_insert_head(tb_list_t* handle, tb_cpointer_t data)
{
    return tb_list_insert_prev(handle, tb_iterator_head(handle), data);
}
tb_size_t tb_list_insert_tail(tb_list_t* handle, tb_cpointer_t data)
{
    return tb_list_insert_prev(handle, tb_iterator_tail(handle), data);
}
tb_size_t tb_list_ninsert_prev(tb_list_t* handle, tb_size_t itor, tb_cpointer_t data, tb_size_t size)
{
    // check
    tb_list_impl_t* list = (tb_list_impl_t*)handle;
    tb_assert_and_check_return_val(list && size, 0);

    // insert items
    tb_size_t node = itor;
    while (size--) node = tb_list_insert_prev(list, node, data);

    // return the first itor
    return node;
}
tb_size_t tb_list_ninsert_next(tb_list_t* handle, tb_size_t itor, tb_cpointer_t data, tb_size_t size)
{
    return tb_list_ninsert_prev(handle, tb_iterator_next(handle, itor), data, size);
}
tb_size_t tb_list_ninsert_head(tb_list_t* handle, tb_cpointer_t data, tb_size_t size)
{
    return tb_list_ninsert_prev(handle, tb_iterator_head(handle), data, size);
}
tb_size_t tb_list_ninsert_tail(tb_list_t* handle, tb_cpointer_t data, tb_size_t size)
{
    return tb_list_ninsert_prev(handle, tb_iterator_tail(handle), data, size);
}
tb_size_t tb_list_replace(tb_list_t* handle, tb_size_t itor, tb_cpointer_t data)
{
    // check
    tb_list_impl_t* list = (tb_list_impl_t*)handle;
    tb_assert_and_check_return_val(list && itor, itor);

    // the item
    tb_list_item_t* item = (tb_list_item_t*)itor;

    // replace data
    list->func.repl(&list->func, &item[1], data);

    // ok
    return itor;
}
tb_size_t tb_list_replace_head(tb_list_t* handle, tb_cpointer_t data)
{
    return tb_list_replace(handle, tb_iterator_head(handle), data);
}
tb_size_t tb_list_replace_last(tb_list_t* handle, tb_cpointer_t data)
{
    return tb_list_replace(handle, tb_iterator_last(handle), data);
}
tb_size_t tb_list_nreplace(tb_list_t* handle, tb_size_t itor, tb_cpointer_t data, tb_size_t size)
{
    // check
    tb_list_impl_t* list = (tb_list_impl_t*)handle;
    tb_assert_and_check_return_val(list && data && size, itor);

    tb_size_t head = itor;
    tb_size_t tail = tb_iterator_tail((tb_iterator_t*)list);
    for (; size-- && itor != tail; itor = tb_iterator_next((tb_iterator_t*)list, itor)) 
        tb_list_replace(list, itor, data);
    return head;
}
tb_size_t tb_list_nreplace_head(tb_list_t* handle, tb_cpointer_t data, tb_size_t size)
{
    return tb_list_nreplace(handle, tb_iterator_head(handle), data, size);
}
tb_size_t tb_list_nreplace_last(tb_list_t* handle, tb_cpointer_t data, tb_size_t size)
{
    tb_size_t node = 0;
    tb_size_t itor = tb_iterator_last(handle);
    tb_size_t tail = tb_iterator_tail(handle);
    for (; size-- && itor != tail; itor = tb_iterator_prev(handle, itor)) 
        node = tb_list_replace(handle, itor, data);

    return node;
}
tb_size_t tb_list_remove(tb_list_t* handle, tb_size_t itor)
{
    // check
    tb_list_impl_t* list = (tb_list_impl_t*)handle;
    tb_assert_and_check_return_val(list && list->pool && itor, itor);

    // detach item
    tb_size_t node = tb_list_detach_item(list, itor);
    tb_assert_and_check_return_val(node && node == itor, itor);

    // next item
    tb_size_t next = tb_iterator_next((tb_iterator_t*)list, node);

    // free item
    if (list->func.free)
        list->func.free(&list->func, &((tb_list_item_t*)node)[1]);

    // free node
    tb_fixed_pool_free(list->pool, (tb_pointer_t)node);

    // return next node
    return next;
}
tb_size_t tb_list_remove_next(tb_list_t* handle, tb_size_t itor)
{
    return tb_list_remove(handle, tb_iterator_next(handle, itor));
}
tb_size_t tb_list_remove_head(tb_list_t* handle)
{
    return tb_list_remove(handle, tb_iterator_head(handle));
}
tb_size_t tb_list_remove_last(tb_list_t* handle)
{
    return tb_list_remove(handle, tb_iterator_last(handle));
}
tb_size_t tb_list_nremove(tb_list_t* handle, tb_size_t itor, tb_size_t size)
{
    // check
    tb_list_impl_t* list = (tb_list_impl_t*)handle;
    tb_assert_and_check_return_val(list && size, itor);

    tb_size_t next = itor;
    while (size--) next = tb_list_remove(list, next);
    return next;
}
tb_size_t tb_list_nremove_head(tb_list_t* handle, tb_size_t size)
{
    while (size-- && tb_list_size(handle)) tb_list_remove_head(handle);
    return tb_iterator_head(handle);
}
tb_size_t tb_list_nremove_last(tb_list_t* handle, tb_size_t size)
{
    while (size-- && tb_list_size(handle)) tb_list_remove_last(handle);
    return tb_iterator_last(handle);
}
tb_size_t tb_list_moveto_prev(tb_list_t* handle, tb_size_t itor, tb_size_t move)
{
    // check
    tb_list_impl_t* list = (tb_list_impl_t*)handle;
    tb_assert_and_check_return_val(list && list->pool && move, move);

    // detach move
    tb_size_t node = tb_list_detach_item(list, move);
    tb_assert_and_check_return_val(node && node == move, move);

    // attach move to prev
    return tb_list_attach_prev(list, itor, node);
}
tb_size_t tb_list_moveto_next(tb_list_t* handle, tb_size_t itor, tb_size_t move)
{
    return tb_list_moveto_prev(handle, tb_iterator_next(handle, itor), move);
}
tb_size_t tb_list_moveto_head(tb_list_t* handle, tb_size_t move)
{
    return tb_list_moveto_prev(handle, tb_iterator_head(handle), move);
}
tb_size_t tb_list_moveto_tail(tb_list_t* handle, tb_size_t move)
{
    return tb_list_moveto_prev(handle, tb_iterator_tail(handle), move);
}
tb_void_t tb_list_walk(tb_list_t* handle, tb_bool_t (*func)(tb_list_t* list, tb_pointer_t item, tb_bool_t* bdel, tb_cpointer_t priv), tb_cpointer_t priv)
{
    // check
    tb_list_impl_t* list = (tb_list_impl_t*)handle;
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
    while (itor)
    {
        // node
        tb_list_item_t* node = (tb_list_item_t*)itor;

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
                list->func.free(&list->func, &((tb_list_item_t*)itor)[1]);

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
                    // the base data
                    tb_list_item_t* pbase = (tb_list_item_t*)base;

                    // base => body
                    pbase->next = body;

                    // prev => base
                    if (body) ((tb_list_item_t*)body)->prev = base;
                    // update last
                    else list->last = base;
                }
                // remove head
                else
                {
                    // head => body
                    list->head = body;

                    // prev => 0
                    if (body) ((tb_list_item_t*)body)->prev = 0;
                    // update last
                    else list->last = 0;
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
