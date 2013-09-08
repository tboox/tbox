/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * test
 */
tb_void_t tb_demo_overflow();
tb_void_t tb_demo_overflow()
{
	tb_cpointer_t data = tb_malloc0(10);
	if (data)
	{
		tb_memset(data, 0, 11);
		tb_free(data);
	}
}
tb_void_t tb_demo_free2();
tb_void_t tb_demo_free2()
{
	tb_cpointer_t data = tb_malloc0(10);
	if (data)
	{
		tb_free(data);
		tb_free(data);
	}
}
tb_void_t tb_demo_leak();
tb_void_t tb_demo_leak()
{
	tb_cpointer_t data = tb_malloc0(10);
	tb_used(data);
}

/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// init
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// done 
	tb_demo_leak();
	tb_demo_free2();
	tb_demo_overflow();

	// exit
	tb_exit();
	return 0;
}
