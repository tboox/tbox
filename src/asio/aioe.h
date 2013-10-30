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
 	TB_AIOE_CODE_NONE 	= 0
, 	TB_AIOE_CODE_CONN 	= 1
, 	TB_AIOE_CODE_ACPT 	= 2
,	TB_AIOE_CODE_RECV 	= 4
,	TB_AIOE_CODE_SEND 	= 8
, 	TB_AIOE_CODE_EALL 	= TB_AIOE_CODE_RECV | TB_AIOE_CODE_SEND | TB_AIOE_CODE_ACPT | TB_AIOE_CODE_CONN

}tb_aioe_code_e;


#endif
