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

	tb_size_t i = 0;
	__tb_try 
	{
		tb_print("try0: b: %lu", i++);

		__tb_try 
		{
			tb_print("try1: b: %lu", i++);
//			tb_abort();
			tb_print("try1: e: %lu", i++);
		}
		__tb_except
		{
			tb_print("except1: %lu", i++);
		}
		__tb_end

		tb_print("end1: %lu", i);
	}
	__tb_except
	{
		tb_print("except0: %lu", i++);
	}
	__tb_end

	tb_print("end0: %lu", i);

	// exit
	tb_exit();
	return 0;
}
