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
 * @file		demo.h
 *
 */
#ifndef TB_DEMO_H
#define TB_DEMO_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../tbox.h"
#include <stdio.h>
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// the main decl
#define TB_DEMO_MAIN_DECL(name) 		tb_int_t tb_demo_##name##_main(tb_int_t argc, tb_char_t** argv)

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

// flv
TB_DEMO_MAIN_DECL(flv);

// libc
TB_DEMO_MAIN_DECL(libc_time);
TB_DEMO_MAIN_DECL(libc_wchar);
TB_DEMO_MAIN_DECL(libc_string);
TB_DEMO_MAIN_DECL(libc_stdlib);

// libm
TB_DEMO_MAIN_DECL(libm_float);
TB_DEMO_MAIN_DECL(libm_double);
TB_DEMO_MAIN_DECL(libm_integer);

// asio
TB_DEMO_MAIN_DECL(asio_addr);
TB_DEMO_MAIN_DECL(asio_aiopc);
TB_DEMO_MAIN_DECL(asio_aiopd);
TB_DEMO_MAIN_DECL(asio_aicpc);
TB_DEMO_MAIN_DECL(asio_aicpd);

// math
TB_DEMO_MAIN_DECL(math_rand);
TB_DEMO_MAIN_DECL(math_fixed);

// other
TB_DEMO_MAIN_DECL(other_test);
TB_DEMO_MAIN_DECL(other_charset);

// container
TB_DEMO_MAIN_DECL(container_heap);
TB_DEMO_MAIN_DECL(container_hash);
TB_DEMO_MAIN_DECL(container_dlist);
TB_DEMO_MAIN_DECL(container_slist);
TB_DEMO_MAIN_DECL(container_queue);
TB_DEMO_MAIN_DECL(container_stack);
TB_DEMO_MAIN_DECL(container_vector);

// algorithm
TB_DEMO_MAIN_DECL(algorithm_find);
TB_DEMO_MAIN_DECL(algorithm_sort);

#endif


