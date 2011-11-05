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
 * \file		check.h
 *
 */
#ifndef TB_PREFIX_CHECK_H
#define TB_PREFIX_CHECK_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "config.h"
#include "trace.h"
#include "abort.h"

/* /////////////////////////////////////////////////////////
 * macros
 */

// check
#define TB_IF_FAIL_RETURN(x) 							do { if (!(x)) return ; } while (0)
#define TB_IF_FAIL_RETURN_VAL(x, v) 					do { if (!(x)) return (v); } while (0)
#define TB_IF_FAIL_GOTO(x, b) 							do { if (!(x)) goto b; } while (0)
#define TB_IF_FAIL_BREAK(x) 							{ if (!(x)) break ; }
#define TB_IF_FAIL_ABORT(x) 							do { if (!(x)) {TB_ABORT();} } while (0)
#define TB_IF_FAIL_CONTINUE(x) 							{ if (!(x)) continue ; }

// c plus plus
#ifdef __cplusplus
}
#endif

#endif


