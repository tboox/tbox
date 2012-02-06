/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
static tb_void_t tb_fpool_test_itor_perf()
{
	// init fpool
	tb_fpool_t* 	fpool = tb_fpool_init(256, 256, tb_item_func_int());
	tb_assert_and_check_return(fpool);

	// clear rand
	tb_rand_clear();

	// add items
	__tb_volatile__ tb_size_t n = 100000;
	while (n--) tb_fpool_put(fpool, tb_rand_uint32(0, TB_MAXU32)); 

	// performance
	tb_int64_t t = tb_mclock();
	__tb_volatile__ tb_uint64_t test[2] = {0};
	__tb_volatile__ tb_size_t 	itor = tb_fpool_itor_head(fpool);
	for (; itor != tb_fpool_itor_tail(fpool); )
	{
		__tb_volatile__ tb_size_t item = tb_fpool_itor_const_at(fpool, itor);
		if (!(((tb_size_t)item >> 25) & 0x1))
		{
			// save 
			tb_size_t next = tb_fpool_itor_next(fpool, itor);

			// remove, hack: the itor of the same item is mutable
			tb_fpool_del(fpool, itor);

			// next
			itor = next;

			// continue 
			continue ;
		}
		else
		{
			test[0] += (tb_size_t)item;
			test[1]++;
		}

		itor = tb_fpool_itor_next(fpool, itor);
	}
	t = tb_mclock() - t;
	tb_print("item: %llx, size: %llu ?= %u, time: %lld", test[0], test[1], tb_fpool_size(fpool), t);

	tb_fpool_exit(fpool);
}
static tb_bool_t tb_fpool_test_walk_item(tb_fpool_t* fpool, tb_pointer_t* item, tb_bool_t* bdel, tb_pointer_t data)
{
	tb_assert_and_check_return_val(fpool && bdel && data, TB_FALSE);

	tb_uint64_t* test = data;
	if (item)
	{
		tb_size_t i = (tb_size_t)*item;
		if (!((i >> 25) & 0x1))
//		if (!(i & 0x7))
//		if (1)
//		if (!(tb_rand_uint32(0, TB_MAXU32) & 0x1))
			*bdel = TB_TRUE;
		else
		{
			test[0] += i;
			test[1]++;
		}
	}

	// ok
	return TB_TRUE;
}
static tb_void_t tb_fpool_test_walk_perf()
{
	// init fpool
	tb_fpool_t* 	fpool = tb_fpool_init(256, 256, tb_item_func_int());
	tb_assert_and_check_return(fpool);

	// clear rand
	tb_rand_clear();

	// add items
	__tb_volatile__ tb_size_t n = 100000;
	while (n--) tb_fpool_put(fpool, tb_rand_uint32(0, TB_MAXU32)); 

	// performance
	tb_int64_t t = tb_mclock();
	__tb_volatile__ tb_uint64_t test[2] = {0};
	tb_fpool_walk(fpool, tb_fpool_test_walk_item, test);
	t = tb_mclock() - t;
	tb_print("item: %llx, size: %llu ?= %u, time: %lld", test[0], test[1], tb_fpool_size(fpool), t);

	tb_fpool_exit(fpool);
}
/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	if (!tb_init(malloc(30 * 1024 * 1024), 30 * 1024 * 1024)) return 0;

#if 1
	tb_fpool_test_itor_perf();
	tb_fpool_test_walk_perf();
#endif

	tb_exit();
	return 0;
}
