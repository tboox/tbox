/* ///////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "tbox.h"
#include <stdio.h>
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * test
 */
tb_cpointer_t tb_exception_test(tb_cpointer_t data);
tb_cpointer_t tb_exception_test(tb_cpointer_t data)
{
	// self
	tb_size_t self = tb_thread_self();
	
	// trace
	tb_print("thread[%lu]: init", self);
 
	// try0
//	tb_size_t i = 0; // FIXME: maybe restored after exception, will leak memory if i is handle
	__tb_volatile__ tb_size_t i = 0;
	__tb_try 
	{
		tb_print("thread[%lu]: try0: b: %lu", self, i++);
		__tb_try 
		{
			tb_print("thread[%lu]: try1: b: %lu", self, i++);
			__tb_try 
			{
				// trace
				// FIXME: debug: if i is been stored in the stack, it will be modified after exception
				// FIXME: relase: if i is been stored in the register, it will be restored after exception
				tb_print("thread[%lu]: try2: b: %lu", self, i++);

				// abort
	//			tb_memset(&i, 0, 8192); // FIXME
	//			*((__tb_volatile__ tb_size_t*)0) = 0;
				tb_abort();
	//			__tb_volatile__ tb_pointer_t p = tb_malloc0(10); tb_memset(p, 0, 8192);
	
				// trace
				tb_print("thread[%lu]: try2: e: %lu", self, i++);
			}
			__tb_except(0)
			{
				tb_print("thread[%lu]: except2: %lu", self, i++);
			}
			__tb_end
			tb_print("thread[%lu]: try1: e: %lu", self, i++);
		}
		__tb_except(1)
		{
			tb_print("thread[%lu]: except1: %lu", self, i++);
			tb_backtrace_dump("\t\t", tb_null, 10);
		}
		__tb_end
		tb_print("thread[%lu]: try0: e: %lu", self, i);
 
		// abort
		tb_abort();

		// end
		tb_print("thread[%lu]: end0: e: %lu", self, i);
	}
	__tb_except(1)
	{
		tb_print("thread[%lu]: except0: %lu", self, i++);
		tb_backtrace_dump("\t", tb_null, 10);
	}
	__tb_end

	// trace
	tb_print("thread[%lu]: exit: %lu", self, i);
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

#if 1
	// done
	tb_thread_init(tb_null, tb_exception_test, tb_null, 0);
	tb_thread_init(tb_null, tb_exception_test, tb_null, 0);
	tb_thread_init(tb_null, tb_exception_test, tb_null, 0);
	tb_thread_init(tb_null, tb_exception_test, tb_null, 0);
#elif 0
	__tb_try 
	{
		tb_print("try: b");
		__tb_leave;
		tb_print("try: e");
	}
	__tb_except
	{
		tb_print("except");
	}
	__tb_end
	tb_print("end");
#else
	__tb_try 
	{
		tb_abort();
	}
	__tb_except
	{
		tb_print("except");
	}
	__tb_end

	__tb_try 
	{
		tb_abort();
	}
	__tb_except
	{
		tb_print("except");
	}
	__tb_end

#endif

	// wait
	getchar();

	// exit
	tb_exit();
	return 0;
}
