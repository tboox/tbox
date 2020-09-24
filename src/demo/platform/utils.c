/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_platform_utils_main(tb_int_t argc, tb_char_t** argv)
{
    // hostname
    tb_char_t hostname[4096] = {0};
    if (tb_hostname(hostname, 4096)) tb_trace_i("hostname: %s", hostname);
    return 0;
}
