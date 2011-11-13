/*!The Tiny Box Library
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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		item.h
 *
 */
#ifndef TB_CONTAINER_ITEM_H
#define TB_CONTAINER_ITEM_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "spool.h"

/* /////////////////////////////////////////////////////////
 * types
 */

// the callback type
struct __tb_item_func_t;
typedef tb_void_t 			(*tb_item_func_free_t)(struct __tb_item_func_t* func, tb_void_t* item);
typedef tb_void_t* 			(*tb_item_func_data_t)(struct __tb_item_func_t* func, tb_void_t const* item);
typedef tb_void_t* 			(*tb_item_func_dupl_t)(struct __tb_item_func_t* func, tb_void_t const* data);
typedef tb_void_t* 			(*tb_item_func_copy_t)(struct __tb_item_func_t* func, tb_void_t* item, tb_void_t const* data);
typedef tb_char_t const* 	(*tb_item_func_cstr_t)(struct __tb_item_func_t* func, tb_void_t const* data, tb_char_t* cstr, tb_size_t maxn);
typedef tb_size_t 			(*tb_item_func_hash_t)(struct __tb_item_func_t* func, tb_void_t const* data, tb_size_t size);
typedef tb_int_t 			(*tb_item_func_comp_t)(struct __tb_item_func_t* func, tb_void_t const* rdata, tb_void_t const* ldata);

// the item func type
typedef struct __tb_item_func_t
{
	// the item func
	tb_item_func_hash_t 	hash;
	tb_item_func_comp_t 	comp;
	tb_item_func_free_t 	free;
	tb_item_func_data_t 	data;
	tb_item_func_dupl_t 	dupl;
	tb_item_func_copy_t 	copy;
	tb_item_func_cstr_t 	cstr;

	// the item pool
	tb_void_t* 				pool;

	// the item size
	tb_size_t 				size;

	// the priv data
	tb_void_t* 				priv;

}tb_item_func_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */

/* the c-string item function
 *
 * using tb_strdup if the spool is null, 
 * using tb_spool_strdup if the spool exists
 *
 */
tb_item_func_t 		tb_item_func_str(tb_spool_t* spool); 

// the integer item function
tb_item_func_t 		tb_item_func_int();

// the pointer item function
tb_item_func_t 		tb_item_func_ptr();

/* the external fixed memory item function
 *
 * storing the index value in the internal item of the container
 *
 * using tb_malloc if the fpool is null, 
 * using tb_fpool_put if the fpool exists
 *
 */
tb_item_func_t 		tb_item_func_efm(tb_size_t size, tb_fpool_t* fpool);

/* the internal fixed memory item function
 *
 * storing it in the internal item of the container directly
 *
 */
tb_item_func_t 		tb_item_func_ifm(tb_size_t size, tb_item_func_free_t free, tb_void_t* priv);

// c plus plus
#ifdef __cplusplus
}
#endif

#endif

