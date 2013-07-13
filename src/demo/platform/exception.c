/* ///////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "tbox.h"
#include <stdio.h>
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * test
 */
static tb_cpointer_t tb_exception_test(tb_cpointer_t data)
{
	// self
	tb_handle_t self = tb_thread_self();
	
	// trace
	tb_print("thread[%p]: init", self);

	// done
	tb_size_t i = 0;
	__tb_try 
	{
		tb_print("thread[%p]: try0: b: %lu", self, i++);
		__tb_try 
		{
			tb_print("thread[%p]: try1: b: %lu", self, i++);
			tb_abort();
//			tb_memset(&i, 0, 8192); // FIXME
//			__tb_volatile__ tb_size_t a = 0; a /= a;
//			__tb_volatile__ tb_pointer_t p = tb_malloc0(10); tb_memset(p, 0, 8192);
			tb_print("thread[%p]: try1: e: %lu", self, i++);
		}
		__tb_except
		{
			tb_print("thread[%p]: except1: %lu", self, i++);
		}
		__tb_end
		tb_print("thread[%p]: end1: %lu", self, i);
	}
	__tb_except
	{
		tb_print("thread[%p]: except0: %lu", self, i++);
	}
	__tb_end
	tb_print("thread[%p]: end0: %lu", self, i);

	// trace
	tb_print("thread[%p]: exit", self);
	tb_thread_return(tb_null);
	return tb_null;
}
/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// init
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// done
	tb_thread_init(tb_null, tb_exception_test, tb_null, 0);
	tb_thread_init(tb_null, tb_exception_test, tb_null, 0);
	tb_thread_init(tb_null, tb_exception_test, tb_null, 0);
	tb_thread_init(tb_null, tb_exception_test, tb_null, 0);

	// wait
	getchar();

	// exit
	tb_exit();
	return 0;
}
