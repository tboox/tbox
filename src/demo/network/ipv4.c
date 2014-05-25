/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_network_ipv4_main(tb_int_t argc, tb_char_t** argv)
{
    // test
    tb_char_t   data[16];
    tb_ipv4_t   ipv4;
    tb_uint32_t u32 = tb_ipv4_set(&ipv4, argv[1]);
    tb_trace_i("%s => %u => %s", argv[1], u32, tb_ipv4_get(&ipv4, data, 16));

    return 0;
}
