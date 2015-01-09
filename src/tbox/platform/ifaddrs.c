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
 * @file        ifaddrs.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "ifaddrs"
#define TB_TRACE_MODULE_DEBUG               (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "ifaddrs.h"
#include "../utils/utils.h"
#include "../algorithm/algorithm.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * instance implementation
 */
static tb_handle_t tb_ifaddrs_instance_init(tb_cpointer_t* ppriv)
{
    // init it
    return tb_ifaddrs_init();
}
static tb_void_t tb_ifaddrs_instance_exit(tb_handle_t ifaddrs, tb_cpointer_t priv)
{
    // exit it
    tb_ifaddrs_exit((tb_ifaddrs_ref_t)ifaddrs);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#ifdef TB_CONFIG_OS_WINDOWS
//#   include "windows/ifaddrs.c"
#elif defined(TB_CONFIG_OS_LINUX)
//#   include "linux/ifaddrs.c"
#elif defined(TB_CONFIG_OS_ANDROID)
//#   include "linux/android/ifaddrs.c"
#elif defined(TB_CONFIG_API_HAVE_POSIX)
#   include "posix/ifaddrs.c"
#else
tb_ifaddrs_ref_t tb_ifaddrs_init()
{
    tb_trace_noimpl();
    return tb_null;
}
tb_void_t tb_ifaddrs_exit(tb_ifaddrs_ref_t ifaddrs)
{
    tb_trace_noimpl();
}
tb_iterator_ref_t tb_ifaddrs_itor(tb_ifaddrs_ref_t ifaddrs, tb_bool_t reload)
{
    tb_trace_noimpl();
    return tb_null;
}
#endif
tb_ifaddrs_ref_t tb_ifaddrs()
{
    return (tb_ifaddrs_ref_t)tb_singleton_instance(TB_SINGLETON_TYPE_IFADDRS, tb_ifaddrs_instance_init, tb_ifaddrs_instance_exit, tb_null);
}
tb_bool_t tb_ifaddrs_hwaddr(tb_ifaddrs_ref_t ifaddrs, tb_char_t const* name, tb_hwaddr_ref_t hwaddr)
{
    // check
    tb_assert_and_check_return_val(ifaddrs && hwaddr, tb_false);

    // clear it first
    tb_hwaddr_clear(hwaddr);

    // done
    tb_bool_t ok        = tb_false;
    tb_bool_t reload    = tb_false;
    do
    {
        // attempt to get it first from the cached interfaces
        tb_for_all_if (tb_ifaddrs_interface_ref_t, interface, tb_ifaddrs_itor(ifaddrs, reload), interface)
        {
            // get hwaddr from the given interface name?
            if (name)
            {
                // is this?
                if (    (interface->flags & TB_IFADDRS_INTERFACE_FLAG_IS_HWADDR)
                    &&  !(interface->flags & TB_IFADDRS_INTERFACE_FLAG_IS_LOOPBACK)
                    &&  (interface->name && !tb_strcmp(interface->name, name)))
                {
                    // save hwaddr
                    tb_hwaddr_copy(hwaddr, &interface->addr.hw);

                    // ok
                    ok = tb_true;
                    break;
                }
            }
            else
            {
                // is this?
                if (    (interface->flags & TB_IFADDRS_INTERFACE_FLAG_IS_HWADDR)
                    &&  (interface->flags & TB_IFADDRS_INTERFACE_FLAG_IS_IPADDR)
                    &&  !(interface->flags & TB_IFADDRS_INTERFACE_FLAG_IS_LOOPBACK))
                {
                    // save hwaddr
                    tb_hwaddr_copy(hwaddr, &interface->addr.hw);

                    // ok
                    ok = tb_true;
                    break;
                }
            }
        }

        // attempt to get it again after reloading interfaces if be failed
        reload = !reload;

    } while (!ok && reload);

    // ok?
    return ok;
}
tb_bool_t tb_ifaddrs_ipaddr(tb_ifaddrs_ref_t ifaddrs, tb_char_t const* name, tb_size_t family, tb_ipaddr_ref_t ipaddr)
{
    // check
    tb_assert_and_check_return_val(ifaddrs && ipaddr, tb_false);

    // clear it first
    tb_ipaddr_clear(ipaddr);

    // done
    tb_bool_t ok        = tb_false;
    tb_bool_t reload    = tb_false;
    do
    {
        // attempt to get it first from the cached interfaces
        tb_for_all_if (tb_ifaddrs_interface_ref_t, interface, tb_ifaddrs_itor(ifaddrs, reload), interface)
        {
            // is this?
            if (    (interface->flags & TB_IFADDRS_INTERFACE_FLAG_IS_IPADDR)
                &&  !(interface->flags & TB_IFADDRS_INTERFACE_FLAG_IS_LOOPBACK)
                &&  (!family || tb_ipaddr_family(&interface->addr.ip) == family)
                &&  (!name || (interface->name && !tb_strcmp(interface->name, name))))
            {
                // save ipaddr
                tb_ipaddr_copy(ipaddr, &interface->addr.ip);

                // ok
                ok = tb_true;
                break;
            }
        }

        // attempt to get it again after reloading interfaces if be failed
        reload = !reload;

    } while (!ok && reload);

    // ok?
    return ok;
}
#ifdef __tb_debug__
tb_void_t tb_ifaddrs_dump(tb_ifaddrs_ref_t ifaddrs)
{
    // trace
    tb_trace_i("");

    // done
    tb_for_all_if (tb_ifaddrs_interface_ref_t, interface, tb_ifaddrs_itor(ifaddrs, tb_true), interface)
    {
        // trace
        tb_trace_i("name: %s%s",                  interface->name, (interface->flags & TB_IFADDRS_INTERFACE_FLAG_IS_LOOPBACK)? "[loopback]" : "");
        if (interface->flags & TB_IFADDRS_INTERFACE_FLAG_IS_IPADDR)
            tb_trace_i("    ipaddr: %{ipaddr}",     &interface->addr.ip);
        if (interface->flags & TB_IFADDRS_INTERFACE_FLAG_IS_HWADDR)
            tb_trace_i("    hwaddr: %{hwaddr}",     &interface->addr.hw);
    }
}
#endif
