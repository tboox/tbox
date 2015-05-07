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

// the heap grow
#ifdef __tb_small__ 
#   define TB_HEAP_GROW             (128)
#else
#   define TB_HEAP_GROW             (256)
#endif

// the heap maxn
#ifdef __tb_small__
#   define TB_HEAP_MAXN             (1 << 16)
#else
#   define TB_HEAP_MAXN             (1 << 30)
#endif

// enable check
#define TB_HEAP_CHECK_ENABLE        (0)

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

    // the element
    tb_element_t            element;

}tb_heap_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
#if TB_HEAP_CHECK_ENABLE
static tb_void_t tb_heap_check(tb_heap_impl_t* impl)
{
    // init
    tb_byte_t*  data = impl->data;
    tb_size_t   tail = impl->size;
    tb_size_t   step = impl->element.size;
    tb_size_t   parent = 0;

    // done
    for (; parent < tail; parent++)
    {   
        // the left child node
        tb_size_t lchild = (parent << 1) + 1;
        tb_check_break(lchild < tail);

        // the parent data
        tb_pointer_t parent_data = impl->element.data(&impl->element, data + parent * step);

        // check?
        if (impl->element.comp(&impl->element, impl->element.data(&impl->element, data + lchild * step), parent_data) < 0) 
        {
            // dump heap
            tb_heap_dump((tb_heap_ref_t)impl);

            // abort
            tb_assertf_abort(0, "lchild[%lu]: invalid, parent: %lu, tail: %lu", lchild, parent, tail);
        }

        // the right child node
        tb_size_t rchild = lchild + 1;
        tb_check_break(rchild < tail);

        // check?
        if (impl->element.comp(&impl->element, impl->element.data(&impl->element, data + rchild * step), parent_data) < 0) 
        {
            // dump heap
            tb_heap_dump((tb_heap_ref_t)impl);

            // abort
            tb_assertf_abort(0, "rchild[%lu]: invalid, parent: %lu, tail: %lu", rchild, parent, tail);
        }
    }
}
#endif
/*! shift up the heap
 *
 * <pre>
 *
 * before:
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
 *                  10      5(hole) <------ data
 * after:
 *
 *                                          1(head)
 *                               -------------------------
 *                              |                         |
 *                              4                         2
 *                        --------------             -------------
 *                       |              |           |             |
 *         data -------> 5(hole)        9           7             8
 *                   ---------       
 *                  |         |     
 *                  10        6
 * </pre>
 */
static tb_pointer_t tb_heap_shift_up(tb_heap_impl_t* impl, tb_size_t hole, tb_cpointer_t data)
{
    // check
    tb_assert_and_check_return_val(impl && impl->data, tb_null);

    // the element function
    tb_element_comp_func_t func_comp = impl->element.comp;
    tb_element_data_func_t func_data = impl->element.data;
    tb_assert_abort(func_comp && func_data);

    // walk, (hole - 1) / 2: the parent node of the hole
    tb_size_t   parent = 0;
    tb_byte_t*  head = impl->data;
    tb_size_t   step = impl->element.size;
    switch (step)
    {
    case sizeof(tb_size_t):
        {
            for (parent = (hole - 1) >> 1; hole && (func_comp(&impl->element, func_data(&impl->element, head + parent * step), data) > 0); parent = (hole - 1) >> 1)
            {
                // move item: parent => hole
                *((tb_size_t*)(head + hole * step)) = *((tb_size_t*)(head + parent * step));

                // move node: hole => parent
                hole = parent;
            }
        }
        break;
    default:
        for (parent = (hole - 1) >> 1; hole && (func_comp(&impl->element, func_data(&impl->element, head + parent * step), data) > 0); parent = (hole - 1) >> 1)
        {
            // move item: parent => hole
            tb_memcpy(head + hole * step, head + parent * step, step);

            // move node: hole => parent
            hole = parent;
        }
        break;
    }

    // ok?
    return head + hole * step;
}
/*! shift down the heap
 *
 * <pre>
 * 
 * before:
 *                                          1(head)
 *                               -------------------------
 *                              |                         |
 *                           (hole)                       2
 *                        --------------             -------------
 *                       |              |           |             |
 *            lchild --> 6(smaller)     7           7             8
 *                   ---------     ------
 *                  |         |   |          
 *                 11        16  10          
 *
 *
 * move hole:
 *                                          1(head)
 *                               -------------------------
 *                              |                         |
 *                              6                         2
 *                        --------------             -------------
 *                       |              |           |             |
 *                     (hole)           7           7             8
 *                   ---------      -----                                                   
 *                  |         |    |                                                  
 *      lchild --> 11(smaller)16  10                                                   
 *
 * 11 >= data: 9? break it
 *
 * move data to hole:
 *                                          1(head)
 *                               -------------------------
 *                              |                         |
 *                              6                         2
 *                        --------------             -------------
 *                       |              |           |             |
 *    data ------------> 9              7           7             8
 *                   ---------       ---                                                   
 *                  |         |     |                                                        
 *                 11        16    10
 * 
 * </pre>
 */
