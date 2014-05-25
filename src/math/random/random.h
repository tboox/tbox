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
 * @file        random.h
 * @ingroup     math
 *
 */
#ifndef TB_MATH_RANDOM_H
#define TB_MATH_RANDOM_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! the random generator instance
 * 
 * @return          the random handle
 */
tb_handle_t         tb_random_generator(tb_noarg_t);

/*! init random
 * 
 * @param type      the generator type
 * @param seed      the random seed
 *
 * @return          the random handle
 */
tb_handle_t         tb_random_init(tb_size_t type, tb_size_t seed);

/*! exit random
 *
 * @param random    the random handle
 */
tb_void_t           tb_random_exit(tb_handle_t random);

/*! update random seed
 *
 * @param random    the random handle
 * @param seed      the random seed
 */
tb_void_t           tb_random_seed(tb_handle_t random, tb_size_t seed);

/*! clear cache value and reset to the initial value
 *
 * @param random    the random handle
 */
tb_void_t           tb_random_clear(tb_handle_t random);

/*! generate the random with range: [beg, end)
 *
 * @param random    the random handle
 * @param beg       the begin value
 * @param end       the end value
 *
 * @return          the random value
 */
tb_long_t           tb_random_range(tb_handle_t random, tb_long_t beg, tb_long_t end);

#ifdef TB_CONFIG_TYPE_FLOAT
/*! generate the float random with range: [beg, end)
 *
 * @param random    the random handle
 * @param beg       the begin value
 * @param end       the end value
 *
 * @return          the random value
 */
tb_float_t          tb_random_rangef(tb_handle_t random, tb_float_t beg, tb_float_t end);
#endif

/*! generate the random with range: [0, max)
 *
 * @param random    the random handle
 *
 * @return          the random value
 */
tb_long_t           tb_random_value(tb_handle_t random);

#endif

