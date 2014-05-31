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
 * @file        base32.h
 * @ingroup     utils
 *
 */
#ifndef TB_UTILS_BASE32_H
#define TB_UTILS_BASE32_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! encode base32
 *
 * @param ib        the input data
 * @param in        the input size
 * @param ob        the output data
 * @param on        the output size
 *
 * @return          the real size
 */
tb_size_t           tb_base32_encode(tb_byte_t const* ib, tb_size_t in, tb_char_t* ob, tb_size_t on);

/*! decode base32
 *
 * @param ib        the input data
 * @param in        the input size
 * @param ob        the output data
 * @param on        the output size
 *
 * @return          the real size
 */
tb_size_t           tb_base32_decode(tb_byte_t const* ib, tb_size_t in, tb_char_t* ob, tb_size_t on);

#endif

