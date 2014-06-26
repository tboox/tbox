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
 * @file        queue.c
 * @ingroup     container
 *
 */
/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "queue.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../memory/memory.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#ifdef __tb_small__
#   define TB_QUEUE_SIZE_DEFAULT            TB_QUEUE_SIZE_SMALL
#else
#   define TB_QUEUE_SIZE_DEFAULT            TB_QUEUE_SIZE_LARGE
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/*! the impl type
 *
 * <pre>
 * impl: |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||------|
 *       head                                                           last    tail
 *
 * impl: ||||||||||||||-----|--------------------------||||||||||||||||||||||||||
 *                   last  tail                       head                
 *
 * head: => the head item
 * last: => the last item
 * tail: => behind the last item, no item
 *
 * performance: 
 *
 * put: O(1)
 * pop: O(1)
 *
 * iterator:
 *
 * next: fast
 * prev: fast
 *
 * </pre>
 *
 * @note the index of the same item is mutable
 *
 */
typedef struct __tb_queue_impl_t
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

    // the func
    tb_item_func_t          func;

}tb_queue_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * iterator
 */
static tb_size_t tb_queue_iterator_size(tb_iterator_ref_t iterator)
{   
    // check
    tb_queue_impl_t* impl = (tb_queue_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);

    // the size
    return ((impl->tail + impl->maxn - impl->head) & (impl->maxn - 1));
}
static tb_size_t tb_queue_iterator_head(tb_iterator_ref_t iterator)
{
    // check
    tb_queue_impl_t* impl = (tb_queue_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);

    // head
    return impl->head;
}
static tb_size_t tb_queue_iterator_tail(tb_iterator_ref_t iterator)
{
    // check
    tb_queue_impl_t* impl = (tb_queue_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);

    // tail
    return impl->tail;
}
static tb_size_t tb_queue_iterator_next(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_queue_impl_t* impl = (tb_queue_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);

    // next
    return ((itor + 1) & (impl->maxn - 1));
}
static tb_size_t tb_queue_iterator_prev(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_queue_impl_t* impl = (tb_queue_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);

    // prev
    return ((itor + impl->maxn - 1) & (impl->maxn - 1));
}
static tb_pointer_t tb_queue_iterator_item(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_queue_impl_t* impl = (tb_queue_impl_t*)iterator;
    tb_assert_and_check_return_val(impl && itor < impl->maxn, tb_null);

    // item
    return impl->func.data(&impl->func, impl->data + itor * iterator->step);
}
static tb_void_t tb_queue_iterator_copy(tb_iterator_ref_t iterator, tb_size_t itor, tb_cpointer_t item)
{
    // check
    tb_queue_impl_t* impl = (tb_queue_impl_t*)iterator;
    tb_assert_return(impl);

    // copy
    impl->func.copy(&impl->func, impl->data + itor * iterator->step, item);
}
static tb_long_t tb_queue_iterator_comp(tb_iterator_ref_t iterator, tb_cpointer_t ltem, tb_cpointer_t rtem)
{
    // check
    tb_queue_impl_t* impl = (tb_queue_impl_t*)iterator;
    tb_assert_and_check_return_val(impl && impl->func.comp, 0);

    // comp
    return impl->func.comp(&impl->func, ltem, rtem);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_queue_ref_t tb_queue_init(tb_size_t maxn, tb_item_func_t func)
{
    // check
    tb_assert_and_check_return_val(func.size && func.dupl && func.data, tb_null);

    // done
    tb_bool_t           ok = tb_false;
    tb_queue_impl_t*    impl = tb_null;
    do
    {
        // make impl
        impl = tb_malloc0_type(tb_queue_impl_t);
        tb_assert_and_check_break(impl);

        // using the default maxn
        if (!maxn) maxn = TB_QUEUE_SIZE_DEFAULT;

        // init impl
        impl->maxn = tb_align_pow2(maxn + 1); // + tail
        impl->func = func;
        tb_assert_and_check_break(tb_ispow2(impl->maxn));

        // init iterator
        impl->itor.mode = TB_ITERATOR_MODE_FORWARD | TB_ITERATOR_MODE_REVERSE;
        impl->itor.priv = tb_null;
        impl->itor.step = func.size;
        impl->itor.size = tb_queue_iterator_size;
        impl->itor.head = tb_queue_iterator_head;
        impl->itor.tail = tb_queue_iterator_tail;
        impl->itor.prev = tb_queue_iterator_prev;
        impl->itor.next = tb_queue_iterator_next;
        impl->itor.item = tb_queue_iterator_item;
        impl->itor.copy = tb_queue_iterator_copy;
        impl->itor.comp = tb_queue_iterator_comp;

        // make data
        impl->data = (tb_byte_t*)tb_nalloc0(impl->maxn, func.size);
        tb_assert_and_check_break(impl->data);

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        if (impl) tb_queue_exit((tb_queue_ref_t)impl);
        impl = tb_null;
    }

    // ok?
    return (tb_queue_ref_t)impl;
}
tb_void_t tb_queue_exit(tb_queue_ref_t queue)
{   
    // check
    tb_queue_impl_t* impl = (tb_queue_impl_t*)queue;
    tb_assert_and_check_return(impl);
    
    // clear data
    tb_queue_clear(queue);

    // free data
    if (impl->data) tb_free(impl->data);

    // free it
    tb_free(impl);
}
tb_void_t tb_queue_clear(tb_queue_ref_t queue)
{
    // check
    tb_queue_impl_t* impl = (tb_queue_impl_t*)queue;
    tb_assert_and_check_return(impl);
    
    // clear it
    while (!tb_queue_null(queue)) tb_queue_pop(queue);
    impl->head = 0;
    impl->tail = 0;
}
tb_void_t tb_queue_put(tb_queue_ref_t queue, tb_cpointer_t data)
{   
    // check
    tb_queue_impl_t* impl = (tb_queue_impl_t*)queue;
    tb_assert_and_check_return(impl && !tb_queue_full(queue));

    // put it
    impl->func.dupl(&impl->func, impl->data + impl->tail * impl->func.size, data);
    impl->tail = ((impl->tail + 1) & (impl->maxn - 1));
}
tb_void_t tb_queue_pop(tb_queue_ref_t queue)
{   
    // check
    tb_queue_impl_t* impl = (tb_queue_impl_t*)queue;
    tb_assert_and_check_return(impl && !tb_queue_null(queue));

    // pop it
    if (impl->func.free) impl->func.free(&impl->func, impl->data + impl->head * impl->func.size);
    impl->head = ((impl->head + 1) & (impl->maxn - 1));
}
tb_pointer_t tb_queue_get(tb_queue_ref_t queue)
{
    return tb_queue_head(queue);
}
tb_pointer_t tb_queue_head(tb_queue_ref_t queue)
{
    return tb_iterator_item((tb_iterator_ref_t)queue, tb_iterator_head((tb_iterator_ref_t)queue));
}
tb_pointer_t tb_queue_last(tb_queue_ref_t queue)
{
    return tb_iterator_item((tb_iterator_ref_t)queue, tb_iterator_last((tb_iterator_ref_t)queue));
}
tb_size_t tb_queue_size(tb_queue_ref_t queue)
{   
    // check
    tb_queue_impl_t* impl = (tb_queue_impl_t*)queue;
    tb_assert_and_check_return_val(impl, 0);

    return ((impl->tail + impl->maxn - impl->head) & (impl->maxn - 1));
}
tb_size_t tb_queue_maxn(tb_queue_ref_t queue)
{   
    // check
    tb_queue_impl_t* impl = (tb_queue_impl_t*)queue;
    tb_assert_and_check_return_val(impl, 0);

    return (impl->maxn? impl->maxn - 1 : 0);
}
tb_bool_t tb_queue_full(tb_queue_ref_t queue)
{   
    // check
    tb_queue_impl_t* impl = (tb_queue_impl_t*)queue;
    tb_assert_and_check_return_val(impl, tb_true);

    return ((impl->head == ((impl->tail + 1) & (impl->maxn - 1)))? tb_true : tb_false);
}
tb_bool_t tb_queue_null(tb_queue_ref_t queue)
{   
    // check
    tb_queue_impl_t* impl = (tb_queue_impl_t*)queue;
    tb_assert_and_check_return_val(impl, tb_true);

    return ((impl->head == impl->tail)? tb_true : tb_false);
}

