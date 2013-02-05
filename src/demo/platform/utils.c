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

	// hostname
	tb_char_t hostname[4096] = {0};
	if (tb_hostname(hostname, 4096)) tb_print("hostname: %s", hostname);

	// exit
	tb_exit();
	return 0;
}
