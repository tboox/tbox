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
 * \author		ruki
 * \file		gpool.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "gpool.h"
#include "vpool.h"
#include "../libc/libc.h"
#include "../math/math.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// the magic number
#define TB_GPOOL_MAGIC 							(0xdead)

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the generic pool type
typedef struct __tb_gpool_t
{
	// the magic 
	tb_size_t 		magic 	: 16;

	// the align
	tb_size_t 		align 	: 8;

	// the data
	tb_byte_t* 		data;

	// the size
	tb_size_t 		size;

	// the vpool
	tb_handle_t 	vpool;

}tb_gpool_t;


/* ///////////////////////////////////////////////////////////////////////
 * the implemention
 */
tb_handle_t tb_gpool_init(tb_pointer_t data, tb_size_t size, tb_size_t align)
{
	// check
	tb_assert_and_check_return_val(data && size, TB_NULL);

	// align data
	tb_size_t byte = (tb_size_t)tb_align((tb_size_t)data, gpool->align) - (tb_size_t)data;
	tb_assert_and_check_return_val(size >= byte, TB_NULL);
	size -= byte;
	data += byte;

	// init gpool
	tb_gpool_t* gpool = data;

	// init magic
	gpool->magic = TB_GPOOL_MAGIC;

	// init align
	gpool->align = align? tb_align_pow2(align) : TB_CPU_BITBYTE;

	// init data
	gpool->data = (tb_byte_t*)tb_align((tb_size_t)&gpool[1], gpool->align);
	tb_assert_and_check_return_val(data + size > gpool->data, TB_NULL);

	// init size
	gpool->size = (tb_byte_t*)data + size - gpool->data;
	tb_assert_and_check_return_val(gpool->size, TB_NULL);

	// init vpool
	gpool->vpool = tb_vpool_init(gpool->data, gpool->size, gpool->align);
	tb_assert_and_check_return_val(gpool->vpool, TB_NULL);

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
	gpool->vpool = TB_NULL;

	// clear head
	tb_memset(gpool, 0, sizeof(tb_gpool_t));	
}
tb_void_t tb_gpool_clear(tb_handle_t handle)
{
	// check 
	tb_gpool_t* gpool = (tb_gpool_t*)handle;
	tb_assert_and_check_return(gpool && gpool->magic == TB_GPOOL_MAGIC);

	// clear vpool
	if (gpool->vpool) tb_vpool_clear(gpool->vpool);
}

#ifndef TB_DEBUG
tb_pointer_t tb_gpool_malloc_impl(tb_handle_t handle, tb_size_t size)
#else
tb_pointer_t tb_gpool_malloc_impl(tb_handle_t handle, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	tb_gpool_t* gpool = (tb_gpool_t*)handle;
	tb_assert_and_check_return_val(gpool && gpool->magic == TB_GPOOL_MAGIC && gpool->vpool, TB_NULL);

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
	tb_assert_and_check_return_val(gpool && gpool->magic == TB_GPOOL_MAGIC && gpool->vpool, TB_NULL);

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
	tb_assert_and_check_return_val(gpool && gpool->magic == TB_GPOOL_MAGIC && gpool->vpool, TB_NULL);

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
	tb_assert_and_check_return_val(gpool && gpool->magic == TB_GPOOL_MAGIC && gpool->vpool, TB_NULL);

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
	tb_assert_and_check_return_val(gpool && gpool->magic == TB_GPOOL_MAGIC && gpool->vpool, TB_NULL);

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
	tb_assert_and_check_return_val(gpool && gpool->magic == TB_GPOOL_MAGIC && gpool->vpool, TB_FALSE);

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

	tb_vpool_dump(gpool->vpool);
}
#endif