static tb_pointer_t tb_heap_shift_down(tb_heap_impl_t* impl, tb_size_t hole, tb_cpointer_t data)
{
    // check
    tb_assert_and_check_return_val(impl && impl->data, tb_null);

    // init element
    tb_element_comp_func_t func_comp = impl->element.comp;
    tb_element_data_func_t func_data = impl->element.data;
    tb_assert_abort(func_comp && func_data);

    // 2 * hole + 1: the left child node of hole
    tb_size_t       step = impl->element.size;
    tb_byte_t*      head = impl->data;
    tb_byte_t*      tail = head + impl->size * step;
    tb_byte_t*      phole = head + hole * step;
    tb_byte_t*      lchild = head + ((hole << 1) + 1) * step;
    tb_pointer_t    data_lchild = tb_null;
    tb_pointer_t    data_rchild = tb_null;
    switch (step)
    {
    case sizeof(tb_size_t):
        {
            for (; lchild < tail; lchild = head + (((lchild - head) << 1) + step))
            {   
                // the smaller child node
                data_lchild = func_data(&impl->element, lchild);
                if (lchild + step < tail && func_comp(&impl->element, data_lchild, (data_rchild = func_data(&impl->element, lchild + step))) > 0) 
                {
                    lchild += step;
                    data_lchild = data_rchild;
                }

                // end?
                if (func_comp(&impl->element, data_lchild, data) >= 0) break;

                // the smaller child node => hole
                *((tb_size_t*)phole) = *((tb_size_t*)lchild);

                // move the hole down to it's smaller child node 
                phole = lchild;
            }
        }
        break;
    default:
        {
            for (; lchild < tail; lchild = head + (((lchild - head) << 1) + step))
            {   
                // the smaller child node
                data_lchild = func_data(&impl->element, lchild);
                if (lchild + step < tail && func_comp(&impl->element, data_lchild, (data_rchild = func_data(&impl->element, lchild + step))) > 0) 
                {
                    lchild += step;
                    data_lchild = data_rchild;
                }

                // end?
                if (func_comp(&impl->element, data_lchild, data) >= 0) break;

                // the smaller child node => hole
                tb_memcpy(phole, lchild, step);

                // move the hole down to it's smaller child node 
                phole = lchild;
            }
        }
        break;
    }

    // ok?
    return phole;
}
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
    return impl->element.data(&impl->element, impl->data + itor * iterator->step);
}
static tb_void_t tb_heap_itor_copy(tb_iterator_ref_t iterator, tb_size_t itor, tb_cpointer_t item)
{
    // check
    tb_heap_impl_t* impl = (tb_heap_impl_t*)iterator;
    tb_assert_and_check_return(impl);

    // copy
    impl->element.copy(&impl->element, impl->data + itor * iterator->step, item);
}
static tb_long_t tb_heap_itor_comp(tb_iterator_ref_t iterator, tb_cpointer_t litem, tb_cpointer_t ritem)
{
    // check
    tb_heap_impl_t* impl = (tb_heap_impl_t*)iterator;
    tb_assert_and_check_return_val(impl && impl->element.comp, 0);

    // comp
    return impl->element.comp(&impl->element, litem, ritem);
}
static tb_void_t tb_heap_itor_remove(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_heap_impl_t* impl = (tb_heap_impl_t*)iterator;
    tb_assert_and_check_return(impl && impl->data && impl->size && itor < impl->size);

    // check the element function
    tb_assert_abort(impl->element.comp && impl->element.data);

    // the step
    tb_size_t step = impl->element.size;
    tb_assert_abort(step);

    // free the item first
    if (impl->element.free) impl->element.free(&impl->element, impl->data + itor * step);

    // the removed item is not the last item?
    if (itor != impl->size - 1)
    {
        // the last and parent
        tb_pointer_t last = impl->data + (impl->size - 1) * step;
        tb_pointer_t parent = impl->data + ((itor - 1) >> 1) * step;

        // the last and parent data
        tb_pointer_t data_last = impl->element.data(&impl->element, last);
        tb_pointer_t data_parent = impl->element.data(&impl->element, parent);

        /* we might need to shift it upward if it is less than its parent, 
         * or downward if it is greater than one or both its children. 
         *
         * since the children are known to be less than the parent, 
         * it can't need to shift both up and down.
         */
        tb_pointer_t hole = tb_null;
        if (itor && impl->element.comp(&impl->element, data_parent, data_last) > 0) 
        {
            // shift up the heap from the given hole
            hole = tb_heap_shift_up(impl, itor, data_last);
        }
        // shift down the heap from the given hole
        else hole = tb_heap_shift_down(impl, itor, data_last);
        tb_assert_abort(hole);

        // copy the last data to the hole
        if (hole != last) tb_memcpy(hole, last, step);
    }

    // size--
    impl->size--;

    // check
#if TB_HEAP_CHECK_ENABLE
    tb_heap_check(impl);
#endif
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_heap_ref_t tb_heap_init(tb_size_t grow, tb_element_t element)
{
    // check
    tb_assert_and_check_return_val(element.size && element.data && element.dupl && element.repl, tb_null);

    // done
    tb_bool_t       ok = tb_false;
    tb_heap_impl_t* impl = tb_null;
    do
    {
        // using the default grow
        if (!grow) grow = TB_HEAP_GROW;

        // make impl
        impl = tb_malloc0_type(tb_heap_impl_t);
        tb_assert_and_check_break(impl);

        // init impl
        impl->size      = 0;
        impl->grow      = grow;
        impl->maxn      = grow;
        impl->element   = element;
        tb_assert_and_check_break(impl->maxn < TB_HEAP_MAXN);

        // init iterator
        impl->itor.mode     = TB_ITERATOR_MODE_FORWARD | TB_ITERATOR_MODE_REVERSE | TB_ITERATOR_MODE_RACCESS | TB_ITERATOR_MODE_MUTABLE;
        impl->itor.priv     = tb_null;
        impl->itor.step     = element.size;
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
        impl->data = (tb_byte_t*)tb_nalloc0(impl->maxn, element.size);
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
    if (impl->element.nfree)
        impl->element.nfree(&impl->element, impl->data, impl->size);

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
tb_void_t tb_heap_put(tb_heap_ref_t heap, tb_cpointer_t data)
{
    // check
    tb_heap_impl_t* impl = (tb_heap_impl_t*)heap;
    tb_assert_and_check_return(impl && impl->element.dupl && impl->data);

    // no enough? grow it
    if (impl->size == impl->maxn)
    {
        // the maxn
        tb_size_t maxn = tb_align4(impl->maxn + impl->grow);
        tb_assert_and_check_return(maxn < TB_HEAP_MAXN);

        // realloc data
        impl->data = (tb_byte_t*)tb_ralloc(impl->data, maxn * impl->element.size);
        tb_assert_and_check_return(impl->data);

        // must be align by 4-bytes
        tb_assert_and_check_return(!(((tb_size_t)(impl->data)) & 3));

        // clear the grow data
        tb_memset(impl->data + impl->size * impl->element.size, 0, (maxn - impl->maxn) * impl->element.size);

        // save maxn
        impl->maxn = maxn;
    }

    // check
    tb_assert_and_check_return(impl->size < impl->maxn);
    
    // shift up the heap from the tail hole
    tb_pointer_t hole = tb_heap_shift_up(impl, impl->size++, data);
    tb_assert_abort(hole);
        
    // save data to the hole
    if (hole) impl->element.dupl(&impl->element, hole, data);

    // check
#if TB_HEAP_CHECK_ENABLE
    tb_heap_check(impl);
#endif
}
tb_void_t tb_heap_pop(tb_heap_ref_t heap)
{
    // check
    tb_heap_impl_t* impl = (tb_heap_impl_t*)heap;
    tb_assert_and_check_return(impl && impl->data && impl->size);

    // free the top item first
    if (impl->element.free) impl->element.free(&impl->element, impl->data);

    // the last item is not in top 
    if (impl->size > 1)
    {
        // check the element function
        tb_assert_abort(impl->element.data);

        // the step
        tb_size_t step = impl->element.size;
        tb_assert_abort(step);

        // the last 
        tb_pointer_t last = impl->data + (impl->size - 1) * step;

        // shift down the heap from the top hole
        tb_pointer_t hole = tb_heap_shift_down(impl, 0, impl->element.data(&impl->element, last));
        tb_assert_abort(hole);

        // copy the last data to the hole
        if (hole != last) tb_memcpy(hole, last, step);
    }

    // update the size
    impl->size--;

    // check
#if TB_HEAP_CHECK_ENABLE
    tb_heap_check(impl);
#endif
}
tb_void_t tb_heap_remove(tb_heap_ref_t heap, tb_size_t itor)
{
    tb_heap_itor_remove(heap, itor);
}
#ifdef __tb_debug__
tb_void_t tb_heap_dump(tb_heap_ref_t heap)
{
    // check
    tb_heap_impl_t* impl = (tb_heap_impl_t*)heap;
    tb_assert_and_check_return(impl);

    // trace
    tb_trace_i("heap: size: %lu", tb_heap_size(heap));

    // done
    tb_char_t cstr[4096];
    tb_for_all (tb_pointer_t, data, heap)
    {
        // trace
        if (impl->element.cstr) 
        {
#if TB_HEAP_CHECK_ENABLE
            tb_trace_i("    [%lu]: %s", data_itor, impl->element.cstr(&impl->element, data, cstr, sizeof(cstr)));
#else
            tb_trace_i("    %s", impl->element.cstr(&impl->element, data, cstr, sizeof(cstr)));
#endif
        }
        else
        {
            tb_trace_i("    %p", data);
        }
    }
}
#endif
