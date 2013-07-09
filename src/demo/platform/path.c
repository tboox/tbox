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

	// full
	tb_char_t full[TB_PATH_MAXN] = {0};
	tb_print("%s", tb_path_full(argv[1], full, TB_PATH_MAXN));

	// exit
	tb_exit();
	return 0;
}
