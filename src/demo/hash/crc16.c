/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_hash_crc16_main(tb_int_t argc, tb_char_t** argv)
{
    tb_trace_i("[crc16_ansi]:   %x\n", tb_crc16_make_from_cstr(argv[1], 0));
    tb_trace_i("[crc16_ccitt]:  %x\n", tb_crc16_ccitt_make_from_cstr(argv[1], 0));
    return 0;
}
