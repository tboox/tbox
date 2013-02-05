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
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * @author		ruki
 * @file		gpool.c
 * @ingroup 	memory
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "gpool.h"
#include "vpool.h"
#include "tpool.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// the magic number
#define TB_GPOOL_MAGIC 							(0xdead)

// the align maxn
#define TB_GPOOL_ALIGN_MAXN 					(128)

// the tpool minn
#define TB_GPOOL_TPOOL_MINN 					(512 * 1024)

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/// the generic pool type
typedef struct __tb_gpool_t
{
	/// the magic 
	tb_size_t 		magic 	: 16;

	/// the align
	tb_size_t 		align 	: 8;

	/// the data
	tb_byte_t* 		data;

	/// the size
	tb_size_t 		size;

	/// the tdata
	tb_byte_t* 		tdata;

	/// the tsize
	tb_size_t 		tsize;

	/// the tpool
	tb_handle_t 	tpool;

	/// the vpool
	tb_handle_t 	vpool;

}tb_gpool_t;


/* ///////////////////////////////////////////////////////////////////////
 * the implementation
 */
tb_handle_t tb_gpool_init(tb_byte_t* data, tb_size_t size, tb_size_t align)
{
	// check
	tb_assert_and_check_return_val(data && size, tb_null);

	// align
	align = align? tb_align_pow2(align) : TB_CPU_BITBYTE;
	align = tb_max(align, TB_CPU_BITBYTE);
	tb_assert_and_check_return_val(align <= TB_GPOOL_ALIGN_MAXN, tb_null);

	// align data
	tb_size_t byte = (tb_size_t)tb_align((tb_size_t)data, align) - (tb_size_t)data;
	tb_assert_and_check_return_val(size >= byte, tb_null);
	size -= byte;
	data += byte;

	// init gpool
	tb_gpool_t* gpool = data;

	// init magic
	gpool->magic = TB_GPOOL_MAGIC;

	// init align
	gpool->align = align;

	// init data
	gpool->data = (tb_byte_t*)tb_align((tb_size_t)&gpool[1], gpool->align);
	tb_assert_and_check_return_val(data + size > gpool->data, tb_null);

	// init size
	gpool->size = (tb_byte_t*)data + size - gpool->data;
	tb_assert_and_check_return_val(gpool->size, tb_null);

	// init vpool
	gpool->vpool = tb_vpool_init(gpool->data, gpool->size, gpool->align);
	tb_assert_and_check_return_val(gpool->vpool, tb_null);
	
	// FIXME: alloc will be too slower now if space is small
#if 0
	// init tpool
	gpool->tsize = gpool->size >> 3;
	if (gpool->tsize >= TB_GPOOL_TPOOL_MINN)
	{
		gpool->tdata = tb_vpool_malloc(gpool->vpool, gpool->tsize);
		if (gpool->tdata)
		{
			gpool->tpool = tb_tpool_init(gpool->tdata, gpool->tsize, gpool->align);
			tb_assert_and_check_return_val(gpool->tpool, tb_null);
		}
	}
#endif

	// ok
	return ((tb_handle_t)gpool);
}
tb_void_t tb_gpool_exit(tb_handle_t handle)
{
	// check 
	tb_gpool_t* gpool = (tb_gpool_t*)handle;
	tb_assert_and_check_return(gpool && gpool->magic == TB_GPOOL_MAGIC);

	// clear body
	tb_gpool_clear(handle);

	// exit vpool
	if (gpool->vpool) tb_vpool_exit(gpool->vpool);

	// clear gpool
	tb_memset(gpool, 0, sizeof(tb_gpool_t));	
}
tb_void_t tb_gpool_clear(tb_handle_t handle)
{
	// check 
	tb_gpool_t* gpool = (tb_gpool_t*)handle;
	tb_assert_and_check_return(gpool && gpool->magic == TB_GPOOL_MAGIC);

	// clear vpool
	if (gpool->vpool) tb_vpool_clear(gpool->vpool);

	// reinit tpool
	gpool->tdata = tb_null;
	gpool->tpool = tb_null;
	if (gpool->tsize >= TB_GPOOL_TPOOL_MINN)
	{
		gpool->tdata = tb_vpool_malloc(gpool->vpool, gpool->tsize);
		if (gpool->tdata) gpool->tpool = tb_tpool_init(gpool->tdata, gpool->tsize, gpool->align);
	}
}

#ifndef TB_DEBUG
tb_pointer_t tb_gpool_malloc_impl(tb_handle_t handle, tb_size_t size)
#else
tb_pointer_t tb_gpool_malloc_impl(tb_handle_t handle, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	tb_gpool_t* gpool = (tb_gpool_t*)handle;
	tb_assert_and_check_return_val(gpool && gpool->magic == TB_GPOOL_MAGIC && gpool->vpool, tb_null);

	// try malloc it from tpool
	if (gpool->tpool && size <= tb_tpool_limit(gpool->tpool))
	{
		tb_pointer_t data = tb_tpool_malloc(gpool->tpool, size);
		if (data) return data;
	}

	// malloc it from vpool
#ifndef TB_DEBUG
	return tb_vpool_malloc_impl(gpool->vpool, size);
#else
	return tb_vpool_malloc_impl(gpool->vpool, size, func, line, file);
#endif
}

