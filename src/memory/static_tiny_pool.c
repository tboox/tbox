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
 * @file        static_tiny_pool.c
 * @ingroup     memory
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "static_tiny_pool.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the magic number
#define TB_STATIC_TINY_POOL_MAGIC                       (0xdead)

// the align maxn
#define TB_STATIC_TINY_POOL_ALIGN_MAXN                  (64)

// the block maxn in the chunk
#define TB_STATIC_TINY_POOL_BLOCK_MAXN                  (sizeof(tb_size_t) << 3)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

#ifdef __tb_debug__
// the pool info type
typedef struct __tb_tiny_pool_info_t
{
    // the used size
    tb_size_t           used;

    // the peak size
    tb_size_t           peak;

    // the need size
    tb_size_t           need;

    // the real size
    tb_size_t           real;

    // the fail count
    tb_size_t           fail;

    // the pred count
    tb_size_t           pred;

    // the aloc count
    tb_size_t           aloc;

}tb_tiny_pool_info_t;
#endif

/*!the tiny pool type
 *
 * <pre>
 * pool: |---------|-----------------|-----------------------------------------------|
 *           head          used                            data                         
 *                 |--------|--------|
 *                    body     last
 *
 * used:
 * last: |---------------------------|-----------------------------|--- ... ---------|
 *             chunk0(32|64 bits)              chunk1                  chunki
 *       |---------------------------|
 *             sizeof(tb_size_t)
 *       |------|----|---------|-----|
 *                blocki..   block0   <= for little endian 
 *
 * 
 * body: |---------------------------|-----------------------------|--- ... ---------|
 *             chunk0(32|64 bits)              chunk1                  chunki
 *       |---------------------------|
 *             sizeof(tb_size_t)
 *       |------||||||||||||---||||--|
 *                blocki..   block0   <= for little endian 
 *
 *
 * data: |----------------------------------------------------|----|--- ... ---------|
 *                         chunk0(32|64 blocks)                         chunki
 *       |--------------|                       |-------------|
 *            block0                                blocki          
 *       |-----|-----|------|--------- ... -----|------|------|----|--- ... ---------|
 *        step0 step1 step2                       stepi   ...
 *
 * pred:
 * pred[i]:       0               : no pred
 * pred[0]:       1 x step        : |----------------------|-------------------------|---...   -------|
 * pred[1]:       2 x step        : -|----------------------|-------------------------|---...   -------|
 * pred[2]:       3 x step        : --|----------------------|-------------------------|---...   -------|
 * pred[3]:       4 x step        : ---|----------------------|-------------------------|---...   -------|
 * pred[4]:       5 x step        : ----|----------------------|-------------------------|---...   -------|
 * pred[5]:       6 x step        : -----|----------------------|-------------------------|---...   -------|
 * pred[6]:       7 x step        : ------|----------------------|-------------------------|---...   -------|
 * ...
 * pred[31|63]:   (32|64) x step
 *
 * note:
 * 1. align bytes <= 64
 * 2. alloc bytes <= (32|64) * 16 == 512|1024 for one chunk
 * 3. step bytes == max(align, 16)
 * </pre>
 */
