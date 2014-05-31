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
 * @file        list.h
 * @ingroup     container
 *
 */
#ifndef TB_CONTAINER_LIST_H
#define TB_CONTAINER_LIST_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "item.h"
#include "iterator.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the list type
typedef tb_void_t   tb_list_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init list
 *
 * @param grow      the item grow
 * @param func      the item func
 *
 * @return          the list
 */
tb_list_t*          tb_list_init(tb_size_t grow, tb_item_func_t func);

/*! exit list
 *
 * @param list      the list
 */
tb_void_t           tb_list_exit(tb_list_t* list);

/*! clear list
 *
 * @param list      the list
 */
tb_void_t           tb_list_clear(tb_list_t* list);

/*! the list head item
 *
 * @param list      the list
 *
 * @return          the head item
 */
tb_pointer_t        tb_list_head(tb_list_t const* list);

/*! the list last item
 *
 * @param list      the list
 *
 * @return          the last item
 */
tb_pointer_t        tb_list_last(tb_list_t const* list);

/*! insert the prev item
 *
 * @param list      the list
 * @param itor      the item itor
 * @param data      the item data
 *
 * @return          the item itor
 */
tb_size_t           tb_list_insert_prev(tb_list_t* list, tb_size_t itor, tb_cpointer_t data);

/*! insert the next item
 *
 * @param list      the list
 * @param itor      the item itor
 * @param data      the item data
 *
 * @return          the item itor
 */
tb_size_t           tb_list_insert_next(tb_list_t* list, tb_size_t itor, tb_cpointer_t data);

/*! insert the head item
 *
 * @param list      the list
 * @param data      the item data
 *
 * @return          the item itor
 */
tb_size_t           tb_list_insert_head(tb_list_t* list, tb_cpointer_t data);

/*! insert the tail item
 *
 * @param list      the list
 * @param data      the item data
 *
 * @return          the item itor
 */
tb_size_t           tb_list_insert_tail(tb_list_t* list, tb_cpointer_t data);

/*! insert the prev items
 *
 * @param list      the list
 * @param itor      the prev item itor
 * @param data      the item data
 * @param size      the item count
 *
 * @return          the first item itor
 */
tb_size_t           tb_list_ninsert_prev(tb_list_t* list, tb_size_t itor, tb_cpointer_t data, tb_size_t size);

/*! insert the next items
 *
 * @param list      the list
 * @param itor      the prev item itor
 * @param data      the item data
 * @param size      the item count
 *
 * @return          the first item itor
 */
tb_size_t           tb_list_ninsert_next(tb_list_t* list, tb_size_t itor, tb_cpointer_t data, tb_size_t size);

/*! insert the head items
 *
 * @param list      the list
 * @param data      the item data
 * @param size      the item count
 *
 * @return          the first item itor
 */
tb_size_t           tb_list_ninsert_head(tb_list_t* list, tb_cpointer_t data, tb_size_t size);

/*! insert the tail items
 *
 * @param list      the list
 * @param data      the item data
 * @param size      the item count
 *
 * @return          the first item itor
 */
tb_size_t           tb_list_ninsert_tail(tb_list_t* list, tb_cpointer_t data, tb_size_t size);

/*! replace the item
 *
 * @param list      the list
 * @param itor      the item itor
 * @param data      the item data
 *
 * @return          the item itor
 */
tb_size_t           tb_list_replace(tb_list_t* list, tb_size_t itor, tb_cpointer_t data);

/*! replace the head item
 *
 * @param list      the list
 * @param data      the item data
 *
 * @return          the item itor
 */
tb_size_t           tb_list_replace_head(tb_list_t* list, tb_cpointer_t data);

/*! replace the tail item
 *
 * @param list      the list
 * @param data      the item data
 *
 * @return          the item itor
 */
tb_size_t           tb_list_replace_last(tb_list_t* list, tb_cpointer_t data);

/*! replace the items
 *
 * @param list      the list
 * @param itor      the first item itor
 * @param data      the item data
 * @param size      the item count
 *
 * @return          the first item itor
 */
tb_size_t           tb_list_nreplace(tb_list_t* list, tb_size_t itor, tb_cpointer_t data, tb_size_t size);

/*! replace the head items
 *
 * @param list      the list
 * @param data      the item data
 * @param size      the item count
 *
 * @return          the first item itor
 */
