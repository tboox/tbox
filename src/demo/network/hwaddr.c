/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_network_hwaddr_main(tb_int_t argc, tb_char_t** argv)
{
    // done
    tb_hwaddr_t addr;
    tb_hwaddr_clear(&addr);
    if (tb_hwaddr_cstr_set(&addr, argv[1]))
    {
        // trace
        tb_trace_i("%s => %{hwaddr}", argv[1], &addr);
    }
    return 0;
}
