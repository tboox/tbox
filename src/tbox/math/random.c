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
#include "impl/random.h"
#include "../utils/utils.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * instance implementation
 */
static tb_handle_t tb_random_instance_init(tb_cpointer_t* ppriv)
{
    // init it
    return (tb_handle_t)tb_random_init(TB_RANDOM_TYPE_LINEAR, 2166136261ul);
}
static tb_void_t tb_random_instance_exit(tb_handle_t handle, tb_cpointer_t priv)
{
    // exit it
    tb_random_exit((tb_random_ref_t)handle);
}
static tb_random_impl_t* tb_random_instance(tb_noarg_t)
{
    return (tb_random_impl_t*)tb_singleton_instance(TB_SINGLETON_TYPE_RANDOM, tb_random_instance_init, tb_random_instance_exit, tb_null, tb_null);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_random_ref_t tb_random_init(tb_size_t type, tb_size_t seed)
{
    // the init func
    static tb_random_impl_t* (*s_init[])(tb_size_t ) = 
    {
        tb_null
    ,   tb_random_linear_init
    };
    tb_assert_and_check_return_val(type < tb_arrayn(s_init) && s_init[type], tb_null);

    // init it
    return (tb_random_ref_t)s_init[type](seed);
}
tb_void_t tb_random_exit(tb_random_ref_t random)
{
    // check
    tb_random_impl_t* impl = (tb_random_impl_t*)random;
    tb_assert_and_check_return(impl);

    // exit it
    if (impl->exit) impl->exit(impl);
}
tb_void_t tb_random_seed(tb_random_ref_t random, tb_size_t seed)
{
    // check
    tb_random_impl_t* impl = random? (tb_random_impl_t*)random : tb_random_instance();
    tb_assert_and_check_return(impl && impl->seed);

    // seed it
    impl->seed(impl, seed);
}
tb_void_t tb_random_clear(tb_random_ref_t random)
{
    // check
    tb_random_impl_t* impl = random? (tb_random_impl_t*)random : tb_random_instance();
    tb_assert_and_check_return(impl && impl->clear);

    // clear it
    impl->clear(impl);
}
tb_long_t tb_random_range(tb_random_ref_t random, tb_long_t beg, tb_long_t end)
{
    // check
    tb_random_impl_t* impl = random? (tb_random_impl_t*)random : tb_random_instance();
    tb_assert_and_check_return_val(impl && impl->range && beg < end, beg);

    // range it
    return impl->range(impl, beg, end);
}
#ifdef TB_CONFIG_TYPE_HAVE_FLOAT
tb_float_t tb_random_rangef(tb_random_ref_t random, tb_float_t beg, tb_float_t end)
{
    // check
    tb_assert_and_check_return_val(beg < end, beg);

    // the factor
    tb_double_t factor = (tb_double_t)tb_random_range(random, 0, TB_MAXS32) / (tb_double_t)TB_MAXS32;

    // the value
    return (tb_float_t)((end - beg) * factor);
}
#endif
tb_long_t tb_random_value(tb_random_ref_t random)
{
#if TB_CPU_BIT64
    return tb_random_range(random, 0, TB_MAXS64);
#else
    return tb_random_range(random, 0, TB_MAXS32);
#endif
}

