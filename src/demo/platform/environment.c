/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_platform_environment_main(tb_int_t argc, tb_char_t** argv)
{
    // get variable
    tb_char_t value[8192] = {0};
    if (tb_environment_get(argv[1], value, sizeof(value)))
    {
        // trace
        tb_trace_i("get %s: %s", argv[1], value);
    }

    // set variable
    if (    argv[2] 
        &&  tb_environment_set(argv[1], argv[2])
        &&  tb_environment_get(argv[1], value, sizeof(value)))
    {
        // trace
        tb_trace_i("set %s: %s", argv[1], value);
    }
    return 0;
}
