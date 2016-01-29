/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_utils_fnv32_main(tb_int_t argc, tb_char_t** argv)
{
    // data
    tb_byte_t*  data = (tb_byte_t*)argv[1];
    tb_size_t   n = tb_strlen((tb_char_t const*)data);

    // trace
    tb_printf("[fnv32]: %x\n", tb_fnv32_encode(data, n));
    return 0;
}
