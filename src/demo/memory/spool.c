/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"

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
	__tb_volatile__ tb_size_t 	size = 0xdead;
	__tb_volatile__ tb_byte_t* 	data = TB_NULL;
	while (1)
	{
		size = (size * 10807 + 1) & 0xff;
		data = tb_spool_malloc0(spool, size);
		tb_check_break(data || !size);
	}
	time = tb_mclock() - time;

end:

	// dump
#ifdef TB_DEBUG
	tb_spool_dump(spool);
#endif

	// trace
	tb_print("spool: %lld ms", time);
	
	// exit spool
	tb_spool_exit(spool);

	// exit tbox
	tb_exit();
	return 0;
}
