/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_platform_hostname_main(tb_int_t argc, tb_char_t** argv)
{
    tb_char_t name[256] = {0};
    if (tb_hostname(name, sizeof(name)))
    {
        tb_trace_i("hostname: %s", name);
    }
    return 0;
}