typedef struct __tb_tiny_pool_t
{
    /// the magic 
    tb_size_t               magic   : 16;

    /// the align
    tb_size_t               align   : 7;

    /// the step
    tb_size_t               step    : 7;

    /// the full
    tb_size_t               full    : 1;

    /// the body
    tb_size_t*              body;

    /// the last
    tb_size_t*              last;

    /// the maxn
    tb_size_t               maxn;

    /// the data
    tb_byte_t*              data;

    /// the pred
    tb_size_t               pred[TB_STATIC_TINY_POOL_BLOCK_MAXN];

    /// the info
#ifdef __tb_debug__
    tb_tiny_pool_info_t     info;
#endif

}tb_tiny_pool_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_size_t tb_tiny_pool_find_free(tb_size_t body, tb_size_t bits, tb_size_t bitn)
{
#if 0
    tb_size_t   blkn = TB_STATIC_TINY_POOL_BLOCK_MAXN;
    tb_size_t   blks = ~body;
    while (((((blks >> (TB_STATIC_TINY_POOL_BLOCK_MAXN - blkn)) & bits) != bits) && blkn--)) ;
    return TB_STATIC_TINY_POOL_BLOCK_MAXN - blkn;
#elif 0
    tb_size_t   blki = 0;
    tb_size_t   blkn = TB_STATIC_TINY_POOL_BLOCK_MAXN;
    tb_size_t   blks = ~body;
    while (blki < TB_STATIC_TINY_POOL_BLOCK_MAXN)
    {
        if (((blks >> (blki + 0)) & bits) == bits) { blki += 0; break; }
        if (((blks >> (blki + 1)) & bits) == bits) { blki += 1; break; }
        if (((blks >> (blki + 2)) & bits) == bits) { blki += 2; break; }
        if (((blks >> (blki + 3)) & bits) == bits) { blki += 3; break; }
        if (((blks >> (blki + 4)) & bits) == bits) { blki += 4; break; }
        if (((blks >> (blki + 5)) & bits) == bits) { blki += 5; break; }
        if (((blks >> (blki + 6)) & bits) == bits) { blki += 6; break; }
        if (((blks >> (blki + 7)) & bits) == bits) { blki += 7; break; }
        blki += 8;
    }
    return blki;
#else
    tb_size_t   blki = 0;
    tb_size_t   blks = ~body;
    tb_size_t   blkb = 0;
    while (blki < TB_STATIC_TINY_POOL_BLOCK_MAXN)
    {
        blkb = (blks >> blki) & bits;
        if (blkb != bits)
        {
            if (!blkb) blki += bitn;
            else if (blkb == 1) blki++;
            else blki += TB_STATIC_TINY_POOL_BLOCK_MAXN - tb_bits_fb0_be(blkb | ~bits);
        }
        else break;
    }
    return blki;
#endif
}
static tb_pointer_t tb_tiny_pool_malloc_pred(tb_tiny_pool_t* pool, tb_size_t size, tb_size_t bits, tb_size_t bitn)
{   
    // no pred?
    tb_check_return_val(pool->pred[bitn - 1], tb_object_null);

    // init
    tb_size_t   maxn = pool->maxn;
    tb_size_t   pred = pool->pred[bitn - 1] - 1;
    tb_size_t*  body = pool->body + pred;
    tb_size_t*  last = pool->last + pred;
        
    // find 
    tb_size_t   blki = tb_tiny_pool_find_free(*body, bits, bitn);
    tb_check_return_val(blki < TB_STATIC_TINY_POOL_BLOCK_MAXN, tb_object_null);

    // alloc it
    tb_byte_t*  data = pool->data + (pred * TB_STATIC_TINY_POOL_BLOCK_MAXN + blki) * pool->step;
    *body |= bits << blki;
    *last |= (tb_size_t)1 << (blki + bitn - 1);
    
    // full? no the next free block space
    if (blki + bitn + bitn > TB_STATIC_TINY_POOL_BLOCK_MAXN)
    {
        // the next
        if (pool->pred[bitn - 1] + TB_STATIC_TINY_POOL_BLOCK_MAXN <= maxn) pool->pred[bitn - 1] += TB_STATIC_TINY_POOL_BLOCK_MAXN;
        // no pred
        else pool->pred[bitn - 1] = 0;
    }

    // update the info
#ifdef __tb_debug__
    pool->info.pred++;
#endif

    // ok
    return data;
}
static tb_pointer_t tb_tiny_pool_malloc_find(tb_tiny_pool_t* pool, tb_size_t size, tb_size_t bits, tb_size_t bitn)
{
    // init
    tb_size_t*  body = pool->body;
    tb_size_t*  last = pool->last;
    tb_size_t   maxn = pool->maxn;
    tb_byte_t*  data = tb_object_null;

    // find the free bit index for the enough space in the little-endian sort
    tb_size_t   blki = TB_STATIC_TINY_POOL_BLOCK_MAXN;
    tb_size_t*  tail = body + maxn;
#if 1
    tb_size_t   i = 0;  
    while (body + 7 < tail)
    {
        // find chunk
        do 
        {
            if (body[0] + 1 && tb_bits_cb0(body[0]) >= bitn) { i = 0; break; }
            if (body[1] + 1 && tb_bits_cb0(body[1]) >= bitn) { i = 1; break; }
            if (body[2] + 1 && tb_bits_cb0(body[2]) >= bitn) { i = 2; break; }
            if (body[3] + 1 && tb_bits_cb0(body[3]) >= bitn) { i = 3; break; }
            if (body[4] + 1 && tb_bits_cb0(body[4]) >= bitn) { i = 4; break; }
            if (body[5] + 1 && tb_bits_cb0(body[5]) >= bitn) { i = 5; break; }
            if (body[6] + 1 && tb_bits_cb0(body[6]) >= bitn) { i = 6; break; }
            if (body[7] + 1 && tb_bits_cb0(body[7]) >= bitn) { i = 7; break; }
            i = 8;

        } while (0);

        // if (*body != 0xffffffff)
        // if (*body != 0xffffffffffffffffL)
        if (i < 8)
        {
            // this chunk
            body += i;
            
            // find 
            blki = tb_tiny_pool_find_free(*body, bits, bitn);

            // ok?
            if (blki < TB_STATIC_TINY_POOL_BLOCK_MAXN)
            {
                last += (body - pool->body);
                break;
            }
            
            // next chunk
            body++;
        }
        else body += 8;
    }
#endif
    if (blki == TB_STATIC_TINY_POOL_BLOCK_MAXN)
    {
        while (body < tail) 
        {
            // if (*body != 0xffffffff)
            // if (*body != 0xffffffffffffffffL)
            if (*body + 1 && tb_bits_cb0(*body) >= bitn)
            {
                // find 
                blki = tb_tiny_pool_find_free(*body, bits, bitn);

                // ok?
                if (blki < TB_STATIC_TINY_POOL_BLOCK_MAXN)
                {
                    last += (body - pool->body);
                    break;
                }
            }

            // next chunk
            body++;
        }
    }

    // no space?
    tb_check_return_val(body < tail && blki < TB_STATIC_TINY_POOL_BLOCK_MAXN, tb_object_null);

    // alloc it
    data = pool->data + ((body - pool->body) * TB_STATIC_TINY_POOL_BLOCK_MAXN + blki) * pool->step;
    *body |= bits << blki;
    *last |= (tb_size_t)1 << (blki + bitn - 1);
    
    // predict the next
    pool->pred[bitn - 1] = (body - pool->body) + 1;
    // full? no the next free block space
    if (blki + bitn + bitn > TB_STATIC_TINY_POOL_BLOCK_MAXN)
    {
        // the next
        if (pool->pred[bitn - 1] + TB_STATIC_TINY_POOL_BLOCK_MAXN <= maxn) pool->pred[bitn - 1] += TB_STATIC_TINY_POOL_BLOCK_MAXN;
        // no pred
        else pool->pred[bitn - 1] = 0;
    }

    // ok
    return data;
}
tb_pointer_t tb_tiny_pool_ralloc_fast(tb_tiny_pool_t* pool, tb_pointer_t data, tb_size_t size, tb_size_t* osize);
tb_pointer_t tb_tiny_pool_ralloc_fast(tb_tiny_pool_t* pool, tb_pointer_t data, tb_size_t size, tb_size_t* osize)
{
    // check
    tb_assert_and_check_return_val(pool && pool->magic == TB_STATIC_TINY_POOL_MAGIC, tb_object_null);

    // check data
    tb_check_return_val(data, tb_object_null);
    tb_check_return_val((tb_byte_t*)data >= pool->data && (tb_byte_t*)data < pool->data + pool->maxn * pool->step * TB_STATIC_TINY_POOL_BLOCK_MAXN, tb_object_null);
    tb_check_return_val(!(((tb_size_t)data) & (pool->align - 1)), tb_object_null);
    tb_check_return_val(!(((tb_byte_t*)data - pool->data) % pool->step), tb_object_null);

    // the block bit index
    tb_size_t   blki = ((tb_byte_t*)data - pool->data) / pool->step;

    // the body & last
    tb_size_t*  body = pool->body + blki / TB_STATIC_TINY_POOL_BLOCK_MAXN;
    tb_size_t*  last = pool->last + (body - pool->body);
    blki &= TB_STATIC_TINY_POOL_BLOCK_MAXN - 1;

    // the block bit number
    tb_size_t   bitn = tb_bits_fb1_le(*last >> blki) + 1;   
    tb_assert_and_check_return_val(bitn <= TB_STATIC_TINY_POOL_BLOCK_MAXN, tb_object_null);

    // osize
    if (osize) *osize = bitn * pool->step;

    // ok?
    return (tb_align(size, pool->step) <= bitn * pool->step)? data : tb_object_null;
}
/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_handle_t tb_tiny_pool_init(tb_byte_t* data, tb_size_t size, tb_size_t align)
{
    // check
    tb_assert_and_check_return_val(data && size, tb_object_null);
    tb_assert_static(TB_STATIC_TINY_POOL_BLOCK_MAXN == sizeof(tb_size_t) << 3);

    // align
    align = align? tb_align_pow2(align) : TB_CPU_BITBYTE;
    align = tb_max(align, TB_CPU_BITBYTE);
    tb_assert_and_check_return_val(align <= TB_STATIC_TINY_POOL_ALIGN_MAXN, tb_object_null);

    // align data
    tb_size_t byte = (tb_size_t)tb_align((tb_size_t)data, align) - (tb_size_t)data;
    tb_assert_and_check_return_val(size >= byte, tb_object_null);
    size -= byte;
    data += byte;
    tb_assert_and_check_return_val(size, tb_object_null);

    // init data
    tb_memset(data, 0, size);

    // init pool
    tb_tiny_pool_t* pool = (tb_tiny_pool_t*)data;

    // init magic
    pool->magic = TB_STATIC_TINY_POOL_MAGIC;

    // init align
    pool->align = align;

    // init step
    pool->step = tb_max(pool->align, 16);

    // init full
    pool->full = 0;

    // init body
    pool->body = (tb_size_t*)tb_align((tb_size_t)&pool[1], pool->align);
    tb_assert_and_check_return_val(data + size > (tb_byte_t*)pool->body, tb_object_null);
    tb_assert_and_check_return_val(!(((tb_size_t)pool->body) & (TB_CPU_BITBYTE - 1)), tb_object_null);

    /*!init maxn
     *
     * <pre>
     * body + last + data < left
     * sizeof(tb_size_t) * maxn * 2 + maxn * sizeof(tb_size_t) * 8 * step < left
     * sizeof(tb_size_t) * maxn * 2 * (1 + 4 * step) < left
     * maxn < left / ((1 + 4 * step) * 2 * sizeof(tb_size_t))
     * </pre>
     */
    pool->maxn = (data + size - (tb_byte_t*)pool->body) / ((1 + (pool->step << 2)) * (sizeof(tb_size_t) << 1));
    tb_assert_and_check_return_val(pool->maxn >= TB_STATIC_TINY_POOL_BLOCK_MAXN, tb_object_null);

    // init last
    pool->last = pool->body + pool->maxn;
    tb_assert_and_check_return_val(data + size > (tb_byte_t*)pool->last, tb_object_null);
    tb_assert_and_check_return_val(!(((tb_size_t)pool->last) & (TB_CPU_BITBYTE - 1)), tb_object_null);

    // init data
    pool->data = (tb_byte_t*)tb_align((tb_size_t)(pool->last + pool->maxn), pool->align);
    tb_assert_and_check_return_val(data + size > pool->data, tb_object_null);
    tb_assert_and_check_return_val(pool->maxn * pool->step * TB_STATIC_TINY_POOL_BLOCK_MAXN <= (tb_size_t)(data + size - pool->data), tb_object_null);

    // init pred => the first chunk index at used
    tb_size_t n = TB_STATIC_TINY_POOL_BLOCK_MAXN; while (n--) pool->pred[n] = n + 1;

    // init info
#ifdef __tb_debug__
    pool->info.used = 0;
    pool->info.peak = 0;
    pool->info.need = 0;
    pool->info.real = 0;
    pool->info.fail = 0;
    pool->info.pred = 0;
    pool->info.aloc = 0;
#endif

    // ok
    return ((tb_handle_t)pool);
}
tb_void_t tb_tiny_pool_exit(tb_handle_t handle)
{
    // check 
    tb_tiny_pool_t* pool = (tb_tiny_pool_t*)handle;
    tb_assert_and_check_return(pool && pool->magic == TB_STATIC_TINY_POOL_MAGIC);

    // clear body
    tb_tiny_pool_clear(handle);

    // clear last
    tb_memset(pool, 0, sizeof(tb_tiny_pool_t));
}
tb_void_t tb_tiny_pool_clear(tb_handle_t handle)
{
    // check 
    tb_tiny_pool_t* pool = (tb_tiny_pool_t*)handle;
    tb_assert_and_check_return(pool && pool->magic == TB_STATIC_TINY_POOL_MAGIC);

    // clear data
    if (pool->data) tb_memset(pool->data, 0, pool->maxn * pool->step * TB_STATIC_TINY_POOL_BLOCK_MAXN);

    // clear body
    if (pool->body) tb_memset(pool->body, 0, pool->maxn * sizeof(tb_size_t));
    
    // clear last
    if (pool->last) tb_memset(pool->last, 0, pool->maxn * sizeof(tb_size_t));

    // reinit pred
    tb_size_t n = TB_STATIC_TINY_POOL_BLOCK_MAXN; while (n--) pool->pred[n] = n + 1;
    
    // reinit full
    pool->full = 0;
    
    // reinit info
#ifdef __tb_debug__
    pool->info.used = 0;
    pool->info.peak = 0;
    pool->info.need = 0;
    pool->info.real = 0;
    pool->info.fail = 0;
    pool->info.pred = 0;
    pool->info.aloc = 0;
#endif
}

