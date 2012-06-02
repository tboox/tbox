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
	if (!argv[1]) return 0;
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	tb_byte_t ob[16];
	tb_size_t on = tb_md5_encode(argv[1], tb_strlen(argv[1]), ob, 16);
	if (on != 16) return 0;

	tb_size_t i = 0;
	tb_char_t md5[256] = {0};
	for (i = 0; i < 16; ++i) tb_snprintf(md5 + (i << 1), 3, "%02X", ob[i]);
	tb_printf("%s\n", md5);


	tb_exit();
	return 0;
}
