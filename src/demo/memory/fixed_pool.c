/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_memory_fixed_pool_main(tb_int_t argc, tb_char_t** argv)
{
	// init rpool
	tb_handle_t rpool = tb_fixed_pool_init(TB_FIXED_POOL_GROW_DEFAULT, 64, 0);
	tb_assert_and_check_return_val(rpool, 0);

	__tb_volatile__ tb_hong_t 	time = tb_mclock();
	__tb_volatile__ tb_byte_t* 	data = tb_null;
	__tb_volatile__ tb_size_t 	maxn = 100000;
	while (maxn--)
	{
		data = tb_fixed_pool_malloc(rpool);
		tb_check_break(data);
	}
	time = tb_mclock() - time;

end:

	// dump
#ifdef __tb_debug__
//	tb_fixed_pool_dump(rpool);
#endif

	// trace
	tb_trace_i("rpool: %lld ms", time);
	
	// exit rpool
	tb_fixed_pool_exit(rpool);

	return 0;
}
