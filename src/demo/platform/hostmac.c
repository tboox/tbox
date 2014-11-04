/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_platform_hostmac_main(tb_int_t argc, tb_char_t** argv)
{
    tb_byte_t mac[6];
    if (tb_hostmac(argv[1], mac))
    {
        // trace
        tb_trace_i("name: %s, mac: %02X:%02X:%02X:%02X:%02X:%02X", argv[1], mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
    return 0;
}
