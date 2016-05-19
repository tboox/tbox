/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_platform_process_main(tb_int_t argc, tb_char_t** argv)
{
    // run bash
    tb_long_t ok = tb_process_run(argv[1], (tb_char_t const**)(argv + 1), tb_null);
//    tb_long_t ok = tb_process_run_cmd(argv[1], tb_null);

    // trace
    tb_trace_i("run: %s: %ld", argv[1], ok);

    // ok
    return 0;
}
