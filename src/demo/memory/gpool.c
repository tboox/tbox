/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"

/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// init
	tb_handle_t gpool = tb_gpool_init(malloc(50 * 1024 * 1024), 50 * 1024 * 1024, 0);
	tb_assert_and_check_return_val(gpool, 0);

	__tb_volatile__ tb_hong_t 	time = tb_mclock();
	__tb_volatile__ tb_size_t 	size = 0xdead;
	__tb_volatile__ tb_byte_t* 	data = TB_NULL;
	while (1)
	{
		size = (size * 10807 + 1) & 0xffff;
		data = tb_gpool_malloc0(gpool, size);
		tb_check_break(data || !size);
	}
	time = tb_mclock() - time;

end:

	// dump
#ifdef TB_DEBUG
	tb_gpool_dump(gpool);
#endif

	// trace
	tb_print("gpool: %lld ms", time);
	
	// exit
	tb_gpool_exit(gpool);
	return 0;
}
