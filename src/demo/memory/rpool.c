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

	// init rpool
	tb_handle_t rpool = tb_rpool_init(TB_RPOOL_GROW_DEFAULT, 64, 0);
	tb_assert_and_check_return_val(rpool, 0);

	__tb_volatile__ tb_hong_t 	time = tb_mclock();
	__tb_volatile__ tb_byte_t* 	data = tb_null;
	__tb_volatile__ tb_size_t 	maxn = 100000;
	while (maxn--)
	{
		data = tb_rpool_malloc(rpool);
		tb_check_break(data);
	}
	time = tb_mclock() - time;

end:

	// dump
#ifdef TB_DEBUG
//	tb_rpool_dump(rpool);
#endif

	// trace
	tb_print("rpool: %lld ms", time);
	
	// exit rpool
	tb_rpool_exit(rpool);

	// exit tbox
	tb_exit();
	return 0;
}
