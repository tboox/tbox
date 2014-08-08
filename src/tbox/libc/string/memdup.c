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
 * @file        memdup.c
 * @ingroup     libc
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "string.h"
#include "../../memory/impl/prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces 
 */
tb_pointer_t tb_memdup_(tb_cpointer_t s, tb_size_t n)
{
    // check
    tb_assert_and_check_return_val(s, tb_null);

    // done
    __tb_register__ tb_pointer_t p = tb_malloc(n);
    if (p) tb_memcpy_(p, s, n);
    return p;
}
tb_pointer_t tb_memdup(tb_cpointer_t s, tb_size_t n)
{
    // check
    tb_assert_and_check_return_val(s, tb_null);
    
    // check
#ifdef __tb_debug__
    {
        // overflow?
        tb_size_t size = tb_pool_data_size(s);
        if (size && n > size)
        {
            tb_trace_i("[memdup]: [overflow]: [%p, %lu] from [%p, %lu]", s, n, s, size);
            tb_backtrace_dump("[memdup]: [overflow]: ", tb_null, 10);
            tb_pool_data_dump(s, tb_true, "\t[malloc]: [from]: ");
            tb_abort();
        }
    }
#endif

    // done
    __tb_register__ tb_pointer_t p = tb_malloc(n);
    if (p) tb_memcpy(p, s, n);
    return p;
}