tb_size_t tb_tiny_pool_limit(tb_handle_t handle)
{
    // check
    tb_tiny_pool_t* pool = (tb_tiny_pool_t*)handle;
    tb_assert_and_check_return_val(pool && pool->magic == TB_STATIC_TINY_POOL_MAGIC, 0);

    return (TB_STATIC_TINY_POOL_BLOCK_MAXN * pool->step);
}
tb_pointer_t tb_tiny_pool_malloc(tb_handle_t handle, tb_size_t size)
{
    // check
    tb_tiny_pool_t* pool = (tb_tiny_pool_t*)handle;
    tb_assert_and_check_return_val(pool && pool->magic == TB_STATIC_TINY_POOL_MAGIC, tb_object_null);

    // no size?
    tb_check_return_val(size, tb_object_null);

    // too large?
    tb_check_return_val(size <= pool->step * TB_STATIC_TINY_POOL_BLOCK_MAXN, tb_object_null);
    
    // larger than limit, maybe slower
    tb_assert(size <= tb_tiny_pool_limit(pool));

    // full?
    tb_check_return_val(!pool->full, tb_object_null);

    // the free block bit in the chunk
    // e.g. 3 blocks => bits: 111
    tb_size_t   bitn = tb_align(size, pool->step) / pool->step;
    tb_size_t   bits = ((tb_size_t)1 << bitn) - 1;
    tb_assert_and_check_return_val(bitn && bitn <= TB_STATIC_TINY_POOL_BLOCK_MAXN, tb_object_null);

    // predict it?
//  tb_pointer_t data = tb_object_null;
    tb_pointer_t data = tb_tiny_pool_malloc_pred(pool, size, bits, bitn);

    // find the free block
    if (!data) data = tb_tiny_pool_malloc_find(pool, size, bits, bitn);

    // update info
#ifdef __tb_debug__
    if (data)
    {
        // update the used size
        pool->info.used += tb_align(size, pool->step);

        // update the need size
        pool->info.need += size;

        // update the real size     
        pool->info.real += tb_align(size, pool->step);

        // update the peak size
        if (pool->info.used > pool->info.peak) pool->info.peak = pool->info.used;
        
    }
    // fail++
    else pool->info.fail++;
    
    // aloc++
    pool->info.aloc++;
#endif

    // full?
    if (!data) pool->full = 1;

    // ok?
    return data;
}

