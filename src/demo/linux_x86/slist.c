/* /////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"

/* /////////////////////////////////////////////////////////
 * macros
 */

#define TB_SLIST_GROW_SIZE 			(256)

/* /////////////////////////////////////////////////////////
 * details
 */

static tb_void_t tb_slist_char_free(tb_void_t* item, tb_void_t* priv)
{
	tb_trace("[free]: %s, %c", (tb_char_t const*)priv, *((tb_char_t*)item));
}
static tb_void_t tb_slist_char_dump(tb_slist_t const* slist)
{
	tb_trace("size: %d, maxn: %d", tb_slist_size(slist), tb_slist_maxn(slist));
	tb_size_t itor = tb_slist_itor_head(slist);
	tb_size_t tail = tb_slist_itor_tail(slist);
	for (; itor != tail; itor = tb_slist_itor_next(slist, itor))
	{
		tb_byte_t const* item = tb_slist_itor_const_at(slist, itor);
		if (item)
		{
			tb_trace("at[%d]: %c", itor, *((tb_char_t const*)item));
		}
	}
}
static tb_size_t tb_slist_insert_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return_val(slist, 0);

	// insert one first
	tb_size_t index = tb_slist_insert_head(slist, "F");

	tb_size_t i = 0;
	tb_size_t n = 10000;
	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_slist_insert(slist, index, "F");
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_slist_insert(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	tb_assert(tb_slist_size(slist) == n + 1);
	tb_assert(((tb_char_t const*)tb_slist_const_at_head(slist))[0] == 'F');
	tb_assert(!tb_strcmp("F", tb_slist_const_at_last(slist)));

	// clear it
	tb_slist_clear(slist);
	tb_assert(!tb_slist_size(slist));

	// exit
	tb_slist_exit(slist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_slist_insert_head_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return_val(slist, 0);

	tb_size_t i = 0;
	tb_size_t n = 1000000;
	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_slist_insert_head(slist, "F");
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_slist_insert_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	tb_assert(tb_slist_size(slist) == n);
	tb_assert(((tb_char_t const*)tb_slist_const_at_head(slist))[0] == 'F');
	tb_assert(!tb_strcmp("F", tb_slist_const_at_last(slist)));

	// clear it
	tb_slist_clear(slist);
	tb_assert(!tb_slist_size(slist));

	// exit
	tb_slist_exit(slist);
	
	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_slist_insert_tail_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return_val(slist, 0);

	tb_size_t i = 0;
	tb_size_t n = 1000000;
	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_slist_insert_tail(slist, "F");
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_slist_insert_tail(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	tb_assert(tb_slist_size(slist) == n);
	tb_assert(((tb_char_t const*)tb_slist_const_at_head(slist))[0] == 'F');
	tb_assert(!tb_strcmp("F", tb_slist_const_at_last(slist)));

	// clear it
	tb_slist_clear(slist);
	tb_assert(!tb_slist_size(slist));

	// exit
	tb_slist_exit(slist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_slist_ninsert_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return_val(slist, 0);

	// insert one first
	tb_size_t index = tb_slist_insert_head(slist, "F");

	tb_size_t n = 1000000;
	tb_int64_t t = tb_mclock();
	tb_slist_ninsert(slist, index, "F", n);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_slist_ninsert(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	tb_assert(tb_slist_size(slist) == n + 1);
	tb_assert(((tb_char_t const*)tb_slist_const_at_head(slist))[0] == 'F');
	tb_assert(!tb_strcmp("F", tb_slist_const_at_last(slist)));

	// clear it
	tb_slist_clear(slist);
	tb_assert(!tb_slist_size(slist));

	// exit
	tb_slist_exit(slist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_slist_ninsert_head_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return_val(slist, 0);

	tb_size_t n = 1000000;
	tb_int64_t t = tb_mclock();
	tb_slist_ninsert_head(slist, "F", n);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_slist_ninsert_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	tb_assert(tb_slist_size(slist) == n);
	tb_assert(((tb_char_t const*)tb_slist_const_at_head(slist))[0] == 'F');
	tb_assert(!tb_strcmp("F", tb_slist_const_at_last(slist)));

	// clear it
	tb_slist_clear(slist);
	tb_assert(!tb_slist_size(slist));

	// exit
	tb_slist_exit(slist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_slist_ninsert_tail_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return_val(slist, 0);

	tb_size_t n = 1000000;
	tb_int64_t t = tb_mclock();
	tb_slist_ninsert_tail(slist, "F", n);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_slist_ninsert_tail(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	tb_assert(tb_slist_size(slist) == n);
	tb_assert(((tb_char_t const*)tb_slist_const_at_head(slist))[0] == 'F');
	tb_assert(!tb_strcmp("F", tb_slist_const_at_last(slist)));

	// clear it
	tb_slist_clear(slist);
	tb_assert(!tb_slist_size(slist));

	// exit
	tb_slist_exit(slist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_slist_remove_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return_val(slist, 0);

	tb_size_t i = 0;
	tb_size_t n = 1000000;
	tb_size_t index = tb_slist_ninsert_head(slist, "F", n);
	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) index = tb_slist_remove(slist, index);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_slist_remove(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	tb_assert(!tb_slist_size(slist));

	// exit
	tb_slist_exit(slist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_slist_remove_head_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return_val(slist, 0);

	tb_size_t i = 0;
	tb_size_t n = 1000000;
	tb_size_t index = tb_slist_ninsert_head(slist, "F", n);
	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_slist_remove_head(slist);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_slist_remove_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	tb_assert(!tb_slist_size(slist));

	// exit
	tb_slist_exit(slist);
}
static tb_size_t tb_slist_remove_last_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return_val(slist, 0);

	tb_size_t i = 0;
	tb_size_t n = 10000;
	tb_size_t index = tb_slist_ninsert_head(slist, "F", n);
	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_slist_remove_last(slist);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_slist_remove_last(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	tb_assert(!tb_slist_size(slist));

	// exit
	tb_slist_exit(slist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_slist_nremove_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return_val(slist, 0);

	tb_size_t n = 1000000;
	tb_size_t index = tb_slist_ninsert_head(slist, "F", n);
	tb_int64_t t = tb_mclock();
	tb_slist_nremove(slist, tb_slist_itor_head(slist), n);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_slist_nremove(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	tb_assert(!tb_slist_size(slist));

	// exit
	tb_slist_exit(slist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_slist_nremove_head_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return_val(slist, 0);

	tb_size_t n = 1000000;
	tb_size_t index = tb_slist_ninsert_head(slist, "F", n);
	tb_int64_t t = tb_mclock();
	tb_slist_nremove_head(slist, n);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_slist_nremove_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	tb_assert(!tb_slist_size(slist));

	// exit
	tb_slist_exit(slist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_slist_nremove_last_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return_val(slist, 0);

	tb_size_t n = 1000000;
	tb_size_t index = tb_slist_ninsert_head(slist, "F", n);
	tb_int64_t t = tb_mclock();
	tb_slist_nremove_last(slist, n);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_slist_nremove_last(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	tb_assert(!tb_slist_size(slist));

	// exit
	tb_slist_exit(slist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_slist_replace_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return_val(slist, 0);

	tb_size_t n = 1000000;
	tb_size_t index = tb_slist_ninsert_head(slist, "F", n);
	tb_size_t itor = tb_slist_itor_head(slist);
	tb_size_t tail = tb_slist_itor_tail(slist);
	tb_int64_t t = tb_mclock();
	for (; itor != tail; itor = tb_slist_itor_next(slist, itor)) tb_slist_replace(slist, itor, "R");
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_slist_replace(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	tb_assert(tb_slist_size(slist) == n);
	tb_assert(((tb_char_t const*)tb_slist_const_at_head(slist))[0] == 'R');
	tb_assert(((tb_char_t const*)tb_slist_const_at_last(slist))[0] == 'R');

	// exit
	tb_slist_exit(slist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_slist_replace_head_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return_val(slist, 0);

	tb_size_t i = 0;
	tb_size_t n = 1000000;
	tb_size_t index = tb_slist_ninsert_head(slist, "F", n);
	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_slist_replace_head(slist, "R");
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_slist_replace_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	tb_assert(tb_slist_size(slist) == n);
	tb_assert(((tb_char_t const*)tb_slist_const_at_head(slist))[0] == 'R');

	// exit
	tb_slist_exit(slist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_slist_replace_last_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return_val(slist, 0);

	tb_size_t i = 0;
	tb_size_t n = 1000000;
	tb_size_t index = tb_slist_ninsert_head(slist, "F", n);
	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_slist_replace_last(slist, "R");
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_slist_replace_last(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	tb_assert(tb_slist_size(slist) == n);
	tb_assert(((tb_char_t const*)tb_slist_const_at_last(slist))[0] == 'R');


	// exit
	tb_slist_exit(slist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_slist_nreplace_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return_val(slist, 0);

	tb_size_t n = 1000000;
	tb_size_t index = tb_slist_ninsert_head(slist, "F", n);
	tb_int64_t t = tb_mclock();
	tb_slist_nreplace(slist, tb_slist_itor_head(slist), "R", n);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_slist_nreplace(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	tb_assert(tb_slist_size(slist) == n);
	tb_assert(((tb_char_t const*)tb_slist_const_at_head(slist))[0] == 'R');
	tb_assert(((tb_char_t const*)tb_slist_const_at_last(slist))[0] == 'R');

	// exit
	tb_slist_exit(slist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_slist_nreplace_head_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return_val(slist, 0);

	tb_size_t n = 1000000;
	tb_size_t index = tb_slist_ninsert_head(slist, "F", n);
	tb_int64_t t = tb_mclock();
	tb_slist_nreplace_head(slist, "R", n);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_slist_nreplace_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	tb_assert(tb_slist_size(slist) == n);
	tb_assert(((tb_char_t const*)tb_slist_const_at_head(slist))[0] == 'R');
	tb_assert(((tb_char_t const*)tb_slist_const_at_last(slist))[0] == 'R');

	// exit
	tb_slist_exit(slist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_slist_nreplace_last_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return_val(slist, 0);

	tb_size_t n = 1000000;
	tb_size_t index = tb_slist_ninsert_head(slist, "F", n);
	tb_int64_t t = tb_mclock();
	tb_slist_nreplace_last(slist, "R", n);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_slist_nreplace_last(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	tb_assert(tb_slist_size(slist) == n);
	tb_assert(((tb_char_t const*)tb_slist_const_at_head(slist))[0] == 'R');
	tb_assert(((tb_char_t const*)tb_slist_const_at_last(slist))[0] == 'R');

	// exit
	tb_slist_exit(slist);

	return n / (tb_int64_to_int32(t) + 1);
}

static tb_size_t tb_slist_iterator_next_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return_val(slist, 0);

	tb_size_t n = 1000000;
	tb_slist_ninsert_head(slist, "F", n);
	tb_size_t itor = tb_slist_itor_head(slist);
	tb_size_t tail = tb_slist_itor_tail(slist);
	tb_int64_t t = tb_mclock();
	for (; itor != tail; itor = tb_slist_itor_next(slist, itor))
	{
		__tb_volatile__ tb_byte_t const* item = tb_slist_itor_const_at(slist, itor);
	}
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_slist_iterator_next(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// exit
	tb_slist_exit(slist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_slist_iterator_prev_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL);
	tb_assert_and_check_return_val(slist, 0);

	tb_size_t n = 10000;
	tb_slist_ninsert_head(slist, "F", n);
	tb_size_t itor = tb_slist_itor_last(slist);
	tb_size_t head = tb_slist_itor_head(slist);
	tb_int64_t t = tb_mclock();
	while (1)
	{
		__tb_volatile__ tb_byte_t const* item = tb_slist_itor_const_at(slist, itor);

		if (itor == head) break;
		itor = tb_slist_itor_prev(slist, itor);
	}
	t = tb_int64_sub(tb_mclock(), t);

	// time
	tb_trace("tb_slist_iterator_prev(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// exit
	tb_slist_exit(slist);

	return n / (tb_int64_to_int32(t) + 1);
}
/* /////////////////////////////////////////////////////////
 * main
 */
int main(int argc, char** argv)
{
	if (!tb_init(malloc(30 * 1024 * 1024), 30 * 1024 * 1024)) return 0;

	// init slist
	tb_slist_item_func_t func = {tb_slist_char_free, "char"};
	tb_slist_t* slist = tb_slist_init(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, &func);
	tb_assert_and_check_goto(slist, end);

	tb_size_t 			i = 0;
	tb_size_t 			j = 0;

	tb_trace("=============================================================");
	tb_trace("insert:");
	tb_slist_ninsert_head(slist, "H", 10); 
	i = tb_slist_ninsert_tail(slist, "T", 10);
	j = tb_slist_insert(slist, i, "0");
	tb_slist_insert(slist, i, "1");
	tb_slist_insert(slist, i, "2");
	tb_slist_insert(slist, i, "3");
	tb_slist_insert(slist, i, "4");
	tb_slist_insert(slist, i, "5");
	tb_slist_insert(slist, i, "6");
	tb_slist_insert(slist, i, "7");
	tb_slist_insert(slist, i, "8");
	tb_slist_insert(slist, i, "9");
	tb_slist_insert_head(slist, "4");
	tb_slist_insert_head(slist, "3");
	tb_slist_insert_head(slist, "2");
	tb_slist_insert_head(slist, "1");
	tb_slist_insert_head(slist, "0");
	tb_slist_insert_tail(slist, "5");
	tb_slist_insert_tail(slist, "6");
	tb_slist_insert_tail(slist, "7");
	tb_slist_insert_tail(slist, "8");
	tb_slist_insert_tail(slist, "9");
	tb_slist_char_dump(slist);

	tb_trace("=============================================================");
	tb_trace("remove:");
	tb_slist_nremove_head(slist, 5);
	tb_slist_nremove_last(slist, 5);
	j = tb_slist_remove(slist, j);
	j = tb_slist_remove(slist, j);
	j = tb_slist_remove(slist, j);
	j = tb_slist_remove(slist, j);
	j = tb_slist_remove(slist, j);
	j = tb_slist_remove(slist, j);
	j = tb_slist_remove(slist, j);
	j = tb_slist_remove(slist, j);
	j = tb_slist_remove(slist, j);
	j = tb_slist_remove(slist, j);	
	tb_slist_char_dump(slist);

	tb_trace("=============================================================");
	tb_trace("replace:");
	tb_slist_nreplace_head(slist, "T", 10);
	tb_slist_nreplace_last(slist, "H", 10);
	tb_slist_replace_head(slist, "O");
	tb_slist_replace_last(slist, "O");
	tb_slist_char_dump(slist);

	tb_trace("=============================================================");
	tb_trace("clear:");
	tb_slist_clear(slist);
	tb_slist_char_dump(slist);

	tb_size_t score = 0;
	tb_trace("=============================================================");
	tb_trace("insert performance:");
	score += tb_slist_insert_test();
	score += tb_slist_insert_head_test();
	score += tb_slist_insert_tail_test();
	score += tb_slist_ninsert_test();
	score += tb_slist_ninsert_head_test();
	score += tb_slist_ninsert_tail_test();

	tb_trace("=============================================================");
	tb_trace("remove performance:");
	score += tb_slist_remove_test();
	score += tb_slist_remove_head_test();
	score += tb_slist_remove_last_test();
	score += tb_slist_nremove_test();
	score += tb_slist_nremove_head_test();
	score += tb_slist_nremove_last_test();

	tb_trace("=============================================================");
	tb_trace("replace performance:");
	score += tb_slist_replace_test();
	score += tb_slist_replace_head_test();
	score += tb_slist_replace_last_test();
	score += tb_slist_nreplace_test();
	score += tb_slist_nreplace_head_test();
	score += tb_slist_nreplace_last_test();

	tb_trace("=============================================================");
	tb_trace("iterator performance:");
	score += tb_slist_iterator_next_test();
	score += tb_slist_iterator_prev_test();

	tb_trace("=============================================================");
	tb_trace("score: %d", score / 100);

	getchar();
end:

	// exit slist
	tb_slist_exit(slist);

	return 0;
}
