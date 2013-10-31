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
 * @file		aicp.c
 * @ingroup 	platform
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_IMPL_TAG 			"proactor"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#if 0//defined(TB_CONFIG_ASIO_HAVE_EPOLL)
# 	include "aicp/aiop.c"
#elif 0//defined(TB_CONFIG_ASIO_HAVE_KQUEUE)
# 	include "aicp/aiop.c"
#elif defined(TB_CONFIG_ASIO_HAVE_AIO)
# 	include "aicp/aio.c"
#elif defined(TB_CONFIG_ASIO_HAVE_POLL)
# 	include "aicp/aiop.c"
#elif defined(TB_CONFIG_ASIO_HAVE_SELECT)
# 	include "aicp/aiop.c"
#else
# 	error have not available event mode
#endif

/* ///////////////////////////////////////////////////////////////////////
 * declaration
 */
tb_aicp_proactor_t* tb_aicp_proactor_init(tb_aicp_t* aicp);

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
#if 0//defined(TB_CONFIG_ASIO_HAVE_EPOLL)
tb_aicp_proactor_t* tb_aicp_proactor_init(tb_aicp_t* aicp)
{
	return tb_aicp_proactor_aiop_init(aicp);
}
#elif 0//defined(TB_CONFIG_ASIO_HAVE_KQUEUE)
tb_aicp_proactor_t* tb_aicp_proactor_init(tb_aicp_t* aicp)
{
	return tb_aicp_proactor_aiop_init(aicp);
}
#elif defined(TB_CONFIG_ASIO_HAVE_AIO)
tb_aicp_proactor_t* tb_aicp_proactor_init(tb_aicp_t* aicp)
{
	return tb_aicp_proactor_aio_init(aicp);
}
#elif defined(TB_CONFIG_ASIO_HAVE_POLL)
tb_aicp_proactor_t* tb_aicp_proactor_init(tb_aicp_t* aicp)
{
	return tb_aicp_proactor_aiop_init(aicp);
}
#elif defined(TB_CONFIG_ASIO_HAVE_SELECT)
tb_aicp_proactor_t* tb_aicp_proactor_init(tb_aicp_t* aicp)
{
	return tb_aicp_proactor_aiop_init(aicp);
}
#endif

