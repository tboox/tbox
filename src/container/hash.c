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
 * @file        hash.c
 * @ingroup     container
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "hash"
#define TB_TRACE_MODULE_DEBUG               (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "hash.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../memory/memory.h"
#include "../platform/platform.h"
#include "../algorithm/algorithm.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// index
#if TB_CPU_BIT64
#   define TB_HASH_INDEX_MAKE(buck, item)           (((tb_size_t)((item) & 0xffffffff) << 32) | ((buck) & 0xffffffff))
#   define TB_HASH_INDEX_BUCK(index)                ((index) & 0xffffffff)
#   define TB_HASH_INDEX_ITEM(index)                (((index) >> 32) & 0xffffffff)
#else
#   define TB_HASH_INDEX_MAKE(buck, item)           (((tb_size_t)((item) & 0xffff) << 16) | ((buck) & 0xffff))
#   define TB_HASH_INDEX_BUCK(index)                ((index) & 0xffff)
#   define TB_HASH_INDEX_ITEM(index)                (((index) >> 16) & 0xffff)
#endif

// the impl bulk default size
#ifdef __tb_small__
#   define TB_HASH_BULK_SIZE_DEFAULT                TB_HASH_BULK_SIZE_MICRO
#else
#   define TB_HASH_BULK_SIZE_DEFAULT                TB_HASH_BULK_SIZE_SMALL
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the impl item list type
typedef struct __tb_hash_item_list_t
{
    tb_size_t               size;
    tb_size_t               maxn;

}tb_hash_item_list_t;

/*!the impl type
 *
 * <pre>
 *                 0        1        3       ...     ...                n       n + 1
 * hash_list: |--------|--------|--------|--------|--------|--------|--------|--------|
 *                         |
 *                       -----    
 * item_list:           |     |       key:0                                      
 *                       -----   
 *                      |     |       key:1                                              
 *                       -----               <= insert by binary search algorithm
 *                      |     |       key:2                                               
 *                       -----  
 *                      |     |       key:3                                               
 *                       -----   
 *                      |     |       key:4                                               
 *                       -----  
 *                      |     |                                              
 *                       -----  
 *                      |     |                                              
 *                       -----  
 *                      |     |                                              
 *                       -----  
 *
 * </pre>
 *
 * @note the itor of the same item is mutable
 */
typedef struct __tb_hash_impl_t
{
    // the item itor
    tb_iterator_t           item_itor;

    /// the impl list
    tb_hash_item_list_t**   hash_list;

    /// the impl list size
    tb_size_t               hash_size;

    /// the item size
    tb_size_t               item_size;

    /// the item maxn
    tb_size_t               item_maxn;

    /// the item grow
    tb_size_t               item_grow;

    /// the impl item
    tb_hash_item_t          hash_item;

    /// the impl name func
    tb_item_func_t          name_func;

    /// the impl data func
    tb_item_func_t          data_func;

}tb_hash_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * finder
 */

