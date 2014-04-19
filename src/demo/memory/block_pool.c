/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_memory_block_pool_main(tb_int_t argc, tb_char_t** argv)
{
	// init spool
	tb_handle_t spool = tb_block_pool_init(TB_BLOCK_POOL_GROW_DEFAULT, 0);
	tb_assert_and_check_return_val(spool, 0);

	__tb_volatile__ tb_hong_t 	time = tb_mclock();
	__tb_volatile__ tb_byte_t* 	data = tb_null;
	__tb_volatile__ tb_size_t 	maxn = 100000;
	while (maxn--)
	{
		data = tb_block_pool_malloc(spool, 64);
		tb_check_break(data);
	}
	time = tb_mclock() - time;

	// dump
#ifdef __tb_debug__
//	tb_block_pool_dump(spool, tb_null);
#endif

	// trace
	tb_trace_i("spool: %lld ms", time);
	
	// exit spool
	tb_block_pool_exit(spool);

	// exit tbox
	tb_exit();
	return 0;
}
