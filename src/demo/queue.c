/* /////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"

/* /////////////////////////////////////////////////////////
 * details
 */
static tb_void_t tb_queue_efm_dump(tb_queue_t const* queue)
{
	tb_trace("size: %d, maxn: %d", tb_queue_size(queue), tb_queue_maxn(queue));
	tb_size_t itor = tb_queue_itor_head(queue);
	tb_size_t tail = tb_queue_itor_tail(queue);
	for (; itor != tail; itor = tb_queue_itor_next(queue, itor))
		tb_trace("at[%d]: %s", itor, tb_queue_itor_const_at(queue, itor));
}
static tb_size_t tb_queue_put_and_pop_test()
{
	// init
	tb_queue_t* queue = tb_queue_init(10, tb_item_func_int());
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
	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) 
	{
		tb_queue_pop(queue);
		tb_queue_put(queue, 0xf);
	}
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_queue_put_and_pop(%d): %lld ms, size: %d, maxn: %d", n, t, tb_queue_size(queue), tb_queue_maxn(queue));

	// check
	tb_assert(tb_queue_size(queue) == 10);
	tb_assert(tb_queue_const_at_head(queue) == 0xf);
	tb_assert(tb_queue_const_at_last(queue) == 0xf);

	// clear it
	tb_queue_clear(queue);
	tb_assert(!tb_queue_size(queue));

	// exit
	tb_queue_exit(queue);

	return n / (tb_int64_to_int32(t) + 1);
}

static tb_size_t tb_queue_iterator_next_test()
{
	// init
	tb_size_t n = 1000000;
	tb_queue_t* queue = tb_queue_init(n, tb_item_func_int());
	tb_assert_and_check_return_val(queue, 0);

	while (n--) tb_queue_put(queue, 0xf);
	__tb_volatile__ tb_size_t itor = tb_queue_itor_head(queue);
	__tb_volatile__ tb_size_t tail = tb_queue_itor_tail(queue);
	tb_int64_t t = tb_mclock();
	for (; itor != tail; itor = tb_queue_itor_next(queue, itor))
	{
		__tb_volatile__ tb_cpointer_t item = tb_queue_itor_const_at(queue, itor);
	}
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_queue_iterator_next(%d): %lld ms, size: %d, maxn: %d", 1000000, t, tb_queue_size(queue), tb_queue_maxn(queue));

	// exit
	tb_queue_exit(queue);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_queue_iterator_prev_test()
{
	// init
	tb_size_t n = 1000000;
	tb_queue_t* queue = tb_queue_init(n, tb_item_func_int());
	tb_assert_and_check_return_val(queue, 0);

	while (n--) tb_queue_put(queue, 0xf);
	__tb_volatile__ tb_size_t itor = tb_queue_itor_last(queue);
	__tb_volatile__ tb_size_t head = tb_queue_itor_head(queue);
	tb_int64_t t = tb_mclock();
	while (1)
	{
		__tb_volatile__ tb_cpointer_t item = tb_queue_itor_const_at(queue, itor);

		if (itor == head) break;
		itor = tb_queue_itor_prev(queue, itor);
	}
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_queue_iterator_prev(%d): %lld ms, size: %d, maxn: %d", 1000000, t, tb_queue_size(queue), tb_queue_maxn(queue));

	// exit
	tb_queue_exit(queue);

	return n / (tb_int64_to_int32(t) + 1);
}

/* /////////////////////////////////////////////////////////
 * main
 */
int main(int argc, char** argv)
{
	if (!tb_init(malloc(30 * 1024 * 1024), 30 * 1024 * 1024)) return 0;

	// init queue
	tb_queue_t* queue = tb_queue_init(10, tb_item_func_efm(11, tb_fpool_init(11, 256, 256, TB_NULL)));
	tb_assert_and_check_goto(queue, end);

	tb_trace("=============================================================");
	tb_trace("put:");
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

	tb_trace("=============================================================");
	tb_trace("pop:");
	tb_queue_pop(queue);
	tb_queue_pop(queue);
	tb_queue_pop(queue);
	tb_queue_pop(queue);
	tb_queue_pop(queue);
	tb_queue_efm_dump(queue);

	tb_trace("=============================================================");
	tb_trace("put:");
	tb_queue_put(queue, "0000000000");
	tb_queue_put(queue, "1111111111");
	tb_queue_put(queue, "2222222222");
	tb_queue_put(queue, "3333333333");
	tb_queue_put(queue, "4444444444");
	tb_queue_efm_dump(queue);

	tb_trace("=============================================================");
	tb_trace("clear:");
	tb_queue_clear(queue);
	tb_queue_efm_dump(queue);

	tb_size_t score = 0;
	tb_trace("=============================================================");
	tb_trace("put & pop performance:");
	score += tb_queue_put_and_pop_test();

	tb_trace("=============================================================");
	tb_trace("iterator performance:");
	score += tb_queue_iterator_next_test();
	score += tb_queue_iterator_prev_test();

	tb_trace("=============================================================");
	tb_trace("score: %d", score / 100);

	getchar();
end:

	// exit queue
	tb_queue_exit(queue);

	return 0;
}
