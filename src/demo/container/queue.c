/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
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
	tb_trace_i("tb_queue_put_and_pop(%lu): %lld ms, size: %lu, maxn: %lu", n, t, tb_queue_size(queue), tb_queue_maxn(queue));

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
	tb_hong_t t = tb_mclock();
	tb_for_all (tb_char_t*, item, queue) tb_used(item);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_queue_iterator_next(%lu): %lld ms, size: %lu, maxn: %lu", 1000000, t, tb_queue_size(queue), tb_queue_maxn(queue));

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
	tb_hong_t t = tb_mclock();
	tb_rfor_all (tb_char_t*, item, queue) tb_used(item);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_queue_iterator_prev(%lu): %lld ms, size: %lu, maxn: %lu", 1000000, t, tb_queue_size(queue), tb_queue_maxn(queue));

	// exit
	tb_queue_exit(queue);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_void_t tb_queue_int_dump(tb_queue_t const* queue)
{
	tb_trace_i("tb_int_t size: %lu, maxn: %lu", tb_queue_size(queue), tb_queue_maxn(queue));
	tb_for_all (tb_char_t*, item, queue)
	{
		tb_trace_i("tb_int_t at[%lu]: %u", item_itor, item);
	}
}
static tb_void_t tb_queue_int_test()
{
	tb_queue_t* queue = tb_queue_init(10, tb_item_func_long());
	tb_assert_and_check_return(queue);

	tb_trace_i("=============================================================");
	tb_trace_i("put:");
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

	tb_trace_i("=============================================================");
	tb_trace_i("pop:");
	tb_queue_pop(queue);
	tb_queue_pop(queue);
	tb_queue_pop(queue);
	tb_queue_pop(queue);
	tb_queue_pop(queue);
	tb_queue_int_dump(queue);

	tb_trace_i("=============================================================");
	tb_trace_i("put:");
	tb_queue_put(queue, 0);
	tb_queue_put(queue, 1);
	tb_queue_put(queue, 2);
	tb_queue_put(queue, 3);
	tb_queue_put(queue, 4);
	tb_queue_int_dump(queue);

	tb_trace_i("=============================================================");
	tb_trace_i("clear:");
	tb_queue_clear(queue);
	tb_queue_int_dump(queue);
	tb_queue_exit(queue);
}
static tb_void_t tb_queue_str_dump(tb_queue_t const* queue)
{
	tb_trace_i("str size: %lu, maxn: %lu", tb_queue_size(queue), tb_queue_maxn(queue));
	tb_for_all (tb_char_t*, item, queue)
	{
		tb_trace_i("str at[%lu]: %s", item_itor, item);
	}
}
static tb_void_t tb_queue_str_test()
{
	tb_queue_t* queue = tb_queue_init(10, tb_item_func_str(tb_true, tb_block_pool_init(TB_BLOCK_POOL_GROW_SMALL, 0)));
	tb_assert_and_check_return(queue);

	tb_trace_i("=============================================================");
	tb_trace_i("put:");
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

	tb_trace_i("=============================================================");
	tb_trace_i("pop:");
	tb_queue_pop(queue);
	tb_queue_pop(queue);
	tb_queue_pop(queue);
	tb_queue_pop(queue);
	tb_queue_pop(queue);
	tb_queue_str_dump(queue);

	tb_trace_i("=============================================================");
	tb_trace_i("put:");
	tb_queue_put(queue, "0000000000");
	tb_queue_put(queue, "1111111111");
	tb_queue_put(queue, "2222222222");
	tb_queue_put(queue, "3333333333");
	tb_queue_put(queue, "4444444444");
	tb_queue_str_dump(queue);

	tb_trace_i("=============================================================");
	tb_trace_i("clear:");
	tb_queue_clear(queue);
	tb_queue_str_dump(queue);
	tb_queue_exit(queue);
}
static tb_void_t tb_queue_efm_dump(tb_queue_t const* queue)
{
	tb_trace_i("efm size: %lu, maxn: %lu", tb_queue_size(queue), tb_queue_maxn(queue));
	tb_for_all (tb_char_t*, item, queue)
	{
		tb_trace_i("efm at[%lu]: %s", item_itor, item);
	}
}
static tb_void_t tb_queue_efm_test()
{
	tb_queue_t* queue = tb_queue_init(10, tb_item_func_efm(11, tb_fixed_pool_init(256, 11, 0)));
	tb_assert_and_check_return(queue);

	tb_trace_i("=============================================================");
	tb_trace_i("put:");
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

	tb_trace_i("=============================================================");
	tb_trace_i("pop:");
	tb_queue_pop(queue);
	tb_queue_pop(queue);
	tb_queue_pop(queue);
	tb_queue_pop(queue);
	tb_queue_pop(queue);
	tb_queue_efm_dump(queue);

	tb_trace_i("=============================================================");
	tb_trace_i("put:");
	tb_queue_put(queue, "0000000000");
	tb_queue_put(queue, "1111111111");
	tb_queue_put(queue, "2222222222");
	tb_queue_put(queue, "3333333333");
	tb_queue_put(queue, "4444444444");
	tb_queue_efm_dump(queue);

	tb_trace_i("=============================================================");
	tb_trace_i("clear:");
	tb_queue_clear(queue);
	tb_queue_efm_dump(queue);
	tb_queue_exit(queue);
}

