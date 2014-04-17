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
 * @file		slist.h
 * @ingroup 	container
 *
 */
#ifndef TB_CONTAINER_SLIST_H
#define TB_CONTAINER_SLIST_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "item.h"
#include "iterator.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the slist type
typedef tb_void_t 	tb_slist_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init slist
 *
 * @param grow 		the item grow
 * @param func 		the item func
 *
 * @return 			the slist
 */
tb_slist_t* 		tb_slist_init(tb_size_t grow, tb_item_func_t func);

/*! exit slist
 *
 * @param slist 	the slist
 */
tb_void_t 			tb_slist_exit(tb_slist_t* slist);

/*! clear slist
 *
 * @param slist 	the slist
 */
tb_void_t 			tb_slist_clear(tb_slist_t* slist);

/*! the slist head item
 *
 * @param slist 	the slist
 *
 * @return 			the head item
 */
tb_pointer_t 		tb_slist_head(tb_slist_t const* slist);

/*! the slist last item
 *
 * @param slist 	the slist
 *
 * @return 			the last item
 */
tb_pointer_t 		tb_slist_last(tb_slist_t const* slist);

/*! insert the prev item
 *
 * @param slist 	the slist
 * @param itor 		the item itor
 * @param data 		the item data
 *
 * @return 			the item itor
 */
tb_size_t 			tb_slist_insert_prev(tb_slist_t* slist, tb_size_t itor, tb_cpointer_t data);

/*! insert the next item
 *
 * @param slist 	the slist
 * @param itor 		the item itor
 * @param data 		the item data
 *
 * @return 			the item itor
 */
tb_size_t 			tb_slist_insert_next(tb_slist_t* slist, tb_size_t itor, tb_cpointer_t data);

/*! insert the head item
 *
 * @param slist 	the slist
 * @param data 		the item data
 *
 * @return 			the item itor
 */
tb_size_t 			tb_slist_insert_head(tb_slist_t* slist, tb_cpointer_t data);

/*! insert the tail item
 *
 * @param slist 	the slist
 * @param data 		the item data
 *
 * @return 			the item itor
 */
tb_size_t 			tb_slist_insert_tail(tb_slist_t* slist, tb_cpointer_t data);

/*! insert the prev items
 *
 * @param slist 	the slist
 * @param itor 		the prev item itor
 * @param data 		the item data
 * @param size 		the item count
 *
 * @return 			the first item itor
 */
tb_size_t 			tb_slist_ninsert_prev(tb_slist_t* slist, tb_size_t itor, tb_cpointer_t data, tb_size_t size);

/*! insert the next items
 *
 * @param slist 	the slist
 * @param itor 		the prev item itor
 * @param data 		the item data
 * @param size 		the item count
 *
 * @return 			the first item itor
 */
tb_size_t 			tb_slist_ninsert_next(tb_slist_t* slist, tb_size_t itor, tb_cpointer_t data, tb_size_t size);

/*! insert the head items
 *
 * @param slist 	the slist
 * @param data 		the item data
 * @param size 		the item count
 *
 * @return 			the first item itor
 */
tb_size_t 			tb_slist_ninsert_head(tb_slist_t* slist, tb_cpointer_t data, tb_size_t size);

/*! insert the tail items
 *
 * @param slist 	the slist
 * @param data 		the item data
 * @param size 		the item count
 *
 * @return 			the first item itor
 */
tb_size_t 			tb_slist_ninsert_tail(tb_slist_t* slist, tb_cpointer_t data, tb_size_t size);

/*! replace the item
 *
 * @param slist 	the slist
 * @param itor 		the item itor
 * @param data 		the item data
 *
 * @return 			the item itor
 */
tb_size_t 			tb_slist_replace(tb_slist_t* slist, tb_size_t itor, tb_cpointer_t data);

/*! replace the head item
 *
 * @param slist 	the slist
 * @param data 		the item data
 *
 * @return 			the item itor
 */
tb_size_t 			tb_slist_replace_head(tb_slist_t* slist, tb_cpointer_t data);

/*! replace the tail item
 *
 * @param slist 	the slist
 * @param data 		the item data
 *
 * @return 			the item itor
 */
tb_size_t 			tb_slist_replace_last(tb_slist_t* slist, tb_cpointer_t data);

/*! replace the items
 *
 * @param slist 	the slist
 * @param itor 		the first item itor
 * @param data 		the item data
 * @param size 		the item count
 *
 * @return 			the first item itor
 */
tb_size_t 			tb_slist_nreplace(tb_slist_t* slist, tb_size_t itor, tb_cpointer_t data, tb_size_t size);

/*! replace the head items
 *
 * @param slist 	the slist
 * @param data 		the item data
 * @param size 		the item count
 *
 * @return 			the first item itor
 */
