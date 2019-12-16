/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_network_unix_main(tb_int_t argc, tb_char_t** argv)
{
    // done
    tb_unixaddr_t unix;
    if (tb_unixaddr_cstr_set(&unix, argv[1]))
    {
        // trace
        tb_trace_i("%s => %{unix}", argv[1], &unix);
    }

    // end
    return 0;
}
