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

// the aico type
typedef enum __tb_aico_type_t
{
 	TB_AICO_TYPE_NONE 	= 0 	//!< for null
, 	TB_AICO_TYPE_FILE 	= 1 	//!< for file
,	TB_AICO_TYPE_SOCK 	= 2 	//!< for socket

}tb_aico_type_t;


#endif
