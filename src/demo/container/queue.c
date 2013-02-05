/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * details
 */
static tb_size_t tb_queue_put_and_pop_test()
{
	// init
	tb_queue_t* queue = tb_queue_init(10, tb_item_func_long());
	tb_assert_and_check_return_val(queue, 0);

	tb_queue_put(queue, 0);
	tb_queue_put(queue, 1);
	tb_queue_put(queue, 2);
	tb_queue_put(queue, 3);
	tb_queue_put(queue, 4);
	tb_queue_put(queue, 5);
	tb_queue_put(queue, 6);
	tb_queue_put(queue, 7);
	tb_queue_put(queue, 8);
	tb_queue_put(queue, 9);

	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 10000;
	tb_hong_t t = tb_mclock();
	for (i = 0; i < n; i++) 
	{
		tb_queue_pop(queue);
		tb_queue_put(queue, 0xf);
	}
	t = tb_mclock() - t;

	// time
	tb_print("tb_queue_put_and_pop(%d): %lld ms, size: %d, maxn: %d", n, t, tb_queue_size(queue), tb_queue_maxn(queue));

	// check
	tb_assert(tb_queue_size(queue) == 10);
	tb_assert(tb_queue_head(queue) == 0xf);
	tb_assert(tb_queue_last(queue) == 0xf);

	// clear it
	tb_queue_clear(queue);
	tb_assert(!tb_queue_size(queue));

	// exit
	tb_queue_exit(queue);

	return n / ((tb_uint32_t)(t) + 1);
}

