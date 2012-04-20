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
 * @file		abort.h
 *
 */
#ifndef TB_PREFIX_ABORT_H
#define TB_PREFIX_ABORT_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "config.h"
#include "trace.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// abort
#define tb_abort_tag(tag)								do { tb_trace_line_tag(tag, "[abort]: "); __tb_volatile__ tb_int_t* p = 0; *p = 0; } while(0)
#define tb_abort()										tb_abort_tag(TB_PRINT_TAG)

#endif


