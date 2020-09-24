/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_demo_addrinfo_test(tb_char_t const* name)
{
    // get the default address (ipv4)
    tb_ipaddr_t addr = {0};
    tb_char_t   host[256];
    if (tb_addrinfo_addr(name, &addr))
        tb_trace_i("%s: %{ipaddr} => %s", name, &addr, tb_addrinfo_name(&addr, host, sizeof(host)));

    // get the ipv6 address by the hint info
    tb_ipaddr_clear(&addr);
    tb_ipaddr_family_set(&addr, TB_IPADDR_FAMILY_IPV6);
    if (tb_addrinfo_addr(name, &addr))
        tb_trace_i("%s: %{ipaddr} => %s", name, &addr, tb_addrinfo_name(&addr, host, sizeof(host)));
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_platform_addrinfo_main(tb_int_t argc, tb_char_t** argv)
{
    // test address
    tb_demo_addrinfo_test(argv[1]);
    return 0;
}
