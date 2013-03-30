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

	// temporary
	tb_char_t temp[4096] = {0};
	if (tb_directory_temp(temp, 4096)) tb_print("temporary: %s", temp);

	// current
	tb_char_t curt[4096] = {0};
	if (tb_directory_curt(curt, 4096)) tb_print("current: %s", curt);

	// exit
	tb_exit();
	return 0;
}
