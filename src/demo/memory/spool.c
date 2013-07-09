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
	// init tbox
	if (!tb_init(malloc(50 * 1024 * 1024), 50 * 1024 * 1024)) return 0;

	// init spool
	tb_handle_t spool = tb_spool_init(TB_SPOOL_GROW_DEFAULT, 0);
	tb_assert_and_check_return_val(spool, 0);

	__tb_volatile__ tb_hong_t 	time = tb_mclock();
	__tb_volatile__ tb_byte_t* 	data = tb_null;
	__tb_volatile__ tb_size_t 	maxn = 100000;
	while (maxn--)
	{
		data = tb_spool_malloc(spool, 64);
		tb_check_break(data);
	}
	time = tb_mclock() - time;

end:

	// dump
#ifdef __tb_debug__
//	tb_spool_dump(spool);
#endif

	// trace
	tb_print("spool: %lld ms", time);
	
	// exit spool
	tb_spool_exit(spool);

	// exit tbox
	tb_exit();
	return 0;
}
