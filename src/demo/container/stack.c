/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * details
 */

static tb_size_t tb_stack_put_and_pop_test()
{
	// init
	tb_stack_t* stack = tb_stack_init(10, tb_item_func_long());
	tb_assert_and_check_return_val(stack, 0);

	tb_stack_put(stack, 0);
	tb_stack_put(stack, 1);
	tb_stack_put(stack, 2);
	tb_stack_put(stack, 3);
	tb_stack_put(stack, 4);
	tb_stack_put(stack, 5);
	tb_stack_put(stack, 6);
	tb_stack_put(stack, 7);
	tb_stack_put(stack, 8);
	tb_stack_put(stack, 9);

	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 10000;
	tb_hong_t t = tb_mclock();
	for (i = 0; i < n; i++) 
	{
		tb_stack_pop(stack);
		tb_stack_put(stack, 0xf);
	}
	t = tb_mclock() - t;

	// time
	tb_print("tb_stack_put_and_pop(%d): %lld ms, size: %d, maxn: %d", n, t, tb_stack_size(stack), tb_stack_maxn(stack));

	// check
	tb_assert(tb_stack_size(stack) == 10);
	tb_assert(tb_stack_head(stack) == 0x0);
	tb_assert(tb_stack_last(stack) == 0xf);

	// clear it
	tb_stack_clear(stack);
	tb_assert(!tb_stack_size(stack));

	// exit
	tb_stack_exit(stack);

	return n / ((tb_uint32_t)(t) + 1);
}

static tb_size_t tb_stack_iterator_next_test()
{
	// init
	tb_size_t n = 1000000;
	tb_stack_t* stack = tb_stack_init(n, tb_item_func_long());
	tb_assert_and_check_return_val(stack, 0);

	tb_size_t i;
	for (i = 0; i < n; i++) tb_stack_put(stack, 0xf);
	__tb_volatile__ tb_size_t itor = tb_iterator_head(stack);
	__tb_volatile__ tb_size_t tail = tb_iterator_tail(stack);
	tb_hong_t t = tb_mclock();
	for (; itor != tail; itor = tb_iterator_next(stack, itor))
	{
		__tb_volatile__ tb_byte_t const* item = tb_iterator_item(stack, itor);
		tb_used(item);
	}
	t = tb_mclock() - t;

	// time
	tb_print("tb_stack_iterator_next(%d): %lld ms, size: %d, maxn: %d", 1000000, t, tb_stack_size(stack), tb_stack_maxn(stack));

	// exit
	tb_stack_exit(stack);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_stack_iterator_prev_test()
{
	// init
	tb_size_t n = 1000000;
	tb_stack_t* stack = tb_stack_init(n, tb_item_func_long());
	tb_assert_and_check_return_val(stack, 0);

	tb_size_t i;
	for (i = 0; i < n; i++) tb_stack_put(stack, 0xf);
	__tb_volatile__ tb_size_t itor = tb_iterator_last(stack);
	__tb_volatile__ tb_size_t head = tb_iterator_head(stack);
	tb_hong_t t = tb_mclock();
	while (1)
	{
		__tb_volatile__ tb_byte_t const* item = tb_iterator_item(stack, itor);
		tb_used(item);

		if (itor == head) break;
		itor = tb_iterator_prev(stack, itor);
	}
	t = tb_mclock() - t;

	// time
	tb_print("tb_stack_iterator_prev(%d): %lld ms, size: %d, maxn: %d", 1000000, t, tb_stack_size(stack), tb_stack_maxn(stack));

	// exit
	tb_stack_exit(stack);

	return n / ((tb_uint32_t)(t) + 1);
}

static tb_void_t tb_stack_efm_dump(tb_stack_t const* stack)
{
	tb_print("efm size: %d, maxn: %d", tb_stack_size(stack), tb_stack_maxn(stack));
	tb_size_t itor = tb_iterator_head(stack);
	tb_size_t tail = tb_iterator_tail(stack);
	for (; itor != tail; itor = tb_iterator_next(stack, itor))
		tb_print("efm at[%d]: %s", itor, tb_iterator_item(stack, itor));
}
static tb_void_t tb_stack_efm_test()
{	
	tb_stack_t* stack = tb_stack_init(10, tb_item_func_efm(11, tb_rpool_init(256, 11, 0)));
	tb_assert_and_check_return(stack);

	tb_print("=============================================================");
	tb_print("put:");
	tb_stack_put(stack, "0000000000");
	tb_stack_put(stack, "1111111111");
	tb_stack_put(stack, "2222222222");
	tb_stack_put(stack, "3333333333");
	tb_stack_put(stack, "4444444444");
	tb_stack_put(stack, "5555555555");
	tb_stack_put(stack, "6666666666");
	tb_stack_put(stack, "7777777777");
	tb_stack_put(stack, "8888888888");
	tb_stack_put(stack, "9999999999");
	tb_stack_efm_dump(stack);

	tb_print("=============================================================");
	tb_print("pop:");
	tb_stack_pop(stack);
	tb_stack_pop(stack);
	tb_stack_pop(stack);
	tb_stack_pop(stack);
	tb_stack_pop(stack);
	tb_stack_efm_dump(stack);

	tb_print("=============================================================");
	tb_print("put:");
	tb_stack_put(stack, "0000000000");
	tb_stack_put(stack, "1111111111");
	tb_stack_put(stack, "2222222222");
	tb_stack_put(stack, "3333333333");
	tb_stack_put(stack, "4444444444");
	tb_stack_efm_dump(stack);

	tb_print("=============================================================");
	tb_print("clear:");
	tb_stack_clear(stack);
	tb_stack_efm_dump(stack);

	tb_stack_exit(stack);
}

static tb_void_t tb_stack_perf_test()
{
	tb_size_t score = 0;
	tb_print("=============================================================");
	tb_print("put & pop performance:");
	score += tb_stack_put_and_pop_test();

	tb_print("=============================================================");
	tb_print("iterator performance:");
	score += tb_stack_iterator_next_test();
	score += tb_stack_iterator_prev_test();

	tb_print("=============================================================");
	tb_print("score: %lu", score / 100);

}
/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	if (!tb_init(malloc(30 * 1024 * 1024), 30 * 1024 * 1024)) return 0;

	tb_stack_efm_test();
	tb_stack_perf_test();

	return 0;
}
