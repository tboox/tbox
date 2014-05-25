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
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the item maxn
#define TB_HEAP_ITEM_MAXN               (1 << 30)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/*! the heap type
 *
 * <pre>
 * heap:    1      4      2      6       9       7       8       10       14       16
 *
 *                                          1(head)
 *                               -------------------------
 *                              |                         |
 *                              4                         2
 *                        --------------             -------------
 *                       |              |           |             |
 *                       6       (last / 2 - 1)9    7             8
 *                   ---------       ----
 *                  |         |     |
 *                  10        14    16(last - 1)
 * </pre>
 *
 * head: => the head item
 * last: => the last item
 * tail: => behind the last item, no item
 *
 * performance: 
 *
 * put: O(lgn)
 * pop: O(1)
 * top: O(1)
 *
 * iterator:
 *
 * next: fast
 * prev: fast
 *
 * </pre>
 *
 * @note the itor of the same item is mutable
 */
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
static tb_void_t tb_heap_check(tb_heap_impl_t* heap)
{
    // init
    tb_byte_t*  data = heap->data;
    tb_size_t   tail = heap->size;
    tb_size_t   step = heap->func.size;
    tb_size_t   parent = 0;

    // walk
    for (; parent < tail; parent++)
    {   
        // the left child node
        tb_size_t   lchild  = (parent << 1) + 1;
        tb_check_break(lchild < tail);

        // the parent data
        tb_pointer_t parent_data = heap->func.data(&heap->func, data + parent * step);

        // check?
        if (heap->func.comp(&heap->func, heap->func.data(&heap->func, data + lchild * step), parent_data) < 0) 
        {
            tb_trace_d("lchild[%lu]: invalid, parent: %lu", lchild, parent);
            break;
        }

        // the right child node
        tb_size_t   rchild  = (parent << 1) + 2;
        tb_check_break(rchild < tail);

        // check?
        if (heap->func.comp(&heap->func, heap->func.data(&heap->func, data + rchild * step), parent_data) < 0) 
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
static tb_size_t tb_heap_iterator_size(tb_iterator_t* iterator)
{
    // check
    tb_heap_impl_t* heap = (tb_heap_impl_t*)iterator;
    tb_assert_and_check_return_val(heap, 0);

    // size
    return heap->size;
}
static tb_size_t tb_heap_iterator_head(tb_iterator_t* iterator)
{
    // check
    tb_heap_impl_t* heap = (tb_heap_impl_t*)iterator;
    tb_assert_and_check_return_val(heap, 0);

    // head
    return 0;
}
static tb_size_t tb_heap_iterator_tail(tb_iterator_t* iterator)
{
    // check
    tb_heap_impl_t* heap = (tb_heap_impl_t*)iterator;
    tb_assert_and_check_return_val(heap, 0);

    // tail
    return heap->size;
}
static tb_size_t tb_heap_iterator_next(tb_iterator_t* iterator, tb_size_t itor)
{
    // check
    tb_heap_impl_t* heap = (tb_heap_impl_t*)iterator;
    tb_assert_and_check_return_val(heap, 0);
    tb_assert_and_check_return_val(itor < heap->size, heap->size);

    // next
    return itor + 1;
}
static tb_size_t tb_heap_iterator_prev(tb_iterator_t* iterator, tb_size_t itor)
{
    // check
    tb_heap_impl_t* heap = (tb_heap_impl_t*)iterator;
    tb_assert_and_check_return_val(heap, 0);
    tb_assert_and_check_return_val(itor && itor < heap->size, 0);

    // prev
    return itor - 1;
}
static tb_pointer_t tb_heap_iterator_item(tb_iterator_t* iterator, tb_size_t itor)
{
    // check
    tb_heap_impl_t* heap = (tb_heap_impl_t*)iterator;
    tb_assert_and_check_return_val(heap && itor < heap->size, tb_null);
    
    // data
    return heap->func.data(&heap->func, heap->data + itor * iterator->step);
}
static tb_void_t tb_heap_iterator_copy(tb_iterator_t* iterator, tb_size_t itor, tb_cpointer_t item)
{
    // check
    tb_heap_impl_t* heap = (tb_heap_impl_t*)iterator;
    tb_assert_and_check_return(heap);

    // copy
    heap->func.copy(&heap->func, heap->data + itor * iterator->step, item);
}
static tb_long_t tb_heap_iterator_comp(tb_iterator_t* iterator, tb_cpointer_t ltem, tb_cpointer_t rtem)
{
    // check
    tb_heap_impl_t* heap = (tb_heap_impl_t*)iterator;
    tb_assert_and_check_return_val(heap && heap->func.comp, 0);

    // comp
    return heap->func.comp(&heap->func, ltem, rtem);
}

/*! remove the heap item
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
static tb_void_t tb_heap_iterator_delt(tb_iterator_t* iterator, tb_size_t itor)
{
    // check
    tb_heap_impl_t* heap = (tb_heap_impl_t*)iterator;
    tb_assert_and_check_return(heap && heap->data && heap->size && itor < heap->size);

    // init func
    tb_item_func_comp_t func_comp = heap->func.comp;
    tb_item_func_data_t func_data = heap->func.data;
    tb_assert_and_check_return(func_comp && func_data);

    // walk, 2 * hole + 1: the left child node of hole
    tb_size_t           step = heap->func.size;
    tb_byte_t*          head = heap->data;
    tb_byte_t*          hole = head + itor * step;
    tb_byte_t*          tail = head + heap->size * step;
    tb_byte_t*          last = head + (heap->size - 1) * step;
    tb_byte_t*          child = head + ((itor << 1) + 1) * step;
    tb_pointer_t        data_child = tb_null;
    tb_pointer_t        data_rchild = tb_null;
    tb_pointer_t        data_last = func_data(&heap->func, last);
    switch (step)
    {
    case sizeof(tb_uint64_t):
        {
            for (; child < tail; child = head + (((child - head) << 1) + step))
            {   
                // the smaller child node
                data_child = func_data(&heap->func, child);
                if (child + step < tail && func_comp(&heap->func, data_child, (data_rchild = func_data(&heap->func, child + step))) > 0) 
                {
                    child += step;
                    data_child = data_rchild;
                }

                // end?
                if (func_comp(&heap->func, data_child, data_last) > 0) break;

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
                data_child = func_data(&heap->func, child);
                if (child + step < tail && func_comp(&heap->func, data_child, (data_rchild = func_data(&heap->func, child + step))) > 0) 
                {
                    child += step;
                    data_child = data_rchild;
                }

                // end?
                if (func_comp(&heap->func, data_child, data_last) > 0) break;

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
                data_child = func_data(&heap->func, child);
                if (child + step < tail && func_comp(&heap->func, data_child, (data_rchild = func_data(&heap->func, child + step))) > 0) 
                {
                    child += step;
                    data_child = data_rchild;
                }

                // end?
                if (func_comp(&heap->func, data_child, data_last) > 0) break;

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
                data_child = func_data(&heap->func, child);
                if (child + step < tail && func_comp(&heap->func, data_child, (data_rchild = func_data(&heap->func, child + step))) > 0) 
                {
                    child += step;
                    data_child = data_rchild;
                }

                // end?
                if (func_comp(&heap->func, data_child, data_last) > 0) break;

                // the smaller child node => hole
                *((tb_uint8_t*)hole) = *((tb_uint8_t*)child);

                // move the hole down to it's larger child node 
                hole = child;
            }

        }
        break;
    default:
        {
            for (; child < tail; child = head + (((child - head) << 1) + step))
            {   
                // the smaller child node
                data_child = func_data(&heap->func, child);
                if (child + step < tail && func_comp(&heap->func, data_child, (data_rchild = func_data(&heap->func, child + step))) > 0) 
                {
                    child += step;
                    data_child = data_rchild;
                }

                // end?
                if (func_comp(&heap->func, data_child, data_last) > 0) break;

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
    heap->size--;

    // check
//  tb_heap_check(heap);
}
/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_heap_t* tb_heap_init(tb_size_t grow, tb_item_func_t func)
{
    // check
    tb_assert_and_check_return_val(grow, tb_null);
    tb_assert_and_check_return_val(func.size && func.data && func.dupl && func.repl, tb_null);

    // make heap
    tb_heap_impl_t* heap = (tb_heap_impl_t*)tb_malloc0(sizeof(tb_heap_impl_t));
    tb_assert_and_check_return_val(heap, tb_null);

    // init heap
    heap->size = 0;
    heap->grow = grow;
    heap->maxn = grow;
    heap->func = func;
    tb_assert_and_check_goto(heap->maxn < TB_HEAP_ITEM_MAXN, fail);

    // init iterator
    heap->itor.mode = TB_ITERATOR_MODE_FORWARD | TB_ITERATOR_MODE_REVERSE | TB_ITERATOR_MODE_RACCESS;
    heap->itor.priv = tb_null;
    heap->itor.step = func.size;
    heap->itor.size = tb_heap_iterator_size;
    heap->itor.head = tb_heap_iterator_head;
    heap->itor.tail = tb_heap_iterator_tail;
    heap->itor.prev = tb_heap_iterator_prev;
    heap->itor.next = tb_heap_iterator_next;
    heap->itor.item = tb_heap_iterator_item;
    heap->itor.copy = tb_heap_iterator_copy;
    heap->itor.comp = tb_heap_iterator_comp;
    heap->itor.delt = tb_heap_iterator_delt;

    // make data
    heap->data = tb_nalloc0(heap->maxn, func.size);
    tb_assert_and_check_goto(heap->data, fail);

    // ok
    return heap;
fail:
    if (heap) tb_heap_exit(heap);
    return tb_null;
}
tb_void_t tb_heap_exit(tb_heap_t* handle)
{
    tb_heap_impl_t* heap = (tb_heap_impl_t*)handle;
    if (heap)
    {
        // clear data
        tb_heap_clear(heap);

        // free data
        if (heap->data) tb_free(heap->data);
        heap->data = tb_null;

        // free it
        tb_free(heap);
    }
}
tb_void_t tb_heap_clear(tb_heap_t* handle)
{   
    tb_heap_impl_t* heap = (tb_heap_impl_t*)handle;
    if (heap) 
    {
        // free data
        if (heap->func.nfree)
            heap->func.nfree(&heap->func, heap->data, heap->size);

        // reset size 
        heap->size = 0;
    }
}
tb_size_t tb_heap_size(tb_heap_t const* handle)
{
    // check
    tb_heap_impl_t const* heap = (tb_heap_impl_t const*)handle;
    tb_assert_and_check_return_val(heap, 0);

    // size
    return heap->size;
}
tb_size_t tb_heap_grow(tb_heap_t const* handle)
{
    // check
    tb_heap_impl_t const* heap = (tb_heap_impl_t const*)handle;
    tb_assert_and_check_return_val(heap, 0);

    // grow
    return heap->grow;
}
tb_size_t tb_heap_maxn(tb_heap_t const* handle)
{
    // check
    tb_heap_impl_t const* heap = (tb_heap_impl_t const*)handle;
    tb_assert_and_check_return_val(heap, 0);

    // maxn
    return heap->maxn;
}
tb_pointer_t tb_heap_top(tb_heap_t* handle)
{
    return tb_iterator_item(handle, tb_iterator_head(handle));
}
/*! put heap
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
tb_void_t tb_heap_put(tb_heap_t* handle, tb_cpointer_t data)
{
    // check
    tb_heap_impl_t* heap = (tb_heap_impl_t*)handle;
    tb_assert_and_check_return(heap && heap->data);

    // full? grow it
    if (heap->size == heap->maxn)
    {
        // the maxn
        tb_size_t maxn = tb_align4(heap->maxn + heap->grow);
        tb_assert_and_check_return(maxn < TB_HEAP_ITEM_MAXN);

        // realloc data
        heap->data = (tb_byte_t*)tb_ralloc(heap->data, maxn * heap->func.size);
        tb_assert_and_check_return(heap->data);

        // must be align by 4-bytes
        tb_assert_and_check_return(!(((tb_size_t)(heap->data)) & 3));

        // clear the grow data
        tb_memset(heap->data + heap->size * heap->func.size, 0, (maxn - heap->maxn) * heap->func.size);

        // save maxn
        heap->maxn = maxn;
    }

    // check
    tb_assert_and_check_return(heap->size < heap->maxn);
    
    // init func
    tb_item_func_comp_t func_comp = heap->func.comp;
    tb_item_func_data_t func_data = heap->func.data;
    tb_assert_and_check_return(func_comp && func_data);

    // walk, (hole - 1) / 2: the parent node of the hole
    tb_size_t           parent = 0;
    tb_byte_t*          head = heap->data;
    tb_size_t           hole = heap->size;
    tb_size_t           step = heap->func.size;
    switch (step)
    {
    case sizeof(tb_uint64_t):
        {
            for (parent = (hole - 1) >> 1; hole && (func_comp(&heap->func, func_data(&heap->func, head + parent * step), data) > 0); parent = (hole - 1) >> 1)
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
            for (parent = (hole - 1) >> 1; hole && (func_comp(&heap->func, func_data(&heap->func, head + parent * step), data) > 0); parent = (hole - 1) >> 1)
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
            for (parent = (hole - 1) >> 1; hole && (func_comp(&heap->func, func_data(&heap->func, head + parent * step), data) > 0); parent = (hole - 1) >> 1)
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
            for (parent = (hole - 1) >> 1; hole && (func_comp(&heap->func, func_data(&heap->func, head + parent * step), data) > 0); parent = (hole - 1) >> 1)
            {
                // move item: parent => hole
                *((tb_uint8_t*)(head + hole * step)) = *((tb_uint8_t*)(head + parent * step));

                // move node: hole => parent
                hole = parent;
            }
        }
        break;
    default:
        for (parent = (hole - 1) >> 1; hole && (func_comp(&heap->func, func_data(&heap->func, head + parent * step), data) > 0); parent = (hole - 1) >> 1)
        {
            // move item: parent => hole
            tb_memcpy(head + hole * step, head + parent * step, step);

            // move node: hole => parent
            hole = parent;
        }
        break;
    }

    // save data
    heap->func.dupl(&heap->func, head + hole * step, data);

    // size++
    heap->size++;

    // check
//  tb_heap_check(heap);
}
tb_void_t tb_heap_pop(tb_heap_t* handle)
{
    tb_heap_iterator_delt(handle, 0);
}
tb_void_t tb_heap_del(tb_heap_t* handle, tb_size_t itor)
{
    tb_heap_iterator_delt(handle, itor);
}

