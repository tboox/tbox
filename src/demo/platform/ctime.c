/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdio.h>
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// init
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	tb_print("%lld %lld", tb_ctime_spak(), tb_ctime_time());
	tb_sleep(1);
	tb_print("%lld %lld", tb_ctime_spak(), tb_ctime_time());
	tb_sleep(1);
	tb_print("%lld %lld", tb_ctime_spak(), tb_ctime_time());


	// exit
	tb_exit();
	return 0;
}
