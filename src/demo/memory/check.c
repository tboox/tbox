/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdlib.h>
 
/* ///////////////////////////////////////////////////////////////////////
 * test
 */
tb_void_t tb_demo_overflow(tb_noarg_t);
tb_void_t tb_demo_overflow()
{
	tb_cpointer_t data = tb_malloc0(10);
	if (data)
	{
		tb_memset(data, 0, 11);
		tb_free(data);
	}
}
tb_void_t tb_demo_cstring(tb_noarg_t);
tb_void_t tb_demo_cstring()
{
	tb_char_t* data = tb_malloc0(10);
	if (data)
	{
		tb_memset(data, 'c', 10);
		tb_strlen(data);
		tb_free(data);
	}
}
tb_void_t tb_demo_free2(tb_noarg_t);
tb_void_t tb_demo_free2()
{
	tb_cpointer_t data = tb_malloc0(10);
	if (data)
	{
		tb_free(data);
		tb_free(data);
	}
}
tb_void_t tb_demo_leak(tb_noarg_t);
tb_void_t tb_demo_leak()
{
	tb_cpointer_t data = tb_malloc0(10);
	tb_used(data);
}
tb_void_t tb_demo_stack(tb_noarg_t);
tb_void_t tb_demo_stack()
{
	__tb_volatile__ tb_size_t data[10] = {0};
	data[11] = 0;
}
tb_void_t tb_demo_overlap(tb_noarg_t);
tb_void_t tb_demo_overlap()
{
	tb_cpointer_t data = tb_malloc(10);
	if (data)
	{
		tb_memcpy(data, data + 1, 5);
		tb_free(data);
	}
}

/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// init
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// done 
//	tb_demo_leak();
//	tb_demo_free2();
	tb_demo_stack();
//	tb_demo_overflow();
//	tb_demo_cstring();
//	tb_demo_overlap();

	// exit
	tb_exit();
	return 0;
}
