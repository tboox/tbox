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
 * @file		item.h
 * @ingroup 	container
 *
 */
#ifndef TB_CONTAINER_ITEM_H
#define TB_CONTAINER_ITEM_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the callback type
struct __tb_item_func_t;
typedef tb_size_t 			(*tb_item_func_hash_t)(struct __tb_item_func_t* func, tb_cpointer_t data, tb_size_t size);
typedef tb_long_t 			(*tb_item_func_comp_t)(struct __tb_item_func_t* func, tb_cpointer_t rdata, tb_cpointer_t ldata);

typedef tb_pointer_t 		(*tb_item_func_data_t)(struct __tb_item_func_t* func, tb_cpointer_t item);
typedef tb_char_t const* 	(*tb_item_func_cstr_t)(struct __tb_item_func_t* func, tb_cpointer_t data, tb_char_t* cstr, tb_size_t maxn);

typedef tb_void_t 			(*tb_item_func_free_t)(struct __tb_item_func_t* func, tb_pointer_t item);
typedef tb_void_t 			(*tb_item_func_dupl_t)(struct __tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data);
typedef tb_void_t 			(*tb_item_func_copy_t)(struct __tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data);

typedef tb_void_t 			(*tb_item_func_nfree_t)(struct __tb_item_func_t* func, tb_pointer_t item, tb_size_t size);
typedef tb_void_t 			(*tb_item_func_ndupl_t)(struct __tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data, tb_size_t size);
typedef tb_void_t 			(*tb_item_func_ncopy_t)(struct __tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data, tb_size_t size);

/// the item type
typedef enum __tb_item_type_t
{
	TB_ITEM_TYPE_NUL 		= 0 	//!< null
,	TB_ITEM_TYPE_LONG 		= 1 	//!< integer for tb_long_t
,	TB_ITEM_TYPE_SIZE 		= 2 	//!< integer for tb_size_t
,	TB_ITEM_TYPE_UINT8 		= 3 	//!< integer for tb_uint8_t
,	TB_ITEM_TYPE_UINT16 	= 4 	//!< integer for tb_uint16_t
,	TB_ITEM_TYPE_UINT32 	= 5 	//!< integer for tb_uint32_t
,	TB_ITEM_TYPE_STR 		= 6 	//!< c-string
,	TB_ITEM_TYPE_PTR 		= 7 	//!< pointer
,	TB_ITEM_TYPE_EFM 		= 8 	//!< external fixed memory
,	TB_ITEM_TYPE_IFM 		= 9 	//!< internal fixed memory
,	TB_ITEM_TYPE_OTR 		= 10 	//!< other

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
	tb_pointer_t 			priv;

	// the item func
	tb_item_func_hash_t 	hash;
	tb_item_func_comp_t 	comp;
	tb_item_func_data_t 	data;
	tb_item_func_cstr_t 	cstr;
	tb_item_func_free_t 	free;
	tb_item_func_dupl_t 	dupl;
	tb_item_func_copy_t 	copy;

	tb_item_func_nfree_t 	nfree;
	tb_item_func_ndupl_t 	ndupl;
	tb_item_func_ncopy_t 	ncopy;

}tb_item_func_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/// the integer item function for tb_long_t
tb_item_func_t 		tb_item_func_long();

/// the integer item function for tb_size_t
tb_item_func_t 		tb_item_func_size();

/// the integer item function for tb_uint8_t
tb_item_func_t 		tb_item_func_uint8();

/// the integer item function for tb_uint16_t
tb_item_func_t 		tb_item_func_uint16();

/// the integer item function for tb_uint32_t
tb_item_func_t 		tb_item_func_uint32();

/*!the c-string item function
 *
 * using tb_strdup if the spool is null, 
 * using tb_spool_strdup if the spool exists
 *
 */
tb_item_func_t 		tb_item_func_str(tb_bool_t bcase, tb_handle_t spool); 

/// the pointer item function
tb_item_func_t 		tb_item_func_ptr();

/*!the external fixed memory item function
 *
 * storing the index value in the internal item of the container
 *
 * using tb_malloc if the rpool is null, 
 * using tb_rpool_malloc if the rpool exists
 *
 */
tb_item_func_t 		tb_item_func_efm(tb_size_t size, tb_handle_t rpool);

/*!the internal fixed memory item function
 *
 * storing it in the internal item of the container directly for saving memory
 *
 */
tb_item_func_t 		tb_item_func_ifm(tb_size_t size, tb_item_func_free_t free, tb_pointer_t priv);

#endif

