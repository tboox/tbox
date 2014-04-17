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
 * @file		dlist.h
 * @ingroup 	container
 *
 */
#ifndef TB_CONTAINER_DLIST_H
#define TB_CONTAINER_DLIST_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "item.h"
#include "iterator.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the dlist type
typedef tb_void_t 	tb_dlist_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init dlist
 *
 * @param grow 		the item grow
 * @param func 		the item func
 *
 * @return 			the dlist
 */
tb_dlist_t* 		tb_dlist_init(tb_size_t grow, tb_item_func_t func);

/*! exit dlist
 *
 * @param dlist 	the dlist
 */
tb_void_t 			tb_dlist_exit(tb_dlist_t* dlist);

/*! clear dlist
 *
 * @param dlist 	the dlist
 */
tb_void_t 			tb_dlist_clear(tb_dlist_t* dlist);

/*! the dlist head item
 *
 * @param dlist 	the dlist
 *
 * @return 			the head item
 */
tb_pointer_t 		tb_dlist_head(tb_dlist_t const* dlist);

/*! the dlist last item
 *
 * @param dlist 	the dlist
 *
 * @return 			the last item
 */
tb_pointer_t 		tb_dlist_last(tb_dlist_t const* dlist);

/*! insert the prev item
 *
 * @param dlist 	the dlist
 * @param itor 		the item itor
 * @param data 		the item data
 *
 * @return 			the item itor
 */
tb_size_t 			tb_dlist_insert_prev(tb_dlist_t* dlist, tb_size_t itor, tb_cpointer_t data);

/*! insert the next item
 *
 * @param dlist 	the dlist
 * @param itor 		the item itor
 * @param data 		the item data
 *
 * @return 			the item itor
 */
tb_size_t 			tb_dlist_insert_next(tb_dlist_t* dlist, tb_size_t itor, tb_cpointer_t data);

/*! insert the head item
 *
 * @param dlist 	the dlist
 * @param data 		the item data
 *
 * @return 			the item itor
 */
tb_size_t 			tb_dlist_insert_head(tb_dlist_t* dlist, tb_cpointer_t data);

/*! insert the tail item
 *
 * @param dlist 	the dlist
 * @param data 		the item data
 *
 * @return 			the item itor
 */
tb_size_t 			tb_dlist_insert_tail(tb_dlist_t* dlist, tb_cpointer_t data);

/*! insert the prev items
 *
 * @param dlist 	the dlist
 * @param itor 		the prev item itor
 * @param data 		the item data
 * @param size 		the item count
 *
 * @return 			the first item itor
 */
tb_size_t 			tb_dlist_ninsert_prev(tb_dlist_t* dlist, tb_size_t itor, tb_cpointer_t data, tb_size_t size);

/*! insert the next items
 *
 * @param dlist 	the dlist
 * @param itor 		the prev item itor
 * @param data 		the item data
 * @param size 		the item count
 *
 * @return 			the first item itor
 */
tb_size_t 			tb_dlist_ninsert_next(tb_dlist_t* dlist, tb_size_t itor, tb_cpointer_t data, tb_size_t size);

/*! insert the head items
 *
 * @param dlist 	the dlist
 * @param data 		the item data
 * @param size 		the item count
 *
 * @return 			the first item itor
 */
tb_size_t 			tb_dlist_ninsert_head(tb_dlist_t* dlist, tb_cpointer_t data, tb_size_t size);

/*! insert the tail items
 *
 * @param dlist 	the dlist
 * @param data 		the item data
 * @param size 		the item count
 *
 * @return 			the first item itor
 */
tb_size_t 			tb_dlist_ninsert_tail(tb_dlist_t* dlist, tb_cpointer_t data, tb_size_t size);

/*! replace the item
 *
 * @param dlist 	the dlist
 * @param itor 		the item itor
 * @param data 		the item data
 *
 * @return 			the item itor
 */
tb_size_t 			tb_dlist_replace(tb_dlist_t* dlist, tb_size_t itor, tb_cpointer_t data);

/*! replace the head item
 *
 * @param dlist 	the dlist
 * @param data 		the item data
 *
 * @return 			the item itor
 */
tb_size_t 			tb_dlist_replace_head(tb_dlist_t* dlist, tb_cpointer_t data);

/*! replace the tail item
 *
 * @param dlist 	the dlist
 * @param data 		the item data
 *
 * @return 			the item itor
 */
tb_size_t 			tb_dlist_replace_last(tb_dlist_t* dlist, tb_cpointer_t data);

/*! replace the items
 *
 * @param dlist 	the dlist
 * @param itor 		the first item itor
 * @param data 		the item data
 * @param size 		the item count
 *
 * @return 			the first item itor
 */
tb_size_t 			tb_dlist_nreplace(tb_dlist_t* dlist, tb_size_t itor, tb_cpointer_t data, tb_size_t size);

/*! replace the head items
 *
 * @param dlist 	the dlist
 * @param data 		the item data
 * @param size 		the item count
 *
 * @return 			the first item itor
 */
