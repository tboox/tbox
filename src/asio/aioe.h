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
 * @file		aioe.h
 * @ingroup 	asio
 *
 */
#ifndef TB_ASIO_AIOE_H
#define TB_ASIO_AIOE_H


/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/// the aioe code enum, only for sock
typedef enum __tb_aioe_code_e
{
 	TB_AIOE_CODE_NONE 		= 0
, 	TB_AIOE_CODE_CONN 		= 1
, 	TB_AIOE_CODE_ACPT 		= 2
,	TB_AIOE_CODE_RECV 		= 4
,	TB_AIOE_CODE_SEND 		= 8
, 	TB_AIOE_CODE_EALL 		= TB_AIOE_CODE_RECV | TB_AIOE_CODE_SEND | TB_AIOE_CODE_ACPT | TB_AIOE_CODE_CONN

}tb_aioe_code_e;

/// the aioe flag enum
typedef enum __tb_aioe_flag_e
{
 	TB_AIOE_FLAG_ADDE 		= 0x00
, 	TB_AIOE_FLAG_DELE 		= 0x01
, 	TB_AIOE_FLAG_SETE 		= 0x02
,	TB_AIOE_FLAG_ET 		= 0x10 //!< edge triggered, @note maybe not supported, lookup aiop feature first
,	TB_AIOE_FLAG_ONESHOT 	= 0x20 //!< one shot

}tb_aioe_flag_e;

/// the aioe type
typedef struct __tb_aioe_t
{
	// the code
	tb_size_t 				code : 8;

	// the flag
	tb_size_t 				flag : 8;

	// the handle 
	tb_handle_t 			handle;

	// the data
	tb_pointer_t 			data;

}tb_aioe_t;


#endif
