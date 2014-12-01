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
 * @file        heap.c
 * @ingroup     container
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "heap"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "heap.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../memory/memory.h"
#include "../stream/stream.h"
#include "../platform/platform.h"
#include "../algorithm/algorithm.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the heap maxn
#ifdef __tb_small__
#   define TB_HEAD_MAXN             (1 << 16)
#else
#   define TB_HEAD_MAXN             (1 << 30)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the heap impl type
typedef struct __tb_heap_impl_t
{
    // the itor
    tb_iterator_t           itor;

    // the data
    tb_byte_t*              data;

    // the size
    tb_size_t               size;

    // the maxn
    tb_size_t               maxn;

    // the grow
    tb_size_t               grow;

    // the func
    tb_item_func_t          func;

}tb_heap_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * checker
 */
#if 0//def __tb_debug__
static tb_void_t tb_heap_check(tb_heap_impl_t* impl)
{
    // init
    tb_byte_t*  data = impl->data;
    tb_size_t   tail = impl->size;
    tb_size_t   step = impl->func.size;
    tb_size_t   parent = 0;

    // walk
    for (; parent < tail; parent++)
    {   
        // the left child node
        tb_size_t   lchild  = (parent << 1) + 1;
        tb_check_break(lchild < tail);

        // the parent data
        tb_pointer_t parent_data = impl->func.data(&impl->func, data + parent * step);

        // check?
        if (impl->func.comp(&impl->func, impl->func.data(&impl->func, data + lchild * step), parent_data) < 0) 
        {
            tb_trace_d("lchild[%lu]: invalid, parent: %lu", lchild, parent);
            break;
        }

        // the right child node
        tb_size_t   rchild  = (parent << 1) + 2;
        tb_check_break(rchild < tail);

        // check?
        if (impl->func.comp(&impl->func, impl->func.data(&impl->func, data + rchild * step), parent_data) < 0) 
        {
            tb_trace_d("rchild[%lu]: invalid, parent: %lu", rchild, parent);
            break;
        }
    }
}
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * iterator
 */
static tb_size_t tb_heap_itor_size(tb_iterator_ref_t iterator)
{
    // check
    tb_heap_impl_t* impl = (tb_heap_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);

    // size
    return impl->size;
}
static tb_size_t tb_heap_itor_head(tb_iterator_ref_t iterator)
{
    // check
    tb_heap_impl_t* impl = (tb_heap_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);

    // head
    return 0;
}
static tb_size_t tb_heap_itor_last(tb_iterator_ref_t iterator)
{
    // check
    tb_heap_impl_t* impl = (tb_heap_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);

    // last
    return impl->size? impl->size - 1 : 0;
}
static tb_size_t tb_heap_itor_tail(tb_iterator_ref_t iterator)
{
    // check
    tb_heap_impl_t* impl = (tb_heap_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);

    // tail
    return impl->size;
}
static tb_size_t tb_heap_itor_next(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_heap_impl_t* impl = (tb_heap_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);
    tb_assert_and_check_return_val(itor < impl->size, impl->size);

    // next
    return itor + 1;
}
static tb_size_t tb_heap_itor_prev(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_heap_impl_t* impl = (tb_heap_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);
    tb_assert_and_check_return_val(itor && itor < impl->size, 0);

    // prev
    return itor - 1;
}
static tb_pointer_t tb_heap_itor_item(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_heap_impl_t* impl = (tb_heap_impl_t*)iterator;
    tb_assert_and_check_return_val(impl && itor < impl->size, tb_null);
    
    // data
    return impl->func.data(&impl->func, impl->data + itor * iterator->step);
}
static tb_void_t tb_heap_itor_copy(tb_iterator_ref_t iterator, tb_size_t itor, tb_cpointer_t item)
{
    // check
    tb_heap_impl_t* impl = (tb_heap_impl_t*)iterator;
    tb_assert_and_check_return(impl);

    // copy
    impl->func.copy(&impl->func, impl->data + itor * iterator->step, item);
}
static tb_long_t tb_heap_itor_comp(tb_iterator_ref_t iterator, tb_cpointer_t ltem, tb_cpointer_t rtem)
{
    // check
    tb_heap_impl_t* impl = (tb_heap_impl_t*)iterator;
    tb_assert_and_check_return_val(impl && impl->func.comp, 0);

    // comp
    return impl->func.comp(&impl->func, ltem, rtem);
}

