/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_hash_fnv32_main(tb_int_t argc, tb_char_t** argv)
{
    // trace
    tb_trace_i("[fnv32]:    %x", tb_fnv32_make_from_cstr(argv[1], 0));
    tb_trace_i("[fnv32-1a]: %x", tb_fnv32_1a_make_from_cstr(argv[1], 0));
    return 0;
}
