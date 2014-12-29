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
 * @file        prefix.h
 * @ingroup     object
 *
 */
#ifndef TB_OBJECT_IMPL_PREFIX_H
#define TB_OBJECT_IMPL_PREFIX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"
#include "../object.h"
#include "../../charset/charset.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// need bytes
#define tb_object_need_bytes(x)     \
                                    (((tb_uint64_t)(x)) < (1ull << 8) ? 1 : \
                                    (((tb_uint64_t)(x)) < (1ull << 16) ? 2 : \
                                    (((tb_uint64_t)(x)) < (1ull << 32) ? 4 : 8)))

#endif
