/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_network_ipv6_main(tb_int_t argc, tb_char_t** argv)
{
    // done
    tb_ipv6_t ipv6;
    if (tb_ipv6_cstr_set(&ipv6, argv[1]))
    {
        // trace
        tb_trace_i("%s => %{ipv6}", argv[1], &ipv6);
    }

    // end
    return 0;
}