#if 0
// linear finder
static tb_bool_t tb_hash_item_find(tb_hash_impl_t* impl, tb_cpointer_t name, tb_size_t* pbuck, tb_size_t* pitem)
{
    tb_assert_and_check_return_val(impl && impl->hash_list && impl->hash_size, tb_false);
    
    // get step
    tb_size_t step = impl->name_func.size + impl->data_func.size;
    tb_assert_and_check_return_val(step, tb_false);

    // comupte impl from name
    tb_size_t buck = impl->name_func.hash(&impl->name_func, name, impl->hash_size - 1, 0);
    tb_assert_and_check_return_val(buck < impl->hash_size, tb_false);

    // update bucket
    if (pbuck) *pbuck = buck;

    // get list
    tb_hash_item_list_t* list = impl->hash_list[buck];
    tb_check_return_val(list && list->size, tb_false);

    // find item
    tb_long_t   r = 1;
    tb_size_t   i = 0;
    tb_size_t   n = list->size;
    for (i = 0; i < n; i++)
    {
        // get item
        tb_byte_t const* item = ((tb_byte_t*)&list[1]) + i * step;

        // compare it
        r = impl->name_func.comp(&impl->name_func, name, impl->name_func.data(&impl->name_func, item));
        if (r <= 0) break;
    }

    // update item
    if (pitem) *pitem = i;

    // ok?
    return !r? tb_true : tb_false;
}
#else
// binary finder
static tb_bool_t tb_hash_item_find(tb_hash_impl_t* impl, tb_cpointer_t name, tb_size_t* pbuck, tb_size_t* pitem)
{
    // check
    tb_assert_and_check_return_val(impl && impl->hash_list && impl->hash_size, tb_false);
    
    // get step
    tb_size_t step = impl->name_func.size + impl->data_func.size;
    tb_assert_and_check_return_val(step, tb_false);

    // comupte impl from name
    tb_size_t buck = impl->name_func.hash(&impl->name_func, name, impl->hash_size - 1, 0);
    tb_assert_and_check_return_val(buck < impl->hash_size, tb_false);

    // update bucket
    if (pbuck) *pbuck = buck;

    // get list
    tb_hash_item_list_t* list = impl->hash_list[buck];
    tb_check_return_val(list && list->size, tb_false);

    // find item
    tb_long_t   t = 1;
    tb_size_t   l = 0;
    tb_size_t   r = list->size;
    tb_size_t   m = (l + r) >> 1;
    while (l < r)
    {
        // get item
        tb_byte_t const* item = ((tb_byte_t*)&list[1]) + m * step;

        // compare it
        t = impl->name_func.comp(&impl->name_func, name, impl->name_func.data(&impl->name_func, item));
        if (t < 0) r = m;
        else if (t > 0) l = m + 1;
        else break;
    
        // next
        m = (l + r) >> 1;
    }

    /* update item
     *
     * @note: m is not the prev not same item if not finded and list has repeat items
     * but this impl not exists repeat
     *
     * @see tb_binary_pfind()
     */
    if (pitem) *pitem = m;

    // ok?
    return !t? tb_true : tb_false;
}
#endif
static tb_bool_t tb_hash_item_at(tb_hash_impl_t* impl, tb_size_t buck, tb_size_t item, tb_pointer_t* pname, tb_pointer_t* pdata)
{
    tb_assert_and_check_return_val(impl && impl->hash_list && impl->hash_size && buck < impl->hash_size, tb_false);
    
    // get step
    tb_size_t step = impl->name_func.size + impl->data_func.size;
    tb_assert_and_check_return_val(step, tb_false);

    // get list
    tb_hash_item_list_t* list = impl->hash_list[buck];
    tb_check_return_val(list && list->size && item < list->size, tb_false);

    // get name
    if (pname) *pname = impl->name_func.data(&impl->name_func, ((tb_byte_t*)&list[1]) + item * step);
    
    // get data
    if (pdata) *pdata = impl->data_func.data(&impl->data_func, ((tb_byte_t*)&list[1]) + item * step + impl->name_func.size);

    // ok
    return tb_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * iterator
 */
static tb_size_t tb_hash_iterator_size(tb_iterator_t* iterator)
{
    // check
    tb_hash_impl_t* impl = (tb_hash_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);

    // the size
    return impl->item_size;
}
static tb_size_t tb_hash_iterator_head(tb_iterator_t* iterator)
{
    // check
    tb_hash_impl_t* impl = (tb_hash_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);

    // find the head
    tb_size_t i = 0;
    tb_size_t n = impl->hash_size;
    for (i = 0; i < n; i++)
    {
        tb_hash_item_list_t* list = impl->hash_list[i];
        if (list && list->size) return TB_HASH_INDEX_MAKE(i + 1, 1);
    }
    return 0;
}
static tb_size_t tb_hash_iterator_tail(tb_iterator_t* iterator)
{
    tb_hash_impl_t* impl = (tb_hash_impl_t*)iterator;
    tb_assert_and_check_return_val(impl, 0);

    return 0;
}
static tb_size_t tb_hash_iterator_next(tb_iterator_t* iterator, tb_size_t itor)
{
    tb_hash_impl_t* impl = (tb_hash_impl_t*)iterator;
    tb_assert_and_check_return_val(impl && impl->hash_list && impl->hash_size, 0);

    // buck & item
    tb_size_t buck = TB_HASH_INDEX_BUCK(itor);
    tb_size_t item = TB_HASH_INDEX_ITEM(itor);
    tb_assert_and_check_return_val(buck && item, 0);

    // the current bucket & item
    buck--;
    item--;
    tb_assert_and_check_return_val(buck < impl->hash_size && (item + 1) < 65536, 0);

    // find the next from the current bucket first
    if (impl->hash_list[buck] && item + 1 < impl->hash_list[buck]->size) return TB_HASH_INDEX_MAKE(buck + 1, item + 2);

    // find the next from the next buckets
    tb_size_t i;
    tb_size_t n = impl->hash_size;
    for (i = buck + 1; i < n; i++)
    {
        tb_hash_item_list_t* list = impl->hash_list[i];
        if (list && list->size) return TB_HASH_INDEX_MAKE(i + 1, 1);
    }
    return 0;
}
static tb_pointer_t tb_hash_iterator_item(tb_iterator_t* iterator, tb_size_t itor)
{
    tb_hash_impl_t* impl = (tb_hash_impl_t*)iterator;
    tb_assert_and_check_return_val(impl && itor, 0);

    // get buck & item
    tb_size_t buck = TB_HASH_INDEX_BUCK(itor);
    tb_size_t item = TB_HASH_INDEX_ITEM(itor);
    tb_assert_and_check_return_val(buck && item, tb_null);

    // get item
    if (tb_hash_item_at(impl, buck - 1, item - 1, &((tb_hash_impl_t*)impl)->hash_item.name, &((tb_hash_impl_t*)impl)->hash_item.data))
        return &(impl->hash_item);
    return tb_null;
}
static tb_void_t tb_hash_iterator_delt(tb_iterator_t* iterator, tb_size_t itor)
{
    // check
    tb_hash_impl_t* impl = (tb_hash_impl_t*)iterator;
    tb_assert_return(impl && impl->hash_list && impl->hash_size);
    
    // buck & item
    tb_size_t buck = TB_HASH_INDEX_BUCK(itor);
    tb_size_t item = TB_HASH_INDEX_ITEM(itor);
    tb_assert_and_check_return(buck && item); buck--; item--;
    tb_assert_and_check_return(buck < impl->hash_size);

    // the step
    tb_size_t step = impl->name_func.size + impl->data_func.size;
    tb_assert_and_check_return(step);

    // get list
    tb_hash_item_list_t* list = impl->hash_list[buck];
    tb_assert_and_check_return(list && list->size && item < list->size);

    // free item
    if (impl->name_func.free) impl->name_func.free(&impl->name_func, ((tb_byte_t*)&list[1]) + item * step);
    if (impl->data_func.free) impl->data_func.free(&impl->data_func, ((tb_byte_t*)&list[1]) + item * step + impl->name_func.size);

    // remove item from the list
    if (list->size > 1)
    {
        // move items
        if (item < list->size - 1) tb_memmov(((tb_byte_t*)&list[1]) + item * step, ((tb_byte_t*)&list[1]) + (item + 1) * step, (list->size - item - 1) * step);

        // update size
        list->size--;
    }
    // remove list
    else 
    {
        // free it
        tb_free(list);

        // reset
        impl->hash_list[buck] = tb_null;
    }

    // update the impl item size
    impl->item_size--;
}
static tb_void_t tb_hash_iterator_copy(tb_iterator_t* iterator, tb_size_t itor, tb_cpointer_t item)
{
    tb_hash_impl_t* impl = (tb_hash_impl_t*)iterator;
    tb_assert_return(impl && impl->hash_list && impl->hash_size);
    
    // buck & item
    tb_size_t b = TB_HASH_INDEX_BUCK(itor);
    tb_size_t i = TB_HASH_INDEX_ITEM(itor);
    tb_assert_return(b && i); b--; i--;
    tb_assert_return(b < impl->hash_size);

    // step
    tb_size_t step = impl->name_func.size + impl->data_func.size;
    tb_assert_and_check_return(step);

    // list
    tb_hash_item_list_t* list = impl->hash_list[b];
    tb_check_return(list && list->size && i < list->size);

    // note: copy data only, will destroy impl index if copy name
    impl->data_func.copy(&impl->data_func, ((tb_byte_t*)&list[1]) + i * step + impl->name_func.size, item);
}
static tb_long_t tb_hash_iterator_comp(tb_iterator_t* iterator, tb_cpointer_t ltem, tb_cpointer_t rtem)
{
    tb_hash_impl_t* impl = (tb_hash_impl_t*)iterator;
    tb_assert_and_check_return_val(impl && impl->name_func.comp && ltem && rtem, 0);
    
    return impl->name_func.comp(&impl->name_func, ((tb_hash_item_t*)ltem)->name, ((tb_hash_item_t*)rtem)->name);
}
/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_hash_t* tb_hash_init(tb_size_t bulk_size, tb_item_func_t name_func, tb_item_func_t data_func)
{
    // check
    tb_assert_and_check_return_val(name_func.size && name_func.hash && name_func.comp && name_func.data && name_func.dupl, tb_null);
    tb_assert_and_check_return_val(data_func.data && data_func.dupl && data_func.repl, tb_null);

    // check bulk size
    if (!bulk_size) bulk_size = TB_HASH_BULK_SIZE_DEFAULT;
    tb_assert_and_check_return_val(bulk_size <= TB_HASH_BULK_SIZE_LARGE, tb_null);

    // make impl
    tb_bool_t       ok = tb_false;
    tb_hash_impl_t* impl = tb_null;
    do
    {
        // make impl
        impl = tb_malloc0_type(tb_hash_impl_t);
        tb_assert_and_check_break(impl);

        // init impl func
        impl->name_func = name_func;
        impl->data_func = data_func;

        // init item itor
        impl->item_itor.mode = TB_ITERATOR_MODE_FORWARD;
        impl->item_itor.priv = tb_null;
        impl->item_itor.step = sizeof(tb_hash_item_t);
        impl->item_itor.size = tb_hash_iterator_size;
        impl->item_itor.head = tb_hash_iterator_head;
        impl->item_itor.tail = tb_hash_iterator_tail;
        impl->item_itor.prev = tb_null;
        impl->item_itor.next = tb_hash_iterator_next;
        impl->item_itor.item = tb_hash_iterator_item;
        impl->item_itor.copy = tb_hash_iterator_copy;
        impl->item_itor.delt = tb_hash_iterator_delt;
        impl->item_itor.comp = tb_hash_iterator_comp;

        // init impl size
        impl->hash_size = tb_align_pow2(bulk_size);
        tb_assert_and_check_break(impl->hash_size <= 65536);

        // init impl list
        impl->hash_list = (tb_hash_item_list_t**)tb_nalloc0(impl->hash_size, sizeof(tb_size_t));
        tb_assert_and_check_break(impl->hash_list);

        // init item grow
        impl->item_grow = tb_isqrti(bulk_size);
        if (impl->item_grow < 8) impl->item_grow = 8;
        impl->item_grow = tb_align_pow2(impl->item_grow);

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (impl) tb_hash_exit((tb_hash_t*)impl);
        impl = tb_null;
    }

    // ok?
    return (tb_hash_t*)impl;
}
tb_void_t tb_hash_exit(tb_hash_t* hash)
{
    // check
    tb_hash_impl_t* impl = (tb_hash_impl_t*)hash;
    tb_assert_and_check_return(impl);

    // clear it
    tb_hash_clear(hash);

    // free impl list
    if (impl->hash_list) tb_free(impl->hash_list);

    // free it
    tb_free(impl);
}
tb_void_t tb_hash_clear(tb_hash_t* hash)
{
    // check
    tb_hash_impl_t* impl = (tb_hash_impl_t*)hash;
    tb_assert_and_check_return(impl && impl->hash_list);

    // step
    tb_size_t step = impl->name_func.size + impl->data_func.size;
    tb_assert_and_check_return(step);

    // clear impl
    tb_size_t i = 0;
    tb_size_t n = impl->hash_size;
    for (i = 0; i < n; i++)
    {
        tb_hash_item_list_t* list = impl->hash_list[i];
        if (list)
        {
            // free items
            if (impl->name_func.free || impl->data_func.free)
            {
                tb_size_t j = 0;
                tb_size_t m = list->size;
                for (j = 0; j < m; j++)
                {
                    tb_byte_t* item = ((tb_byte_t*)&list[1]) + j * step;
                    if (impl->name_func.free) impl->name_func.free(&impl->name_func, item);
                    if (impl->data_func.free) impl->data_func.free(&impl->data_func, item + impl->name_func.size);
                }
            }

            // free list
            tb_free(list);
        }
        impl->hash_list[i] = tb_null;
    }

    // reset info
    impl->item_size = 0;
    impl->item_maxn = 0;
    tb_memset(&impl->hash_item, 0, sizeof(tb_hash_item_t));
}
tb_size_t tb_hash_itor(tb_hash_t const* hash, tb_cpointer_t name)
{
    // check
    tb_hash_impl_t* impl = (tb_hash_impl_t*)hash;
    tb_assert_and_check_return_val(impl, 0);

    // find
    tb_size_t buck = 0;
    tb_size_t item = 0;
    return tb_hash_item_find(impl, name, &buck, &item)? TB_HASH_INDEX_MAKE(buck + 1, item + 1) : 0;
}
tb_pointer_t tb_hash_get(tb_hash_t const* hash, tb_cpointer_t name)
{
    // check
    tb_hash_impl_t* impl = (tb_hash_impl_t*)hash;
    tb_assert_and_check_return_val(impl, tb_null);

    // find
    tb_size_t buck = 0;
    tb_size_t item = 0;
    if (tb_hash_item_find(impl, name, &buck, &item))
    {
        tb_pointer_t data = tb_null;
        if (tb_hash_item_at(impl, buck, item, tb_null, &data)) return (tb_pointer_t)data;
    }
    return tb_null;
}
tb_void_t tb_hash_del(tb_hash_t* hash, tb_cpointer_t name)
{
    // check
    tb_hash_impl_t* impl = (tb_hash_impl_t*)hash;
    tb_assert_and_check_return(impl);

    // find it
    tb_size_t buck = 0;
    tb_size_t item = 0;
    if (tb_hash_item_find(impl, name, &buck, &item))
        tb_hash_iterator_delt((tb_iterator_t*)impl, TB_HASH_INDEX_MAKE(buck + 1, item + 1));
}
tb_size_t tb_hash_set(tb_hash_t* hash, tb_cpointer_t name, tb_cpointer_t data)
{
    // check
    tb_hash_impl_t* impl = (tb_hash_impl_t*)hash;
    tb_assert_and_check_return_val(impl, 0);

    // the step
    tb_size_t step = impl->name_func.size + impl->data_func.size;
    tb_assert_and_check_return_val(step, 0);

    // find it
    tb_size_t buck = 0;
    tb_size_t item = 0;
    if (tb_hash_item_find(impl, name, &buck, &item))
    {
        // check
        tb_assert_and_check_return_val(buck < impl->hash_size, 0);

        // get list
        tb_hash_item_list_t* list = impl->hash_list[buck];
        tb_assert_and_check_return_val(list && list->size && item < list->size, 0);

        // replace data
        impl->data_func.repl(&impl->data_func, ((tb_byte_t*)&list[1]) + item * step + impl->name_func.size, data);
    }
    else
    {
        // check
        tb_assert_and_check_return_val(buck < impl->hash_size, 0);

        // get list
        tb_hash_item_list_t* list = impl->hash_list[buck];
        
        // insert item
        if (list)
        {
            // grow?
            if (list->size >= list->maxn)
            {
                // check
                tb_assert_and_check_return_val(impl->item_grow, 0);

                // resize maxn
                tb_size_t maxn = tb_align_pow2(list->maxn + impl->item_grow);
                tb_assert_and_check_return_val(maxn > list->maxn, 0);

                // realloc it
                list = (tb_hash_item_list_t*)tb_ralloc(list, sizeof(tb_hash_item_list_t) + maxn * step);  
                tb_assert_and_check_return_val(list, 0);

                // update the impl item maxn
                impl->item_maxn += maxn - list->maxn;

                // update maxn
                list->maxn = maxn;

                // reattach list
                impl->hash_list[buck] = list;
            }
            tb_assert_and_check_return_val(item <= list->size && list->size < list->maxn, 0);

            // move items
            if (item != list->size) tb_memmov(((tb_byte_t*)&list[1]) + (item + 1) * step, ((tb_byte_t*)&list[1]) + item * step, (list->size - item) * step);

            // dupl item
            list->size++;
            impl->name_func.dupl(&impl->name_func, ((tb_byte_t*)&list[1]) + item * step, name);
            impl->data_func.dupl(&impl->data_func, ((tb_byte_t*)&list[1]) + item * step + impl->name_func.size, data);

        }
        // create list for adding item
        else
        {
            // check
            tb_assert_and_check_return_val(impl->item_grow, 0);

            // make list
            list = (tb_hash_item_list_t*)tb_malloc0(sizeof(tb_hash_item_list_t) + impl->item_grow * step);
            tb_assert_and_check_return_val(list, 0);

            // init list
            list->size = 1;
            list->maxn = impl->item_grow;
            impl->name_func.dupl(&impl->name_func, ((tb_byte_t*)&list[1]), name);
            impl->data_func.dupl(&impl->data_func, ((tb_byte_t*)&list[1]) + impl->name_func.size, data);

            // attach list
            impl->hash_list[buck] = list;

            // update the impl item maxn
            impl->item_maxn += list->maxn;
        }

        // update the impl item size
        impl->item_size++;
    }

    // ok?
    return TB_HASH_INDEX_MAKE(buck + 1, item + 1);
}
tb_size_t tb_hash_size(tb_hash_t const* hash)
{
    // check
    tb_hash_impl_t const* impl = (tb_hash_impl_t const*)hash;
    tb_assert_and_check_return_val(impl, 0);

    // the size
    return impl->item_size;
}
tb_size_t tb_hash_maxn(tb_hash_t const* hash)
{
    // check
    tb_hash_impl_t const* impl = (tb_hash_impl_t const*)hash;
    tb_assert_and_check_return_val(impl, 0);

    // the maxn
    return impl->item_maxn;
}
tb_void_t tb_hash_walk(tb_hash_t* hash, tb_bool_t (*func)(tb_hash_t* impl, tb_hash_item_t* item, tb_bool_t* bdel, tb_cpointer_t priv), tb_cpointer_t priv)
{
    // check
    tb_hash_impl_t* impl = (tb_hash_impl_t*)hash;
    tb_assert_and_check_return(impl && impl->hash_list && func);

    // step
    tb_size_t step = impl->name_func.size + impl->data_func.size;
    tb_assert_and_check_return(step);

    // bdel
    tb_bool_t bdel = tb_false;

    // item
    tb_hash_item_t item;

    // walk
    tb_size_t i = 0;
    tb_size_t n = impl->hash_size;
    for (i = 0; i < n; i++)
    {
        tb_hash_item_list_t* list = impl->hash_list[i];
        if (list)
        {
            tb_size_t j = 0;
            tb_size_t b = -1;
            tb_size_t l = list->size;
            tb_bool_t stop = tb_false;
            for (j = 0; j < l; j++)
            {
                // init item
                tb_byte_t* it = ((tb_byte_t*)&list[1]) + j * step;
                item.name = impl->name_func.data(&impl->name_func, it);
                item.data = impl->data_func.data(&impl->data_func, it + impl->name_func.size);

                // init bdel
                bdel = tb_false;

                // callback: item
                if (!func(hash, &item, &bdel, priv)) stop = tb_true;

                // free it?
                if (bdel)
                {
                    // save
                    if (b == -1) b = j;

                    // free item
                    if (impl->name_func.free) impl->name_func.free(&impl->name_func, it);
                    if (impl->data_func.free) impl->data_func.free(&impl->data_func, it + impl->name_func.size);
                }

                // remove items?
                if (!bdel || j + 1 == l || stop)
                {
                    // has deleted items?
                    if (b != -1)
                    {
                        // the removed items end
                        tb_size_t e = !bdel? j : j + 1;
                        if (e > b)
                        {
                            // the items number
                            tb_size_t m = e - b;
                            tb_assert(l >= m);
//                          tb_trace_d("del: b: %u, e: %u, d: %u", b, e, bdel);

                            // remove items
                            if (e < l) tb_memmov(((tb_byte_t*)&list[1]) + b * step, ((tb_byte_t*)&list[1]) + e * step, (l - e) * step);

                            // remove all?
                            if (l > m) 
                            {
                                // update the list size
                                l -= m;
                                list->size = l;

                                // update j
                                j = b;
                            }
                            else
                            {
                                // update the list size
                                l = 0;

                                // free it
                                tb_free(list);

                                // reset
                                list = tb_null;
                                impl->hash_list[i] = tb_null;
                            }

                            // update the impl item size
                            impl->item_size -= m;
                        }
                    }

                    // reset
                    b = -1;

                    // stop?
                    tb_check_goto(!stop, end);
                }
            }
        }
    }

end:

    return ;
}
#ifdef __tb_debug__
tb_void_t tb_hash_dump(tb_hash_t const* hash)
{
    // check
    tb_hash_impl_t* impl = (tb_hash_impl_t*)hash;
    tb_assert_and_check_return(impl && impl->hash_list);

    // the step
    tb_size_t step = impl->name_func.size + impl->data_func.size;
    tb_assert_and_check_return(step);

    // trace
    tb_trace_i("=========================================================");
    tb_trace_i("hash_list: hash_size: %d, item_size: %u, item_maxn: %u", impl->hash_size, impl->item_size, impl->item_maxn);
    tb_trace_i("=========================================================");

#if 0
    // dump
    tb_size_t i = 0;
    tb_char_t name[4096];
    tb_char_t data[4096];
    for (i = 0; i < impl->hash_size; i++)
    {
        tb_hash_item_list_t* list = impl->hash_list[i];
        if (list)
        {
            tb_size_t j = 0;
            for (j = 0; j < list->size; j++)
            {
                tb_byte_t const* item = ((tb_byte_t*)&list[1]) + j * step;
                tb_pointer_t item_name = impl->name_func.data(&impl->name_func, item);
                tb_pointer_t item_data = impl->data_func.data(&impl->data_func, item + impl->name_func.size);

                if (impl->name_func.cstr && impl->data_func.cstr) 
                    tb_trace_i("bucket[%d:%d] => [%d]:\t%s\t\t=> %s", i, j
                        , impl->name_func.hash(&impl->name_func, item_name, impl->hash_size - 1, 0)
                        , impl->name_func.cstr(&impl->name_func, item_name, name, 4096)
                        , impl->data_func.cstr(&impl->data_func, item_data, data, 4096));
                else if (impl->name_func.cstr) 
                    tb_trace_i("bucket[%d:%d] => [%d]:\t%s\t\t=> %x", i, j
                        , impl->name_func.hash(&impl->name_func, item_name, impl->hash_size - 1, 0)
                        , impl->name_func.cstr(&impl->name_func, item_name, name, 4096)
                        , item_data);
                else if (impl->data_func.cstr) 
                    tb_trace_i("bucket[%d:%d] => [%d]:\t%x\t\t=> %x", i, j
                        , impl->name_func.hash(&impl->name_func, item_name, impl->hash_size - 1, 0)
                        , item_name
                        , impl->data_func.cstr(&impl->data_func, item_data, data, 4096));
                else tb_trace_i("bucket[%d:%d] => [%d]:\t%x\t\t=> %x", i, j
                        , impl->name_func.hash(&impl->name_func, item_name, impl->hash_size - 1, 0)
                        , item_name
                        , item_data);
            }

            tb_trace_i("bucket[%u]: size: %u, maxn: %u", i, list->size, list->maxn);
        }
    }

    tb_trace_i("");
    tb_for_all (tb_hash_item_t*, item, impl)
    {
        if (item)
        {
            if (impl->name_func.cstr && impl->data_func.cstr) 
                tb_trace_i("item[%d] => [%d]:\t%s\t\t=> %s", item_itor
                    , impl->name_func.hash(&impl->name_func, item->name, impl->hash_size - 1, 0)
                    , impl->name_func.cstr(&impl->name_func, item->name, name, 4096)
                    , impl->data_func.cstr(&impl->data_func, item->data, data, 4096));
            else if (impl->name_func.cstr) 
                tb_trace_i("item[%d] => [%d]:\t%s\t\t=> %x", item_itor
                    , impl->name_func.hash(&impl->name_func, item->name, impl->hash_size - 1, 0)
                    , impl->name_func.cstr(&impl->name_func, item->name, name, 4096)
                    , item->data);
            else if (impl->data_func.cstr) 
                tb_trace_i("item[%d] => [%d]:\t%x\t\t=> %x", item_itor
                    , impl->name_func.hash(&impl->name_func, item->name, impl->hash_size - 1, 0)
                    , item->name
                    , impl->data_func.cstr(&impl->data_func, item->data, data, 4096));
            else tb_trace_i("item[%d] => [%d]:\t%x\t\t=> %x", item_itor
                    , impl->name_func.hash(&impl->name_func, item->name, impl->hash_size - 1, 0)
                    , item->name
                    , item->data);
        }
    }
#else
    tb_size_t i = 0;
    for (i = 0; i < impl->hash_size; i++)
    {
        tb_hash_item_list_t* list = impl->hash_list[i];
        if (list)
        {
            tb_trace_i("bucket[%u]: size: %u, maxn: %u", i, list->size, list->maxn);
        }
    }

#endif
}
#endif
