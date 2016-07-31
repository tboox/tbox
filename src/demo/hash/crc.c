/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_hash_crc_main(tb_int_t argc, tb_char_t** argv)
{
    tb_byte_t*  p = (tb_byte_t*)argv[1];
    tb_size_t   n = tb_strlen((tb_char_t const*)p);

#ifndef __tb_small__
    tb_printf("[crc]: TB_CRC_MODE_8_ATM = %x\n", tb_crc_make(TB_CRC_MODE_8_ATM, p, n, 0));
    tb_printf("[crc]: TB_CRC_MODE_16_ANSI = %x\n", tb_crc_make(TB_CRC_MODE_16_ANSI, p, n, 0));
    tb_printf("[crc]: TB_CRC_MODE_16_CCITT = %x\n", tb_crc_make(TB_CRC_MODE_16_CCITT, p, n, 0));
    tb_printf("[crc]: TB_CRC_MODE_32_IEEE = %x\n", tb_crc_make(TB_CRC_MODE_32_IEEE, p, n, 0));
    tb_printf("[crc]: TB_CRC_MODE_32_IEEE_LE = %x\n", tb_crc_make(TB_CRC_MODE_32_IEEE_LE, p, n, 0));
#else
    tb_printf("[crc]: TB_CRC_MODE_16_CCITT = %x\n", tb_crc_make(TB_CRC_MODE_16_CCITT, p, n, 0));
    tb_printf("[crc]: TB_CRC_MODE_32_IEEE_LE = %x\n", tb_crc_make(TB_CRC_MODE_32_IEEE_LE, p, n, 0));
#endif

    return 0;
}
