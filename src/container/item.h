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
typedef tb_void_t 			(*tb_item_func_free_t)(tb_void_t* item, tb_void_t* priv);
typedef tb_void_t* 			(*tb_item_func_dupl_t)(tb_void_t const* item, tb_void_t* priv);
typedef tb_char_t const* 	(*tb_item_func_cstr_t)(tb_void_t const* item, tb_char_t* data, tb_size_t maxn, tb_void_t* priv);
typedef tb_size_t 			(*tb_item_func_hash_t)(tb_void_t const* item, tb_size_t size, tb_void_t* priv);
typedef tb_int_t 			(*tb_item_func_comp_t)(tb_void_t const* litem, tb_void_t const* ritem, tb_void_t* priv);

// the item func type
typedef struct __tb_item_func_t
{
	// the item func
	tb_item_func_hash_t 	hash;
	tb_item_func_comp_t 	comp;
	tb_item_func_dupl_t 	dupl;
	tb_item_func_cstr_t 	cstr;
	tb_item_func_free_t 	free;

	// the priv data
	tb_void_t* 				priv;

}tb_item_func_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */

// the item func
tb_item_func_t 		tb_item_func_str(tb_spool_t* spool); 
tb_item_func_t 		tb_item_func_int();
tb_item_func_t 		tb_item_func_ptr();
tb_item_func_t 		tb_item_func_mem(tb_size_t size);

// c plus plus
#ifdef __cplusplus
}
#endif

#endif