tb_size_t 			tb_slist_nreplace_head(tb_slist_t* slist, tb_cpointer_t data, tb_size_t size);

/*! replace the tail items
 *
 * @param slist 	the slist
 * @param data 		the item data
 * @param size 		the item count
 *
 * @return 			the first item itor
 */
tb_size_t 			tb_slist_nreplace_last(tb_slist_t* slist, tb_cpointer_t data, tb_size_t size);

/*! remove the item
 *
 * @param slist 	the slist
 * @param itor 		the item itor
 *
 * @return 			the prev item itor
 */
tb_size_t 			tb_slist_remove(tb_slist_t* slist, tb_size_t itor);

/*! remove the next item
 *
 * @param slist 	the slist
 * @param itor 		the item itor
 *
 * @return 			the prev item itor
 */
tb_size_t 			tb_slist_remove_next(tb_slist_t* slist, tb_size_t itor);

/*! remove the head item
 *
 * @param slist 	the slist
 *
 * @return 			the head item itor
 */
tb_size_t 			tb_slist_remove_head(tb_slist_t* slist);

/*! remove the last item
 *
 * @param slist 	the slist
 *
 * @return 			the last item itor
 */
tb_size_t 			tb_slist_remove_last(tb_slist_t* slist);

/*! remove the next items
 *
 * @param slist 	the slist
 * @param itor 		the item itor
 * @param size 		the item count
 *
 * @return 			the prev item itor
 */
tb_size_t 			tb_slist_nremove(tb_slist_t* slist, tb_size_t itor, tb_size_t size);

/*! remove the next items
 *
 * @param slist 	the slist
 * @param itor 		the item itor
 * @param size 		the item count
 *
 * @return 			the prev item itor
 */
tb_size_t 			tb_slist_nremove_next(tb_slist_t* slist, tb_size_t itor, tb_size_t size);

/*! remove the head items
 *
 * @param slist 	the slist
 * @param size 		the item count
 *
 * @return 			the head item itor
 */
tb_size_t 			tb_slist_nremove_head(tb_slist_t* slist, tb_size_t size);

/*! remove the last items
 *
 * @param slist 	the slist
 * @param size 		the item count
 *
 * @return 			the last item itor
 */
tb_size_t 			tb_slist_nremove_last(tb_slist_t* slist, tb_size_t size);

/*! moveto the prev item
 *
 * @param slist 	the slist
 * @param itor 		the item itor
 * @param move 		the move itor
 *
 * @return 			the move itor
 */
tb_size_t 			tb_slist_moveto_prev(tb_slist_t* slist, tb_size_t itor, tb_size_t move);

/*! moveto the next item
 *
 * @param slist 	the slist
 * @param itor 		the item itor
 * @param move 		the move itor
 *
 * @return 			the move itor
 */
tb_size_t 			tb_slist_moveto_next(tb_slist_t* slist, tb_size_t itor, tb_size_t move);

/*! moveto the head item
 *
 * @param slist 	the slist
 * @param itor 		the item itor
 *
 * @return 			the move itor
 */
tb_size_t 			tb_slist_moveto_head(tb_slist_t* slist, tb_size_t move);

/*! moveto the tail item
 *
 * @param slist 	the slist
 * @param itor 		the item itor
 *
 * @return 			the move itor
 */
tb_size_t 			tb_slist_moveto_tail(tb_slist_t* slist, tb_size_t move);

/*! the item count
 *
 * @param slist 	the slist
 *
 * @return 			the item count
 */
tb_size_t 			tb_slist_size(tb_slist_t const* slist);

/*! the item max count
 *
 * @param slist 	the slist
 *
 * @return 			the item max count
 */
tb_size_t 			tb_slist_maxn(tb_slist_t const* slist);

/*! walk list items
 *
 * be faster than the iterator mode, optimizate to remove items for walking
 *
 * @code
 * tb_bool_t tb_slist_item_func(tb_slist_t* slist, tb_pointer_t item, tb_bool_t* bdel, tb_pointer_t priv)
 * {
 *  	tb_assert_and_check_return_val(slist && bdel, tb_false);
 *
 * 		// delete it?
 * 		// *bdel = tb_true;
 *
 * 		// continue or break
 * 		return tb_true;
 * }
 * @endcode
 *
 * @param slist 	the slist
 * @param func 		the walk func
 * @param priv 		the walk priv
 *
 */
tb_void_t 			tb_slist_walk(tb_slist_t* slist, tb_bool_t (*func)(tb_slist_t* slist, tb_pointer_t item, tb_bool_t* bdel, tb_pointer_t priv), tb_pointer_t priv);



#endif

