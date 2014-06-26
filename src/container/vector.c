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
 * @file        vector.c
 * @ingroup     container
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "vector"
#define TB_TRACE_MODULE_DEBUG               (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "vector.h"
#include "../libc/libc.h"
#include "../utils/utils.h"
#include "../memory/memory.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the item maxn
#define TB_VECTOR_ITEM_MAXN             (1 << 30)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the vector impl type
typedef struct __tb_vector_impl_t
{
    // the itor
    tb_iterator_t           itor;

    // the data
    tb_byte_t*              data;
    tb_size_t               size;
    tb_size_t               grow;
    tb_size_t               maxn;

    // the func
    tb_item_func_t          func;

}tb_vector_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * iterator
 */
static tb_size_t tb_vector_iterator_size(tb_iterator_ref_t iterator)
{
    // check
    tb_vector_impl_t* impl = (tb_vector_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);

    // size
    return impl->size;
}
static tb_size_t tb_vector_iterator_head(tb_iterator_ref_t iterator)
{
    // check
    tb_vector_impl_t* impl = (tb_vector_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);

    // head
    return 0;
}
static tb_size_t tb_vector_iterator_tail(tb_iterator_ref_t iterator)
{
    // check
    tb_vector_impl_t* impl = (tb_vector_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);

    // tail
    return impl->size;
}
static tb_size_t tb_vector_iterator_next(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_vector_impl_t* impl = (tb_vector_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);
    tb_assert_and_check_return_val(itor < impl->size, impl->size);

    // next
    return itor + 1;
}
static tb_size_t tb_vector_iterator_prev(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_vector_impl_t* impl = (tb_vector_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);
    tb_assert_and_check_return_val(itor && itor <= impl->size, 0);

    // prev
    return itor - 1;
}
static tb_pointer_t tb_vector_iterator_item(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_vector_impl_t* impl = (tb_vector_impl_t*)iterator;
    tb_assert_and_check_return_val(impl && itor < impl->size, tb_null);
    
    // data
    return impl->func.data(&impl->func, impl->data + itor * iterator->step);
}
static tb_void_t tb_vector_iterator_copy(tb_iterator_ref_t iterator, tb_size_t itor, tb_cpointer_t item)
{
    // check
    tb_vector_impl_t* impl = (tb_vector_impl_t*)iterator;
    tb_assert_and_check_return(impl);

    // copy
    impl->func.copy(&impl->func, impl->data + itor * iterator->step, item);
}
static tb_long_t tb_vector_iterator_comp(tb_iterator_ref_t iterator, tb_cpointer_t ltem, tb_cpointer_t rtem)
{
    // check
    tb_vector_impl_t* impl = (tb_vector_impl_t*)iterator;
    tb_assert_and_check_return_val(impl && impl->func.comp, 0);

    // comp
    return impl->func.comp(&impl->func, ltem, rtem);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_vector_ref_t tb_vector_init(tb_size_t grow, tb_item_func_t func)
{
    // check
    tb_assert_and_check_return_val(grow, tb_null);
    tb_assert_and_check_return_val(func.size && func.data && func.dupl && func.repl && func.ndupl && func.nrepl, tb_null);

    // done
    tb_bool_t           ok = tb_false;
    tb_vector_impl_t*   impl = tb_null;
    do
    {
        // make impl
        impl = tb_malloc0_type(tb_vector_impl_t);
        tb_assert_and_check_break(impl);

        // init impl
        impl->size = 0;
        impl->grow = grow;
        impl->maxn = grow;
        impl->func = func;
        tb_assert_and_check_break(impl->maxn < TB_VECTOR_ITEM_MAXN);

        // init iterator
        impl->itor.mode = TB_ITERATOR_MODE_FORWARD | TB_ITERATOR_MODE_REVERSE | TB_ITERATOR_MODE_RACCESS;
        impl->itor.priv = tb_null;
        impl->itor.step = func.size;
        impl->itor.size = tb_vector_iterator_size;
        impl->itor.head = tb_vector_iterator_head;
        impl->itor.tail = tb_vector_iterator_tail;
        impl->itor.prev = tb_vector_iterator_prev;
        impl->itor.next = tb_vector_iterator_next;
        impl->itor.item = tb_vector_iterator_item;
        impl->itor.copy = tb_vector_iterator_copy;
        impl->itor.comp = tb_vector_iterator_comp;

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
        if (impl) tb_vector_exit((tb_vector_ref_t)impl);
        impl = tb_null;
    }

    // ok?
    return (tb_vector_ref_t)impl;
}
tb_void_t tb_vector_exit(tb_vector_ref_t vector)
{
    // check
    tb_vector_impl_t* impl = (tb_vector_impl_t*)vector;
    tb_assert_and_check_return(impl);

    // clear data
    tb_vector_clear(vector);

    // free data
    if (impl->data) tb_free(impl->data);
    impl->data = tb_null;

    // free it
    tb_free(impl);
}
tb_void_t tb_vector_clear(tb_vector_ref_t vector)
{
    // check
    tb_vector_impl_t* impl = (tb_vector_impl_t*)vector;
    tb_assert_and_check_return(impl);

    // free data
    if (impl->func.nfree)
        impl->func.nfree(&impl->func, impl->data, impl->size);

    // reset size 
    impl->size = 0;
}
tb_void_t tb_vector_copy(tb_vector_ref_t vector, tb_vector_ref_t hcopy)
{
    // check
    tb_vector_impl_t*       impl = (tb_vector_impl_t*)vector;
    tb_vector_impl_t const* copy = (tb_vector_impl_t const*)hcopy;
    tb_assert_and_check_return(impl && copy);

    // check func
    tb_assert_and_check_return(impl->func.type == copy->func.type);
    tb_assert_and_check_return(impl->func.size == copy->func.size);

    // check itor
    tb_assert_and_check_return(impl->itor.mode == copy->itor.mode);
    tb_assert_and_check_return(impl->itor.step == copy->itor.step);

    // null? clear it
    if (!copy->size) 
    {
        tb_vector_clear(vector);
        return ;
    }
    
    // resize if small
    if (impl->size < copy->size) tb_vector_resize(vector, copy->size);
    tb_assert_and_check_return(impl->data && copy->data && impl->size >= copy->size);

    // copy data
    if (copy->data != impl->data) tb_memcpy(impl->data, copy->data, copy->size * copy->func.size);

    // copy size
    impl->size = copy->size;
}
tb_pointer_t tb_vector_data(tb_vector_ref_t vector)
{
    // check
    tb_vector_impl_t* impl = (tb_vector_impl_t*)vector;
    tb_assert_and_check_return_val(impl, tb_null);

    // data
    return impl->data;
}
tb_pointer_t tb_vector_head(tb_vector_ref_t vector)
{
    return tb_iterator_item(vector, tb_iterator_head(vector));
}
tb_pointer_t tb_vector_last(tb_vector_ref_t vector)
{
    return tb_iterator_item(vector, tb_iterator_last(vector));
}
tb_size_t tb_vector_size(tb_vector_ref_t vector)
{
    // check
    tb_vector_impl_t const* impl = (tb_vector_impl_t const*)vector;
    tb_assert_and_check_return_val(impl, 0);

    // size
    return impl->size;
}
tb_size_t tb_vector_grow(tb_vector_ref_t vector)
{
    // check
    tb_vector_impl_t const* impl = (tb_vector_impl_t const*)vector;
    tb_assert_and_check_return_val(impl, 0);

    // grow
    return impl->grow;
}
tb_size_t tb_vector_maxn(tb_vector_ref_t vector)
{
    // check
    tb_vector_impl_t const* impl = (tb_vector_impl_t const*)vector;
    tb_assert_and_check_return_val(impl, 0);

    // maxn
    return impl->maxn;
}
tb_bool_t tb_vector_resize(tb_vector_ref_t vector, tb_size_t size)
{
    // check
    tb_vector_impl_t* impl = (tb_vector_impl_t*)vector;
    tb_assert_and_check_return_val(impl, tb_false);
    
    // free items if the impl is decreased
    if (size < impl->size)
    {
        // free data
        if (impl->func.nfree) 
            impl->func.nfree(&impl->func, impl->data + size * impl->func.size, impl->size - size);
    }

    // resize buffer
    if (size > impl->maxn)
    {
        tb_size_t maxn = tb_align4(size + impl->grow);
        tb_assert_and_check_return_val(maxn < TB_VECTOR_ITEM_MAXN, tb_false);

        // realloc data
        impl->data = (tb_byte_t*)tb_ralloc(impl->data, maxn * impl->func.size);
        tb_assert_and_check_return_val(impl->data, tb_false);

        // must be align by 4-bytes
        tb_assert_and_check_return_val(!(((tb_size_t)(impl->data)) & 3), tb_false);

        // clear the grow data
        tb_memset(impl->data + impl->size * impl->func.size, 0, (maxn - impl->maxn) * impl->func.size);

        // save maxn
        impl->maxn = maxn;
    }

    // update size
    impl->size = size;
    return tb_true;
}
tb_void_t tb_vector_insert_prev(tb_vector_ref_t vector, tb_size_t itor, tb_cpointer_t data)
{
    // check
    tb_vector_impl_t* impl = (tb_vector_impl_t*)vector;
    tb_assert_and_check_return(impl && impl->data && impl->func.size && itor <= impl->size);

    // save size
    tb_size_t osize = impl->size;

    // grow a item
    if (!tb_vector_resize(vector, osize + 1)) 
    {
        tb_trace_d("impl resize: %u => %u failed", osize, osize + 1);
        return ;
    }

    // move items if not at tail
    if (osize != itor) tb_memmov(impl->data + (itor + 1) * impl->func.size, impl->data + itor * impl->func.size, (osize - itor) * impl->func.size);

    // save data
    impl->func.dupl(&impl->func, impl->data + itor * impl->func.size, data);
}
tb_void_t tb_vector_insert_next(tb_vector_ref_t vector, tb_size_t itor, tb_cpointer_t data)
{
    tb_vector_insert_prev(vector, tb_iterator_next(vector, itor), data);
}
tb_void_t tb_vector_insert_head(tb_vector_ref_t vector, tb_cpointer_t data)
{
    tb_vector_insert_prev(vector, 0, data);
}
tb_void_t tb_vector_insert_tail(tb_vector_ref_t vector, tb_cpointer_t data)
{
    tb_vector_insert_prev(vector, tb_vector_size(vector), data);
}
tb_void_t tb_vector_ninsert_prev(tb_vector_ref_t vector, tb_size_t itor, tb_cpointer_t data, tb_size_t size)
{
    // check
    tb_vector_impl_t* impl = (tb_vector_impl_t*)vector;
    tb_assert_and_check_return(impl && impl->data && size && itor <= impl->size);

    // save size
    tb_size_t osize = impl->size;

    // grow size
    if (!tb_vector_resize(vector, osize + size)) 
    {
        tb_trace_d("impl resize: %u => %u failed", osize, osize + 1);
        return ;
    }

    // move items if not at tail
    if (osize != itor) tb_memmov(impl->data + (itor + size) * impl->func.size, impl->data + itor * impl->func.size, (osize - itor) * impl->func.size);

    // duplicate data
    impl->func.ndupl(&impl->func, impl->data + itor * impl->func.size, data, size);
}
tb_void_t tb_vector_ninsert_next(tb_vector_ref_t vector, tb_size_t itor, tb_cpointer_t data, tb_size_t size)
{
    tb_vector_ninsert_prev(vector, tb_iterator_next(vector, itor), data, size);
}
tb_void_t tb_vector_ninsert_head(tb_vector_ref_t vector, tb_cpointer_t data, tb_size_t size)
{
    tb_vector_ninsert_prev(vector, 0, data, size);
}
tb_void_t tb_vector_ninsert_tail(tb_vector_ref_t vector, tb_cpointer_t data, tb_size_t size)
{
    tb_vector_ninsert_prev(vector, tb_vector_size(vector), data, size);
}
tb_void_t tb_vector_replace(tb_vector_ref_t vector, tb_size_t itor, tb_cpointer_t data)
{
    // check
    tb_vector_impl_t* impl = (tb_vector_impl_t*)vector;
    tb_assert_and_check_return(impl && impl->data && itor <= impl->size);

    // replace data
    impl->func.repl(&impl->func, impl->data + itor * impl->func.size, data);
}
tb_void_t tb_vector_replace_head(tb_vector_ref_t vector, tb_cpointer_t data)
{
    tb_vector_replace(vector, 0, data);
}
tb_void_t tb_vector_replace_last(tb_vector_ref_t vector, tb_cpointer_t data)
{
    // check
    tb_vector_impl_t* impl = (tb_vector_impl_t*)vector;
    tb_assert_and_check_return(impl && impl->size);

    // replace
    tb_vector_replace(vector, impl->size - 1, data);
}
tb_void_t tb_vector_nreplace(tb_vector_ref_t vector, tb_size_t itor, tb_cpointer_t data, tb_size_t size)
{
    // check
    tb_vector_impl_t* impl = (tb_vector_impl_t*)vector;
    tb_assert_and_check_return(impl && impl->data && impl->size && itor <= impl->size && size);

    // strip size
    if (itor + size > impl->size) size = impl->size - itor;

    // replace data
    impl->func.nrepl(&impl->func, impl->data + itor * impl->func.size, data, size);
}
tb_void_t tb_vector_nreplace_head(tb_vector_ref_t vector, tb_cpointer_t data, tb_size_t size)
{
    tb_vector_nreplace(vector, 0, data, size);
}
tb_void_t tb_vector_nreplace_last(tb_vector_ref_t vector, tb_cpointer_t data, tb_size_t size)
{
    // check
    tb_vector_impl_t* impl = (tb_vector_impl_t*)vector;
    tb_assert_and_check_return(impl && impl->size && size);

    // replace
    tb_vector_nreplace(vector, size >= impl->size? 0 : impl->size - size, data, size);
}
tb_void_t tb_vector_remove(tb_vector_ref_t vector, tb_size_t itor)
{   
    // check
    tb_vector_impl_t* impl = (tb_vector_impl_t*)vector;
    tb_assert_and_check_return(impl && itor < impl->size);

    if (impl->size)
    {
        // do free
        if (impl->func.free) impl->func.free(&impl->func, impl->data + itor * impl->func.size);

        // move data if itor is not last
        if (itor < impl->size - 1) tb_memmov(impl->data + itor * impl->func.size, impl->data + (itor + 1) * impl->func.size, (impl->size - itor - 1) * impl->func.size);

        // resize
        impl->size--;
    }
}
tb_void_t tb_vector_remove_head(tb_vector_ref_t vector)
{
    tb_vector_remove(vector, 0);
}
tb_void_t tb_vector_remove_last(tb_vector_ref_t vector)
{
    // check
    tb_vector_impl_t* impl = (tb_vector_impl_t*)vector;
    tb_assert_and_check_return(impl);

    if (impl->size)
    {
        // do free
        if (impl->func.free) impl->func.free(&impl->func, impl->data + (impl->size - 1) * impl->func.size);

        // resize
        impl->size--;
    }
}
tb_void_t tb_vector_nremove(tb_vector_ref_t vector, tb_size_t itor, tb_size_t size)
{
    // check
    tb_vector_impl_t* impl = (tb_vector_impl_t*)vector;
    tb_assert_and_check_return(impl && size && itor < impl->size);

    // clear it
    if (!itor && size >= impl->size) 
    {
        tb_vector_clear(vector);
        return ;
    }
    
    // strip size
    if (itor + size > impl->size) size = impl->size - itor;

    // compute the left size
    tb_size_t left = impl->size - itor - size;

    // free data
    if (impl->func.nfree)
        impl->func.nfree(&impl->func, impl->data + itor * impl->func.size, size);

    // move the left data
    if (left)
    {
        tb_byte_t* pd = impl->data + itor * impl->func.size;
        tb_byte_t* ps = impl->data + (itor + size) * impl->func.size;
        tb_memmov(pd, ps, left * impl->func.size);
    }

    // update size
    impl->size -= size;
}
tb_void_t tb_vector_nremove_head(tb_vector_ref_t vector, tb_size_t size)
{
    // check
    tb_vector_impl_t* impl = (tb_vector_impl_t*)vector;
    tb_assert_and_check_return(impl && size);

    // clear it
    if (size >= impl->size)
    {
        tb_vector_clear(vector);
        return ;
    }

    // remove head
    tb_vector_nremove(vector, 0, size);
}
tb_void_t tb_vector_nremove_last(tb_vector_ref_t vector, tb_size_t size)
{   
    // check
    tb_vector_impl_t* impl = (tb_vector_impl_t*)vector;
    tb_assert_and_check_return(impl && size);

    // clear it
    if (size >= impl->size)
    {
        tb_vector_clear(vector);
        return ;
    }

    // remove last
    tb_vector_nremove(vector, impl->size - size, size);
}
tb_void_t tb_vector_walk(tb_vector_ref_t vector, tb_bool_t (*func)(tb_vector_ref_t impl, tb_pointer_t item, tb_bool_t* bdel, tb_cpointer_t priv), tb_cpointer_t priv)
{
    // check
    tb_vector_impl_t* impl = (tb_vector_impl_t*)vector;
    tb_assert_and_check_return(impl && impl->data && func);

    // step
    tb_size_t step = impl->func.size;
    tb_assert_and_check_return(step);

    // walk
    tb_size_t   i = 0;
    tb_size_t   b = -1;
    tb_size_t   n = impl->size;
    tb_byte_t*  d = impl->data;
    tb_bool_t   bdel = tb_false;
    tb_bool_t   stop = tb_false;
    for (i = 0; i < n; i++)
    {
        // item
        tb_pointer_t item = impl->func.data(&impl->func, d + i * step);

        // bdel
        bdel = tb_false;

        // callback: item
        if (!func(vector, item, &bdel, priv)) stop = tb_true;

        // free it?
        if (bdel)
        {
            // save
            if (b == -1) b = i;

            // free item
            if (impl->func.free) impl->func.free(&impl->func, d + i * step);
        }

        // remove items?
        if (!bdel || i + 1 == n || stop)
        {
            // has removed items?
            if (b != -1)
            {
                // the removed items end
                tb_size_t e = !bdel? i : i + 1;
                if (e > b)
                {
                    // the items number
                    tb_size_t m = e - b;
                    tb_assert(n >= m);
                    tb_trace_d("del: b: %u, e: %u, d: %u", b, e, bdel);

                    // remove items
                    if (e < n) tb_memmov(d + b * step, d + e * step, (n - e) * step);

                    // remove all?
                    if (n > m) 
                    {
                        // update the list size
                        n -= m;
                        impl->size = n;

                        // update i
                        i = b;
                    }
                    else
                    {
                        // update the list size
                        n = 0;
                        impl->size = 0;
                    }
                }
            }

            // reset
            b = -1;

            // stop?
            tb_check_goto(!stop, end);
        }
    }

end:
    return ;
}
