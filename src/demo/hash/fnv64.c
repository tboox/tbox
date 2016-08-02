/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_hash_fnv64_main(tb_int_t argc, tb_char_t** argv)
{
    // trace
    tb_trace_i("[fnv64]:    %llx", tb_fnv64_make_from_cstr(argv[1], 0));
    tb_trace_i("[fnv64-1a]: %llx", tb_fnv64_1a_make_from_cstr(argv[1], 0));
    return 0;
}
