/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_utils_url_main(tb_int_t argc, tb_char_t** argv)
{
    tb_char_t ob[4096];
    //tb_size_t on = tb_url_encode(argv[1], tb_strlen(argv[1]), ob, 4096);
    tb_size_t on = tb_url_decode(argv[1], tb_strlen(argv[1]), ob, 4096);
    tb_trace_i("%s: %lu", ob, on);

    return 0;
}
