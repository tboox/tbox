/*!The Tiny Box Library
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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		type.h
 *
 */
#ifndef TB_PREFIX_TYPE_H
#define TB_PREFIX_TYPE_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "config.h"

/* /////////////////////////////////////////////////////////
 * macros
 */

// bool values
#define TB_TRUE						((tb_bool_t)1)
#define	TB_FALSE					((tb_bool_t)0)

// null
#ifdef __cplusplus
# 	define TB_NULL 					(0)
#else
# 	define TB_NULL 					((tb_void_t*)0)
#endif

// check config
#if !defined(TB_CONFIG_TYPE_SCALAR_IS_FIXED) \
	&& !defined(TB_CONFIG_TYPE_FLOAT)
# 	define TB_CONFIG_TYPE_SCALAR_IS_FIXED
#endif

/* /////////////////////////////////////////////////////////
 * types
 */

// basic
typedef signed int				tb_int_t;
typedef unsigned int			tb_uint_t;
typedef signed long				tb_long_t;
typedef unsigned long			tb_ulong_t;
typedef tb_ulong_t				tb_size_t;
typedef tb_int_t				tb_bool_t;
typedef signed char				tb_int8_t;
typedef tb_int8_t				tb_sint8_t;
typedef unsigned char			tb_uint8_t;
typedef signed short			tb_int16_t;
typedef tb_int16_t				tb_sint16_t;
typedef unsigned short			tb_uint16_t;
typedef tb_int_t				tb_int32_t;
typedef tb_int32_t				tb_sint32_t;
typedef tb_uint_t				tb_uint32_t;
typedef char 					tb_char_t;
typedef tb_int32_t 				tb_wchar_t;
typedef tb_int32_t 				tb_uchar_t;
typedef tb_uint8_t				tb_byte_t;
typedef void 					tb_void_t;
typedef tb_void_t* 				tb_handle_t;
typedef tb_void_t* 				tb_pointer_t;
typedef tb_void_t const* 		tb_cpointer_t;

// int64
#ifdef TB_CONFIG_TYPE_INT64
typedef signed long long 		tb_int64_t;
typedef unsigned long long 		tb_uint64_t;
typedef tb_int64_t				tb_sint64_t;
#else
typedef struct __tb_sint64_t
{
	tb_uint32_t l;
	tb_sint32_t h;
} 								tb_sint64_t;
typedef struct __tb_uint64_t
{
	tb_uint32_t l;
	tb_uint32_t h;
} 								tb_uint64_t;
typedef tb_sint64_t 			tb_int64_t;
#endif

// float
#ifdef TB_CONFIG_TYPE_FLOAT
typedef double 					tb_float_t;
#endif

// fixed
typedef tb_int32_t 				tb_fixed6_t;
typedef tb_int32_t 				tb_fixed16_t;
typedef tb_int32_t 				tb_fixed30_t;
typedef tb_fixed16_t 			tb_fixed_t;

// scalar
#ifdef TB_CONFIG_TYPE_SCALAR_IS_FIXED
typedef tb_fixed_t 				tb_scalar_t;
#else
typedef tb_float_t 				tb_scalar_t;
#endif

// c plus plus
#ifdef __cplusplus
}
#endif

#endif


