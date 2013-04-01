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
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	tb_char_t ob[4096];
	//tb_size_t on = tb_base64_encode(argv[1], tb_strlen(argv[1]), ob, 4096);
	tb_size_t on = tb_base64_decode(argv[1], tb_strlen(argv[1]), ob, 4096);
	tb_printf("%s: %lu\n", ob, on);

	tb_exit();
	return 0;
}
