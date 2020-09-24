/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_network_ipaddr_main(tb_int_t argc, tb_char_t** argv)
{
    // done
    tb_ipaddr_t addr;
    tb_ipaddr_clear(&addr);
    if (tb_ipaddr_ip_cstr_set(&addr, argv[1], TB_IPADDR_FAMILY_NONE))
    {
        // trace
        tb_trace_i("%s => %{ipaddr}", argv[1], &addr);
    }
    return 0;
}