static tb_void_t tb_queue_ifm_free(tb_item_func_t* func, tb_pointer_t item)
{
	tb_trace_i("ifm free: %s, priv: %s", item, func->priv);
}
static tb_void_t tb_queue_ifm_dump(tb_queue_t const* queue)
{
	tb_trace_i("ifm size: %lu, maxn: %lu", tb_queue_size(queue), tb_queue_maxn(queue));
	tb_for_all (tb_char_t*, item, queue)
	{
		tb_trace_i("ifm at[%lu]: %s", item_itor, item);
	}
}
static tb_void_t tb_queue_ifm_test()
{
	tb_queue_t* queue = tb_queue_init(10, tb_item_func_ifm(11, tb_queue_ifm_free, "ifm"));
	tb_assert_and_check_return(queue);

	tb_trace_i("=============================================================");
	tb_trace_i("put:");
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

	tb_trace_i("=============================================================");
	tb_trace_i("pop:");
	tb_queue_pop(queue);
	tb_queue_pop(queue);
	tb_queue_pop(queue);
	tb_queue_pop(queue);
	tb_queue_pop(queue);
	tb_queue_ifm_dump(queue);

	tb_trace_i("=============================================================");
	tb_trace_i("put:");
	tb_queue_put(queue, "0000000000");
	tb_queue_put(queue, "1111111111");
	tb_queue_put(queue, "2222222222");
	tb_queue_put(queue, "3333333333");
	tb_queue_put(queue, "4444444444");
	tb_queue_ifm_dump(queue);

	tb_trace_i("=============================================================");
	tb_trace_i("clear:");
	tb_queue_clear(queue);
	tb_queue_ifm_dump(queue);
	tb_queue_exit(queue);
}

static tb_void_t tb_queue_perf_test()
{
	tb_size_t score = 0;
	tb_trace_i("=============================================================");
	tb_trace_i("put & pop performance:");
	score += tb_queue_put_and_pop_test();

	tb_trace_i("=============================================================");
	tb_trace_i("iterator performance:");
	score += tb_queue_iterator_next_test();
	score += tb_queue_iterator_prev_test();

	tb_trace_i("=============================================================");
	tb_trace_i("score: %lu", score / 100);

}


/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_container_queue_main(tb_int_t argc, tb_char_t** argv)
{
	tb_queue_int_test();
	tb_queue_str_test();
	tb_queue_efm_test();
	tb_queue_ifm_test();
	tb_queue_perf_test();

	return 0;
}
