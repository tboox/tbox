/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// init
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// test
	tb_char_t 	data[16];
	tb_ipv4_t 	ipv4;
	tb_uint32_t u32 = tb_ipv4_set(&ipv4, argv[1]);
	tb_print("%s => %u => %s", argv[1], u32, tb_ipv4_get(&ipv4, data, 16));

	// exit
	tb_exit();
	return 0;
}
