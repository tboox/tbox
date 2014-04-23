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
 * @author		ruki
 * @file		item.h
 * @ingroup 	container
 *
 */
#ifndef TB_CONTAINER_ITEM_H
#define TB_CONTAINER_ITEM_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the callback type
struct __tb_item_func_t;
typedef tb_size_t 				(*tb_item_func_hash_t)(struct __tb_item_func_t* func, tb_cpointer_t data, tb_size_t size);
typedef tb_long_t 				(*tb_item_func_comp_t)(struct __tb_item_func_t* func, tb_cpointer_t ldata, tb_cpointer_t rdata);

typedef tb_pointer_t 			(*tb_item_func_data_t)(struct __tb_item_func_t* func, tb_cpointer_t item);
typedef tb_char_t const* 		(*tb_item_func_cstr_t)(struct __tb_item_func_t* func, tb_cpointer_t data, tb_char_t* cstr, tb_size_t maxn);

typedef tb_void_t 				(*tb_item_func_free_t)(struct __tb_item_func_t* func, tb_pointer_t item);
typedef tb_void_t 				(*tb_item_func_dupl_t)(struct __tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data);
typedef tb_void_t 				(*tb_item_func_repl_t)(struct __tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data);
typedef tb_void_t 				(*tb_item_func_copy_t)(struct __tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data);

typedef tb_void_t 				(*tb_item_func_nfree_t)(struct __tb_item_func_t* func, tb_pointer_t item, tb_size_t size);
typedef tb_void_t 				(*tb_item_func_ndupl_t)(struct __tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data, tb_size_t size);
typedef tb_void_t 				(*tb_item_func_nrepl_t)(struct __tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data, tb_size_t size);
typedef tb_void_t 				(*tb_item_func_ncopy_t)(struct __tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data, tb_size_t size);

/// the item type
typedef enum __tb_item_type_t
{
	TB_ITEM_TYPE_NULL 			= 0 	//!< null
,	TB_ITEM_TYPE_LONG 			= 1 	//!< integer for tb_long_t
,	TB_ITEM_TYPE_SIZE 			= 2 	//!< integer for tb_size_t
,	TB_ITEM_TYPE_UINT8 			= 3 	//!< integer for tb_uint8_t
,	TB_ITEM_TYPE_UINT16 		= 4 	//!< integer for tb_uint16_t
,	TB_ITEM_TYPE_UINT32 		= 5 	//!< integer for tb_uint32_t
,	TB_ITEM_TYPE_STR 			= 6 	//!< string
,	TB_ITEM_TYPE_PTR 			= 7 	//!< pointer
,	TB_ITEM_TYPE_MEM 			= 8 	//!< memory
,	TB_ITEM_TYPE_OBJ 			= 9 	//!< object
,	TB_ITEM_TYPE_TRUE 			= 10 	//!< true
,	TB_ITEM_TYPE_STRING_POOL 	= 11 	//!< string pool
,	TB_ITEM_TYPE_OTHER 			= 12 	//!< other

}tb_item_type_t;

/// the item func type
typedef struct __tb_item_func_t
{
	/// the item type
	tb_size_t 				type;

	/// the item size
	tb_size_t 				size;

	/// the item pool
	tb_handle_t 			pool;

	/// the priv data
	tb_cpointer_t 			priv;

	/// the hash func
	tb_item_func_hash_t 	hash;

	/// the compare func
	tb_item_func_comp_t 	comp;

	/// the data func of the item 
	tb_item_func_data_t 	data;

	/// the c-string func of the item 
	tb_item_func_cstr_t 	cstr;

	/// the free item func
	tb_item_func_free_t 	free;

	/// the duplicate func, duplicate data to item
	tb_item_func_dupl_t 	dupl;

	/// the replace func, free the prev item and copy data to item, replace it
	tb_item_func_repl_t 	repl;

	/// the copy func, only copy data to item
	tb_item_func_copy_t 	copy; 

	/// the free items func
	tb_item_func_nfree_t 	nfree;

	/// the duplicate items func
	tb_item_func_ndupl_t 	ndupl;

	/// the replace items func
	tb_item_func_nrepl_t 	nrepl;

	/// the copy items func
	tb_item_func_ncopy_t 	ncopy;

}tb_item_func_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! the true item function for hash data: null 
 *
 * @return 			the item func
 */
tb_item_func_t 		tb_item_func_true(tb_noarg_t);

/*! the integer item function for tb_long_t 
 *
 * @return 			the item func
 */
tb_item_func_t 		tb_item_func_long(tb_noarg_t);

/*! the integer item function for tb_size_t
 *
 * @return 			the item func
 */
tb_item_func_t 		tb_item_func_size(tb_noarg_t);

/*! the integer item function for tb_uint8_t
 *
 * @return 			the item func
 */
tb_item_func_t 		tb_item_func_uint8(tb_noarg_t);

/*! the integer item function for tb_uint16_t
 *
 * @return 			the item func
 */
tb_item_func_t 		tb_item_func_uint16(tb_noarg_t);

/*! the integer item function for tb_uint32_t
 *
 * @return 			the item func
 */
tb_item_func_t 		tb_item_func_uint32(tb_noarg_t);

/*! the string item function
 *
 * using tb_strdup if the bpool is null, 
 * using tb_block_pool_strdup if the bpool exists
 *
 * @param bcase 	is case?
 * @param bpool 	the block pool

 * @return 			the item func
 */
tb_item_func_t 		tb_item_func_str(tb_bool_t bcase, tb_handle_t bpool); 

/*! the pointer item function
 *
 * @note if the free func have been hooked, the nfree need hook too.
 *
 * @param free 		the item free func
 * @param priv 		the private data of the item free func
 *
 * @return 			the item func
 */
tb_item_func_t 		tb_item_func_ptr(tb_item_func_free_t free, tb_cpointer_t priv);

/*! the object item function 
 *
 * @return 			the item func
 */
tb_item_func_t 		tb_item_func_obj(tb_noarg_t);

/*! the external fixed memory item function
 *
 * storing the index value in the internal item of the container
 *
 * using tb_malloc if the fpool is null, 
 * using tb_fixed_pool_malloc if the fpool exists
 *
 * @param size 		the item size
 * @param rpool 	the item rpool
 *
 * @return 			the item func
 */
tb_item_func_t 		tb_item_func_efm(tb_size_t size, tb_handle_t fpool);

/*! the internal fixed memory item function
 *
 * storing it in the internal item of the container directly for saving memory
 *
 * @param size 		the item size
 * @param free 		the item free func
 * @param priv 		the private data of the item free func
 *
 * @return 			the item func
 */
tb_item_func_t 		tb_item_func_mem(tb_size_t size, tb_item_func_free_t free, tb_cpointer_t priv);

/*! the string item function for scache
 *
 * @param bcase 	is case?

 * @return 			the item func
 */
tb_item_func_t 		tb_item_func_string_pool(tb_handle_t spool); 

#endif

