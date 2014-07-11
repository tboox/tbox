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
 * @file        static_fixed_pool_old.c
 * @ingroup     memory
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "static_fixed_pool_old.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the magic number
#define TB_STATIC_FIXED_POOL_OLD_MAGIC                          (0xdeaf)

// the align maxn
#define TB_STATIC_FIXED_POOL_OLD_ALIGN_MAXN                     (128)

// the used sets
#define tb_static_fixed_pool_old_used_set1(used, i)             do {(used)[(i) >> 3] |= (0x1 << ((i) & 7));} while (0)
#define tb_static_fixed_pool_old_used_set0(used, i)             do {(used)[(i) >> 3] &= ~(0x1 << ((i) & 7));} while (0)
#define tb_static_fixed_pool_old_used_bset(used, i)             ((used)[(i) >> 3] & (0x1 << ((i) & 7)))

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

#ifdef __tb_debug__
// the static fixed pool info type
typedef struct __tb_static_fixed_pool_old_info_t
{
    // the peak size
    tb_size_t                   peak;

    // the fail count
    tb_size_t                   fail;

    // the pred count
    tb_size_t                   pred;

    // the aloc count
    tb_size_t                   aloc;

}tb_static_fixed_pool_old_info_t;
#endif

// the static fixed pool impl type
typedef struct __tb_static_fixed_pool_old_impl_t
{
    // the magic 
    tb_uint16_t                 magic;

    // the align
    tb_uint16_t                 align;

    // the step
    tb_size_t                   step;

    // the maxn
    tb_size_t                   maxn;

    // the size
    tb_size_t                   size;

    // the data
    tb_byte_t*                  data;

    // the used
    tb_byte_t*                  used;

    // the pred
    tb_byte_t*                  pred;

#ifdef __tb_debug__
    // the info
    tb_static_fixed_pool_old_info_t info;
#endif

}tb_static_fixed_pool_old_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_pointer_t tb_static_fixed_pool_old_malloc_pred(tb_static_fixed_pool_old_impl_t* impl)
{
    // init data
    tb_pointer_t data = tb_null;

    // has pred?
    if (impl->pred)
    {
        // init index
        tb_size_t i = (impl->pred - impl->data) / impl->step;
        tb_assert_and_check_return_val(!((impl->pred - impl->data) % impl->step), tb_null);

        // is free?
        if (!tb_static_fixed_pool_old_used_bset(impl->used, i)) 
        {
            // ok
            data = impl->pred;
            tb_static_fixed_pool_old_used_set1(impl->used, i);

            // predict the next block
            if (i + 1 < impl->maxn && !tb_static_fixed_pool_old_used_bset(impl->used, i + 1))
                impl->pred = impl->data + (i + 1) * impl->step;

#ifdef __tb_debug__
            // pred++
            impl->info.pred++;
#endif
        }
    }

    // ok?
    return data;
}

