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
 * @file        memmem.c
 * @ingroup     libc
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "string.h"
#include "../../memory/impl/prefix.h"
#ifdef TB_CONFIG_LIBC_HAVE_MEMMEM
#   include <string.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation 
 */
#if defined(TB_CONFIG_LIBC_HAVE_MEMMEM)
static tb_pointer_t tb_memmem_impl(tb_cpointer_t s1, tb_size_t n1, tb_cpointer_t s2, tb_size_t n2)
{
    // check
    tb_assert_and_check_return_val(s1 && s2, tb_null);

    // done
    return memmem(s1, n1, s2, n2);
}
#elif !defined(TB_LIBC_STRING_IMPL_MEMMEM)
static tb_pointer_t tb_memmem_impl(tb_cpointer_t s1, tb_size_t n1, tb_cpointer_t s2, tb_size_t n2)
{
    // check
    tb_assert_and_check_return_val(s1 && s2, tb_null);

    // find empty data?
	if (!n2) return (tb_pointer_t)s1;

    // done
	if (n1 >= n2) 
    {
        tb_size_t           n = 0;
		tb_byte_t const*    ph = (tb_byte_t const*)s1;
		tb_byte_t const*    pn = (tb_byte_t const*)s2;
		tb_byte_t const*    plast = ph + (n1 - n2);
		do 
        {
			n = 0;
			while (ph[n] == pn[n]) 
            {
                // found?
				if (++n == n2) return (tb_pointer_t)ph;
			}

		} while (++ph <= plast);
	}

    // not found?
	return tb_null;
}
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces 
 */
tb_pointer_t tb_memmem_(tb_cpointer_t s1, tb_size_t n1, tb_cpointer_t s2, tb_size_t n2)
{
    // done
    return tb_memmem_impl(s1, n1, s2, n2);
}
tb_pointer_t tb_memmem(tb_cpointer_t s1, tb_size_t n1, tb_cpointer_t s2, tb_size_t n2)
{
    // check
#ifdef __tb_debug__
    // TODO
#endif

    // done
    return tb_memmem_impl(s1, n1, s2, n2);
}
