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
 * @file		cspool.h
 * @ingroup 	memory
 *
 */
#ifndef TB_MEMORY_CSPOOL_H
#define TB_MEMORY_CSPOOL_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init const string pool
 *
 * readonly, strip repeat strings and decrease memory fragmens
 *
 * @param align 	the cpu align bytes
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_cspool_init(tb_size_t align);

/// exit cspool
tb_void_t 			tb_cspool_exit();

/// clear cspool
tb_void_t 			tb_cspool_clear();

/*! duplicate string to cspool
 *
 * @param data 		the string data
 *
 * @return 			the string data
 */
tb_char_t*			tb_cspool_strdup(tb_char_t const* data);

/*! duplicate string to cspool with size
 *
 * @param data 		the string data
 * @param size 		the string size
 *
 * @return 			the string data
 */
tb_char_t* 			tb_cspool_strndup(tb_char_t const* data, tb_size_t size);

/*! free string from cspool
 *
 * @param data 		the string data
 */
tb_void_t 			tb_cspool_free(tb_char_t* data);

/// dump cspool
tb_void_t 			tb_cspool_dump();

#endif
