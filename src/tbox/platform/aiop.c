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
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author      ruki
 * @file        aiop.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "platform_aiop"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../asio/impl/prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#if defined(TB_CONFIG_OS_WINDOWS)
#   include "posix/aiop_select.c"
    tb_aiop_rtor_impl_t* tb_aiop_rtor_impl_init(tb_aiop_impl_t* aiop)
    {
        return tb_aiop_rtor_select_init(aiop);
    }
#elif defined(TB_CONFIG_OS_LINUX) || defined(TB_CONFIG_OS_ANDROID)
#   include "linux/aiop_epoll.c"
    tb_aiop_rtor_impl_t* tb_aiop_rtor_impl_init(tb_aiop_impl_t* aiop)
    {
        return tb_aiop_rtor_epoll_init(aiop);
    }
#elif defined(TB_CONFIG_OS_MAC)
#   include "mach/aiop_kqueue.c"
    tb_aiop_rtor_impl_t* tb_aiop_rtor_impl_init(tb_aiop_impl_t* aiop)
    {
        return tb_aiop_rtor_kqueue_init(aiop);
    }
#elif defined(TB_CONFIG_API_HAVE_POSIX)
#   include "posix/aiop_poll.c"
    tb_aiop_rtor_impl_t* tb_aiop_rtor_impl_init(tb_aiop_impl_t* aiop)
    {
        return tb_aiop_rtor_poll_init(aiop);
    }
#else
#   error have not available event mode
#endif