#ifndef TB_DEBUG
tb_pointer_t tb_gpool_malloc0_impl(tb_handle_t handle, tb_size_t size)
#else
tb_pointer_t tb_gpool_malloc0_impl(tb_handle_t handle, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	tb_gpool_t* gpool = (tb_gpool_t*)handle;
	tb_assert_and_check_return_val(gpool && gpool->magic == TB_GPOOL_MAGIC && gpool->vpool, tb_null);

	// try malloc it from tpool
	if (gpool->tpool && size <= tb_tpool_limit(gpool->tpool))
	{
		tb_pointer_t data = tb_tpool_malloc0(gpool->tpool, size);
		if (data) return data;
	}

	// malloc it from vpool
#ifndef TB_DEBUG
	return tb_vpool_malloc0_impl(gpool->vpool, size);
#else
	return tb_vpool_malloc0_impl(gpool->vpool, size, func, line, file);
#endif
}

#ifndef TB_DEBUG
tb_pointer_t tb_gpool_nalloc_impl(tb_handle_t handle, tb_size_t item, tb_size_t size)
#else
tb_pointer_t tb_gpool_nalloc_impl(tb_handle_t handle, tb_size_t item, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	tb_gpool_t* gpool = (tb_gpool_t*)handle;
	tb_assert_and_check_return_val(gpool && gpool->magic == TB_GPOOL_MAGIC && gpool->vpool, tb_null);

	// try malloc it from tpool
	if (gpool->tpool && item * size <= tb_tpool_limit(gpool->tpool))
	{
		tb_pointer_t data = tb_tpool_nalloc(gpool->tpool, item, size);
		if (data) return data;
	}

	// malloc it from vpool
#ifndef TB_DEBUG
	return tb_vpool_nalloc_impl(gpool->vpool, item, size);
#else
	return tb_vpool_nalloc_impl(gpool->vpool, item, size, func, line, file);
#endif
}

#ifndef TB_DEBUG
tb_pointer_t tb_gpool_nalloc0_impl(tb_handle_t handle, tb_size_t item, tb_size_t size)
#else
tb_pointer_t tb_gpool_nalloc0_impl(tb_handle_t handle, tb_size_t item, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	tb_gpool_t* gpool = (tb_gpool_t*)handle;
	tb_assert_and_check_return_val(gpool && gpool->magic == TB_GPOOL_MAGIC && gpool->vpool, tb_null);

	// try malloc it from tpool
	if (gpool->tpool && item * size <= tb_tpool_limit(gpool->tpool))
	{
		tb_pointer_t data = tb_tpool_nalloc0(gpool->tpool, item, size);
		if (data) return data;
	}

	// malloc it from vpool
#ifndef TB_DEBUG
	return tb_vpool_nalloc0_impl(gpool->vpool, item, size);
#else
	return tb_vpool_nalloc0_impl(gpool->vpool, item, size, func, line, file);
#endif
}

#ifndef TB_DEBUG
tb_pointer_t tb_gpool_ralloc_impl(tb_handle_t handle, tb_pointer_t data, tb_size_t size)
#else
tb_pointer_t tb_gpool_ralloc_impl(tb_handle_t handle, tb_pointer_t data, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	tb_gpool_t* gpool = (tb_gpool_t*)handle;
	tb_assert_and_check_return_val(gpool && gpool->magic == TB_GPOOL_MAGIC && gpool->vpool, tb_null);

	// try ralloc it from tpool
	if (gpool->tpool && data > gpool->tdata && data < gpool->tdata + gpool->tsize)
	{
		// ralloc it
		tb_pointer_t pdata = tb_null;
		if (size <= tb_tpool_limit(gpool->tpool) && (pdata = tb_tpool_ralloc(gpool->tpool, data, size))) return pdata;
		else
		{
			// malloc it
#ifndef TB_DEBUG
			pdata = tb_vpool_malloc_impl(gpool->vpool, size);
#else
			pdata = tb_vpool_malloc_impl(gpool->vpool, size, func, line, file);
#endif
			tb_check_return_val(pdata, tb_null);
			tb_assert_and_check_return_val(pdata != data, pdata);

			// copy data
			tb_size_t osize = tb_min(tb_tpool_limit(gpool->tpool), size);
			tb_memcpy(pdata, data, osize);
			
			// free it
			tb_tpool_free(gpool->tpool, data);

			// ok
			return pdata;
		}
	}

	// ralloc it from vpool
#ifndef TB_DEBUG
	return tb_vpool_ralloc_impl(gpool->vpool, data, size);
#else
	return tb_vpool_ralloc_impl(gpool->vpool, data, size, func, line, file);
#endif
}

#ifndef TB_DEBUG
tb_bool_t tb_gpool_free_impl(tb_handle_t handle, tb_pointer_t data)
#else
tb_bool_t tb_gpool_free_impl(tb_handle_t handle, tb_pointer_t data, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	tb_gpool_t* gpool = (tb_gpool_t*)handle;
	tb_assert_and_check_return_val(gpool && gpool->magic == TB_GPOOL_MAGIC && gpool->vpool, tb_false);

	// free it to tpool
	if (gpool->tpool && data > gpool->tdata && data < gpool->tdata + gpool->tsize)
		return tb_tpool_free(gpool->tpool, data);

	// free it to vpool
#ifndef TB_DEBUG
	return tb_vpool_free_impl(gpool->vpool, data);
#else
	return tb_vpool_free_impl(gpool->vpool, data, func, line, file);
#endif
}


#ifdef TB_DEBUG
tb_void_t tb_gpool_dump(tb_handle_t handle)
{
	// check 
	tb_gpool_t* gpool = (tb_gpool_t*)handle;
	tb_assert_and_check_return(gpool && gpool->magic == TB_GPOOL_MAGIC && gpool->vpool);

	// dump tpool
	if (gpool->tpool) tb_tpool_dump(gpool->tpool);

	// dump vpool
	tb_vpool_dump(gpool->vpool);
}
#endif
