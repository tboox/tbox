/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_platform_ifaddrs_main(tb_int_t argc, tb_char_t** argv)
{
#ifdef __tb_debug__
    // dump ifaddrs
    tb_ifaddrs_dump(tb_ifaddrs());
#endif

    // dump hwaddr
    tb_hwaddr_t hwaddr;
    if (tb_ifaddrs_hwaddr(tb_ifaddrs(), argv[1], tb_false, &hwaddr))
    {
        // trace
        tb_trace_i("name: %s, hwaddr: %{hwaddr}", argv[1], &hwaddr);
    }

    // dump ipaddr4
    tb_ipaddr_t ipaddr4;
    if (tb_ifaddrs_ipaddr(tb_ifaddrs(), argv[1], tb_false, TB_IPADDR_FAMILY_IPV4, &ipaddr4))
    {
        // trace
        tb_trace_i("name: %s, ipaddr4: %{ipaddr}", argv[1], &ipaddr4);
    }

    // dump ipaddr6
    tb_ipaddr_t ipaddr6;
    if (tb_ifaddrs_ipaddr(tb_ifaddrs(), argv[1], tb_false, TB_IPADDR_FAMILY_IPV6, &ipaddr6))
    {
        // trace
        tb_trace_i("name: %s, ipaddr6: %{ipaddr}", argv[1], &ipaddr6);
    }

    return 0;
}
