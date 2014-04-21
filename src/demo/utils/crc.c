/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_utils_crc_main(tb_int_t argc, tb_char_t** argv)
{
	tb_byte_t* 	p = (tb_byte_t*)argv[1];
	tb_size_t 	n = tb_strlen((tb_char_t const*)p);

#ifndef __tb_small__
	tb_printf("[crc]: TB_CRC_MODE_8_ATM = %x\n", tb_crc_encode(TB_CRC_MODE_8_ATM, 0, p, n));
	tb_printf("[crc]: TB_CRC_MODE_16_ANSI = %x\n", tb_crc_encode(TB_CRC_MODE_16_ANSI, 0, p, n));
	tb_printf("[crc]: TB_CRC_MODE_16_CCITT = %x\n", tb_crc_encode(TB_CRC_MODE_16_CCITT, 0, p, n));
	tb_printf("[crc]: TB_CRC_MODE_32_IEEE = %x\n", tb_crc_encode(TB_CRC_MODE_32_IEEE, 0, p, n));
	tb_printf("[crc]: TB_CRC_MODE_32_IEEE_LE = %x\n", tb_crc_encode(TB_CRC_MODE_32_IEEE_LE, 0, p, n));
#else
	tb_printf("[crc]: TB_CRC_MODE_16_CCITT = %x\n", tb_crc_encode(TB_CRC_MODE_16_CCITT, 0, p, n));
	tb_printf("[crc]: TB_CRC_MODE_32_IEEE_LE = %x\n", tb_crc_encode(TB_CRC_MODE_32_IEEE_LE, 0, p, n));
#endif

	return 0;
}
