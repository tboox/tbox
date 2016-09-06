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
 * @file        dns.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../dns.h"
#include "../file.h"
#include "../../libc/libc.h"
#include "../../stream/stream.h"
#include "../../network/network.h"
#include <resolv.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the res_ninit func type
typedef tb_int_t (*tb_dns_res_ninit_func_t)(res_state);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_bool_t tb_dns_init()
{
    // done
    tb_size_t   count = 0;
    tb_handle_t library = tb_dynamic_init("libresolv.dylib");
    if (library) 
    {
        // the res_ninit func
        tb_dns_res_ninit_func_t pres_ninit = (tb_dns_res_ninit_func_t)tb_dynamic_func(library, "res_9_ninit");
        if (pres_ninit)
        {
            // init state
            struct __res_state state;
            if (!pres_ninit(&state))
            {
                // walk it
                tb_size_t i = 0;
                for (i = 0; i < state.nscount; i++, count++)
                {
                    // the address
                    tb_char_t const* addr = inet_ntoa(state.nsaddr_list[i].sin_addr);
                    tb_assert_and_check_continue(addr);

                    // trace
                    tb_trace_d("addr: %s", addr);

                    // add address
                    tb_dns_server_add(addr);
                }
            }
        }
    }

    // ok
    return tb_true;
}
tb_void_t tb_dns_exit()
{
}

