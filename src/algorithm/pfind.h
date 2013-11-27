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
 * @file		pfind.h
 * @ingroup 	algorithm
 *
 */
#ifndef TB_ALGORITHM_PFIND_H
#define TB_ALGORITHM_PFIND_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! the finder with prev 
 *
 * find the prev item if be not finded, the prev is tail if be finded
 *
 * @note the container must be sorted
 *
 * @param iterator 	the iterator
 * @param pprev 	the iterator prev pointer
 * @param head 		the iterator head
 * @param tail 		the iterator tail
 * @param data 		the finded data
 *
 * @return 			the iterator itor
 */
tb_size_t 			tb_pfind(tb_iterator_t* iterator, tb_size_t* pprev, tb_size_t head, tb_size_t tail, tb_cpointer_t data);

/*! the finder for all with prev 
 *
 * find the prev item if be not finded, the prev is tail if be finded
 *
 * @note the container must be sorted
 *
 * @param iterator 	the iterator
 * @param pprev 	the iterator prev pointer
 * @param data 		the finded data
 *
 * @return 			the iterator itor
 */
tb_size_t 			tb_pfind_all(tb_iterator_t* iterator, tb_size_t* pprev, tb_cpointer_t data);

/*! the binary finder for ascending with prev 
 *
 * find the prev item if be not finded, the prev is tail if be finded
 *
 * @note the container must be sorted
 *
 * @param iterator 	the iterator
 * @param pprev 	the iterator prev pointer
 * @param head 		the iterator head
 * @param tail 		the iterator tail
 * @param data 		the finded data
 *
 * @return 			the iterator itor
 */
tb_size_t 			tb_binary_pfind(tb_iterator_t* iterator, tb_size_t* pprev, tb_size_t head, tb_size_t tail, tb_cpointer_t data);

/*! the binary finder for all with prev 
 *
 * find the prev item if be not finded, the prev is tail if be finded
 *
 * @note the container must be sorted
 *
 * @param iterator 	the iterator
 * @param pprev 	the iterator prev pointer
 * @param data 		the finded data
 *
 * @return 			the iterator itor
 */
tb_size_t 			tb_binary_pfind_all(tb_iterator_t* iterator, tb_size_t* pprev, tb_cpointer_t data);


#endif
