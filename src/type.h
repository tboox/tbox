/*!The Tiny Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		type.h
 *
 */
#ifndef TB_TYPE_H
#define TB_TYPE_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "tplat/tplat.h"
#include "option.h"

/* /////////////////////////////////////////////////////////
 * macros
 */

// bool values
#define TB_TRUE					TPLAT_TRUE
#define	TB_FALSE				TPLAT_FALSE

// invalidate handle
#define TB_INVALID_HANDLE 		TPLAT_INVALID_HANDLE

// null
#define TB_NULL 				TPLAT_NULL

/* /////////////////////////////////////////////////////////
 * basic types
 */
typedef tplat_int_t				tb_int_t;
typedef tplat_uint_t			tb_uint_t;
typedef tplat_bool_t			tb_bool_t;
typedef tplat_size_t			tb_size_t;
typedef tplat_int8_t			tb_int8_t;
typedef tplat_sint8_t			tb_sint8_t;
typedef tplat_uint8_t			tb_uint8_t;
typedef tplat_int16_t			tb_int16_t;
typedef tplat_sint16_t			tb_sint16_t;
typedef tplat_uint16_t			tb_uint16_t;
typedef tplat_int32_t			tb_int32_t;
typedef tplat_sint32_t			tb_sint32_t;
typedef tplat_uint32_t			tb_uint32_t;
typedef tplat_byte_t			tb_byte_t;
typedef tplat_handle_t			tb_handle_t;
typedef tplat_char_t 			tb_char_t;
typedef tplat_float_t 			tb_float_t;
typedef tplat_int64_t 			tb_int64_t;
typedef tplat_sint64_t			tb_sint64_t;
typedef tplat_uint64_t 			tb_uint64_t;


// c plus plus
#ifdef __cplusplus
}
#endif

#endif
