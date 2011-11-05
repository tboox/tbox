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
 * \file		limits.h
 *
 */
#ifndef TB_PREFIX_LIMITS_H
#define TB_PREFIX_LIMITS_H

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

#define TB_MAXS16 				(32767)
#define TB_MINS16 				(-32767)
#define TB_MAXU16 				(0xffff)
#define TB_MINU16 				(0)
#define TB_MAXS32 				(0x7fffffff)
#define TB_MINS32 				(0x80000001)
#define TB_MAXU32 				(0xffffffff)
#define TB_MINU32 				(0)
#define TB_NAN32 				(0x80000000)


// c plus plus
#ifdef __cplusplus
}
#endif

#endif