/*! remove the impl item
 *
 * <pre>
 * init:
 *                                          1(head)
 *                               -------------------------
 *                              |                         |
 *                           (hole)                       2
 *                        --------------             -------------
 *                       |              |           |             |
 *                       6(smaler)      9           7             8
 *                   ---------       ----                                            (hole) <-
 *                  |         |     |                                                         |
 *                 10        16    8 (last)---------------------------------------------> 8 (val)
 *
 *
 * after:
 *                                          1(head)
 *                               -------------------------
 *                              |                         |
 *                              6                         2
 *                        --------------             -------------
 *                       |              |           |             |
 *                     (hole)           9           7             8
 *                   ---------                                                              <-
 *                  |         |                                                               |
 *                 10(smaller)16                                                          8 (val)
 *
 *
 * after:
 *                                          1(head)
 *                               -------------------------
 *                              |                         |
 *                              6                         2
 *                        --------------             -------------
 *                       |              |           |             |
 *                       8              9           7             8
 *                   ---------                                                              
 *                  |         |                                                               
 *                 10        16 
 * 
 * </pre>
 */
static tb_void_t tb_heap_itor_remove(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_heap_impl_t* impl = (tb_heap_impl_t*)iterator;
    tb_assert_and_check_return(impl && impl->data && impl->size && itor < impl->size);

    // init func
    tb_item_func_comp_t func_comp = impl->func.comp;
    tb_item_func_data_t func_data = impl->func.data;
    tb_assert_and_check_return(func_comp && func_data);

    // walk, 2 * hole + 1: the left child node of hole
    tb_size_t           step = impl->func.size;
    tb_byte_t*          head = impl->data;
    tb_byte_t*          hole = head + itor * step;
    tb_byte_t*          tail = head + impl->size * step;
    tb_byte_t*          last = head + (impl->size - 1) * step;
    tb_byte_t*          child = head + ((itor << 1) + 1) * step;
    tb_pointer_t        data_child = tb_null;
    tb_pointer_t        data_rchild = tb_null;
    tb_pointer_t        data_last = func_data(&impl->func, last);
    switch (step)
    {
#ifndef __tb_small__
    case sizeof(tb_uint64_t):
        {
            for (; child < tail; child = head + (((child - head) << 1) + step))
            {   
                // the smaller child node
                data_child = func_data(&impl->func, child);
                if (child + step < tail && func_comp(&impl->func, data_child, (data_rchild = func_data(&impl->func, child + step))) > 0) 
                {
                    child += step;
                    data_child = data_rchild;
                }

                // end?
                if (func_comp(&impl->func, data_child, data_last) > 0) break;

                // the smaller child node => hole
                *((tb_uint64_t*)hole) = *((tb_uint64_t*)child);

                // move the hole down to it's larger child node 
                hole = child;
            }
        }
        break;
    case sizeof(tb_uint32_t):
        {
            for (; child < tail; child = head + (((child - head) << 1) + step))
            {   
                // the smaller child node
                data_child = func_data(&impl->func, child);
                if (child + step < tail && func_comp(&impl->func, data_child, (data_rchild = func_data(&impl->func, child + step))) > 0) 
                {
                    child += step;
                    data_child = data_rchild;
                }

                // end?
                if (func_comp(&impl->func, data_child, data_last) > 0) break;

                // the smaller child node => hole
                *((tb_uint32_t*)hole) = *((tb_uint32_t*)child);

                // move the hole down to it's larger child node 
                hole = child;
            }
        }
        break;
    case sizeof(tb_uint16_t):
        {
            for (; child < tail; child = head + (((child - head) << 1) + step))
            {   
                // the smaller child node
                data_child = func_data(&impl->func, child);
                if (child + step < tail && func_comp(&impl->func, data_child, (data_rchild = func_data(&impl->func, child + step))) > 0) 
                {
                    child += step;
                    data_child = data_rchild;
                }

                // end?
                if (func_comp(&impl->func, data_child, data_last) > 0) break;

                // the smaller child node => hole
                *((tb_uint16_t*)hole) = *((tb_uint16_t*)child);

                // move the hole down to it's larger child node 
                hole = child;
            }
        }
        break;
    case sizeof(tb_uint8_t):
        {
            for (; child < tail; child = head + (((child - head) << 1) + step))
            {   
                // the smaller child node
                data_child = func_data(&impl->func, child);
                if (child + step < tail && func_comp(&impl->func, data_child, (data_rchild = func_data(&impl->func, child + step))) > 0) 
                {
                    child += step;
                    data_child = data_rchild;
                }

                // end?
                if (func_comp(&impl->func, data_child, data_last) > 0) break;

                // the smaller child node => hole
                *((tb_uint8_t*)hole) = *((tb_uint8_t*)child);

                // move the hole down to it's larger child node 
                hole = child;
            }

        }
        break;
#endif
    default:
        {
            for (; child < tail; child = head + (((child - head) << 1) + step))
            {   
                // the smaller child node
                data_child = func_data(&impl->func, child);
                if (child + step < tail && func_comp(&impl->func, data_child, (data_rchild = func_data(&impl->func, child + step))) > 0) 
                {
                    child += step;
                    data_child = data_rchild;
                }

                // end?
                if (func_comp(&impl->func, data_child, data_last) > 0) break;

                // the smaller child node => hole
                tb_memcpy(hole, child, step);

                // move the hole down to it's larger child node 
                hole = child;
            }
        }
        break;
    }

    // the last node => hole
    if (hole != last) tb_memcpy(hole, last, step);

    // size--
    impl->size--;

    // check
//  tb_heap_check(impl);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_heap_ref_t tb_heap_init(tb_size_t grow, tb_item_func_t func)
{
    // check
    tb_assert_and_check_return_val(grow, tb_null);
    tb_assert_and_check_return_val(func.size && func.data && func.dupl && func.repl, tb_null);

    // done
    tb_bool_t       ok = tb_false;
    tb_heap_impl_t* impl = tb_null;
    do
    {
        // make impl
        impl = tb_malloc0_type(tb_heap_impl_t);
        tb_assert_and_check_break(impl);

        // init impl
        impl->size = 0;
        impl->grow = grow;
        impl->maxn = grow;
        impl->func = func;
        tb_assert_and_check_break(impl->maxn < TB_HEAD_MAXN);

        // init iterator
        impl->itor.mode     = TB_ITERATOR_MODE_FORWARD | TB_ITERATOR_MODE_REVERSE | TB_ITERATOR_MODE_RACCESS | TB_ITERATOR_MODE_MUTABLE;
        impl->itor.priv     = tb_null;
        impl->itor.step     = func.size;
        impl->itor.size     = tb_heap_itor_size;
        impl->itor.head     = tb_heap_itor_head;
        impl->itor.last     = tb_heap_itor_last;
        impl->itor.tail     = tb_heap_itor_tail;
        impl->itor.prev     = tb_heap_itor_prev;
        impl->itor.next     = tb_heap_itor_next;
        impl->itor.item     = tb_heap_itor_item;
        impl->itor.copy     = tb_heap_itor_copy;
        impl->itor.comp     = tb_heap_itor_comp;
        impl->itor.remove   = tb_heap_itor_remove;

        // make data
        impl->data = (tb_byte_t*)tb_nalloc0(impl->maxn, func.size);
        tb_assert_and_check_break(impl->data);

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (impl) tb_heap_exit((tb_heap_ref_t)impl);
        impl = tb_null;
    }

    // ok?
    return (tb_heap_ref_t)impl;
}
tb_void_t tb_heap_exit(tb_heap_ref_t heap)
{
    // check
    tb_heap_impl_t* impl = (tb_heap_impl_t*)heap;
    tb_assert_and_check_return(impl);

    // clear data
    tb_heap_clear(heap);

    // free data
    if (impl->data) tb_free(impl->data);
    impl->data = tb_null;

    // free it
    tb_free(impl);
}
tb_void_t tb_heap_clear(tb_heap_ref_t heap)
{   
    // check
    tb_heap_impl_t* impl = (tb_heap_impl_t*)heap;
    tb_assert_and_check_return(impl);

    // free data
    if (impl->func.nfree)
        impl->func.nfree(&impl->func, impl->data, impl->size);

    // reset size 
    impl->size = 0;
}
tb_size_t tb_heap_size(tb_heap_ref_t heap)
{
    // check
    tb_heap_impl_t const* impl = (tb_heap_impl_t const*)heap;
    tb_assert_and_check_return_val(impl, 0);

    // size
    return impl->size;
}
tb_size_t tb_heap_maxn(tb_heap_ref_t heap)
{
    // check
    tb_heap_impl_t const* impl = (tb_heap_impl_t const*)heap;
    tb_assert_and_check_return_val(impl, 0);

    // maxn
    return impl->maxn;
}
tb_pointer_t tb_heap_top(tb_heap_ref_t heap)
{
    return tb_iterator_item(heap, tb_iterator_head(heap));
}
/*! put impl
 *
 * <pre>
 * init:
 * 
 *                                          1(head)
 *                               -------------------------
 *                              |                         |
 *                              4                         2
 *                        --------------             -------------
 *                       |              |           |             |
 *                       6(parent)      9           7             8
 *                   ---------       
 *                  |         |     
 *                  10(last) (hole) <= 5(val)
 * after:
 *
 *                                          1(head)
 *                               -------------------------
 *                              |                         |
 *                              4                         2
 *                        --------------             -------------
 *                       |              |           |             |
 *                       5(hole)      9           7             8
 *                   ---------       
 *                  |         |     
 *                  10(last)  6(last)
 * </pre>
 */
tb_void_t tb_heap_put(tb_heap_ref_t heap, tb_cpointer_t data)
{
    // check
    tb_heap_impl_t* impl = (tb_heap_impl_t*)heap;
    tb_assert_and_check_return(impl && impl->data);

    // full? grow it
    if (impl->size == impl->maxn)
    {
        // the maxn
        tb_size_t maxn = tb_align4(impl->maxn + impl->grow);
        tb_assert_and_check_return(maxn < TB_HEAD_MAXN);

        // realloc data
        impl->data = (tb_byte_t*)tb_ralloc(impl->data, maxn * impl->func.size);
        tb_assert_and_check_return(impl->data);

        // must be align by 4-bytes
        tb_assert_and_check_return(!(((tb_size_t)(impl->data)) & 3));

        // clear the grow data
        tb_memset(impl->data + impl->size * impl->func.size, 0, (maxn - impl->maxn) * impl->func.size);

        // save maxn
        impl->maxn = maxn;
    }

    // check
    tb_assert_and_check_return(impl->size < impl->maxn);
    
    // init func
    tb_item_func_comp_t func_comp = impl->func.comp;
    tb_item_func_data_t func_data = impl->func.data;
    tb_assert_and_check_return(func_comp && func_data);

    // walk, (hole - 1) / 2: the parent node of the hole
    tb_size_t           parent = 0;
    tb_byte_t*          head = impl->data;
    tb_size_t           hole = impl->size;
    tb_size_t           step = impl->func.size;
    switch (step)
    {
#ifndef __tb_small__
    case sizeof(tb_uint64_t):
        {
            for (parent = (hole - 1) >> 1; hole && (func_comp(&impl->func, func_data(&impl->func, head + parent * step), data) > 0); parent = (hole - 1) >> 1)
            {
                // move item: parent => hole
                *((tb_uint64_t*)(head + hole * step)) = *((tb_uint64_t*)(head + parent * step));

                // move node: hole => parent
                hole = parent;
            }
        }
        break;
    case sizeof(tb_uint32_t):
        {
            for (parent = (hole - 1) >> 1; hole && (func_comp(&impl->func, func_data(&impl->func, head + parent * step), data) > 0); parent = (hole - 1) >> 1)
            {
                // move item: parent => hole
                *((tb_uint32_t*)(head + hole * step)) = *((tb_uint32_t*)(head + parent * step));

                // move node: hole => parent
                hole = parent;
            }
        }
        break;
    case sizeof(tb_uint16_t):
        {
            for (parent = (hole - 1) >> 1; hole && (func_comp(&impl->func, func_data(&impl->func, head + parent * step), data) > 0); parent = (hole - 1) >> 1)
            {
                // move item: parent => hole
                *((tb_uint16_t*)(head + hole * step)) = *((tb_uint16_t*)(head + parent * step));

                // move node: hole => parent
                hole = parent;
            }
        }
        break;
    case sizeof(tb_uint8_t):
        {
            for (parent = (hole - 1) >> 1; hole && (func_comp(&impl->func, func_data(&impl->func, head + parent * step), data) > 0); parent = (hole - 1) >> 1)
            {
                // move item: parent => hole
                *((tb_uint8_t*)(head + hole * step)) = *((tb_uint8_t*)(head + parent * step));

                // move node: hole => parent
                hole = parent;
            }
        }
        break;
#endif
    default:
        for (parent = (hole - 1) >> 1; hole && (func_comp(&impl->func, func_data(&impl->func, head + parent * step), data) > 0); parent = (hole - 1) >> 1)
        {
            // move item: parent => hole
            tb_memcpy(head + hole * step, head + parent * step, step);

            // move node: hole => parent
            hole = parent;
        }
        break;
    }

    // save data
    impl->func.dupl(&impl->func, head + hole * step, data);

    // size++
    impl->size++;

    // check
//  tb_heap_check(impl);
}
tb_void_t tb_heap_pop(tb_heap_ref_t heap)
{
    tb_heap_itor_remove(heap, 0);
}
tb_void_t tb_heap_del(tb_heap_ref_t heap, tb_size_t itor)
{
    tb_heap_itor_remove(heap, itor);
}

