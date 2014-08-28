/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_other_test_main(tb_int_t argc, tb_char_t** argv)
{
    // init ipv4
    tb_ipv4_t ipv4;
    tb_ipv4_set(&ipv4, "127.0.0.1");

    // trace ipv4, output: "ipv4: 127.0.0.1"
    tb_trace_i("ipv4: %{ipv4}: hello: %F", &ipv4, TB_INF);
    return 0;
}
