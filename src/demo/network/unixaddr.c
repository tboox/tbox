/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_network_unixaddr_main(tb_int_t argc, tb_char_t** argv)
{
    // done
    tb_unixaddr_t unixaddr;
    if (tb_unixaddr_cstr_set(&unixaddr, argv[1], tb_false))
    {
        // trace
        tb_trace_i("%s => %{unixaddr}", argv[1], &unixaddr);
    }

    // end
    return 0;
}
