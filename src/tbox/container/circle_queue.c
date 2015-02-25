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
 * @file        circle_queue.c
 * @ingroup     container
 *
 */
/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "circle_queue.h"
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
#ifdef __tb_small__
#   define TB_CIRCLE_QUEUE_SIZE_DEFAULT            (255)
#else
#   define TB_CIRCLE_QUEUE_SIZE_DEFAULT            (65535)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the circle_queue impl type
typedef struct __tb_circle_queue_impl_t
{
    // the itor
    tb_iterator_t           itor;

    // the data
    tb_byte_t*              data;
    
    // the head
    tb_size_t               head;

    // the tail
    tb_size_t               tail;

    // the maxn
    tb_size_t               maxn;

    // the element
    tb_element_t            element;

}tb_circle_queue_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_size_t tb_circle_queue_itor_size(tb_iterator_ref_t iterator)
{   
    // check
    tb_circle_queue_impl_t* impl = (tb_circle_queue_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);

    // the size
    return ((impl->tail + impl->maxn - impl->head) & (impl->maxn - 1));
}
static tb_size_t tb_circle_queue_itor_head(tb_iterator_ref_t iterator)
{
    // check
    tb_circle_queue_impl_t* impl = (tb_circle_queue_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);

    // head
    return impl->head;
}
static tb_size_t tb_circle_queue_itor_last(tb_iterator_ref_t iterator)
{
    // check
    tb_circle_queue_impl_t* impl = (tb_circle_queue_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);

    // last
    return ((impl->tail + impl->maxn - 1) & (impl->maxn - 1));
}
static tb_size_t tb_circle_queue_itor_tail(tb_iterator_ref_t iterator)
{
    // check
    tb_circle_queue_impl_t* impl = (tb_circle_queue_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);

    // tail
    return impl->tail;
}
static tb_size_t tb_circle_queue_itor_next(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_circle_queue_impl_t* impl = (tb_circle_queue_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);

    // next
    return ((itor + 1) & (impl->maxn - 1));
}
static tb_size_t tb_circle_queue_itor_prev(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_circle_queue_impl_t* impl = (tb_circle_queue_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);

    // prev
    return ((itor + impl->maxn - 1) & (impl->maxn - 1));
}
static tb_pointer_t tb_circle_queue_itor_item(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_circle_queue_impl_t* impl = (tb_circle_queue_impl_t*)iterator;
    tb_assert_and_check_return_val(impl && itor < impl->maxn, tb_null);

    // item
    return impl->element.data(&impl->element, impl->data + itor * iterator->step);
}
static tb_void_t tb_circle_queue_itor_copy(tb_iterator_ref_t iterator, tb_size_t itor, tb_cpointer_t item)
{
    // check
    tb_circle_queue_impl_t* impl = (tb_circle_queue_impl_t*)iterator;
    tb_assert_return(impl);

    // copy
    impl->element.copy(&impl->element, impl->data + itor * iterator->step, item);
}
static tb_long_t tb_circle_queue_itor_comp(tb_iterator_ref_t iterator, tb_cpointer_t litem, tb_cpointer_t ritem)
{
    // check
    tb_circle_queue_impl_t* impl = (tb_circle_queue_impl_t*)iterator;
    tb_assert_and_check_return_val(impl && impl->element.comp, 0);

    // comp
    return impl->element.comp(&impl->element, litem, ritem);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_circle_queue_ref_t tb_circle_queue_init(tb_size_t maxn, tb_element_t element)
{
    // check
    tb_assert_and_check_return_val(element.size && element.dupl && element.data, tb_null);

    // done
    tb_bool_t                   ok = tb_false;
    tb_circle_queue_impl_t*     impl = tb_null;
    do
    {
        // make impl
        impl = tb_malloc0_type(tb_circle_queue_impl_t);
        tb_assert_and_check_break(impl);

        // using the default maxn
        if (!maxn) maxn = TB_CIRCLE_QUEUE_SIZE_DEFAULT;

        // init impl
        impl->maxn = tb_align_pow2(maxn + 1); // + tail
        impl->element = element;
        tb_assert_and_check_break(tb_ispow2(impl->maxn));

        // init iterator
        impl->itor.mode = TB_ITERATOR_MODE_FORWARD | TB_ITERATOR_MODE_REVERSE | TB_ITERATOR_MODE_MUTABLE;
        impl->itor.priv = tb_null;
        impl->itor.step = element.size;
        impl->itor.size = tb_circle_queue_itor_size;
        impl->itor.head = tb_circle_queue_itor_head;
        impl->itor.last = tb_circle_queue_itor_last;
        impl->itor.tail = tb_circle_queue_itor_tail;
        impl->itor.prev = tb_circle_queue_itor_prev;
        impl->itor.next = tb_circle_queue_itor_next;
        impl->itor.item = tb_circle_queue_itor_item;
        impl->itor.copy = tb_circle_queue_itor_copy;
        impl->itor.comp = tb_circle_queue_itor_comp;

        // make data
        impl->data = (tb_byte_t*)tb_nalloc0(impl->maxn, element.size);
        tb_assert_and_check_break(impl->data);

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        if (impl) tb_circle_queue_exit((tb_circle_queue_ref_t)impl);
        impl = tb_null;
    }

    // ok?
    return (tb_circle_queue_ref_t)impl;
}
tb_void_t tb_circle_queue_exit(tb_circle_queue_ref_t circle_queue)
{   
    // check
    tb_circle_queue_impl_t* impl = (tb_circle_queue_impl_t*)circle_queue;
    tb_assert_and_check_return(impl);
    
    // clear data
    tb_circle_queue_clear(circle_queue);

    // free data
    if (impl->data) tb_free(impl->data);

    // free it
    tb_free(impl);
}
tb_void_t tb_circle_queue_clear(tb_circle_queue_ref_t circle_queue)
{
    // check
    tb_circle_queue_impl_t* impl = (tb_circle_queue_impl_t*)circle_queue;
    tb_assert_and_check_return(impl);
    
    // clear it
    while (!tb_circle_queue_null(circle_queue)) tb_circle_queue_pop(circle_queue);

    // reset head and tail
    impl->head = 0;
    impl->tail = 0;
}
tb_void_t tb_circle_queue_put(tb_circle_queue_ref_t circle_queue, tb_cpointer_t data)
{   
    // check
    tb_circle_queue_impl_t* impl = (tb_circle_queue_impl_t*)circle_queue;
    tb_assert_and_check_return(impl && !tb_circle_queue_full(circle_queue));

    // put it
    impl->element.dupl(&impl->element, impl->data + impl->tail * impl->element.size, data);
    impl->tail = ((impl->tail + 1) & (impl->maxn - 1));
}
tb_void_t tb_circle_queue_pop(tb_circle_queue_ref_t circle_queue)
{   
    // check
    tb_circle_queue_impl_t* impl = (tb_circle_queue_impl_t*)circle_queue;
    tb_assert_and_check_return(impl && !tb_circle_queue_null(circle_queue));

    // pop it
    if (impl->element.free) impl->element.free(&impl->element, impl->data + impl->head * impl->element.size);
    impl->head = ((impl->head + 1) & (impl->maxn - 1));
}
tb_pointer_t tb_circle_queue_get(tb_circle_queue_ref_t circle_queue)
{
    // get the head item
    return tb_circle_queue_head(circle_queue);
}
tb_pointer_t tb_circle_queue_head(tb_circle_queue_ref_t circle_queue)
{
    // the head item
    return tb_iterator_item((tb_iterator_ref_t)circle_queue, tb_iterator_head((tb_iterator_ref_t)circle_queue));
}
tb_pointer_t tb_circle_queue_last(tb_circle_queue_ref_t circle_queue)
{
    // the last item
    return tb_iterator_item((tb_iterator_ref_t)circle_queue, tb_iterator_last((tb_iterator_ref_t)circle_queue));
}
tb_size_t tb_circle_queue_size(tb_circle_queue_ref_t circle_queue)
{   
    // check
    tb_circle_queue_impl_t* impl = (tb_circle_queue_impl_t*)circle_queue;
    tb_assert_and_check_return_val(impl, 0);

    // the size
    return ((impl->tail + impl->maxn - impl->head) & (impl->maxn - 1));
}
tb_size_t tb_circle_queue_maxn(tb_circle_queue_ref_t circle_queue)
{   
    // check
    tb_circle_queue_impl_t* impl = (tb_circle_queue_impl_t*)circle_queue;
    tb_assert_and_check_return_val(impl, 0);

    // the maxn
    return (impl->maxn? impl->maxn - 1 : 0);
}
tb_bool_t tb_circle_queue_full(tb_circle_queue_ref_t circle_queue)
{   
    // check
    tb_circle_queue_impl_t* impl = (tb_circle_queue_impl_t*)circle_queue;
    tb_assert_and_check_return_val(impl, tb_true);

    // is full?
    return (impl->head == ((impl->tail + 1) & (impl->maxn - 1)));
}
tb_bool_t tb_circle_queue_null(tb_circle_queue_ref_t circle_queue)
{   
    // check
    tb_circle_queue_impl_t* impl = (tb_circle_queue_impl_t*)circle_queue;
    tb_assert_and_check_return_val(impl, tb_true);

    // is null?
    return (impl->head == impl->tail);
}
#ifdef __tb_debug__
tb_void_t tb_circle_queue_dump(tb_circle_queue_ref_t circle_queue)
{
    // check
    tb_circle_queue_impl_t* impl = (tb_circle_queue_impl_t*)circle_queue;
    tb_assert_and_check_return(impl);

    // trace
    tb_trace_i("circle_queue: size: %lu", tb_circle_queue_size(circle_queue));

    // done
    tb_char_t cstr[4096];
    tb_for_all (tb_pointer_t, data, circle_queue)
    {
        // trace
        if (impl->element.cstr) 
        {
            tb_trace_i("    %s", impl->element.cstr(&impl->element, data, cstr, sizeof(cstr)));
        }
        else
        {
            tb_trace_i("    %p", data);
        }
    }
}
#endif
