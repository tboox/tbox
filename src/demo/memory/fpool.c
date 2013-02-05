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
	tb_handle_t fpool = tb_fpool_init(malloc(50 * 1024 * 1024), 50 * 1024 * 1024, 64, 0);
	tb_assert_and_check_return_val(fpool, 0);

	__tb_volatile__ tb_hong_t 	time = tb_mclock();
	__tb_volatile__ tb_byte_t* 	data = tb_null;
	__tb_volatile__ tb_size_t 	maxn = 100000;
	while (maxn--)
	{
		data = tb_fpool_malloc(fpool);
		tb_check_break(data);
	}
	time = tb_mclock() - time;

end:

	// dump
#ifdef TB_DEBUG
	tb_fpool_dump(fpool);
#endif

	// trace
	tb_print("fpool: %lld ms", time);
	
	// exit
	tb_fpool_exit(fpool);
	return 0;
}