tb_size_t           tb_list_nreplace_head(tb_list_t* list, tb_cpointer_t data, tb_size_t size);

/*! replace the tail items
 *
 * @param list      the list
 * @param data      the item data
 * @param size      the item count
 *
 * @return          the first item itor
 */
tb_size_t           tb_list_nreplace_last(tb_list_t* list, tb_cpointer_t data, tb_size_t size);

/*! remove the item
 *
 * @param list      the list
 * @param itor      the item itor
 *
 * @return          the prev item itor
 */
tb_size_t           tb_list_remove(tb_list_t* list, tb_size_t itor);

/*! remove the next item
 *
 * @param list      the list
 * @param itor      the item itor
 *
 * @return          the prev item itor
 */
tb_size_t           tb_list_remove_next(tb_list_t* list, tb_size_t itor);

/*! remove the head item
 *
 * @param list      the list
 *
 * @return          the head item itor
 */
tb_size_t           tb_list_remove_head(tb_list_t* list);

/*! remove the last item
 *
 * @param list      the list
 *
 * @return          the last item itor
 */
tb_size_t           tb_list_remove_last(tb_list_t* list);

/*! remove the next items
 *
 * @param list      the list
 * @param itor      the item itor
 * @param size      the item count
 *
 * @return          the prev item itor
 */
tb_size_t           tb_list_nremove(tb_list_t* list, tb_size_t itor, tb_size_t size);

/*! remove the next items
 *
 * @param list      the list
 * @param itor      the item itor
 * @param size      the item count
 *
 * @return          the prev item itor
 */
tb_size_t           tb_list_nremove_next(tb_list_t* list, tb_size_t itor, tb_size_t size);

/*! remove the head items
 *
 * @param list      the list
 * @param size      the item count
 *
 * @return          the head item itor
 */
tb_size_t           tb_list_nremove_head(tb_list_t* list, tb_size_t size);

/*! remove the last items
 *
 * @param list      the list
 * @param size      the item count
 *
 * @return          the last item itor
 */
tb_size_t           tb_list_nremove_last(tb_list_t* list, tb_size_t size);

/*! moveto the prev item
 *
 * @param list      the list
 * @param itor      the item itor
 * @param move      the move itor
 *
 * @return          the move itor
 */
tb_size_t           tb_list_moveto_prev(tb_list_t* list, tb_size_t itor, tb_size_t move);

/*! moveto the next item
 *
 * @param list      the list
 * @param itor      the item itor
 * @param move      the move itor
 *
 * @return          the move itor
 */
tb_size_t           tb_list_moveto_next(tb_list_t* list, tb_size_t itor, tb_size_t move);

/*! moveto the head item
 *
 * @param list      the list
 * @param itor      the item itor
 *
 * @return          the move itor
 */
tb_size_t           tb_list_moveto_head(tb_list_t* list, tb_size_t move);

/*! moveto the tail item
 *
 * @param list      the list
 * @param itor      the item itor
 *
 * @return          the move itor
 */
tb_size_t           tb_list_moveto_tail(tb_list_t* list, tb_size_t move);

/*! the item count
 *
 * @param list      the list
 *
 * @return          the item count
 */
tb_size_t           tb_list_size(tb_list_t const* list);

/*! the item max count
 *
 * @param list      the list
 *
 * @return          the item max count
 */
tb_size_t           tb_list_maxn(tb_list_t const* list);

/*! walk list items
 *
 * be faster than the iterator mode, optimizate to remove items for walking
 *
 * @code
 * tb_bool_t tb_list_item_func(tb_list_t* list, tb_pointer_t item, tb_bool_t* bdel, tb_cpointer_t priv)
 * {
 *      // check
 *      tb_assert_and_check_return_val(list && bdel, tb_false);
 *
 *      // delete it?
 *      // *bdel = tb_true;
 *
 *      // continue or break
 *      return tb_true;
 * }
 * @endcode
 *
 * @param list      the list
 * @param func      the walk func
 * @param priv      the walk priv
 *
 */
tb_void_t           tb_list_walk(tb_list_t* list, tb_bool_t (*func)(tb_list_t* list, tb_pointer_t item, tb_bool_t* bdel, tb_cpointer_t priv), tb_cpointer_t priv);


#endif

