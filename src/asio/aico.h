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
 * @file		aico.h
 * @ingroup 	asio
 *
 */
#ifndef TB_ASIO_AICO_H
#define TB_ASIO_AICO_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/// the aico type enum
typedef enum __tb_aico_type_e
{
 	TB_AICO_TYPE_NONE 	= 0 	//!< for null
,	TB_AICO_TYPE_SOCK 	= 1 	//!< for sock
, 	TB_AICO_TYPE_FILE 	= 2 	//!< for file
, 	TB_AICO_TYPE_MAXN 	= 3

}tb_aico_type_e;

/// the aico type
typedef struct __tb_aico_t
{
	/// the type
	tb_size_t 			type;

	/// the handle
	tb_handle_t 		handle;

	/// the post size, must be zero or one
#ifdef __tb_debug__
	tb_atomic_t 		post;
#endif

}tb_aico_t;


#endif