tb_pointer_t tb_tiny_pool_malloc0(tb_handle_t handle, tb_size_t size)
{
    // malloc
    tb_pointer_t p = tb_tiny_pool_malloc(handle, size);

    // clear
    if (p && size) tb_memset(p, 0, size);

    // ok?
    return p;
}

tb_pointer_t tb_tiny_pool_nalloc(tb_handle_t handle, tb_size_t item, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(item, tb_object_null);

    // malloc
    return tb_tiny_pool_malloc(handle, item * size);
}

tb_pointer_t tb_tiny_pool_nalloc0(tb_handle_t handle, tb_size_t item, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(item, tb_object_null);

    // malloc
    return tb_tiny_pool_malloc0(handle, item * size);
}

tb_pointer_t tb_tiny_pool_ralloc(tb_handle_t handle, tb_pointer_t data, tb_size_t size)
{
    // check
    tb_tiny_pool_t* pool = (tb_tiny_pool_t*)handle;
    tb_assert_and_check_return_val(pool && pool->magic == TB_STATIC_TINY_POOL_MAGIC, tb_object_null);

    // free it if no size
    if (!size)
    {
        tb_tiny_pool_free(pool, data);
        return tb_object_null;
    }

    // alloc it if no data?
    if (!data) return tb_tiny_pool_malloc(pool, size);
        
    // larger than limit, maybe slower
    tb_assert(size <= tb_tiny_pool_limit(pool));

    // ralloc it with fast mode if enough
    tb_size_t       osize = 0;
    tb_pointer_t    pdata = tb_tiny_pool_ralloc_fast(pool, data, size, &osize);
    tb_check_return_val(!pdata, pdata);
    tb_assert_and_check_return_val(osize && osize < size, tb_object_null);

    // malloc it
    pdata = tb_tiny_pool_malloc(pool, size);
    tb_check_return_val(pdata, tb_object_null);
    tb_assert_and_check_return_val(pdata != data, pdata);

    // copy data
    tb_memcpy(pdata, data, osize);
    
    // free it
    tb_tiny_pool_free(pool, data);

    // ok
    return pdata;
}

