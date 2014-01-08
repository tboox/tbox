/* ///////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../demo.h"

/* ///////////////////////////////////////////////////////////////////////
 * demo
 */ 
static tb_size_t tb_base64_encode_test(tb_byte_t const* ib, tb_size_t in, tb_char_t* ob, tb_size_t on, tb_char_t const* table)
{
	// encode
	tb_char_t* 		op = ob;
	tb_uint32_t 	bits = 0;
	tb_int_t 		left = in;
	tb_int_t 		shift = 0;
	while (left) 
	{
		bits = (bits << 8) + *ib++;
		left--;
		shift += 8;

		do 
		{
			*op++ = table[(bits << 6 >> shift) & 0x3f];
			shift -= 6;
		} 
		while (shift > 6 || (left == 0 && shift > 0));
	}

	while ((op - ob) & 3) *op++ = '=';
	*op = '\0';

	return (op - ob);
}

/* ///////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_utils_base64_main(tb_int_t argc, tb_char_t** argv)
{
#if 0
	tb_char_t ob[4096] = {0};
	tb_size_t on = tb_base64_encode(argv[1], tb_strlen(argv[1]), ob, 4096);
	//tb_size_t on = tb_base64_decode(argv[1], tb_strlen(argv[1]), ob, 4096);
	tb_printf("%s: %lu\n", ob, on);
#else
	{
		tb_char_t table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-";
		while (1)
		{
			tb_size_t i = tb_rand_uint32(0, 63);
			tb_size_t j = 63 - i;
			tb_char_t t = table[i];
			table[i] = table[j];
			table[j] = t;
			tb_char_t ob[64];
			tb_base64_encode_test("<?xml version=", 14, ob, 64, table);
			if (!tb_strncmp(ob, "zfgPAH1dqbbo3RKRmIIc", 18))
			{
				tb_print("%s", table);
				break;
			}
		}
	}
#endif

	return 0;
}