#if 1
static tb_pointer_t tb_static_fixed_pool_old_malloc_find(tb_static_fixed_pool_old_impl_t* impl)
{
    tb_size_t   i = 0;
#if TB_CPU_BIT64
    tb_size_t   m = tb_align(impl->maxn, 64) >> 6;
#elif TB_CPU_BIT32
    tb_size_t   m = tb_align(impl->maxn, 32) >> 5;
#endif
    tb_size_t*  p = (tb_size_t*)impl->used;
    tb_size_t*  e = (tb_size_t*)impl->used + m;
    tb_byte_t*  d = tb_null;

    // check align
    tb_assert_and_check_return_val(!(((tb_size_t)p) & (TB_CPU_BITBYTE - 1)), tb_null);

    // find the free chunk, step * 32|64 items
#if 0
//  while (p < e && *p == 0xffffffff) p++;
//  while (p < e && *p == 0xffffffffffffffffL) p++;
    while (p < e && !(*p + 1)) p++;
#else
    while (p + 7 < e)
    {
        if (p[0] + 1) { p += 0; break; }
        if (p[1] + 1) { p += 1; break; }
        if (p[2] + 1) { p += 2; break; }
        if (p[3] + 1) { p += 3; break; }
        if (p[4] + 1) { p += 4; break; }
        if (p[5] + 1) { p += 5; break; }
        if (p[6] + 1) { p += 6; break; }
        if (p[7] + 1) { p += 7; break; }
        p += 8;
    }
    while (p < e && !(*p + 1)) p++; 
#endif
    tb_check_return_val(p < e, tb_null);

    // find the free bit index
    m = impl->maxn;
    i = (((tb_byte_t*)p - impl->used) << 3) + tb_bits_fb0_le(*p);
    tb_check_return_val(i < m, tb_null);

    // alloc it
    d = impl->data + i * impl->step;
    tb_static_fixed_pool_old_used_set1(impl->used, i);

    // predict the next block
    if (i + 1 < m && !tb_static_fixed_pool_old_used_bset(impl->used, i + 1))
        impl->pred = impl->data + (i + 1) * impl->step;

    // ok?
    return d;
}
#else
static tb_pointer_t tb_static_fixed_pool_old_malloc_find(tb_static_fixed_pool_old_impl_t* impl)
{
    tb_size_t   i = 0;
    tb_size_t   m = impl->maxn;
    tb_byte_t*  p = impl->used;
    tb_byte_t   u = *p;
    tb_byte_t   b = 0;
    tb_byte_t*  d = tb_null;
    for (i = 0; i < m; ++i)
    {
        // bit
        b = i & 0x07;

        // u++
        if (!b) 
        {
            u = *p++;
                
            // skip the non-free byte
            //if (u == 0xff)
            if (!(u + 1))
            {
                i += 7;
                continue ;
            }
        }

        // is free?
        // if (!tb_static_fixed_pool_old_used_bset(impl->used, i))
        if (!(u & (0x01 << b)))
        {
            // ok
            d = impl->data + i * impl->step;
            // tb_static_fixed_pool_old_used_set1(impl->used, i);
            *(p - 1) |= (0x01 << b);

            // predict the next block
            if (i + 1 < m && !tb_static_fixed_pool_old_used_bset(impl->used, i + 1))
                impl->pred = impl->data + (i + 1) * impl->step;

            break;
        }
    }

    // ok?
    return d;
}
#endif


