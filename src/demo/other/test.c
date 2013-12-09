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

	// trace
	tb_print("%lu %lu", tb_offsetof(tb_iovec_t, data), tb_offsetof(tb_iovec_t, size));

	// exit
	tb_exit();
	return 0;
}
