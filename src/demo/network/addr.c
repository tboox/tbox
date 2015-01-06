/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_network_addr_main(tb_int_t argc, tb_char_t** argv)
{
    // done
    tb_addr_t addr;
    tb_addr_clear(&addr);
    if (tb_addr_set_ip_cstr(&addr, argv[1], TB_ADDR_FAMILY_NONE))
    {
        // trace
        tb_trace_i("%s => %{addr}", argv[1], &addr);
    }
    return 0;
}
