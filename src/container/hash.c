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
#include "../stream/stream.h"
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

// the hash bulk default size
#ifdef __tb_small__
#   define TB_HASH_BULK_SIZE_DEFAULT                TB_HASH_BULK_SIZE_MICRO
#else
#   define TB_HASH_BULK_SIZE_DEFAULT                TB_HASH_BULK_SIZE_SMALL
#endif

// the hash bulk maximum size
#define TB_HASH_BULK_MAXN                           (1 << 16)

// the hash bulk item maximum size
#define TB_HASH_BULK_ITEM_MAXN                      (1 << 16)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the hash impl item list type
typedef struct __tb_hash_item_list_t
{
    tb_size_t               size;
    tb_size_t               maxn;

}tb_hash_item_list_t;

// the hash impl type
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
static tb_size_t tb_hash_itor_size(tb_iterator_ref_t iterator)
{
    // check
    tb_hash_impl_t* impl = (tb_hash_impl_t*)iterator;
    tb_assert_return_val(impl, 0);

    // the size
    return impl->item_size;
}
static tb_size_t tb_hash_itor_head(tb_iterator_ref_t iterator)
{
    // check
    tb_hash_impl_t* impl = (tb_hash_impl_t*)iterator;
    tb_assert_return_val(impl, 0);

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
static tb_size_t tb_hash_itor_tail(tb_iterator_ref_t iterator)
{
    return 0;
}
static tb_size_t tb_hash_itor_next(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_hash_impl_t* impl = (tb_hash_impl_t*)iterator;
    tb_assert_return_val(impl && impl->hash_list && impl->hash_size, 0);

    // the current bucket and item
    tb_size_t buck = TB_HASH_INDEX_BUCK(itor);
    tb_size_t item = TB_HASH_INDEX_ITEM(itor);
    tb_assert_return_val(buck && item, 0);

    // compute index
    buck--;
    item--;
    tb_assert_return_val(buck < impl->hash_size && (item + 1) < TB_HASH_BULK_ITEM_MAXN, 0);

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

    // tail
    return 0;
}
static tb_pointer_t tb_hash_itor_item(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_hash_impl_t* impl = (tb_hash_impl_t*)iterator;
    tb_assert_return_val(impl && itor, 0);

    // get the buck and item
    tb_size_t buck = TB_HASH_INDEX_BUCK(itor);
    tb_size_t item = TB_HASH_INDEX_ITEM(itor);
    tb_assert_and_check_return_val(buck && item, tb_null);

    // get item
    if (tb_hash_item_at(impl, buck - 1, item - 1, &((tb_hash_impl_t*)impl)->hash_item.name, &((tb_hash_impl_t*)impl)->hash_item.data))
        return &(impl->hash_item);
    return tb_null;
}
static tb_void_t tb_hash_itor_copy(tb_iterator_ref_t iterator, tb_size_t itor, tb_cpointer_t item)
{
    // check
    tb_hash_impl_t* impl = (tb_hash_impl_t*)iterator;
    tb_assert_return(impl && impl->hash_list && impl->hash_size);
    
    // the buck and item
    tb_size_t b = TB_HASH_INDEX_BUCK(itor);
    tb_size_t i = TB_HASH_INDEX_ITEM(itor);
    tb_assert_return(b && i); b--; i--;
    tb_assert_return(b < impl->hash_size);

    // step
    tb_size_t step = impl->name_func.size + impl->data_func.size;
    tb_assert_return(step);

    // list
    tb_hash_item_list_t* list = impl->hash_list[b];
    tb_check_return(list && list->size && i < list->size);

    // note: copy data only, will destroy impl index if copy name
    impl->data_func.copy(&impl->data_func, ((tb_byte_t*)&list[1]) + i * step + impl->name_func.size, item);
}
static tb_long_t tb_hash_itor_comp(tb_iterator_ref_t iterator, tb_cpointer_t ltem, tb_cpointer_t rtem)
{
    // check
    tb_hash_impl_t* impl = (tb_hash_impl_t*)iterator;
    tb_assert_return_val(impl && impl->name_func.comp && ltem && rtem, 0);
    
    // done
    return impl->name_func.comp(&impl->name_func, ((tb_hash_item_t*)ltem)->name, ((tb_hash_item_t*)rtem)->name);
}
static tb_void_t tb_hash_itor_remove(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_hash_impl_t* impl = (tb_hash_impl_t*)iterator;
    tb_assert_return(impl && impl->hash_list && impl->hash_size);
    
    // buck & item
    tb_size_t buck = TB_HASH_INDEX_BUCK(itor);
    tb_size_t item = TB_HASH_INDEX_ITEM(itor);
    tb_assert_return(buck && item); buck--; item--;
    tb_assert_return(buck < impl->hash_size);

    // the step
    tb_size_t step = impl->name_func.size + impl->data_func.size;
    tb_assert_return(step);

    // get list
    tb_hash_item_list_t* list = impl->hash_list[buck];
    tb_assert_return(list && list->size && item < list->size);

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
static tb_void_t tb_hash_itor_remove_range(tb_iterator_ref_t iterator, tb_size_t prev, tb_size_t next, tb_size_t size)
{
    // check
    tb_hash_impl_t* impl = (tb_hash_impl_t*)iterator;
    tb_assert_return(impl && impl->hash_list && impl->hash_size);

    // no size
    tb_check_return(size);

    // the step
    tb_size_t step = impl->name_func.size + impl->data_func.size;
    tb_assert_return(step);

    // the first itor
    tb_size_t itor = prev? tb_hash_itor_next(iterator, prev) : tb_hash_itor_head(iterator);

    // the head buck and item
    tb_size_t buck_head = TB_HASH_INDEX_BUCK(itor);
    tb_size_t item_head = TB_HASH_INDEX_ITEM(itor);
    tb_assert_return(buck_head && item_head);

    // compute index
    buck_head--;
    item_head--;
    tb_assert_return(buck_head < impl->hash_size && item_head < TB_HASH_BULK_ITEM_MAXN);

    // the last buck and the tail item
    tb_size_t buck_last;
    tb_size_t item_tail;
    if (next)
    {
        // next => buck and item
        buck_last = TB_HASH_INDEX_BUCK(next);
        item_tail = TB_HASH_INDEX_ITEM(next);
        tb_assert_return(buck_last && item_tail);

        // compute index
        buck_last--;
        item_tail--;
        tb_assert_return(buck_last < impl->hash_size && item_tail < TB_HASH_BULK_ITEM_MAXN);
    }
    else 
    {
        buck_last = impl->hash_size - 1;
        item_tail = -1;
    }

    // remove items: [itor, next)
    tb_size_t buck;
    tb_size_t item;
    tb_item_func_free_t name_free = impl->name_func.free;
    tb_item_func_free_t data_free = impl->data_func.free;
    for (buck = buck_head, item = item_head; buck <= buck_last; buck++, item = 0)
    {
        // the list
        tb_hash_item_list_t* list = impl->hash_list[buck];
        tb_check_continue(list && list->size);

        // the tail
        tb_size_t tail = (buck == buck_last && next)? item_tail : list->size;
        tb_assert_abort(tail != -1);
        tb_check_continue(item < tail);

        // the data
        tb_byte_t* data = (tb_byte_t*)&list[1];

        // free items
        tb_size_t i = 0;
        for (i = item; i < tail; i++)
        {
            if (name_free) name_free(&impl->name_func, data + i * step);
            if (data_free) data_free(&impl->data_func, data + i * step + impl->name_func.size);
        }

        // move items
        if (buck == buck_last && tail < list->size) tb_memmov(data + item * step, data + tail * step, (list->size - tail) * step);

        // update the list size
        list->size -= tail - item;

        // update the item size
        impl->item_size -= tail - item;
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_hash_ref_t tb_hash_init(tb_size_t bulk_size, tb_item_func_t name_func, tb_item_func_t data_func)
{
    // check
    tb_assert_and_check_return_val(name_func.size && name_func.hash && name_func.comp && name_func.data && name_func.dupl, tb_null);
    tb_assert_and_check_return_val(data_func.data && data_func.dupl && data_func.repl, tb_null);

    // check bulk size
    if (!bulk_size) bulk_size = TB_HASH_BULK_SIZE_DEFAULT;
    tb_assert_and_check_return_val(bulk_size <= TB_HASH_BULK_SIZE_LARGE, tb_null);

    // done
    tb_bool_t       ok = tb_false;
    tb_hash_impl_t* impl = tb_null;
    do
    {
        // make hash
        impl = tb_malloc0_type(tb_hash_impl_t);
        tb_assert_and_check_break(impl);

        // init hash func
        impl->name_func = name_func;
        impl->data_func = data_func;

        // init item itor
        impl->item_itor.mode            = TB_ITERATOR_MODE_FORWARD | TB_ITERATOR_MODE_MUTABLE;
        impl->item_itor.priv            = tb_null;
        impl->item_itor.step            = sizeof(tb_hash_item_t);
        impl->item_itor.size            = tb_hash_itor_size;
        impl->item_itor.head            = tb_hash_itor_head;
        impl->item_itor.tail            = tb_hash_itor_tail;
        impl->item_itor.prev            = tb_null;
        impl->item_itor.next            = tb_hash_itor_next;
        impl->item_itor.item            = tb_hash_itor_item;
        impl->item_itor.copy            = tb_hash_itor_copy;
        impl->item_itor.comp            = tb_hash_itor_comp;
        impl->item_itor.remove          = tb_hash_itor_remove;
        impl->item_itor.remove_range    = tb_hash_itor_remove_range;

        // init hash size
        impl->hash_size = tb_align_pow2(bulk_size);
        tb_assert_and_check_break(impl->hash_size <= TB_HASH_BULK_MAXN);

        // init hash list
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
        if (impl) tb_hash_exit((tb_hash_ref_t)impl);
        impl = tb_null;
    }

    // ok?
    return (tb_hash_ref_t)impl;
}
tb_void_t tb_hash_exit(tb_hash_ref_t hash)
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
tb_void_t tb_hash_clear(tb_hash_ref_t hash)
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
tb_size_t tb_hash_itor(tb_hash_ref_t hash, tb_cpointer_t name)
{
    // check
    tb_hash_impl_t* impl = (tb_hash_impl_t*)hash;
    tb_assert_and_check_return_val(impl, 0);

    // find
    tb_size_t buck = 0;
    tb_size_t item = 0;
    return tb_hash_item_find(impl, name, &buck, &item)? TB_HASH_INDEX_MAKE(buck + 1, item + 1) : 0;
}
tb_pointer_t tb_hash_get(tb_hash_ref_t hash, tb_cpointer_t name)
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
tb_void_t tb_hash_del(tb_hash_ref_t hash, tb_cpointer_t name)
{
    // check
    tb_hash_impl_t* impl = (tb_hash_impl_t*)hash;
    tb_assert_and_check_return(impl);

    // find it
    tb_size_t buck = 0;
    tb_size_t item = 0;
    if (tb_hash_item_find(impl, name, &buck, &item))
        tb_hash_itor_remove((tb_iterator_ref_t)impl, TB_HASH_INDEX_MAKE(buck + 1, item + 1));
}
tb_size_t tb_hash_set(tb_hash_ref_t hash, tb_cpointer_t name, tb_cpointer_t data)
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
tb_size_t tb_hash_size(tb_hash_ref_t hash)
{
    // check
    tb_hash_impl_t const* impl = (tb_hash_impl_t const*)hash;
    tb_assert_and_check_return_val(impl, 0);

    // the size
    return impl->item_size;
}
tb_size_t tb_hash_maxn(tb_hash_ref_t hash)
{
    // check
    tb_hash_impl_t const* impl = (tb_hash_impl_t const*)hash;
    tb_assert_and_check_return_val(impl, 0);

    // the maxn
    return impl->item_maxn;
}
tb_bool_t tb_hash_load(tb_hash_ref_t hash, tb_stream_ref_t stream)
{
    // check
    tb_hash_impl_t* impl = (tb_hash_impl_t*)hash;
    tb_assert_and_check_return_val(impl && stream, tb_false);
    tb_assert_and_check_return_val(impl->name_func.load && impl->data_func.load, tb_false);
    tb_assert_and_check_return_val(impl->name_func.free && impl->data_func.free, tb_false);

    // clear the hash first
    tb_hash_clear(hash);
  
    // the offset
    tb_hize_t offset = tb_stream_offset(stream);

    // done
    tb_bool_t       ok = tb_false;
    tb_pointer_t    name_buff = tb_null;
    tb_pointer_t    data_buff = tb_null;
    do
    {
        // load type
        tb_uint32_t type = tb_stream_bread_u32_be(stream);
        tb_assert_and_check_break(type == 'hash');

        // load item type 
        tb_uint16_t name_type = tb_stream_bread_u16_be(stream);
        tb_uint16_t data_type = tb_stream_bread_u16_be(stream);
        tb_assert_and_check_break(name_type == impl->name_func.type);
        tb_assert_and_check_break(data_type == impl->data_func.type);

        // load item size 
        tb_uint16_t name_size = tb_stream_bread_u16_be(stream);
        tb_uint16_t data_size = tb_stream_bread_u16_be(stream);
        tb_assert_and_check_break(name_size == impl->name_func.size);
        tb_assert_and_check_break(data_size == impl->data_func.size);

        // make name buffer
        name_buff = impl->name_func.size? tb_malloc(impl->name_func.size) : tb_null;

        // make data buffer
        data_buff = impl->data_func.size? tb_malloc(impl->data_func.size) : tb_null;

        // load size
        tb_uint32_t size = tb_stream_bread_u32_be(stream);

        // load hash
        tb_uint32_t load = 0;
        for (load = 0; load < size; load++)
        {
            // load name
            if (!impl->name_func.load(&impl->name_func, name_buff, stream)) break;

            // load data
            if (!impl->data_func.load(&impl->data_func, data_buff, stream)) break;

            // save name and data
            tb_hash_set(hash, impl->name_func.data(&impl->name_func, name_buff), impl->data_func.data(&impl->data_func, data_buff));

            // free name
            impl->name_func.free(&impl->name_func, name_buff);

            // free data
            impl->data_func.free(&impl->data_func, data_buff);
        }

        // ok
        ok = tb_true;

    } while (0);

    // failed? 
    if (!ok) 
    {
        // restore it
        tb_stream_seek(stream, offset);

        // clear it
        tb_hash_clear(hash);
    }

    // exit name buffer
    if (name_buff) tb_free(name_buff);
    name_buff = tb_null;

    // exit data buffer
    if (data_buff) tb_free(data_buff);
    data_buff = tb_null;

    // ok?
    return ok;
}
tb_bool_t tb_hash_save(tb_hash_ref_t hash, tb_stream_ref_t stream)
{
    // check
    tb_hash_impl_t* impl = (tb_hash_impl_t*)hash;
    tb_assert_and_check_return_val(impl && stream, tb_false);
    tb_assert_and_check_return_val(impl->name_func.save && impl->data_func.save, tb_false);
    
    // the offset
    tb_hize_t offset = tb_stream_offset(stream);

    // done
    tb_bool_t ok = tb_false;
    do
    {
        // the size
        tb_uint32_t size = (tb_uint32_t)tb_hash_size(hash);

        // save type
        if (!tb_stream_bwrit_u32_be(stream, 'hash')) break;
       
        // save item type
        if (!tb_stream_bwrit_u16_be(stream, impl->name_func.type)) break;
        if (!tb_stream_bwrit_u16_be(stream, impl->data_func.type)) break;
        
        // save item size
        if (!tb_stream_bwrit_u16_be(stream, impl->name_func.size)) break;
        if (!tb_stream_bwrit_u16_be(stream, impl->data_func.size)) break;
        
        // save size
        if (!tb_stream_bwrit_u32_be(stream, size)) break;
        
        // save hash
        tb_uint32_t save = 0;
        tb_for_all_if (tb_hash_item_t*, item, hash, item)
        {
            // save name
            if (!impl->name_func.save(&impl->name_func, item->name, stream)) break;

            // save data
            if (!impl->data_func.save(&impl->data_func, item->data, stream)) break;

            // update the save count
            save++;
        }

        // check
        tb_assert_and_check_break(save == size); 

        // ok
        ok = tb_true;

    } while (0);

    // failed? restore it
    if (!ok) tb_stream_seek(stream, offset);

    // ok?
    return ok;
}
#ifdef __tb_debug__
tb_void_t tb_hash_dump(tb_hash_ref_t hash)
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
