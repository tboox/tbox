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
 * @file		backtrace.h
 * @ingroup 	platform
 *
 */
#ifndef TB_PLATFORM_BACKTRACE_H
#define TB_PLATFORM_BACKTRACE_H


/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init backtrace  
 *
 * @return 				the backtrace handle
 */
tb_handle_t 			tb_backtrace_init();

/*! exit backtrace
 *
 * @param backtrace 	the backtrace handle
 */
tb_void_t 				tb_backtrace_exit(tb_handle_t backtrace);

/*! the backtrace frame size
 *
 * @param backtrace 	the backtrace handle
 *
 * @return 				the frame size
 */
tb_size_t 				tb_backtrace_size(tb_handle_t backtrace);

/*! the backtrace frame address
 *
 * @param backtrace 	the backtrace handle
 * @param frame 		the frame index
 *
 * @return 				the frame address
 */
tb_cpointer_t 			tb_backtrace_getp(tb_handle_t backtrace, tb_size_t frame);

/*! the backtrace frame symbol name
 *
 * @param backtrace 	the backtrace handle
 * @param frame 		the frame index
 *
 * @return 				the frame symbol name
 */
tb_char_t const* 		tb_backtrace_name(tb_handle_t backtrace, tb_size_t frame);

#endif
