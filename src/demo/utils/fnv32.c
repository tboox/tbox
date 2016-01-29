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
    tb_byte_t const*    data = (tb_byte_t const*)argv[1];
    tb_size_t           size = tb_strlen((tb_char_t const*)data);

    // trace
    tb_trace_i("[fnv32]: %x", tb_fnv32_encode(data, size));
    return 0;
}
