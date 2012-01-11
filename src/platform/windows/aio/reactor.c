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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		reactor.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../../aio/aio.h"
#include "../../../math/math.h"
#include "../../../memory/memory.h"
#include "../../../container/container.h"
#include "winsock2.h"
#include "windows.h"

// aio
#ifdef TB_CONFIG_AIO_HAVE_WAITO
# 	include "reactor/aioo/waito.c"
#endif

#ifdef TB_CONFIG_AIO_HAVE_SELECT
# 	include "reactor/aioo/select.c"
#endif

// aiop
#ifdef TB_CONFIG_AIO_HAVE_WAITO
# 	include "reactor/aiop/waito.c"
#endif

#ifdef TB_CONFIG_AIO_HAVE_SELECT
# 	include "reactor/aiop/select.c"
#endif

/* /////////////////////////////////////////////////////////
 * aio
 */
#ifdef TB_CONFIG_AIO_HAVE_WAITO

tb_long_t tb_aioo_reactor_file_wait(tb_aioo_t* object, tb_long_t timeout)
{
	return tb_aioo_reactor_waito_wait(object, timeout);
}
#endif


#ifdef TB_CONFIG_AIO_HAVE_SELECT
tb_long_t tb_aioo_reactor_sock_wait(tb_aioo_t* object, tb_long_t timeout)
{
	return tb_aioo_reactor_select_wait(object, timeout);
}

#endif

/* /////////////////////////////////////////////////////////
 * aiop
 */
#ifdef TB_CONFIG_AIO_HAVE_SELECT
tb_aiop_reactor_t* tb_aiop_reactor_file_init(tb_aiop_t* aiop)
{
	return tb_aiop_reactor_waito_init(aiop);
}
#endif

#ifdef TB_CONFIG_AIO_HAVE_SELECT
tb_aiop_reactor_t* tb_aiop_reactor_sock_init(tb_aiop_t* aiop)
{
	return tb_aiop_reactor_select_init(aiop);
}
#endif

