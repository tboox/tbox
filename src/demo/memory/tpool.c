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
	tb_handle_t tpool = tb_tpool_init(malloc(50 * 1024 * 1024), 50 * 1024 * 1024 >> 3, 0);
	tb_assert_and_check_return_val(tpool, 0);

#if 0
	__tb_volatile__ tb_hong_t 	time = tb_mclock();
	__tb_volatile__ tb_byte_t* 	data = tb_null;
	__tb_volatile__ tb_size_t 	maxn = 100000;
	while (maxn--)
	{
		data = tb_tpool_malloc(tpool, 64);
		tb_check_break(data);
	}
	time = tb_mclock() - time;
#elif 1
	__tb_volatile__ tb_hong_t 	time = tb_mclock();
	__tb_volatile__ tb_byte_t* 	data = tb_null;
	__tb_volatile__ tb_size_t 	maxn = 100000;
	__tb_volatile__ tb_size_t 	size = 0xdeaf;
	__tb_volatile__ tb_size_t 	lmod = 511;//tb_tpool_limit(tpool) - 1;
	while (maxn--)
	{
		size = (size * 10807 + 1) & 0xffffffff;
		data = tb_tpool_malloc(tpool, (size & lmod)? (size & lmod) : 1);
		tb_check_break(data);
	}
	time = tb_mclock() - time;
#else
	__tb_volatile__ tb_hong_t 	time = 0;
	
	tb_byte_t* p0 = tb_tpool_malloc(tpool, 1);
	tb_byte_t* p1 = tb_tpool_malloc(tpool, 2);
	tb_byte_t* p2 = tb_tpool_malloc(tpool, 3);
	tb_byte_t* p3 = tb_tpool_malloc(tpool, 4);
	tb_byte_t* p4 = tb_tpool_malloc(tpool, 8);
	tb_byte_t* p5 = tb_tpool_malloc(tpool, 15);
	tb_byte_t* p6 = tb_tpool_malloc(tpool, 64);
	tb_byte_t* p7 = tb_tpool_malloc(tpool, 65);
	tb_byte_t* p8 = tb_tpool_malloc(tpool, 233);
	tb_byte_t* p9 = tb_tpool_malloc(tpool, 123);
	tb_byte_t* p10 = tb_tpool_malloc(tpool, 56);
	tb_byte_t* p11 = tb_tpool_malloc(tpool, 67);
	tb_byte_t* p12 = tb_tpool_malloc(tpool, 89);
	tb_byte_t* p13 = tb_tpool_malloc(tpool, 256);
	tb_byte_t* p14 = tb_tpool_malloc(tpool, 300);
	tb_byte_t* p15 = tb_tpool_malloc(tpool, 512);
#if 1
	tb_tpool_free(tpool, p0);
	tb_tpool_free(tpool, p1);
	tb_tpool_free(tpool, p2);
	tb_tpool_free(tpool, p3);
	tb_tpool_free(tpool, p4);
	tb_tpool_free(tpool, p5);
	tb_tpool_free(tpool, p6);
	tb_tpool_free(tpool, p7);
	tb_tpool_free(tpool, p8);
	tb_tpool_free(tpool, p9);
	tb_tpool_free(tpool, p10);
	tb_tpool_free(tpool, p11);
	tb_tpool_free(tpool, p12);
	tb_tpool_free(tpool, p13);
	tb_tpool_free(tpool, p14);
	tb_tpool_free(tpool, p15);
#endif
#endif

	// dump
#ifdef TB_DEBUG
	tb_tpool_dump(tpool);
#endif

	// trace
	tb_print("tpool: %lld ms", time);
	
	// exit
	tb_tpool_exit(tpool);
	return 0;
}
