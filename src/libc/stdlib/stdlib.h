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
 * @file		stdlib.h
 * @ingroup 	libc
 *
 */
#ifndef TB_LIBC_STDLIB_H
#define TB_LIBC_STDLIB_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// for signed int32
#define tb_s2toi32(s) 				((tb_int32_t)tb_s2tou32(s))
#define tb_s8toi32(s) 				((tb_int32_t)tb_s8tou32(s))
#define tb_s10toi32(s) 				((tb_int32_t)tb_s10tou32(s))
#define tb_s16toi32(s) 				((tb_int32_t)tb_s16tou32(s))
#define tb_stoi32(s) 				((tb_int32_t)tb_stou32(s))
#define tb_sbtoi32(s, b) 			((tb_int32_t)tb_sbtou32(s, b))

// FIXME
#define tb_s2toi64(s) 				((tb_hong_t)tb_s2toi32(s))
#define tb_s8toi64(s) 				((tb_hong_t)tb_s8toi32(s))
#define tb_s10toi64(s) 				((tb_hong_t)tb_s10toi32(s))
#define tb_s16toi64(s) 				((tb_hong_t)tb_s16toi32(s))
#define tb_stoi64(s) 				((tb_hong_t)tb_stoi32(s))
#define tb_sbtoi64(s, b) 			((tb_hong_t)tb_sbtoi32(s, b))

// FIXME
#define tb_s2tou64(s) 				((tb_hize_t)tb_s2tou32(s))
#define tb_s8tou64(s) 				((tb_hize_t)tb_s8tou32(s))
#define tb_s10tou64(s) 				((tb_hize_t)tb_s10tou32(s))
#define tb_s16tou64(s) 				((tb_hize_t)tb_s16tou32(s))
#define tb_stou64(s) 				((tb_hize_t)tb_stou32(s))
#define tb_sbtou64(s, b) 			((tb_hize_t)tb_sbtou32(s, b))

// c porting
#define tb_atoi(s) 					((tb_int32_t)tb_stou32(s))
#define tb_atof(s) 					tb_stof(s)

#define tb_strtod(s, e) 			tb_stof(s)
#define tb_strtold(s, e) 			tb_stof(s)

#define tb_strtol(s, e, b) 			((tb_long_t)tb_sbtou32(s, b))
#define tb_strtoul(s, e, b) 		((tb_ulong_t)tb_sbtou32(s, b))

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_uint32_t 		tb_s2tou32(tb_char_t const* s);
tb_uint32_t 		tb_s8tou32(tb_char_t const* s);
tb_uint32_t 		tb_s10tou32(tb_char_t const* s);
tb_uint32_t 		tb_s16tou32(tb_char_t const* s);
tb_uint32_t 		tb_stou32(tb_char_t const* s);
tb_uint32_t 		tb_sbtou32(tb_char_t const* s, tb_int_t base);

#ifdef TB_CONFIG_TYPE_FLOAT

tb_float_t 			tb_s2tof(tb_char_t const* s);
tb_float_t 			tb_s8tof(tb_char_t const* s);
tb_float_t 			tb_s10tof(tb_char_t const* s);
tb_float_t 			tb_s16tof(tb_char_t const* s);
tb_float_t 			tb_stof(tb_char_t const* s);
tb_float_t 			tb_sbtof(tb_char_t const* s, tb_int_t base);

#endif


#endif