tb_bool_t tb_tiny_pool_free(tb_handle_t handle, tb_pointer_t data)
{
    // check
    tb_tiny_pool_t* pool = (tb_tiny_pool_t*)handle;
    tb_assert_and_check_return_val(pool && pool->magic == TB_STATIC_TINY_POOL_MAGIC, tb_false);

    // no data?
    tb_check_return_val(data, tb_true);

    // check data
    tb_check_return_val((tb_byte_t*)data >= pool->data && (tb_byte_t*)data < pool->data + pool->maxn * pool->step * TB_STATIC_TINY_POOL_BLOCK_MAXN, tb_false);
    tb_check_return_val(!(((tb_size_t)data) & (pool->align - 1)), tb_false);
    tb_check_return_val(!(((tb_byte_t*)data - pool->data) % pool->step), tb_false);

    // the block bit index
    tb_size_t   blki = ((tb_byte_t*)data - pool->data) / pool->step;

    // the body & last
    tb_size_t*  body = pool->body + blki / TB_STATIC_TINY_POOL_BLOCK_MAXN;
    tb_size_t*  last = pool->last + (body - pool->body);
    blki &= TB_STATIC_TINY_POOL_BLOCK_MAXN - 1;

    // the block bit number
    tb_size_t   bitn = tb_bits_fb1_le(*last >> blki) + 1;   
    tb_size_t   bits = ((tb_size_t)1 << bitn) - 1;  
    tb_assert_and_check_return_val(bitn <= TB_STATIC_TINY_POOL_BLOCK_MAXN, tb_false);

    // double free? valid?
    tb_assert_and_check_return_val(((*body >> blki) & bits) == bits, tb_false); 

    // free it
    *body &= ~(bits << blki);
    *last &= ~((tb_size_t)1 << (blki + bitn - 1));
        
    // predict it
    pool->pred[bitn - 1] = (body - pool->body) + 1;

    // null? no full
    if (!*body) pool->full = 0;

    // update the info
#ifdef __tb_debug__
    pool->info.used -= bitn * pool->step;
#endif

    // ok
    return tb_true;
}


