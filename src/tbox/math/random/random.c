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
 * @file        random.c
 * @ingroup     math
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "random.h"
#include "../../libc/libc.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the initial seed
#define TB_RANDOM_SEED_INIT     (2166136261ul)

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_void_t tb_random_seed(tb_size_t seed)
{
    tb_random_linear_seed(seed);
}
tb_void_t tb_random_reset()
{
    tb_random_seed(TB_RANDOM_SEED_INIT);
}
tb_long_t tb_random_value()
{
    return tb_random_linear_value();
}
tb_long_t tb_random_range(tb_long_t begin, tb_long_t end)
{
    // check
    tb_assert_and_check_return_val(begin < end, begin);

    // make range
    return (begin + (tb_long_t)((tb_size_t)tb_random_value() % (end - begin)));
}
#ifdef TB_CONFIG_TYPE_HAVE_FLOAT
tb_float_t tb_random_rangef(tb_float_t begin, tb_float_t end)
{
    // check
    tb_assert_and_check_return_val(begin < end, begin);

    // the factor
    tb_double_t factor = (tb_double_t)tb_random_range(0, TB_MAXS32) / (tb_double_t)TB_MAXS32;

    // the value
    return (tb_float_t)((end - begin) * factor);
}
#endif

