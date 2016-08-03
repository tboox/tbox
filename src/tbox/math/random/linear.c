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
 * @file        linear.c
 * @ingroup     math
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "random_linear"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "linear.h"
#include "../../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the value
static tb_size_t        g_value = 2166136261ul;

// the lock
static tb_spinlock_t    g_lock = TB_SPINLOCK_INIT;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_void_t tb_random_linear_seed(tb_size_t seed)
{
    // enter 
    tb_spinlock_enter(&g_lock);

    // update value
    g_value = seed;

    // leave
    tb_spinlock_leave(&g_lock);
}
tb_long_t tb_random_linear_value()
{
    // enter 
    tb_spinlock_enter(&g_lock);

    // generate the next value
    g_value = (g_value * 10807 + 1) & 0xffffffff;

    // leave 
    tb_spinlock_leave(&g_lock);

    // ok
    return (tb_long_t)g_value;
}