#ifdef __tb_debug__
tb_void_t tb_tiny_pool_dump(tb_handle_t handle)
{
    tb_tiny_pool_t* pool = (tb_tiny_pool_t*)handle;
    tb_assert_and_check_return(pool);

    tb_trace_i("======================================================================");
    tb_trace_i("pool: magic: %#lx", pool->magic);
    tb_trace_i("pool: align: %lu",  pool->align);
    tb_trace_i("pool: limit: %lu",  tb_tiny_pool_limit(pool));
    tb_trace_i("pool: step: %lu",   pool->step);
    tb_trace_i("pool: head: %lu",   pool->data - (tb_byte_t*)pool);
    tb_trace_i("pool: data: %p",    pool->data);
    tb_trace_i("pool: size: %lu",   pool->maxn * pool->step * TB_STATIC_TINY_POOL_BLOCK_MAXN);
    tb_trace_i("pool: full: %lu",   pool->full);
    tb_trace_i("pool: used: %lu",   pool->info.used);
    tb_trace_i("pool: peak: %lu",   pool->info.peak);
    tb_trace_i("pool: wast: %lu%%",     (pool->info.real + (pool->data - (tb_byte_t*)pool) - pool->info.need) * 100 / (pool->info.real + (pool->data - (tb_byte_t*)pool)));
    tb_trace_i("pool: fail: %lu",   pool->info.fail);
    tb_trace_i("pool: pred: %lu%%",     pool->info.aloc? ((pool->info.pred * 100) / pool->info.aloc) : 0);

    tb_trace_i("");
    tb_size_t   i = 0;
    tb_size_t   m = TB_STATIC_TINY_POOL_BLOCK_MAXN;
    for (i = 0; i < m; i++)
    {
        tb_size_t pred = pool->pred[i];
#if TB_CPU_BIT64
        tb_trace_i("pool: [%lu]: pred: %lu, body: %064lb, last: %064lb", i, pred, pred? pool->body[pred - 1] : 0, pred? pool->last[pred - 1] : 0);
#elif TB_CPU_BIT32
        tb_trace_i("pool: [%lu]: pred: %lu, body: %032lb, last: %032lb", i, pred, pred? pool->body[pred - 1] : 0, pred? pool->last[pred - 1] : 0);
#endif
    }

    tb_trace_i("");
    m = pool->maxn;
    for (i = 0; i < m; i++)
    {
#if TB_CPU_BIT64
        if (pool->body[i]) tb_trace_i("pool: [%lu]: body: %064lb, last: %064lb", i, pool->body[i], pool->last[i]);
#elif TB_CPU_BIT32
        if (pool->body[i]) tb_trace_i("pool: [%lu]: body: %032lb, last: %032lb", i, pool->body[i], pool->last[i]);
#endif
    }

}
#endif
