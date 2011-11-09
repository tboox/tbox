/* /////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"

/* /////////////////////////////////////////////////////////
 * macros
 */

#define TB_VECTOR_GROW_SIZE 			(256)

/* /////////////////////////////////////////////////////////
 * details
 */

static tb_void_t tb_vector_char_free(tb_void_t* data, tb_void_t* priv)
{
	tb_trace("[free]: %s, %c", (tb_char_t const*)priv, *((tb_char_t*)data));
}
static tb_void_t tb_vector_char_dump(tb_vector_t const* vector)
{
	tb_trace("size: %d, maxn: %d", tb_vector_size(vector), tb_vector_maxn(vector));
	tb_size_t itor = tb_vector_itor_head(vector);
	tb_size_t tail = tb_vector_itor_tail(vector);
	for (; itor != tail; itor = tb_vector_itor_next(vector, itor))
	{
		tb_byte_t const* item = tb_vector_itor_const_at(vector, itor);
		if (item)
		{
			tb_trace("at[%d]: %c", itor, *((tb_char_t const*)item));
		}
	}
}

static tb_size_t tb_vector_insert_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return(vector);

	tb_size_t i = 0;
	tb_size_t n = 100000;
	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_vector_insert(vector, tb_vector_size(vector) >> 1, "F");
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_vector_insert(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	tb_assert(tb_vector_size(vector) == n);
	tb_assert(((tb_char_t const*)tb_vector_const_at_head(vector))[0] == 'F');
	tb_assert(((tb_char_t const*)tb_vector_const_at_last(vector))[0] == 'F');

	// clear it
	tb_vector_clear(vector);
	tb_assert(!tb_vector_size(vector));

	// exit
	tb_vector_exit(vector);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_vector_insert_head_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return(vector);

	tb_size_t i = 0;
	tb_size_t n = 100000;
	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_vector_insert_head(vector, "F");
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_vector_insert_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	tb_assert(tb_vector_size(vector) == n);
	tb_assert(((tb_char_t const*)tb_vector_const_at_head(vector))[0] == 'F');
	tb_assert(((tb_char_t const*)tb_vector_const_at_last(vector))[0] == 'F');

	// clear it
	tb_vector_clear(vector);
	tb_assert(!tb_vector_size(vector));

	// exit
	tb_vector_exit(vector);
	
	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_vector_insert_tail_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return(vector);

	tb_size_t i = 0;
	tb_size_t n = 1000000;
	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_vector_insert_tail(vector, "F");
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_vector_insert_tail(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	tb_assert(tb_vector_size(vector) == n);
	tb_assert(((tb_char_t const*)tb_vector_const_at_head(vector))[0] == 'F');
	tb_assert(((tb_char_t const*)tb_vector_const_at_last(vector))[0] == 'F');

	// clear it
	tb_vector_clear(vector);
	tb_assert(!tb_vector_size(vector));

	// exit
	tb_vector_exit(vector);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_vector_ninsert_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return(vector);

	// insert one first
	tb_vector_insert_head(vector, "F");

	tb_size_t n = 1000000;
	tb_int64_t t = tb_mclock();
	tb_vector_ninsert(vector, 1, "F", n);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_vector_ninsert(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	tb_assert(tb_vector_size(vector) == n + 1);
	tb_assert(((tb_char_t const*)tb_vector_const_at_head(vector))[0] == 'F');
	tb_assert(((tb_char_t const*)tb_vector_const_at_last(vector))[0] == 'F');

	// clear it
	tb_vector_clear(vector);
	tb_assert(!tb_vector_size(vector));

	// exit
	tb_vector_exit(vector);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_vector_ninsert_head_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return(vector);

	tb_size_t n = 1000000;
	tb_int64_t t = tb_mclock();
	tb_vector_ninsert_head(vector, "F", n);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_vector_ninsert_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	tb_assert(tb_vector_size(vector) == n);
	tb_assert(((tb_char_t const*)tb_vector_const_at_head(vector))[0] == 'F');
	tb_assert(((tb_char_t const*)tb_vector_const_at_last(vector))[0] == 'F');

	// clear it
	tb_vector_clear(vector);
	tb_assert(!tb_vector_size(vector));

	// exit
	tb_vector_exit(vector);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_vector_ninsert_tail_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return(vector);

	tb_size_t n = 1000000;
	tb_int64_t t = tb_mclock();
	tb_vector_ninsert_tail(vector, "F", n);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_vector_ninsert_tail(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	tb_assert(tb_vector_size(vector) == n);
	tb_assert(((tb_char_t const*)tb_vector_const_at_head(vector))[0] == 'F');
	tb_assert(((tb_char_t const*)tb_vector_const_at_last(vector))[0] == 'F');

	// clear it
	tb_vector_clear(vector);
	tb_assert(!tb_vector_size(vector));

	// exit
	tb_vector_exit(vector);

	return n / (tb_int64_to_int32(t) + 1);
}

static tb_size_t tb_vector_remove_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return(vector);

	tb_size_t i = 0;
	tb_size_t n = 100000;
	tb_vector_ninsert_head(vector, "F", n);
	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_vector_remove(vector, tb_vector_size(vector) >> 1);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_vector_remove(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	tb_assert(!tb_vector_size(vector));

	// exit
	tb_vector_exit(vector);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_vector_remove_head_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return(vector);

	tb_size_t i = 0;
	tb_size_t n = 100000;
	tb_vector_ninsert_head(vector, "F", n);
	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_vector_remove_head(vector);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_vector_remove_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	tb_assert(!tb_vector_size(vector));

	// exit
	tb_vector_exit(vector);
}
static tb_size_t tb_vector_remove_last_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return(vector);

	tb_size_t i = 0;
	tb_size_t n = 1000000;
	tb_vector_ninsert_head(vector, "F", n);
	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_vector_remove_last(vector);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_vector_remove_last(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	tb_assert(!tb_vector_size(vector));

	// exit
	tb_vector_exit(vector);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_vector_nremove_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return(vector);

	tb_size_t n = 1000000;
	tb_vector_ninsert_head(vector, "F", n);
	tb_int64_t t = tb_mclock();
	tb_vector_nremove(vector, tb_vector_itor_head(vector), n);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_vector_nremove(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	tb_assert(!tb_vector_size(vector));

	// exit
	tb_vector_exit(vector);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_vector_nremove_head_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return(vector);

	tb_size_t n = 1000000;
	tb_vector_ninsert_head(vector, "F", n);
	tb_int64_t t = tb_mclock();
	tb_vector_nremove_head(vector, n);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_vector_nremove_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	tb_assert(!tb_vector_size(vector));

	// exit
	tb_vector_exit(vector);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_vector_nremove_last_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return(vector);

	tb_size_t n = 1000000;
	tb_vector_ninsert_head(vector, "F", n);
	tb_int64_t t = tb_mclock();
	tb_vector_nremove_last(vector, n);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_vector_nremove_last(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	tb_assert(!tb_vector_size(vector));

	// exit
	tb_vector_exit(vector);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_vector_replace_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return(vector);

	tb_size_t n = 1000000;
	tb_vector_ninsert_head(vector, "F", n);
	tb_size_t itor = tb_vector_itor_head(vector);
	tb_size_t tail = tb_vector_itor_tail(vector);
	tb_int64_t t = tb_mclock();
	for (; itor != tail; itor = tb_vector_itor_next(vector, itor)) tb_vector_replace(vector, itor, "R");
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_vector_replace(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	tb_assert(tb_vector_size(vector) == n);
	tb_assert(((tb_char_t const*)tb_vector_const_at_head(vector))[0] == 'R');
	tb_assert(((tb_char_t const*)tb_vector_const_at_last(vector))[0] == 'R');

	// exit
	tb_vector_exit(vector);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_vector_replace_head_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return(vector);

	tb_size_t i = 0;
	tb_size_t n = 1000000;
	tb_vector_ninsert_head(vector, "F", n);
	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_vector_replace_head(vector, "R");
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_vector_replace_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	tb_assert(tb_vector_size(vector) == n);
	tb_assert(((tb_char_t const*)tb_vector_const_at_head(vector))[0] == 'R');

	// exit
	tb_vector_exit(vector);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_vector_replace_last_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return(vector);

	tb_size_t i = 0;
	tb_size_t n = 1000000;
	tb_vector_ninsert_head(vector, "F", n);
	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_vector_replace_last(vector, "R");
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_vector_replace_last(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	tb_assert(tb_vector_size(vector) == n);
	tb_assert(((tb_char_t const*)tb_vector_const_at_last(vector))[0] == 'R');


	// exit
	tb_vector_exit(vector);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_vector_nreplace_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return(vector);

	tb_size_t n = 1000000;
	tb_vector_ninsert_head(vector, "F", n);
	tb_int64_t t = tb_mclock();
	tb_vector_nreplace(vector, tb_vector_itor_head(vector), "R", n);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_vector_nreplace(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	tb_assert(tb_vector_size(vector) == n);
	tb_assert(((tb_char_t const*)tb_vector_const_at_head(vector))[0] == 'R');
	tb_assert(((tb_char_t const*)tb_vector_const_at_last(vector))[0] == 'R');

	// exit
	tb_vector_exit(vector);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_vector_nreplace_head_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return(vector);

	tb_size_t n = 1000000;
	tb_vector_ninsert_head(vector, "F", n);
	tb_int64_t t = tb_mclock();
	tb_vector_nreplace_head(vector, "R", n);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_vector_nreplace_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	tb_assert(tb_vector_size(vector) == n);
	tb_assert(((tb_char_t const*)tb_vector_const_at_head(vector))[0] == 'R');
	tb_assert(((tb_char_t const*)tb_vector_const_at_last(vector))[0] == 'R');

	// exit
	tb_vector_exit(vector);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_vector_nreplace_last_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return(vector);

	tb_size_t n = 1000000;
	tb_vector_ninsert_head(vector, "F", n);
	tb_int64_t t = tb_mclock();
	tb_vector_nreplace_last(vector, "R", n);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_vector_nreplace_last(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	tb_assert(tb_vector_size(vector) == n);
	tb_assert(((tb_char_t const*)tb_vector_const_at_head(vector))[0] == 'R');
	tb_assert(((tb_char_t const*)tb_vector_const_at_last(vector))[0] == 'R');


	// exit
	tb_vector_exit(vector);

	return n / (tb_int64_to_int32(t) + 1);
}

static tb_size_t tb_vector_iterator_next_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return(vector);

	tb_size_t n = 1000000;
	tb_vector_ninsert_head(vector, "F", n);
	tb_size_t itor = tb_vector_itor_head(vector);
	tb_size_t tail = tb_vector_itor_tail(vector);
	tb_int64_t t = tb_mclock();
	for (; itor != tail; itor = tb_vector_itor_next(vector, itor))
	{
		__tb_volatile__ tb_byte_t const* item = tb_vector_itor_const_at(vector, itor);
	}
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_vector_iterator_next(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// exit
	tb_vector_exit(vector);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_vector_iterator_prev_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return(vector);

	tb_size_t n = 1000000;
	tb_vector_ninsert_head(vector, "F", n);
	tb_size_t itor = tb_vector_itor_last(vector);
	tb_size_t head = tb_vector_itor_head(vector);
	tb_int64_t t = tb_mclock();
	while (1)
	{
		__tb_volatile__ tb_byte_t const* item = tb_vector_itor_const_at(vector, itor);

		if (itor == head) break;
		itor = tb_vector_itor_prev(vector, itor);
	}
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_vector_iterator_prev(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// exit
	tb_vector_exit(vector);

	return n / (tb_int64_to_int32(t) + 1);
}

/* /////////////////////////////////////////////////////////
 * main
 */
int main(int argc, char** argv)
{
	if (!tb_init(malloc(30 * 1024 * 1024), 30 * 1024 * 1024)) return 0;

	// init vector
	tb_vector_item_func_t func = {tb_vector_char_free, "char"};
	tb_vector_t* vector = tb_vector_init(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, &func);
	tb_assert_and_check_goto(vector, end);

	tb_trace("=============================================================");
	tb_trace("insert:");
	tb_vector_ninsert_head(vector, "H", 10); 
	tb_vector_ninsert_tail(vector, "T", 10);
	tb_vector_insert(vector, 10, "0");
	tb_vector_insert(vector, 10, "1");
	tb_vector_insert(vector, 10, "2");
	tb_vector_insert(vector, 10, "3");
	tb_vector_insert(vector, 10, "4");
	tb_vector_insert(vector, 10, "5");
	tb_vector_insert(vector, 10, "6");
	tb_vector_insert(vector, 10, "7");
	tb_vector_insert(vector, 10, "8");
	tb_vector_insert(vector, 10, "9");
	tb_vector_insert_head(vector, "4");
	tb_vector_insert_head(vector, "3");
	tb_vector_insert_head(vector, "2");
	tb_vector_insert_head(vector, "1");
	tb_vector_insert_head(vector, "0");
	tb_vector_insert_tail(vector, "5");
	tb_vector_insert_tail(vector, "6");
	tb_vector_insert_tail(vector, "7");
	tb_vector_insert_tail(vector, "8");
	tb_vector_insert_tail(vector, "9");
	tb_vector_char_dump(vector);

	tb_trace("=============================================================");
	tb_trace("remove:");
	tb_vector_nremove_head(vector, 5);
	tb_vector_nremove_last(vector, 5);
	tb_vector_char_dump(vector);

	tb_trace("=============================================================");
	tb_trace("replace:");
	tb_vector_nreplace_head(vector, "T", 10);
	tb_vector_nreplace_last(vector, "H", 10);
	tb_vector_replace_head(vector, "O");
	tb_vector_replace_last(vector, "O");
	tb_vector_char_dump(vector);

	tb_trace("=============================================================");
	tb_trace("clear:");
	tb_vector_clear(vector);
	tb_vector_char_dump(vector);

	tb_size_t score = 0;
	tb_trace("=============================================================");
	tb_trace("insert performance:");
	score += tb_vector_insert_test();
	score += tb_vector_insert_head_test();
	score += tb_vector_insert_tail_test();
	score += tb_vector_ninsert_test();
	score += tb_vector_ninsert_head_test();
	score += tb_vector_ninsert_tail_test();

	tb_trace("=============================================================");
	tb_trace("remove performance:");
	score += tb_vector_remove_test();
	score += tb_vector_remove_head_test();
	score += tb_vector_remove_last_test();
	score += tb_vector_nremove_test();
	score += tb_vector_nremove_head_test();
	score += tb_vector_nremove_last_test();

	tb_trace("=============================================================");
	tb_trace("replace performance:");
	score += tb_vector_replace_test();
	score += tb_vector_replace_head_test();
	score += tb_vector_replace_last_test();
	score += tb_vector_nreplace_test();
	score += tb_vector_nreplace_head_test();
	score += tb_vector_nreplace_last_test();

	tb_trace("=============================================================");
	tb_trace("iterator performance:");
	score += tb_vector_iterator_next_test();
	score += tb_vector_iterator_prev_test();

	tb_trace("=============================================================");
	tb_trace("score: %d", score / 100);

	getchar();
end:

	// exit vector
	tb_vector_exit(vector);

	return 0;
}
