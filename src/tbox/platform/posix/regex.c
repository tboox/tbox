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
 * @file        regex.c
 * @ingroup     platform
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <regex.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_regex_ref_t tb_regex_init(tb_char_t const* pattern, tb_size_t mode)
{
    tb_assert_noimpl();
    return tb_null;
}
tb_void_t tb_regex_exit(tb_regex_ref_t regex)
{
    tb_assert_noimpl();
}
tb_long_t tb_regex_match(tb_regex_ref_t regex, tb_char_t const* cstr, tb_size_t size, tb_size_t start, tb_size_t* plength, tb_vector_ref_t* presults)
{
    tb_assert_noimpl();
    return -1;
}
tb_char_t const* tb_regex_replace(tb_regex_ref_t regex, tb_char_t const* cstr, tb_size_t size, tb_size_t start, tb_char_t const* replace_cstr, tb_size_t replace_size, tb_size_t* plength)
{
    tb_assert_noimpl();
    return tb_null;
}
