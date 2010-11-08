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
 * \file		conv.h
 *
 */
#ifndef TB_CONV_H
#define TB_CONV_H

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <stdlib.h>

/* ////////////////////////////////////////////////////////////////////////
 * macros
 */

// is
#define TB_CONV_ISSPACE(x) 					(isspace(x))
#define TB_CONV_ISALPHA(x) 					(isalpha(x))
#define TB_CONV_ISDIGIT(x) 					(isdigit(x))
#define TB_CONV_ISUPPER(x) 					(isupper(x))
#define TB_CONV_ISLOWER(x) 					(islower(x))
#define TB_CONV_ISASCII(x) 					(isascii(x))

// int to string
#define TB_CONV_ITOA(x) 					(itoa((x)))
#define TB_CONV_LTOA(x) 					(ltoa((x)))
#define TB_CONV_LLTOA(x) 					(lltoa((x)))
#define TB_CONV_UTOA(x) 					(utoa((x)))
#define TB_CONV_ULTOA(x) 					(ultoa((x)))

// string to int
#define TB_CONV_ATOI(x) 					(atoi((x)))
#define TB_CONV_ATOL(x) 					(atol((x)))
#define TB_CONV_ATOLL(x) 					(atoll((x)))

// float to string
//#define TB_CONV_FTOA(x, s, n) 					
//#define TB_CONV_LFTOA(x, s, n) 					

// string to float
#define TB_CONV_ATOF(x) 					(atof((x)))
#define TB_CONV_ATOLF(x) 					(strtod((x), TB_NULL)) // is double?

// to lower & upper
#define TB_CONV_TOLOWER(x) 					(tolower(x))
#define TB_CONV_TOUPPER(x) 					(toupper(x))

/* ////////////////////////////////////////////////////////////////////////
 * interfaces
 */



#endif

