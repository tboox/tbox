/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_network_ipv4_main(tb_int_t argc, tb_char_t** argv)
{
    // done
    tb_ipv4_t ipv4;
    if (tb_ipv4_cstr_set(&ipv4, argv[1]))
    {
        // trace
        tb_trace_i("%s => %{ipv4}", argv[1], &ipv4);
    }

    // end
    return 0;
}