static tb_size_t tb_queue_iterator_next_test()
{
	// init
	tb_size_t n = 1000000;
	tb_queue_t* queue = tb_queue_init(n, tb_item_func_long());
	tb_assert_and_check_return_val(queue, 0);

	while (n--) tb_queue_put(queue, 0xf);
	__tb_volatile__ tb_size_t itor = tb_iterator_head(queue);
	__tb_volatile__ tb_size_t tail = tb_iterator_tail(queue);
	tb_hong_t t = tb_mclock();
	for (; itor != tail; itor = tb_iterator_next(queue, itor))
	{
		__tb_volatile__ tb_cpointer_t item = tb_iterator_item(queue, itor);
	}
	t = tb_mclock() - t;

	// time
	tb_print("tb_queue_iterator_next(%d): %lld ms, size: %d, maxn: %d", 1000000, t, tb_queue_size(queue), tb_queue_maxn(queue));

	// exit
	tb_queue_exit(queue);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_queue_iterator_prev_test()
{
	// init
	tb_size_t n = 1000000;
	tb_queue_t* queue = tb_queue_init(n, tb_item_func_long());
	tb_assert_and_check_return_val(queue, 0);

	while (n--) tb_queue_put(queue, 0xf);
	__tb_volatile__ tb_size_t itor = tb_iterator_last(queue);
	__tb_volatile__ tb_size_t head = tb_iterator_head(queue);
	tb_hong_t t = tb_mclock();
	while (1)
	{
		__tb_volatile__ tb_cpointer_t item = tb_iterator_item(queue, itor);

		if (itor == head) break;
		itor = tb_iterator_prev(queue, itor);
	}
	t = tb_mclock() - t;

	// time
	tb_print("tb_queue_iterator_prev(%d): %lld ms, size: %d, maxn: %d", 1000000, t, tb_queue_size(queue), tb_queue_maxn(queue));

	// exit
	tb_queue_exit(queue);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_void_t tb_queue_int_dump(tb_queue_t const* queue)
{
	tb_print("tb_int_t size: %d, maxn: %d", tb_queue_size(queue), tb_queue_maxn(queue));
	tb_size_t itor = tb_iterator_head(queue);
	tb_size_t tail = tb_iterator_tail(queue);
	for (; itor != tail; itor = tb_iterator_next(queue, itor))
		tb_print("tb_int_t at[%d]: %u", itor, tb_iterator_item(queue, itor));
}
static tb_void_t tb_queue_int_test()
{
	tb_queue_t* queue = tb_queue_init(10, tb_item_func_long());
	tb_assert_and_check_return(queue);

	tb_print("=============================================================");
	tb_print("put:");
	tb_queue_put(queue, 0);
	tb_queue_put(queue, 1);
	tb_queue_put(queue, 2);
	tb_queue_put(queue, 3);
	tb_queue_put(queue, 4);
	tb_queue_put(queue, 5);
	tb_queue_put(queue, 6);
	tb_queue_put(queue, 7);
	tb_queue_put(queue, 8);
	tb_queue_put(queue, 9);
	tb_queue_int_dump(queue);

	tb_print("=============================================================");
	tb_print("pop:");
	tb_queue_pop(queue);
	tb_queue_pop(queue);
	tb_queue_pop(queue);
	tb_queue_pop(queue);
	tb_queue_pop(queue);
	tb_queue_int_dump(queue);

	tb_print("=============================================================");
	tb_print("put:");
	tb_queue_put(queue, 0);
	tb_queue_put(queue, 1);
	tb_queue_put(queue, 2);
	tb_queue_put(queue, 3);
	tb_queue_put(queue, 4);
	tb_queue_int_dump(queue);

	tb_print("=============================================================");
	tb_print("clear:");
	tb_queue_clear(queue);
	tb_queue_int_dump(queue);
	tb_queue_exit(queue);
}
static tb_void_t tb_queue_str_dump(tb_queue_t const* queue)
{
	tb_print("str size: %d, maxn: %d", tb_queue_size(queue), tb_queue_maxn(queue));
	tb_size_t itor = tb_iterator_head(queue);
	tb_size_t tail = tb_iterator_tail(queue);
	for (; itor != tail; itor = tb_iterator_next(queue, itor))
		tb_print("str at[%d]: %s", itor, tb_iterator_item(queue, itor));
}
static tb_void_t tb_queue_str_test()
{
	tb_queue_t* queue = tb_queue_init(10, tb_item_func_str(tb_true, tb_spool_init(TB_SPOOL_GROW_SMALL, 0)));
	tb_assert_and_check_return(queue);

	tb_print("=============================================================");
	tb_print("put:");
	tb_queue_put(queue, "0000000000");
	tb_queue_put(queue, "1111111111");
	tb_queue_put(queue, "2222222222");
	tb_queue_put(queue, "3333333333");
	tb_queue_put(queue, "4444444444");
	tb_queue_put(queue, "5555555555");
	tb_queue_put(queue, "6666666666");
	tb_queue_put(queue, "7777777777");
	tb_queue_put(queue, "8888888888");
	tb_queue_put(queue, "9999999999");
	tb_queue_str_dump(queue);

	tb_print("=============================================================");
	tb_print("pop:");
	tb_queue_pop(queue);
	tb_queue_pop(queue);
	tb_queue_pop(queue);
	tb_queue_pop(queue);
	tb_queue_pop(queue);
	tb_queue_str_dump(queue);

	tb_print("=============================================================");
	tb_print("put:");
	tb_queue_put(queue, "0000000000");
	tb_queue_put(queue, "1111111111");
	tb_queue_put(queue, "2222222222");
	tb_queue_put(queue, "3333333333");
	tb_queue_put(queue, "4444444444");
	tb_queue_str_dump(queue);

	tb_print("=============================================================");
	tb_print("clear:");
	tb_queue_clear(queue);
	tb_queue_str_dump(queue);
	tb_queue_exit(queue);
}
static tb_void_t tb_queue_efm_dump(tb_queue_t const* queue)
{
	tb_print("efm size: %d, maxn: %d", tb_queue_size(queue), tb_queue_maxn(queue));
	tb_size_t itor = tb_iterator_head(queue);
	tb_size_t tail = tb_iterator_tail(queue);
	for (; itor != tail; itor = tb_iterator_next(queue, itor))
		tb_print("efm at[%d]: %s", itor, tb_iterator_item(queue, itor));
}
static tb_void_t tb_queue_efm_test()
{
	tb_queue_t* queue = tb_queue_init(10, tb_item_func_efm(11, tb_rpool_init(256, 11, 0)));
	tb_assert_and_check_return(queue);

	tb_print("=============================================================");
	tb_print("put:");
	tb_queue_put(queue, "0000000000");
	tb_queue_put(queue, "1111111111");
	tb_queue_put(queue, "2222222222");
	tb_queue_put(queue, "3333333333");
	tb_queue_put(queue, "4444444444");
	tb_queue_put(queue, "5555555555");
	tb_queue_put(queue, "6666666666");
	tb_queue_put(queue, "7777777777");
	tb_queue_put(queue, "8888888888");
	tb_queue_put(queue, "9999999999");
	tb_queue_efm_dump(queue);

	tb_print("=============================================================");
	tb_print("pop:");
	tb_queue_pop(queue);
	tb_queue_pop(queue);
	tb_queue_pop(queue);
	tb_queue_pop(queue);
	tb_queue_pop(queue);
	tb_queue_efm_dump(queue);

	tb_print("=============================================================");
	tb_print("put:");
	tb_queue_put(queue, "0000000000");
	tb_queue_put(queue, "1111111111");
	tb_queue_put(queue, "2222222222");
	tb_queue_put(queue, "3333333333");
	tb_queue_put(queue, "4444444444");
	tb_queue_efm_dump(queue);

	tb_print("=============================================================");
	tb_print("clear:");
	tb_queue_clear(queue);
	tb_queue_efm_dump(queue);
	tb_queue_exit(queue);
}

static tb_void_t tb_queue_ifm_free(tb_item_func_t* func, tb_pointer_t item)
{
	tb_print("ifm free: %s, priv: %s", item, func->priv);
}
static tb_void_t tb_queue_ifm_dump(tb_queue_t const* queue)
{
	tb_print("ifm size: %d, maxn: %d", tb_queue_size(queue), tb_queue_maxn(queue));
	tb_size_t itor = tb_iterator_head(queue);
	tb_size_t tail = tb_iterator_tail(queue);
	for (; itor != tail; itor = tb_iterator_next(queue, itor))
		tb_print("ifm at[%d]: %s", itor, tb_iterator_item(queue, itor));
}
static tb_void_t tb_queue_ifm_test()
{
	tb_queue_t* queue = tb_queue_init(10, tb_item_func_ifm(11, tb_queue_ifm_free, "ifm"));
	tb_assert_and_check_return(queue);

	tb_print("=============================================================");
	tb_print("put:");
	tb_queue_put(queue, "0000000000");
	tb_queue_put(queue, "1111111111");
	tb_queue_put(queue, "2222222222");
	tb_queue_put(queue, "3333333333");
	tb_queue_put(queue, "4444444444");
	tb_queue_put(queue, "5555555555");
	tb_queue_put(queue, "6666666666");
	tb_queue_put(queue, "7777777777");
	tb_queue_put(queue, "8888888888");
	tb_queue_put(queue, "9999999999");
	tb_queue_ifm_dump(queue);

	tb_print("=============================================================");
	tb_print("pop:");
	tb_queue_pop(queue);
	tb_queue_pop(queue);
	tb_queue_pop(queue);
	tb_queue_pop(queue);
	tb_queue_pop(queue);
	tb_queue_ifm_dump(queue);

	tb_print("=============================================================");
	tb_print("put:");
	tb_queue_put(queue, "0000000000");
	tb_queue_put(queue, "1111111111");
	tb_queue_put(queue, "2222222222");
	tb_queue_put(queue, "3333333333");
	tb_queue_put(queue, "4444444444");
	tb_queue_ifm_dump(queue);

	tb_print("=============================================================");
	tb_print("clear:");
	tb_queue_clear(queue);
	tb_queue_ifm_dump(queue);
	tb_queue_exit(queue);
}

static tb_void_t tb_queue_perf_test()
{
	tb_size_t score = 0;
	tb_print("=============================================================");
	tb_print("put & pop performance:");
	score += tb_queue_put_and_pop_test();

	tb_print("=============================================================");
	tb_print("iterator performance:");
	score += tb_queue_iterator_next_test();
	score += tb_queue_iterator_prev_test();

	tb_print("=============================================================");
	tb_print("score: %d", score / 100);

}
static tb_void_t tb_queue_test_itor_perf()
{
	// init queue
	tb_queue_t* 	queue = tb_queue_init(100000, tb_item_func_long());
	tb_assert_and_check_return(queue);

	// clear rand
	tb_rand_clear();

	// add items
	__tb_volatile__ tb_size_t n = 100000;
	while (n--) tb_queue_put(queue, tb_rand_uint32(0, TB_MAXU32)); 

	// performance
	tb_hong_t t = tb_mclock();
	__tb_volatile__ tb_hize_t test[2] = {0};
	__tb_volatile__ tb_size_t 	itor = tb_iterator_head(queue);
	for (; itor != tb_iterator_tail(queue); )
	{
		__tb_volatile__ tb_size_t item = tb_iterator_item(queue, itor);
#if 0
		if (!(((tb_size_t)item >> 25) & 0x1))
		{
			// remove, hack: the itor of the same item is mutable
			tb_queue_remove(queue, itor);

			// continue 
			continue ;
		}
		else
#endif
		{
			test[0] += (tb_size_t)item;
			test[1]++;
		}

		itor = tb_iterator_next(queue, itor);
	}
	t = tb_mclock() - t;
	tb_print("item: %llx, size: %llu ?= %u, time: %lld", test[0], test[1], tb_queue_size(queue), t);

	tb_queue_exit(queue);
}
static tb_bool_t tb_queue_test_walk_item(tb_queue_t* queue, tb_pointer_t* item, tb_bool_t* bdel, tb_pointer_t data)
{
	tb_assert_and_check_return_val(queue && bdel && data, tb_false);

	tb_hize_t* test = data;
	if (item)
	{
		tb_size_t i = (tb_size_t)*item;
#if 0
		if (!((i >> 25) & 0x1))
//		if (!(i & 0x7))
//		if (1)
//		if (!(tb_rand_uint32(0, TB_MAXU32) & 0x1))
			*bdel = tb_true;
		else
#endif
		{
			test[0] += i;
			test[1]++;
		}
	}

	// ok
	return tb_true;
}
static tb_void_t tb_queue_test_walk_perf()
{
	// init queue
	tb_queue_t* 	queue = tb_queue_init(100000, tb_item_func_long());
	tb_assert_and_check_return(queue);

	// clear rand
	tb_rand_clear();

	// add items
	__tb_volatile__ tb_size_t n = 100000;
	while (n--) tb_queue_put(queue, tb_rand_uint32(0, TB_MAXU32)); 

	// performance
	tb_hong_t t = tb_mclock();
	__tb_volatile__ tb_hize_t test[2] = {0};
	tb_queue_walk(queue, tb_queue_test_walk_item, test);
	t = tb_mclock() - t;
	tb_print("item: %llx, size: %llu ?= %u, time: %lld", test[0], test[1], tb_queue_size(queue), t);

	tb_queue_exit(queue);
}
/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	if (!tb_init(malloc(30 * 1024 * 1024), 30 * 1024 * 1024)) return 0;

#if 0
	tb_queue_int_test();
	tb_queue_str_test();
	tb_queue_efm_test();
	tb_queue_ifm_test();
#endif

#if 0
	tb_queue_perf_test();
#endif

#if 1
	tb_queue_test_itor_perf();
	tb_queue_test_walk_perf();
#endif

	return 0;
}
