/* /////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"

/* /////////////////////////////////////////////////////////
 * details
 */

static tb_void_t tb_stack_char_free(tb_void_t* data, tb_void_t* priv)
{
	tb_trace("[free]: %s, %c", (tb_char_t const*)priv, *((tb_char_t*)data));
}
static tb_void_t tb_stack_char_dump(tb_stack_t const* stack)
{
	tb_trace("size: %d, maxn: %d", tb_stack_size(stack), tb_stack_maxn(stack));
	tb_size_t itor = tb_stack_head(stack);
	tb_size_t tail = tb_stack_tail(stack);
	for (; itor != tail; itor = tb_stack_next(stack, itor))
	{
		tb_byte_t const* item = tb_stack_const_at(stack, itor);
		if (item)
		{
			tb_trace("at[%d]: %c", itor, *((tb_char_t const*)item));
		}
	}
}
static tb_size_t tb_stack_put_and_pop_test()
{
	// init
	tb_stack_t* stack = tb_stack_init(sizeof(tb_char_t), 10, TB_NULL, TB_NULL);
	tb_assert_and_check_return_val(stack, 0);

	tb_stack_put(stack, "0");
	tb_stack_put(stack, "1");
	tb_stack_put(stack, "2");
	tb_stack_put(stack, "3");
	tb_stack_put(stack, "4");
	tb_stack_put(stack, "5");
	tb_stack_put(stack, "6");
	tb_stack_put(stack, "7");
	tb_stack_put(stack, "8");
	tb_stack_put(stack, "9");

	tb_size_t i = 0;
	tb_size_t n = 10000;
	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) 
	{
		tb_stack_pop(stack, TB_NULL);
		tb_stack_put(stack, "F");
	}
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_stack_put_and_pop(%d): %lld ms, size: %d, maxn: %d", n, t, tb_stack_size(stack), tb_stack_maxn(stack));

	// check
	tb_assert(tb_stack_size(stack) == 10);
	tb_assert(tb_stack_const_at_head(stack)[0] == '0');
	tb_assert(tb_stack_const_at_last(stack)[0] == 'F');

	// clear it
	tb_stack_clear(stack);
	tb_assert(!tb_stack_size(stack));

	// exit
	tb_stack_exit(stack);

	return n / (tb_int64_to_int32(t) + 1);
}

static tb_size_t tb_stack_iterator_next_test()
{
	// init
	tb_size_t n = 1000000;
	tb_stack_t* stack = tb_stack_init(sizeof(tb_char_t), n, TB_NULL, TB_NULL);
	tb_assert_and_check_return_val(stack, 0);

	while (n--) tb_stack_put(stack, "F");
	tb_size_t itor = tb_stack_head(stack);
	tb_size_t tail = tb_stack_tail(stack);
	tb_int64_t t = tb_mclock();
	for (; itor != tail; itor = tb_stack_next(stack, itor))
	{
		__tb_volatile__ tb_byte_t const* item = tb_stack_const_at(stack, itor);
	}
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_stack_iterator_next(%d): %lld ms, size: %d, maxn: %d", 1000000, t, tb_stack_size(stack), tb_stack_maxn(stack));

	// exit
	tb_stack_exit(stack);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_stack_iterator_prev_test()
{
	// init
	tb_size_t n = 1000000;
	tb_stack_t* stack = tb_stack_init(sizeof(tb_char_t), n, TB_NULL, TB_NULL);
	tb_assert_and_check_return_val(stack, 0);

	while (n--) tb_stack_put(stack, "F");
	tb_size_t itor = tb_stack_last(stack);
	tb_size_t head = tb_stack_head(stack);
	tb_int64_t t = tb_mclock();
	while (1)
	{
		__tb_volatile__ tb_byte_t const* item = tb_stack_const_at(stack, itor);

		if (itor == head) break;
		itor = tb_stack_prev(stack, itor);
	}
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_stack_iterator_prev(%d): %lld ms, size: %d, maxn: %d", 1000000, t, tb_stack_size(stack), tb_stack_maxn(stack));

	// exit
	tb_stack_exit(stack);

	return n / (tb_int64_to_int32(t) + 1);
}

/* /////////////////////////////////////////////////////////
 * main
 */
int main(int argc, char** argv)
{
	if (!tb_init(malloc(30 * 1024 * 1024), 30 * 1024 * 1024)) return 0;

	// init stack
	tb_stack_t* stack = tb_stack_init(sizeof(tb_char_t), 10, tb_stack_char_free, "char");
	tb_assert_and_check_goto(stack, end);


	tb_trace("=============================================================");
	tb_trace("put:");
	tb_stack_put(stack, "0");
	tb_stack_put(stack, "1");
	tb_stack_put(stack, "2");
	tb_stack_put(stack, "3");
	tb_stack_put(stack, "4");
	tb_stack_put(stack, "5");
	tb_stack_put(stack, "6");
	tb_stack_put(stack, "7");
	tb_stack_put(stack, "8");
	tb_stack_put(stack, "9");
	tb_stack_char_dump(stack);

	tb_trace("=============================================================");
	tb_trace("pop:");
	tb_stack_pop(stack, TB_NULL);
	tb_stack_pop(stack, TB_NULL);
	tb_stack_pop(stack, TB_NULL);
	tb_stack_pop(stack, TB_NULL);
	tb_stack_pop(stack, TB_NULL);
	tb_stack_char_dump(stack);

	tb_trace("=============================================================");
	tb_trace("put:");
	tb_stack_put(stack, "0");
	tb_stack_put(stack, "1");
	tb_stack_put(stack, "2");
	tb_stack_put(stack, "3");
	tb_stack_put(stack, "4");
	tb_stack_char_dump(stack);

	tb_trace("=============================================================");
	tb_trace("clear:");
	tb_stack_clear(stack);
	tb_stack_char_dump(stack);

	tb_size_t score = 0;
	tb_trace("=============================================================");
	tb_trace("put & pop performance:");
	score += tb_stack_put_and_pop_test();

	tb_trace("=============================================================");
	tb_trace("iterator performance:");
	score += tb_stack_iterator_next_test();
	score += tb_stack_iterator_prev_test();

	tb_trace("=============================================================");
	tb_trace("score: %d", score / 100);

	getchar();
end:

	// exit stack
	tb_stack_exit(stack);

	return 0;
}