/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_static_fixed_pool_old_ref_t tb_static_fixed_pool_old_init(tb_byte_t* data, tb_size_t size, tb_size_t step, tb_size_t align)
{
    // check
    tb_assert_and_check_return_val(data && step && size, tb_null);

    // align
    align = align? tb_align_pow2(align) : TB_CPU_BITBYTE;
    align = tb_max(align, TB_CPU_BITBYTE);
    tb_assert_and_check_return_val(align <= TB_STATIC_FIXED_POOL_OLD_ALIGN_MAXN, tb_null);

    // align data
    tb_size_t byte = (tb_size_t)((tb_hize_t)tb_align((tb_hize_t)(tb_size_t)data, align) - (tb_hize_t)(tb_size_t)data);
    tb_assert_and_check_return_val(size >= byte, tb_null);
    size -= byte;
    data += byte;
    tb_assert_and_check_return_val(size, tb_null);

    // init data
    tb_memset(data, 0, size);

    // init pool
    tb_static_fixed_pool_old_impl_t* impl = (tb_static_fixed_pool_old_impl_t*)data;

    // init magic
    impl->magic = TB_STATIC_FIXED_POOL_OLD_MAGIC;

    // init align
    impl->align = (tb_uint16_t)align;

    // init step
    impl->step = tb_align(step, impl->align);

    // init used
    impl->used = (tb_byte_t*)(tb_size_t)tb_align((tb_hize_t)(tb_size_t)&impl[1], (tb_hize_t)impl->align);
    tb_assert_and_check_return_val(data + size > impl->used, tb_null);

    /* init maxn
     *
     * used + maxn * step < left
     * align8(maxn) / 8 + maxn * step < left
     * (maxn + 7) / 8 + maxn * step < left
     * (maxn / 8) + (7 / 8) + maxn * step < left
     * maxn * (1 / 8 + step) < left - (7 / 8)
     * maxn < (left - (7 / 8)) / (1 / 8 + step)
     * maxn < (left * 8 - 7) / (1 + step * 8)
     */
    impl->maxn = (((data + size - impl->used) << 3) - 7) / (1 + (impl->step << 3));
    tb_assert_and_check_return_val(impl->maxn, tb_null);

    // init data
    impl->data = (tb_byte_t*)(tb_size_t)tb_align((tb_hize_t)(tb_size_t)impl->used + (tb_align8(impl->maxn) >> 3), (tb_hize_t)impl->align);
    tb_assert_and_check_return_val(data + size > impl->data, tb_null);
    tb_assert_and_check_return_val(impl->maxn * impl->step <= (tb_size_t)(data + size - impl->data), tb_null);

    // init size
    impl->size = 0;

    // init pred
    impl->pred = impl->data;

    // init info
#ifdef __tb_debug__
    impl->info.peak = 0;
    impl->info.fail = 0;
    impl->info.pred = 0;
    impl->info.aloc = 0;
#endif

    // ok
    return (tb_static_fixed_pool_old_ref_t)impl;
}
tb_void_t tb_static_fixed_pool_old_exit(tb_static_fixed_pool_old_ref_t pool)
{
    // check 
    tb_static_fixed_pool_old_impl_t* impl = (tb_static_fixed_pool_old_impl_t*)pool;
    tb_assert_and_check_return(impl && impl->magic == TB_STATIC_FIXED_POOL_OLD_MAGIC);

    // clear body
    tb_static_fixed_pool_old_clear(pool);

    // clear head
    tb_memset(impl, 0, sizeof(tb_static_fixed_pool_old_impl_t));
}
tb_void_t tb_static_fixed_pool_old_clear(tb_static_fixed_pool_old_ref_t pool)
{
    // check 
    tb_static_fixed_pool_old_impl_t* impl = (tb_static_fixed_pool_old_impl_t*)pool;
    tb_assert_and_check_return(impl && impl->magic == TB_STATIC_FIXED_POOL_OLD_MAGIC);

    // clear data
    if (impl->data) tb_memset(impl->data, 0, impl->maxn * impl->step);
    
    // clear used
    if (impl->used) tb_memset(impl->used, 0, (tb_align8(impl->maxn) >> 3));

    // reinit size
    impl->size = 0;
    
    // reinit pred
    impl->pred = impl->data;
    
    // reinit info
#ifdef __tb_debug__
    impl->info.peak = 0;
    impl->info.fail = 0;
    impl->info.pred = 0;
    impl->info.aloc = 0;
#endif
}
tb_size_t tb_static_fixed_pool_old_size(tb_static_fixed_pool_old_ref_t pool)
{
    // check 
    tb_static_fixed_pool_old_impl_t* impl = (tb_static_fixed_pool_old_impl_t*)pool;
    tb_assert_and_check_return_val(impl && impl->magic == TB_STATIC_FIXED_POOL_OLD_MAGIC, 0);

    // size
    return impl->size;
}
tb_pointer_t tb_static_fixed_pool_old_malloc(tb_static_fixed_pool_old_ref_t pool)
{
    // check 
    tb_static_fixed_pool_old_impl_t* impl = (tb_static_fixed_pool_old_impl_t*)pool;
    tb_assert_and_check_return_val(impl && impl->magic == TB_STATIC_FIXED_POOL_OLD_MAGIC, tb_null);
    tb_assert_and_check_return_val(impl->step, tb_null);

    // no space?
    tb_check_return_val(impl->size < impl->maxn, tb_null);

    // predict it?
//  tb_pointer_t data = tb_null;
    tb_pointer_t data = tb_static_fixed_pool_old_malloc_pred(impl);

    // find the free block
    if (!data) data = tb_static_fixed_pool_old_malloc_find(impl);

    // size++
    if (data) impl->size++;

    // update info
#ifdef __tb_debug__
    if (impl->size > impl->info.peak) impl->info.peak = impl->size;
    impl->info.fail += data? 0 : 1;
    impl->info.aloc++;
#endif

    // ok?
    return data;
}
tb_pointer_t tb_static_fixed_pool_old_malloc0(tb_static_fixed_pool_old_ref_t pool)
{
    // check 
    tb_static_fixed_pool_old_impl_t* impl = (tb_static_fixed_pool_old_impl_t*)pool;
    tb_assert_and_check_return_val(impl && impl->magic == TB_STATIC_FIXED_POOL_OLD_MAGIC, tb_null);

    // malloc
    tb_pointer_t p = tb_static_fixed_pool_old_malloc(pool);

    // clear
    if (p) tb_memset(p, 0, impl->step);

    // ok?
    return p;
}
tb_pointer_t tb_static_fixed_pool_old_memdup(tb_static_fixed_pool_old_ref_t pool, tb_cpointer_t data)
{
    // check 
    tb_static_fixed_pool_old_impl_t* impl = (tb_static_fixed_pool_old_impl_t*)pool;
    tb_assert_and_check_return_val(impl && data, tb_null);

    // init
    tb_size_t       n = impl->step;
    tb_pointer_t    p = tb_static_fixed_pool_old_malloc(pool);

    // copy
    if (p) tb_memcpy(p, data, n);

    // ok?
    return p;
}
tb_bool_t tb_static_fixed_pool_old_free(tb_static_fixed_pool_old_ref_t pool, tb_pointer_t data)
{
    // check 
    tb_static_fixed_pool_old_impl_t* impl = (tb_static_fixed_pool_old_impl_t*)pool;
    tb_assert_and_check_return_val(impl && impl->magic == TB_STATIC_FIXED_POOL_OLD_MAGIC && impl->step, tb_false);

    // check size
    tb_check_return_val(impl->size, tb_false);

    // check data
    tb_check_return_val((tb_byte_t*)data >= impl->data && (tb_byte_t*)data + impl->step <= impl->data + impl->maxn * impl->step, tb_false); 
    tb_check_return_val(!(((tb_size_t)data) & (impl->align - 1)), tb_false);
    tb_check_return_val(!(((tb_byte_t*)data - impl->data) % impl->step), tb_false);

    // item
    tb_size_t i = ((tb_byte_t*)data - impl->data) / impl->step;

    // double free?
    tb_assert_return_val(tb_static_fixed_pool_old_used_bset(impl->used, i), tb_true);

    // free it
    tb_static_fixed_pool_old_used_set0(impl->used, i);
    
    // predict it
    impl->pred = (tb_byte_t*)data;

    // size--
    impl->size--;

    // ok
    return tb_true;
}
tb_void_t tb_static_fixed_pool_old_walk(tb_static_fixed_pool_old_ref_t pool, tb_bool_t (*func)(tb_pointer_t , tb_cpointer_t ), tb_cpointer_t priv)
{
    // check 
    tb_static_fixed_pool_old_impl_t* impl = (tb_static_fixed_pool_old_impl_t*)pool;
    tb_assert_and_check_return(impl && impl->magic == TB_STATIC_FIXED_POOL_OLD_MAGIC && impl->maxn && impl->step && func);

    // walk
    tb_size_t   i = 0;
    tb_size_t   m = impl->maxn;
    tb_byte_t*  p = impl->used;
    tb_byte_t   u = *p;
    tb_byte_t   b = 0;
    for (i = 0; i < m; ++i)
    {
        // bit
        b = i & 0x07;

        // u++
        if (!b) 
        {
            u = *p++;
                
            // this byte is all occupied?
            //if (u == 0xff)
            if (!(u + 1))
            {
                // done func
                func(impl->data + (i + 0) * impl->step, priv);
                func(impl->data + (i + 1) * impl->step, priv);
                func(impl->data + (i + 2) * impl->step, priv);
                func(impl->data + (i + 3) * impl->step, priv);
                func(impl->data + (i + 4) * impl->step, priv);
                func(impl->data + (i + 5) * impl->step, priv);
                func(impl->data + (i + 6) * impl->step, priv);
                func(impl->data + (i + 7) * impl->step, priv);

                // skip this byte and continue it
                i += 7;
                continue ;
            }
        }

        // is occupied?
        // if (tb_static_fixed_pool_old_used_bset(impl->used, i))
        if ((u & (0x01 << b)))
        {
            // done func
            func(impl->data + i * impl->step, priv);
        }
    }
}

