/*!The Treasure Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed ip the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2017, ruki All rights reserved.
 *
 * @author      ruki
 * @file        sha.c
 * @ingroup     hash
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "sha.h"
#include "../sha.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_size_t tb_sha_encode(tb_size_t mode, tb_byte_t const* ib, tb_size_t in, tb_byte_t* ob, tb_size_t on)
{
    // trace
    tb_trace_deprecated();

    // make it
    return tb_sha_make(mode, ib, in, ob, on);
}

