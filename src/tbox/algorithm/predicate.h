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
 * @file        predicate.h
 * @ingroup     algorithm
 */
#ifndef TB_ALGORITHM_PREDICATE_H
#define TB_ALGORITHM_PREDICATE_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/*! the unary predicate func type
 *
 * @param iterator  the iterator
 * @param item      the inner item of the container
 * @param value     the outer value
 *
 * @return          tb_true or tb_false
 */
typedef tb_bool_t   (*tb_iterator_pred_unary_t)(tb_iterator_ref_t iterator, tb_cpointer_t item, tb_cpointer_t value);

/*! the binary predicate func type
 *
 * @param iterator  the iterator
 * @param item      the inner item of the container
 * @param value1    the first outer value
 * @param value2    the second outer value
 *
 * @return          tb_true or tb_false
 */
typedef tb_bool_t   (*tb_iterator_pred_binary_t)(tb_iterator_ref_t iterator, tb_cpointer_t item, tb_cpointer_t value1, tb_cpointer_t value2);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! the unary predicate: if (item == value)?
 *
 * @param iterator  the iterator
 * @param item      the inner item of the container
 * @param value     the outer value
 *
 * @return          tb_true or tb_false
 */
tb_bool_t           tb_iterator_pred_eq(tb_iterator_ref_t iterator, tb_cpointer_t item, tb_cpointer_t value);

/*! the unary predicate: if (item < value)?
 *
 * @param iterator  the iterator
 * @param item      the inner item of the container
 * @param value     the outer value
 *
 * @return          tb_true or tb_false
 */
tb_bool_t           tb_iterator_pred_le(tb_iterator_ref_t iterator, tb_cpointer_t item, tb_cpointer_t value);

/*! the unary predicate: if (item > value)?
 *
 * @param iterator  the iterator
 * @param item      the inner item of the container
 * @param value     the outer value
 *
 * @return          tb_true or tb_false
 */
tb_bool_t           tb_iterator_pred_be(tb_iterator_ref_t iterator, tb_cpointer_t item, tb_cpointer_t value);

/*! the unary predicate: if (item <= value)?
 *
 * @param iterator  the iterator
 * @param item      the inner item of the container
 * @param value     the outer value
 *
 * @return          tb_true or tb_false
 */
tb_bool_t           tb_iterator_pred_leq(tb_iterator_ref_t iterator, tb_cpointer_t item, tb_cpointer_t value);

/*! the unary predicate: if (item >= value)?
 *
 * @param iterator  the iterator
 * @param item      the inner item of the container
 * @param value     the outer value
 *
 * @return          tb_true or tb_false
 */
tb_bool_t           tb_iterator_pred_beq(tb_iterator_ref_t iterator, tb_cpointer_t item, tb_cpointer_t value);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__
#endif
