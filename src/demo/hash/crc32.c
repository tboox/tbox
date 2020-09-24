/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_hash_crc32_main(tb_int_t argc, tb_char_t** argv)
{
    tb_trace_i("[crc32_ieee]:       %x\n", tb_crc32_make_from_cstr(argv[1], 0));
    tb_trace_i("[crc32_ieee_le]:    %x\n", tb_crc32_le_make_from_cstr(argv[1], 0));
    return 0;
}
