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
	tb_handle_t gpool = tb_gpool_init(malloc(50 * 1024 * 1024), 50 * 1024 * 1024, 0);
	tb_assert_and_check_return_val(gpool, 0);

#if 0
	__tb_volatile__ tb_hong_t 	time = tb_mclock();
	__tb_volatile__ tb_byte_t* 	data = tb_null;
	__tb_volatile__ tb_size_t 	maxn = 100000;
	while (maxn--)
	{
		data = tb_gpool_malloc(gpool, 64);
		tb_check_break(data);
	}
	time = tb_mclock() - time;
#elif 1
	__tb_volatile__ tb_hong_t 	time = tb_mclock();
	__tb_volatile__ tb_byte_t* 	data = tb_null;
	__tb_volatile__ tb_size_t 	maxn = 100000;
	__tb_volatile__ tb_size_t 	size = 0xdeaf;
	__tb_volatile__ tb_size_t 	lmod = 511;
	while (maxn--)
	{
		size = (size * 10807 + 1) & 0xffffffff;
		data = tb_gpool_malloc(gpool, (size & lmod)? (size & lmod) : 1);
		tb_check_break(data);
	}
	time = tb_mclock() - time;
#endif

end:

	// dump
#ifdef TB_DEBUG
//	tb_gpool_dump(gpool);
#endif

	// trace
	tb_print("gpool: %lld ms", time);
	
	// exit
	tb_gpool_exit(gpool);
	return 0;
}
