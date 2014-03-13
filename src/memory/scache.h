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
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * @author		ruki
 * @file		scache.h
 * @ingroup 	memory
 *
 */
#ifndef TB_MEMORY_scache_H
#define TB_MEMORY_scache_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init string cache for small, readonly and repeat strings
 *
 * readonly, strip repeat strings and decrease memory fragmens
 *
 * @param align 	the cpu align bytes
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_scache_init(tb_size_t align);

/// exit scache
tb_void_t 			tb_scache_exit(tb_noarg_t);

/// clear scache
tb_void_t 			tb_scache_clear(tb_noarg_t);

/*! put string to scache
 *
 * @param data 		the string data
 *
 * @return 			the string data
 */
tb_char_t const*	tb_scache_put(tb_char_t const* data);

/*! del string from scache
 *
 * @param data 		the string data
 */
tb_void_t 			tb_scache_del(tb_char_t const* data);

/// dump scache
tb_void_t 			tb_scache_dump(tb_noarg_t);

#endif
