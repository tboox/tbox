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

	// done
	tb_bool_t ok = tb_process_done(argv[1]);
	tb_print("done: %s: %s", argv[1], ok? "ok" : "no");

	// exit
	tb_exit();
	return 0;
}
