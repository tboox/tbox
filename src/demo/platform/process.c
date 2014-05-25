/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_platform_process_main(tb_int_t argc, tb_char_t** argv)
{
    // done
    tb_bool_t ok = tb_process_done(argv[1]);
    tb_trace_i("done: %s: %s", argv[1], ok? "ok" : "no");
    return 0;
}
