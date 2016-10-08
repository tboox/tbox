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
 * Copyright (C) 2009 - 2017, ruki All rights reserved.
 *
 * @author      ruki
 * @file        poller.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "poller.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#if defined(TB_CONFIG_OS_WINDOWS)
#   include "posix/poller_select.c"
#elif defined(TB_CONFIG_POSIX_HAVE_EPOLL_CREATE) \
    && defined(TB_CONFIG_POSIX_HAVE_EPOLL_WAIT)
#   include "linux/poller_epoller.c"
#elif defined(TB_CONFIG_OS_MACOSX)
#   include "mach/poller_kqueue.c"
#elif defined(TB_CONFIG_POSIX_HAVE_POLL)
#   include "posix/poller_poller.c"
#else
tb_poller_ref_t tb_poller_init(tb_size_t maxn)
{
    tb_trace_noimpl();
    return tb_null;
}
tb_void_t tb_poller_exit(tb_poller_ref_t poller)
{
    tb_trace_noimpl();
}
tb_void_t tb_poller_clear(tb_poller_ref_t poller)
{
    tb_trace_noimpl();
}
tb_void_t tb_poller_kill(tb_poller_ref_t poller)
{
    tb_trace_noimpl();
}
tb_void_t tb_poller_spak(tb_poller_ref_t poller)
{
    tb_trace_noimpl();
}
tb_bool_t tb_poller_support(tb_poller_ref_t poller, tb_size_t events)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_poller_insert(tb_poller_ref_t poller, tb_socket_ref_t sock, tb_size_t events, tb_cpointer_t priv)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_poller_remove(tb_poller_ref_t poller, tb_socket_ref_t sock)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_poller_modify(tb_poller_ref_t poller, tb_socket_ref_t sock, tb_size_t events, tb_cpointer_t priv)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_long_t tb_poller_wait(tb_poller_ref_t poller, tb_poller_event_func_t func, tb_long_t timeout)
{
    tb_trace_noimpl();
    return 0;
}
#endif

