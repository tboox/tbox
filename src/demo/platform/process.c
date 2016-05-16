/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_platform_process_main(tb_int_t argc, tb_char_t** argv)
{
    // init environments
    tb_char_t const* envp[] = 
    {
        "PATH=/tmp"
    ,   tb_null
    };

    // run bash
    tb_long_t ok = tb_process_run(argv[1], (tb_char_t const**)(argv + 1), envp);

    // trace
    tb_trace_i("run: %s: %ld", argv[1], ok);

    // ok
    return 0;
}
