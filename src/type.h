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
 * macros
 */

// bool values
#define TB_TRUE						((tb_bool_t)1)
#define	TB_FALSE					((tb_bool_t)0)

// invalidate handle
#define TB_INVALID_HANDLE 			((tb_handle_t)-1)

// null
#define TB_NULL 					((void*)0)

/* /////////////////////////////////////////////////////////
 * basic types
 */
typedef signed int			tb_int_t;
typedef unsigned int		tb_uint_t;
typedef tb_int_t			tb_bool_t;
typedef tb_uint_t			tb_size_t;
typedef signed char			tb_int8_t;
typedef tb_int8_t			tb_sint8_t;
typedef unsigned char		tb_uint8_t;
typedef signed short		tb_int16_t;
typedef tb_int16_t			tb_sint16_t;
typedef unsigned short		tb_uint16_t;
typedef tb_int_t			tb_int32_t;
typedef tb_int32_t			tb_sint32_t;
typedef tb_uint_t			tb_uint32_t;
typedef signed long long 	tb_int64_t;
typedef tb_int64_t			tb_sint64_t;
typedef unsigned long long 	tb_uint64_t;
typedef tb_uint8_t			tb_byte_t;
typedef void* 				tb_handle_t;
typedef char 				tb_char_t;
typedef double 				tb_float_t;


// c plus plus
#ifdef __cplusplus
}
#endif

#endif