tb_size_t 			tb_dlist_nreplace_head(tb_dlist_t* dlist, tb_cpointer_t data, tb_size_t size);

/*! replace the tail items
 *
 * @param dlist 	the dlist
 * @param data 		the item data
 * @param size 		the item count
 *
 * @return 			the first item itor
 */
tb_size_t 			tb_dlist_nreplace_last(tb_dlist_t* dlist, tb_cpointer_t data, tb_size_t size);

/*! remove the item
 *
 * @param dlist 	the dlist
 * @param itor 		the item itor
 *
 * @return 			the prev item itor
 */
tb_size_t 			tb_dlist_remove(tb_dlist_t* dlist, tb_size_t itor);

/*! remove the next item
 *
 * @param dlist 	the dlist
 * @param itor 		the item itor
 *
 * @return 			the prev item itor
 */
tb_size_t 			tb_dlist_remove_next(tb_dlist_t* dlist, tb_size_t itor);

/*! remove the head item
 *
 * @param dlist 	the dlist
 *
 * @return 			the head item itor
 */
tb_size_t 			tb_dlist_remove_head(tb_dlist_t* dlist);

/*! remove the last item
 *
 * @param dlist 	the dlist
 *
 * @return 			the last item itor
 */
tb_size_t 			tb_dlist_remove_last(tb_dlist_t* dlist);

/*! remove the next items
 *
 * @param dlist 	the dlist
 * @param itor 		the item itor
 * @param size 		the item count
 *
 * @return 			the prev item itor
 */
tb_size_t 			tb_dlist_nremove(tb_dlist_t* dlist, tb_size_t itor, tb_size_t size);

/*! remove the next items
 *
 * @param dlist 	the dlist
 * @param itor 		the item itor
 * @param size 		the item count
 *
 * @return 			the prev item itor
 */
tb_size_t 			tb_dlist_nremove_next(tb_dlist_t* dlist, tb_size_t itor, tb_size_t size);

/*! remove the head items
 *
 * @param dlist 	the dlist
 * @param size 		the item count
 *
 * @return 			the head item itor
 */
tb_size_t 			tb_dlist_nremove_head(tb_dlist_t* dlist, tb_size_t size);

/*! remove the last items
 *
 * @param dlist 	the dlist
 * @param size 		the item count
 *
 * @return 			the last item itor
 */
tb_size_t 			tb_dlist_nremove_last(tb_dlist_t* dlist, tb_size_t size);

/*! moveto the prev item
 *
 * @param dlist 	the dlist
 * @param itor 		the item itor
 * @param move 		the move itor
 *
 * @return 			the move itor
 */
tb_size_t 			tb_dlist_moveto_prev(tb_dlist_t* dlist, tb_size_t itor, tb_size_t move);

/*! moveto the next item
 *
 * @param dlist 	the dlist
 * @param itor 		the item itor
 * @param move 		the move itor
 *
 * @return 			the move itor
 */
tb_size_t 			tb_dlist_moveto_next(tb_dlist_t* dlist, tb_size_t itor, tb_size_t move);

/*! moveto the head item
 *
 * @param dlist 	the dlist
 * @param itor 		the item itor
 *
 * @return 			the move itor
 */
tb_size_t 			tb_dlist_moveto_head(tb_dlist_t* dlist, tb_size_t move);

/*! moveto the tail item
 *
 * @param dlist 	the dlist
 * @param itor 		the item itor
 *
 * @return 			the move itor
 */
tb_size_t 			tb_dlist_moveto_tail(tb_dlist_t* dlist, tb_size_t move);

/*! the item count
 *
 * @param dlist 	the dlist
 *
 * @return 			the item count
 */
tb_size_t 			tb_dlist_size(tb_dlist_t const* dlist);

/*! the item max count
 *
 * @param dlist 	the dlist
 *
 * @return 			the item max count
 */
tb_size_t 			tb_dlist_maxn(tb_dlist_t const* dlist);

/*! walk list items
 *
 * be faster than the iterator mode, optimizate to remove items for walking
 *
 * @code
 * tb_bool_t tb_dlist_item_func(tb_dlist_t* dlist, tb_pointer_t item, tb_bool_t* bdel, tb_pointer_t priv)
 * {
 * 		// check
 *  	tb_assert_and_check_return_val(dlist && bdel, tb_false);
 *
 * 		// delete it?
 * 		// *bdel = tb_true;
 *
 * 		// continue or break
 * 		return tb_true;
 * }
 * @endcode
 *
 * @param dlist 	the dlist
 * @param func 		the walk func
 * @param priv 		the walk priv
 *
 */
tb_void_t 			tb_dlist_walk(tb_dlist_t* dlist, tb_bool_t (*func)(tb_dlist_t* dlist, tb_pointer_t item, tb_bool_t* bdel, tb_pointer_t priv), tb_pointer_t priv);


#endif

