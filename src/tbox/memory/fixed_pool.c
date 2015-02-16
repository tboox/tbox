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
 * @file        fixed_pool.c
 * @ingroup     memory
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "fixed_pool"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "fixed_pool.h"
#include "large_pool.h"
#include "impl/static_fixed_pool.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the item belong to this slot?
#define tb_fixed_pool_slot_exists(slot, item)               (((tb_byte_t*)(item) > (tb_byte_t*)(slot)) && ((tb_byte_t*)(item) < (tb_byte_t*)slot + (slot)->size))

// malloc for large pool
#define tb_large_pool_malloc_diff(pool, size, real, diff)   tb_large_pool_malloc_diff_(pool, size, real, diff __tb_debug_vals__)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the fixed pool slot type
typedef struct __tb_fixed_pool_slot_t
{
    // the size: sizeof(slot) + data
    tb_size_t                       size;

    // the pool
    tb_static_fixed_pool_ref_t      pool;

    // the list entry
    tb_list_entry_t                 entry;

}tb_fixed_pool_slot_t;

// the fixed pool impl type
typedef struct __tb_fixed_pool_impl_t
{
    // the large pool
    tb_large_pool_ref_t             large_pool;

    // the slot size
    tb_size_t                       slot_size;

    // the item size
    tb_size_t                       item_size;

    // the item count
    tb_size_t                       item_count;

    // the init func
    tb_fixed_pool_item_init_func_t  func_init;

    // the exit func
    tb_fixed_pool_item_exit_func_t  func_exit;

    // the private data
    tb_cpointer_t                   func_priv;

    // the current slot
    tb_fixed_pool_slot_t*           current_slot;

    // the partial slot
    tb_list_entry_head_t            partial_slots;

    // the full slot
    tb_list_entry_head_t            full_slots;

    // the slot list
    tb_fixed_pool_slot_t**          slot_list;

    // the slot count
    tb_size_t                       slot_count;

    // the slot space
    tb_size_t                       slot_space;

    // for small pool
    tb_bool_t                       for_small_pool;

}tb_fixed_pool_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */
__tb_extern_c__ tb_fixed_pool_ref_t tb_fixed_pool_init_(tb_large_pool_ref_t large_pool, tb_size_t slot_size, tb_size_t item_size, tb_bool_t for_small_pool, tb_fixed_pool_item_init_func_t item_init, tb_fixed_pool_item_exit_func_t item_exit, tb_cpointer_t priv);

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_bool_t tb_fixed_pool_item_exit(tb_pointer_t data, tb_cpointer_t priv)
{
    // check
    tb_fixed_pool_impl_t* impl = (tb_fixed_pool_impl_t*)priv;
    tb_assert_return_val(impl && impl->func_exit, tb_false);

    // done exit
    impl->func_exit(data, impl->func_priv);

    // continue
    return tb_true;
}
static tb_void_t tb_fixed_pool_slot_exit(tb_fixed_pool_impl_t* impl, tb_fixed_pool_slot_t* slot)
{
    // check
    tb_assert_and_check_return(impl && impl->large_pool && slot);
    tb_assert_and_check_return(impl->slot_list && impl->slot_count);

    // trace
    tb_trace_d("slot[%lu]: exit: size: %lu", impl->item_size, slot->size);

    // init the iterator
    tb_iterator_t iterator = tb_iterator_init_ptr((tb_pointer_t*)impl->slot_list, impl->slot_count);

    // find the slot from the slot list
    tb_size_t itor = tb_binary_find_all(&iterator, (tb_cpointer_t)slot);
    tb_assert_abort(itor != tb_iterator_tail(&iterator) && itor < impl->slot_count && impl->slot_list[itor]);
    tb_check_return(itor != tb_iterator_tail(&iterator) && itor < impl->slot_count && impl->slot_list[itor]);
    
    // remove the slot
    if (itor + 1 < impl->slot_count) tb_memmov_(impl->slot_list + itor, impl->slot_list + itor + 1, (impl->slot_count - itor - 1) * sizeof(tb_fixed_pool_slot_t*));

    // update the slot count
    impl->slot_count--;

    // exit slot
    tb_large_pool_free(impl->large_pool, slot);
}
static tb_fixed_pool_slot_t* tb_fixed_pool_slot_init(tb_fixed_pool_impl_t* impl)
{
    // check
    tb_assert_and_check_return_val(impl && impl->large_pool && impl->slot_size && impl->item_size, tb_null);

    // done
    tb_bool_t               ok = tb_false;
    tb_fixed_pool_slot_t*   slot = tb_null;
    do
    {
#ifdef __tb_debug__
        // init patch for checking underflow
        tb_size_t patch = 1;
#else
        tb_size_t patch = 0;
#endif

        // the item space
        tb_size_t item_space = sizeof(tb_pool_data_head_t) + impl->item_size + patch;

        // the need space
        tb_size_t need_space = sizeof(tb_fixed_pool_slot_t) + impl->slot_size * item_space;

        // make slot
        tb_size_t real_space = 0;
        slot = (tb_fixed_pool_slot_t*)tb_large_pool_malloc(impl->large_pool, need_space, &real_space);
        tb_assert_and_check_break(slot);
        tb_assert_and_check_break(real_space > sizeof(tb_fixed_pool_slot_t) + item_space);

        // init slot
        slot->size = real_space;
        slot->pool = tb_static_fixed_pool_init((tb_byte_t*)&slot[1], real_space - sizeof(tb_fixed_pool_slot_t), impl->item_size, impl->for_small_pool); 
        tb_assert_and_check_break(slot->pool);

        // no list?
        if (!impl->slot_list)
        {
            // init the slot list
            tb_size_t size = 0;
            impl->slot_list = (tb_fixed_pool_slot_t**)tb_large_pool_nalloc(impl->large_pool, 64, sizeof(tb_fixed_pool_slot_t*), &size);
            tb_assert_and_check_break(impl->slot_list && size);

            // init the slot count
            impl->slot_count = 0;

            // init the slot space
            impl->slot_space = size / sizeof(tb_fixed_pool_slot_t*);
            tb_assert_and_check_break(impl->slot_space);
        }
        // no enough space?
        else if (impl->slot_count == impl->slot_space)
        {
            // grow the slot list
            tb_size_t size = 0;
            impl->slot_list = (tb_fixed_pool_slot_t**)tb_large_pool_ralloc(impl->large_pool, impl->slot_list, (impl->slot_space << 1) * sizeof(tb_fixed_pool_slot_t*), &size);
            tb_assert_and_check_break(impl->slot_list && size);

            // update the slot space
            impl->slot_space = size / sizeof(tb_fixed_pool_slot_t*);
            tb_assert_and_check_break(impl->slot_space);
        }

        // check
        tb_assert_and_check_break(impl->slot_count < impl->slot_space);

        // insert the slot to the slot list in the increasing order
        tb_size_t i = 0;
        tb_size_t n = impl->slot_count;
        for (i = 0; i < n; i++) if (slot <= impl->slot_list[i]) break;
        if (i < n) tb_memmov_(impl->slot_list + i + 1, impl->slot_list + i, (n - i) * sizeof(tb_fixed_pool_slot_t*));
        impl->slot_list[i] = slot;

        // update the slot count
        impl->slot_count++;

        // trace
        tb_trace_d("slot[%lu]: init: size: %lu => %lu, item: %lu => %lu", impl->item_size, need_space, real_space, impl->slot_size, tb_static_fixed_pool_maxn(slot->pool));

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (slot) tb_fixed_pool_slot_exit(impl, slot);
        slot = tb_null;
    }

    // ok?
    return slot;
}
#if 0
static tb_fixed_pool_slot_t* tb_fixed_pool_slot_find(tb_fixed_pool_impl_t* impl, tb_pointer_t data)
{
    // check
    tb_assert_and_check_return_val(impl && data, tb_null);

    // done
    tb_fixed_pool_slot_t* slot = tb_null;
    do
    {
        // belong to the current slot?
        if (impl->current_slot && tb_fixed_pool_slot_exists(impl->current_slot, data))
        {
            slot = impl->current_slot;
            break;
        }
            
        // find the slot from the partial slots
        tb_for_all_if(tb_fixed_pool_slot_t*, partial_slot, tb_list_entry_itor(&impl->partial_slots), partial_slot)
        {
            // is this?
            if (tb_fixed_pool_slot_exists(partial_slot, data))
            {
                slot = partial_slot;
                break;
            }
        }
        
        // no found?
        tb_check_break(!slot);

        // find the slot from the full slots
        tb_for_all_if(tb_fixed_pool_slot_t*, full_slot, tb_list_entry_itor(&impl->full_slots), full_slot)
        {
            // is this?
            if (tb_fixed_pool_slot_exists(full_slot, data))
            {
                slot = full_slot;
                break;
            }
        }

    } while (0);

    // ok?
    return slot;
}
#else
static tb_long_t tb_fixed_pool_slot_comp(tb_iterator_ref_t iterator, tb_cpointer_t item, tb_cpointer_t data)
{
    // the slot
    tb_fixed_pool_slot_t* slot = (tb_fixed_pool_slot_t*)item;
    tb_assert_return_val(slot, -1);

    // comp
    return (tb_byte_t*)data < (tb_byte_t*)slot? 1 : ((tb_byte_t*)data >= (tb_byte_t*)slot + slot->size? -1 : 0);
}
static tb_fixed_pool_slot_t* tb_fixed_pool_slot_find(tb_fixed_pool_impl_t* impl, tb_pointer_t data)
{
    // check
    tb_assert_and_check_return_val(impl && data, tb_null);

    // init the iterator
    tb_iterator_t iterator = tb_iterator_init_ptr((tb_pointer_t*)impl->slot_list, impl->slot_count);

    // find it
    tb_size_t itor = tb_binary_find_all_if(&iterator, tb_fixed_pool_slot_comp, data);
    tb_check_return_val(itor != tb_iterator_tail(&iterator), tb_null);

    // the slot
    tb_fixed_pool_slot_t* slot = impl->slot_list[itor];
    tb_assert_and_check_return_val(slot, tb_null);

    // check
    tb_assert_abort(tb_fixed_pool_slot_exists(slot, data));

    // ok?
    return slot;
}
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_fixed_pool_ref_t tb_fixed_pool_init_(tb_large_pool_ref_t large_pool, tb_size_t slot_size, tb_size_t item_size, tb_bool_t for_small_pool, tb_fixed_pool_item_init_func_t item_init, tb_fixed_pool_item_exit_func_t item_exit, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(item_size, tb_null);

    // done
    tb_bool_t               ok = tb_false;
    tb_fixed_pool_impl_t*   impl = tb_null;
    do
    {
        // using the default large pool 
        if (!large_pool) large_pool = tb_large_pool();
        tb_assert_and_check_break(large_pool);

        // make pool
        impl = (tb_fixed_pool_impl_t*)tb_large_pool_malloc0(large_pool, sizeof(tb_fixed_pool_impl_t), tb_null);
        tb_assert_and_check_break(impl);

        // init pool
        impl->large_pool        = large_pool;
        impl->slot_size         = slot_size? slot_size : (tb_page_size() >> 4);
        impl->item_size         = item_size;
        impl->func_init         = item_init;
        impl->func_exit         = item_exit;
        impl->func_priv         = priv;
        impl->for_small_pool    = for_small_pool;
        tb_assert_and_check_break(impl->slot_size);

        // init partial slots
        tb_list_entry_init(&impl->partial_slots, tb_fixed_pool_slot_t, entry, tb_null);

        // init full slots
        tb_list_entry_init(&impl->full_slots, tb_fixed_pool_slot_t, entry, tb_null);

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (impl) tb_fixed_pool_exit((tb_fixed_pool_ref_t)impl);
        impl = tb_null;
    }

    // ok?
    return (tb_fixed_pool_ref_t)impl;
}
tb_fixed_pool_ref_t tb_fixed_pool_init(tb_large_pool_ref_t large_pool, tb_size_t slot_size, tb_size_t item_size, tb_fixed_pool_item_init_func_t item_init, tb_fixed_pool_item_exit_func_t item_exit, tb_cpointer_t priv)
{
    return tb_fixed_pool_init_(large_pool, slot_size, item_size, tb_false, item_init, item_exit, priv);
}
tb_void_t tb_fixed_pool_exit(tb_fixed_pool_ref_t pool)
{
    // check
    tb_fixed_pool_impl_t* impl = (tb_fixed_pool_impl_t*)pool;
    tb_assert_and_check_return(impl);

    // clear it
    tb_fixed_pool_clear(pool);

    // exit the slot list
    if (impl->slot_list) tb_large_pool_free(tb_large_pool(), impl->slot_list);
    impl->slot_list = tb_null;
    impl->slot_count = 0;
    impl->slot_space = 0;

    // exit it
    tb_large_pool_free(tb_large_pool(), impl);
}
tb_size_t tb_fixed_pool_size(tb_fixed_pool_ref_t pool)
{
    // check
    tb_fixed_pool_impl_t* impl = (tb_fixed_pool_impl_t*)pool;
    tb_assert_and_check_return_val(impl, 0);

    // the item count
    return impl->item_count;
}
tb_size_t tb_fixed_pool_item_size(tb_fixed_pool_ref_t pool)
{
    // check
    tb_fixed_pool_impl_t* impl = (tb_fixed_pool_impl_t*)pool;
    tb_assert_and_check_return_val(impl, 0);

    // the item size
    return impl->item_size;
}
tb_void_t tb_fixed_pool_clear(tb_fixed_pool_ref_t pool)
{
    // check
    tb_fixed_pool_impl_t* impl = (tb_fixed_pool_impl_t*)pool;
    tb_assert_and_check_return(impl);

    // exit items
    if (impl->func_exit) tb_fixed_pool_walk(pool, tb_fixed_pool_item_exit, (tb_pointer_t)impl);

    // exit the current slot first
    if (impl->current_slot) tb_fixed_pool_slot_exit(impl, impl->current_slot);
    impl->current_slot = tb_null;

    // exit the partial slots 
    tb_iterator_ref_t partial_iterator = tb_list_entry_itor(&impl->partial_slots);
    if (partial_iterator)
    {
        // walk it
        tb_size_t itor = tb_iterator_head(partial_iterator);
        while (itor != tb_iterator_tail(partial_iterator))
        {
            // the slot
            tb_fixed_pool_slot_t* slot = (tb_fixed_pool_slot_t*)tb_iterator_item(partial_iterator, itor);
            tb_assert_and_check_break(slot);

            // save next
            tb_size_t next = tb_iterator_next(partial_iterator, itor);

            // exit data
            tb_fixed_pool_slot_exit(impl, slot);

            // next
            itor = next;
        }
    }

    // exit the full slots 
    tb_iterator_ref_t full_iterator = tb_list_entry_itor(&impl->full_slots);
    if (full_iterator)
    {
        // walk it
        tb_size_t itor = tb_iterator_head(full_iterator);
        while (itor != tb_iterator_tail(full_iterator))
        {
            // the slot
            tb_fixed_pool_slot_t* slot = (tb_fixed_pool_slot_t*)tb_iterator_item(full_iterator, itor);
            tb_assert_and_check_break(slot);

            // save next
            tb_size_t next = tb_iterator_next(full_iterator, itor);

            // exit data
            tb_fixed_pool_slot_exit(impl, slot);

            // next
            itor = next;
        }
    }

    // clear item count
    impl->item_count = 0;

    // clear current slot
    impl->current_slot = tb_null;

    // clear partial slots
    tb_list_entry_clear(&impl->partial_slots);

    // clear full slots
    tb_list_entry_clear(&impl->full_slots);
}
tb_pointer_t tb_fixed_pool_malloc_(tb_fixed_pool_ref_t pool __tb_debug_decl__)
{
    // check
    tb_fixed_pool_impl_t* impl = (tb_fixed_pool_impl_t*)pool;
    tb_assert_and_check_return_val(impl, tb_null);

    // done
    tb_bool_t       ok = tb_false;
    tb_pointer_t    data = tb_null;
    do
    {
        // no current slot or the current slot is full? update the current slot
        if (!impl->current_slot || tb_static_fixed_pool_full(impl->current_slot->pool))
        {
            // move the current slot to the full slots if exists
            if (impl->current_slot) tb_list_entry_insert_tail(&impl->full_slots, &impl->current_slot->entry);

            // clear the current slot
            impl->current_slot = tb_null;

            // attempt to get a slot from the partial slots
            if (!tb_list_entry_is_null(&impl->partial_slots))
            {
                // the head entry
                tb_list_entry_ref_t entry = tb_list_entry_head(&impl->partial_slots);
                tb_assert_and_check_break(entry);

                // the head slot
                impl->current_slot = (tb_fixed_pool_slot_t*)tb_list_entry(&impl->partial_slots, entry);
                tb_assert_and_check_break(impl->current_slot);

                // remove this slot from the partial slots
                tb_list_entry_remove(&impl->partial_slots, entry);
            }
            // make a new slot
            else impl->current_slot = tb_fixed_pool_slot_init(impl);
        }

        // check
        tb_assert_and_check_break(impl->current_slot && impl->current_slot->pool);
        tb_assert_and_check_break(!tb_static_fixed_pool_full(impl->current_slot->pool));

        // make data from the current slot
        data = tb_static_fixed_pool_malloc(impl->current_slot->pool __tb_debug_args__);
        tb_assert_and_check_break(data);
        
        // done init
        if (impl->func_init && !impl->func_init(data, impl->func_priv)) break;

        // update the item count
        impl->item_count++;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit data
        if (data && impl->current_slot && impl->current_slot->pool) 
            tb_static_fixed_pool_free(impl->current_slot->pool, data __tb_debug_args__);
        data = tb_null;
    }

    // check
    tb_assertf_abort(data, "malloc(%lu) failed!", impl->item_size);

    // ok?
    return data;
}
tb_pointer_t tb_fixed_pool_malloc0_(tb_fixed_pool_ref_t pool __tb_debug_decl__)
{
    // check
    tb_fixed_pool_impl_t* impl = (tb_fixed_pool_impl_t*)pool;
    tb_assert_and_check_return_val(impl, tb_null);

    // done
    tb_pointer_t data = tb_fixed_pool_malloc_(pool __tb_debug_args__);
    tb_assert_and_check_return_val(data, tb_null);

    // clear it
    tb_memset_(data, 0, impl->item_size);

    // ok
    return data;
}
tb_bool_t tb_fixed_pool_free_(tb_fixed_pool_ref_t pool, tb_pointer_t data __tb_debug_decl__)
{ 
    // check
    tb_fixed_pool_impl_t* impl = (tb_fixed_pool_impl_t*)pool;
    tb_assert_and_check_return_val(impl, tb_false);

    // done
    tb_bool_t ok = tb_false;
    do
    {
        // check
        tb_assertf_and_check_break(impl->item_count, "double free data: %p", data);

        // find the slot 
        tb_fixed_pool_slot_t* slot = tb_fixed_pool_slot_find(impl, data);
        tb_assertf_and_check_break(slot, "the data: %p not belong to pool: %p", data, pool);
        tb_assert_and_check_break(slot->pool);

        // the slot is full?
        tb_bool_t full = tb_static_fixed_pool_full(slot->pool);

        // done exit
        if (impl->func_exit) impl->func_exit(data, impl->func_priv);

        // free it
        if (!tb_static_fixed_pool_free(slot->pool, data __tb_debug_args__)) break;

        // not the current slot?
        if (slot != impl->current_slot)
        {
            // is full? move the slot to the partial slots
            if (full)
            {
                tb_list_entry_remove(&impl->full_slots, &slot->entry);
                tb_list_entry_insert_tail(&impl->partial_slots, &slot->entry);
            }
            // is null? exit the slot
            else if (tb_static_fixed_pool_null(slot->pool))
            {
                tb_list_entry_remove(&impl->partial_slots, &slot->entry);
                tb_fixed_pool_slot_exit(impl, slot);
            }
        }

        // update the item count
        impl->item_count--;
 
        // ok
        ok = tb_true;

    } while (0);

    // failed? dump it
#ifdef __tb_debug__
    if (!ok) 
    {
        // trace
        tb_trace_e("free(%p) failed! at %s(): %lu, %s", data, func_, line_, file_);

        // dump data
        tb_pool_data_dump((tb_byte_t const*)data, tb_true, "[fixed_pool]: [error]: ");

        // abort
        tb_abort();
    }
#endif

    // ok?
    return ok;
}
tb_void_t tb_fixed_pool_walk(tb_fixed_pool_ref_t pool, tb_fixed_pool_item_walk_func_t func, tb_cpointer_t priv)
{
    // check
    tb_fixed_pool_impl_t* impl = (tb_fixed_pool_impl_t*)pool;
    tb_assert_and_check_return(impl && func);

    // walk the current slot first
    if (impl->current_slot && impl->current_slot->pool)
        tb_static_fixed_pool_walk(impl->current_slot->pool, func, priv);

    // walk the partial slots
    tb_for_all_if(tb_fixed_pool_slot_t*, partial_slot, tb_list_entry_itor(&impl->partial_slots), partial_slot && partial_slot->pool)
    {
        // check
        tb_assert_abort(!tb_static_fixed_pool_full(partial_slot->pool));

        // walk
        tb_static_fixed_pool_walk(partial_slot->pool, func, priv);
    }

    // walk the full slots
    tb_for_all_if(tb_fixed_pool_slot_t*, full_slot, tb_list_entry_itor(&impl->full_slots), full_slot && full_slot->pool)
    {
        // check
        tb_assert_abort(tb_static_fixed_pool_full(full_slot->pool));

        // walk
        tb_static_fixed_pool_walk(full_slot->pool, func, priv);
    }
}
#ifdef __tb_debug__
tb_void_t tb_fixed_pool_dump(tb_fixed_pool_ref_t pool)
{ 
    // check
    tb_fixed_pool_impl_t* impl = (tb_fixed_pool_impl_t*)pool;
    tb_assert_and_check_return(impl);

    // dump the current slot first
    if (impl->current_slot && impl->current_slot->pool)
        tb_static_fixed_pool_dump(impl->current_slot->pool);

    // dump the partial slots
    tb_for_all_if(tb_fixed_pool_slot_t*, partial_slot, tb_list_entry_itor(&impl->partial_slots), partial_slot && partial_slot->pool)
    {
        // check
        tb_assert_abort(!tb_static_fixed_pool_full(partial_slot->pool));

        // dump
        tb_static_fixed_pool_dump(partial_slot->pool);
    }

    // dump the full slots
    tb_for_all_if(tb_fixed_pool_slot_t*, full_slot, tb_list_entry_itor(&impl->full_slots), full_slot && full_slot->pool)
    {
        // check
        tb_assert_abort(tb_static_fixed_pool_full(full_slot->pool));

        // dump
        tb_static_fixed_pool_dump(full_slot->pool);
    }
}
#endif
