/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * test
 */
tb_void_t tb_demo_test3();
tb_void_t tb_demo_test3()
{
	tb_cpointer_t data = tb_malloc0(10);
	tb_memset(data, 0, 11);
	tb_free(data);
//	tb_backtrace_dump("\t", tb_null, 10);
}
static tb_void_t tb_demo_test2()
{
	tb_demo_test3();
}
tb_void_t tb_demo_test(tb_size_t size);
tb_void_t tb_demo_test(tb_size_t size)
{
	if (size) tb_demo_test(size - 1);
	else tb_demo_test2();
}

/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// init
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// done 
	tb_demo_test(argv[1]? tb_atoi(argv[1]) : 10);

	// exit
	tb_exit();
	return 0;
}
