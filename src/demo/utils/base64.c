/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_utils_base64_main(tb_int_t argc, tb_char_t** argv)
{
    tb_char_t ob[4096] = {0};
    tb_size_t on = tb_base64_encode((tb_byte_t*)argv[1], tb_strlen(argv[1]), ob, 4096);
    //tb_size_t on = tb_base64_decode((tb_byte_t*)argv[1], tb_strlen(argv[1]), ob, 4096);
    tb_printf("%s: %lu\n", ob, on);

    return 0;
}