#ifdef __tb_debug__
tb_void_t tb_static_fixed_pool_old_dump(tb_static_fixed_pool_old_ref_t pool)
{
    tb_static_fixed_pool_old_impl_t* impl = (tb_static_fixed_pool_old_impl_t*)pool;
    tb_assert_and_check_return(impl);

    tb_trace_i("======================================================================");
    tb_trace_i("pool: magic: %#lx", impl->magic);
    tb_trace_i("pool: align: %lu",  impl->align);
    tb_trace_i("pool: head: %lu",   impl->data - (tb_byte_t*)pool);
    tb_trace_i("pool: data: %p",    impl->data);
    tb_trace_i("pool: size: %lu",   impl->size);
    tb_trace_i("pool: step: %lu",   impl->step);
    tb_trace_i("pool: maxn: %lu",   impl->maxn);
    tb_trace_i("pool: peak: %lu",   impl->info.peak);
    tb_trace_i("pool: wast: %lu%%", (impl->data - (tb_byte_t*)pool) * 100 / (impl->maxn * impl->step));
    tb_trace_i("pool: fail: %lu",   impl->info.fail);
    tb_trace_i("pool: pred: %lu%%", impl->info.aloc? ((impl->info.pred * 100) / impl->info.aloc) : 0);

    tb_size_t   i = 0;
    tb_size_t   m = impl->maxn;
    for (i = 0; i < m; ++i)
    {
        if (!(i & 0x7) && impl->used[i >> 3]) 
            tb_trace_i("\tpool: block[%lu]: %08b", i >> 3, impl->used[i >> 3]);
    }
}
#endif
