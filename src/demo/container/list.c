/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

#define TB_LIST_GROW_SIZE 			(256)

/* //////////////////////////////////////////////////////////////////////////////////////
 * details
 */
static tb_size_t tb_list_insert_test()
{
	// init
	tb_list_t* list = tb_list_init(TB_LIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(list, 0);

	// insert one first
	tb_size_t itor = tb_list_insert_head(list, (tb_pointer_t)0xd);

	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 10000;
	tb_hong_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_list_insert_prev(list, itor, (tb_pointer_t)0xd);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_list_insert_prev(%d): %lld ms, size: %d, maxn: %d", n, t, tb_list_size(list), tb_list_maxn(list));

	// check
	tb_assert(tb_list_size(list) == n + 1);
	tb_assert(tb_list_head(list) == (tb_pointer_t)0xd);
	tb_assert(tb_list_last(list) == (tb_pointer_t)0xd);

	// clear it
	tb_list_clear(list);
	tb_assert(!tb_list_size(list));

	// exit
	tb_list_exit(list);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_list_insert_head_test()
{
	// init
	tb_list_t* list = tb_list_init(TB_LIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(list, 0);

	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 1000000;
	tb_hong_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_list_insert_head(list, (tb_pointer_t)0xd);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_list_insert_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_list_size(list), tb_list_maxn(list));

	// check
	tb_assert(tb_list_size(list) == n);
	tb_assert(tb_list_head(list) == (tb_pointer_t)0xd);
	tb_assert(tb_list_last(list) == (tb_pointer_t)0xd);

	// clear it
	tb_list_clear(list);
	tb_assert(!tb_list_size(list));

	// exit
	tb_list_exit(list);
	
	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_list_insert_tail_test()
{
	// init
	tb_list_t* list = tb_list_init(TB_LIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(list, 0);

	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 1000000;
	tb_hong_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_list_insert_tail(list, (tb_pointer_t)0xd);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_list_insert_tail(%d): %lld ms, size: %d, maxn: %d", n, t, tb_list_size(list), tb_list_maxn(list));

	// check
	tb_assert(tb_list_size(list) == n);
	tb_assert(tb_list_head(list) == (tb_pointer_t)0xd);
	tb_assert(tb_list_last(list) == (tb_pointer_t)0xd);

	// clear it
	tb_list_clear(list);
	tb_assert(!tb_list_size(list));

	// exit
	tb_list_exit(list);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_list_ninsert_test()
{
	// init
	tb_list_t* list = tb_list_init(TB_LIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(list, 0);

	// insert one first
	tb_size_t itor = tb_list_insert_head(list, (tb_pointer_t)0xd);

	tb_size_t n = 1000000;
	tb_hong_t t = tb_mclock();
	tb_list_ninsert_prev(list, itor, (tb_pointer_t)0xd, n);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_list_ninsert_prev(%d): %lld ms, size: %d, maxn: %d", n, t, tb_list_size(list), tb_list_maxn(list));

	// check
	tb_assert(tb_list_size(list) == n + 1);
	tb_assert(tb_list_head(list) == (tb_pointer_t)0xd);
	tb_assert(tb_list_last(list) == (tb_pointer_t)0xd);

	// clear it
	tb_list_clear(list);
	tb_assert(!tb_list_size(list));

	// exit
	tb_list_exit(list);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_list_ninsert_head_test()
{
	// init
	tb_list_t* list = tb_list_init(TB_LIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(list, 0);

	tb_size_t n = 1000000;
	tb_hong_t t = tb_mclock();
	tb_list_ninsert_head(list, (tb_pointer_t)0xd, n);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_list_ninsert_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_list_size(list), tb_list_maxn(list));

	// check
	tb_assert(tb_list_size(list) == n);
	tb_assert(tb_list_head(list) == (tb_pointer_t)0xd);
	tb_assert(tb_list_last(list) == (tb_pointer_t)0xd);

	// clear it
	tb_list_clear(list);
	tb_assert(!tb_list_size(list));

	// exit
	tb_list_exit(list);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_list_ninsert_tail_test()
{
	// init
	tb_list_t* list = tb_list_init(TB_LIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(list, 0);

	tb_size_t n = 1000000;
	tb_hong_t t = tb_mclock();
	tb_list_ninsert_tail(list, (tb_pointer_t)0xd, n);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_list_ninsert_tail(%d): %lld ms, size: %d, maxn: %d", n, t, tb_list_size(list), tb_list_maxn(list));

	// check
	tb_assert(tb_list_size(list) == n);
	tb_assert(tb_list_head(list) == (tb_pointer_t)0xd);
	tb_assert(tb_list_last(list) == (tb_pointer_t)0xd);

	// clear it
	tb_list_clear(list);
	tb_assert(!tb_list_size(list));

	// exit
	tb_list_exit(list);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_list_remove_test()
{
	// init
	tb_list_t* list = tb_list_init(TB_LIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(list, 0);

	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 1000000;
	tb_size_t itor = tb_list_ninsert_head(list, (tb_pointer_t)0xd, n);
	tb_hong_t t = tb_mclock();
	for (i = 0; i < n; i++) itor = tb_list_remove(list, itor);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_list_remove(%d): %lld ms, size: %d, maxn: %d", n, t, tb_list_size(list), tb_list_maxn(list));

	// check
	tb_assert(!tb_list_size(list));

	// exit
	tb_list_exit(list);

	// ok?
	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_list_remove_head_test()
{
	// init
	tb_list_t* list = tb_list_init(TB_LIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(list, 0);

	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 1000000;
	tb_list_ninsert_head(list, (tb_pointer_t)0xd, n);
	tb_hong_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_list_remove_head(list);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_list_remove_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_list_size(list), tb_list_maxn(list));

	// check
	tb_assert(!tb_list_size(list));

	// exit
	tb_list_exit(list);

	// ok?
	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_list_remove_last_test()
{
	// init
	tb_list_t* list = tb_list_init(TB_LIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(list, 0);

	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 10000;
	tb_list_ninsert_head(list, (tb_pointer_t)0xd, n);
	tb_hong_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_list_remove_last(list);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_list_remove_last(%d): %lld ms, size: %d, maxn: %d", n, t, tb_list_size(list), tb_list_maxn(list));

	// check
	tb_assert(!tb_list_size(list));

	// exit
	tb_list_exit(list);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_list_nremove_test()
{
	// init
	tb_list_t* list = tb_list_init(TB_LIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(list, 0);

	tb_size_t n = 1000000;
	tb_list_ninsert_head(list, (tb_pointer_t)0xd, n);
	tb_hong_t t = tb_mclock();
	tb_list_nremove(list, tb_iterator_head(list), n);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_list_nremove(%d): %lld ms, size: %d, maxn: %d", n, t, tb_list_size(list), tb_list_maxn(list));

	// check
	tb_assert(!tb_list_size(list));

	// exit
	tb_list_exit(list);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_list_nremove_head_test()
{
	// init
	tb_list_t* list = tb_list_init(TB_LIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(list, 0);

	tb_size_t n = 1000000;
	tb_list_ninsert_head(list, (tb_pointer_t)0xd, n);
	tb_hong_t t = tb_mclock();
	tb_list_nremove_head(list, n);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_list_nremove_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_list_size(list), tb_list_maxn(list));

	// check
	tb_assert(!tb_list_size(list));

	// exit
	tb_list_exit(list);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_list_nremove_last_test()
{
	// init
	tb_list_t* list = tb_list_init(TB_LIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(list, 0);

	tb_size_t n = 1000000;
	tb_list_ninsert_head(list, (tb_pointer_t)0xd, n);
	tb_hong_t t = tb_mclock();
	tb_list_nremove_last(list, n);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_list_nremove_last(%d): %lld ms, size: %d, maxn: %d", n, t, tb_list_size(list), tb_list_maxn(list));

	// check
	tb_assert(!tb_list_size(list));

	// exit
	tb_list_exit(list);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_list_replace_test()
{
	// init
	tb_list_t* list = tb_list_init(TB_LIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(list, 0);

	tb_size_t n = 1000000;
	tb_list_ninsert_head(list, (tb_pointer_t)0xd, n);
	tb_size_t itor = tb_iterator_head(list);
	tb_size_t tail = tb_iterator_tail(list);
	tb_hong_t t = tb_mclock();
	for (; itor != tail; itor = tb_iterator_next(list, itor)) tb_list_replace(list, itor, (tb_pointer_t)0xe);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_list_replace(%d): %lld ms, size: %d, maxn: %d", n, t, tb_list_size(list), tb_list_maxn(list));

	// check
	tb_assert(tb_list_size(list) == n);
	tb_assert(tb_list_head(list) == (tb_pointer_t)0xe);
	tb_assert(tb_list_last(list) == (tb_pointer_t)0xe);

	// exit
	tb_list_exit(list);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_list_replace_head_test()
{
	// init
	tb_list_t* list = tb_list_init(TB_LIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(list, 0);

	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 1000000;
	tb_list_ninsert_head(list, (tb_pointer_t)0xd, n);
	tb_hong_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_list_replace_head(list, (tb_pointer_t)0xe);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_list_replace_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_list_size(list), tb_list_maxn(list));

	// check
	tb_assert(tb_list_size(list) == n);
	tb_assert(tb_list_head(list) == (tb_pointer_t)0xe);

	// exit
	tb_list_exit(list);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_list_replace_last_test()
{
	// init
	tb_list_t* list = tb_list_init(TB_LIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(list, 0);

	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 1000000;
	tb_list_ninsert_head(list, (tb_pointer_t)0xd, n);
	tb_hong_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_list_replace_last(list, (tb_pointer_t)0xe);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_list_replace_last(%d): %lld ms, size: %d, maxn: %d", n, t, tb_list_size(list), tb_list_maxn(list));

	// check
	tb_assert(tb_list_size(list) == n);
	tb_assert(tb_list_last(list) == (tb_pointer_t)0xe);


	// exit
	tb_list_exit(list);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_list_nreplace_test()
{
	// init
	tb_list_t* list = tb_list_init(TB_LIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(list, 0);

	tb_size_t n = 1000000;
	tb_list_ninsert_head(list, (tb_pointer_t)0xd, n);
	tb_hong_t t = tb_mclock();
	tb_list_nreplace(list, tb_iterator_head(list), (tb_pointer_t)0xe, n);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_list_nreplace(%d): %lld ms, size: %d, maxn: %d", n, t, tb_list_size(list), tb_list_maxn(list));

	// check
	tb_assert(tb_list_size(list) == n);
	tb_assert(tb_list_head(list) == (tb_pointer_t)0xe);
	tb_assert(tb_list_last(list) == (tb_pointer_t)0xe);

	// exit
	tb_list_exit(list);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_list_nreplace_head_test()
{
	// init
	tb_list_t* list = tb_list_init(TB_LIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(list, 0);

	tb_size_t n = 1000000;
	tb_list_ninsert_head(list, (tb_pointer_t)0xd, n);
	tb_hong_t t = tb_mclock();
	tb_list_nreplace_head(list, (tb_pointer_t)0xe, n);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_list_nreplace_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_list_size(list), tb_list_maxn(list));

	// check
	tb_assert(tb_list_size(list) == n);
	tb_assert(tb_list_head(list) == (tb_pointer_t)0xe);
	tb_assert(tb_list_last(list) == (tb_pointer_t)0xe);

	// exit
	tb_list_exit(list);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_list_nreplace_last_test()
{
	// init
	tb_list_t* list = tb_list_init(TB_LIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(list, 0);

	tb_size_t n = 1000000;
	tb_list_ninsert_head(list, (tb_pointer_t)0xd, n);
	tb_hong_t t = tb_mclock();
	tb_list_nreplace_last(list, (tb_pointer_t)0xe, n);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_list_nreplace_last(%d): %lld ms, size: %d, maxn: %d", n, t, tb_list_size(list), tb_list_maxn(list));

	// check
	tb_assert(tb_list_size(list) == n);
	tb_assert(tb_list_head(list) == (tb_pointer_t)0xe);
	tb_assert(tb_list_last(list) == (tb_pointer_t)0xe);


	// exit
	tb_list_exit(list);

	return n / ((tb_uint32_t)(t) + 1);
}

static tb_size_t tb_list_iterator_next_test()
{
	// init
	tb_list_t* list = tb_list_init(TB_LIST_GROW_SIZE, tb_item_func_size());
	tb_assert_and_check_return_val(list, 0);

	tb_size_t n = 1000000;
	tb_list_ninsert_head(list, (tb_pointer_t)0xd, n);
	tb_hong_t t = tb_mclock();
	tb_for_all(tb_size_t, item, list) tb_used(item);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_list_iterator_next(%d): %lld ms, size: %d, maxn: %d", n, t, tb_list_size(list), tb_list_maxn(list));

	// exit
	tb_list_exit(list);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_list_iterator_prev_test()
{
	// init
	tb_list_t* list = tb_list_init(TB_LIST_GROW_SIZE, tb_item_func_size());
	tb_assert_and_check_return_val(list, 0);

	tb_size_t n = 10000;
	tb_list_ninsert_head(list, (tb_pointer_t)0xd, n);
	tb_hong_t t = tb_mclock();
	tb_rfor_all(tb_size_t, item, list) tb_used(item);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_list_iterator_prev(%d): %lld ms, size: %d, maxn: %d", n, t, tb_list_size(list), tb_list_maxn(list));

	// exit
	tb_list_exit(list);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_void_t tb_list_int_dump(tb_list_t const* list)
{
	tb_trace_i("tb_int_t size: %d, maxn: %d", tb_list_size(list), tb_list_maxn(list));
	tb_for_all(tb_char_t*, item, list)
	{
		tb_trace_i("tb_int_t at[%lx]: %x", item_itor, item);
	}
}
static tb_void_t tb_list_int_test()
{
	tb_list_t* list = tb_list_init(TB_LIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return(list);

	tb_size_t 			i = 0;
	tb_size_t 			j = 0;

	tb_trace_i("=============================================================");
	tb_trace_i("insert:");
	tb_list_ninsert_head(list, (tb_pointer_t)0xa, 10);
	i = tb_list_ninsert_tail(list, (tb_pointer_t)0xf, 10);
	j = tb_list_insert_prev(list, i, (tb_pointer_t)0);
	tb_list_insert_prev(list, i, (tb_pointer_t)1);
	tb_list_insert_prev(list, i, (tb_pointer_t)2);
	tb_list_insert_prev(list, i, (tb_pointer_t)3);
	tb_list_insert_prev(list, i, (tb_pointer_t)4);
	tb_list_insert_prev(list, i, (tb_pointer_t)5);
	tb_list_insert_prev(list, i, (tb_pointer_t)6);
	tb_list_insert_prev(list, i, (tb_pointer_t)7);
	tb_list_insert_prev(list, i, (tb_pointer_t)8);
	tb_list_insert_prev(list, i, (tb_pointer_t)9);
	tb_list_insert_head(list, (tb_pointer_t)4);
	tb_list_insert_head(list, (tb_pointer_t)3);
	tb_list_insert_head(list, (tb_pointer_t)2);
	tb_list_insert_head(list, (tb_pointer_t)1);
	tb_list_insert_head(list, (tb_pointer_t)0);
	tb_list_insert_tail(list, (tb_pointer_t)5);
	tb_list_insert_tail(list, (tb_pointer_t)6);
	tb_list_insert_tail(list, (tb_pointer_t)7);
	tb_list_insert_tail(list, (tb_pointer_t)8);
	tb_list_insert_tail(list, (tb_pointer_t)9);
	tb_list_int_dump(list);

	tb_trace_i("=============================================================");
	tb_trace_i("remove:");
	tb_list_nremove_head(list, 5);
	tb_list_nremove_last(list, 5);
	j = tb_list_remove(list, j);
	j = tb_list_remove(list, j);
	j = tb_list_remove(list, j);
	j = tb_list_remove(list, j);
	j = tb_list_remove(list, j);
	j = tb_list_remove(list, j);
	j = tb_list_remove(list, j);
	j = tb_list_remove(list, j);
	j = tb_list_remove(list, j);
	j = tb_list_remove(list, j);	
	tb_list_int_dump(list);

	tb_trace_i("=============================================================");
	tb_trace_i("replace:");
	tb_list_nreplace_head(list, (tb_pointer_t)0xf, 10);
	tb_list_nreplace_last(list, (tb_pointer_t)0xa, 10);
	tb_list_replace_head(list, (tb_pointer_t)0);
	tb_list_replace_last(list, (tb_pointer_t)1);
	tb_list_int_dump(list);

	tb_trace_i("=============================================================");
	tb_trace_i("moveto:");
	tb_list_moveto_head(list, tb_iterator_last(list));
	tb_list_moveto_tail(list, tb_iterator_next(list, tb_iterator_head(list)));
	tb_list_int_dump(list);

	tb_trace_i("=============================================================");
	tb_trace_i("clear:");
	tb_list_clear(list);
	tb_list_int_dump(list);

	tb_list_exit(list);
}
static tb_void_t tb_list_str_dump(tb_list_t const* list)
{
	tb_trace_i("str size: %d, maxn: %d", tb_list_size(list), tb_list_maxn(list));
	tb_for_all (tb_char_t*, item, list)
	{
		tb_trace_i("str at[%lx]: %s", item_itor, item);
	}
}
static tb_void_t tb_list_str_test()
{
	tb_handle_t bpool = tb_block_pool_init(TB_BLOCK_POOL_GROW_SMALL, 0);
	tb_list_t* list = tb_list_init(TB_LIST_GROW_SIZE, tb_item_func_str(tb_true, bpool));
	tb_assert_and_check_return(list);

	tb_size_t 			i = 0;
	tb_size_t 			j = 0;

	tb_trace_i("=============================================================");
	tb_trace_i("insert:");
	tb_list_ninsert_head(list, "HHHHHHHHHH", 10);
	i = tb_list_ninsert_tail(list, "TTTTTTTTTT", 10);
	j = tb_list_insert_prev(list, i, "0000000000");
	tb_list_insert_prev(list, i, "1111111111");
	tb_list_insert_prev(list, i, "2222222222");
	tb_list_insert_prev(list, i, "3333333333");
	tb_list_insert_prev(list, i, "4444444444");
	tb_list_insert_prev(list, i, "5555555555");
	tb_list_insert_prev(list, i, "6666666666");
	tb_list_insert_prev(list, i, "7777777777");
	tb_list_insert_prev(list, i, "8888888888");
	tb_list_insert_prev(list, i, "9999999999");
	tb_list_insert_head(list, "4444444444");
	tb_list_insert_head(list, "3333333333");
	tb_list_insert_head(list, "2222222222");
	tb_list_insert_head(list, "1111111111");
	tb_list_insert_head(list, "0000000000");
	tb_list_insert_tail(list, "5555555555");
	tb_list_insert_tail(list, "6666666666");
	tb_list_insert_tail(list, "7777777777");
	tb_list_insert_tail(list, "8888888888");
	tb_list_insert_tail(list, "9999999999");
	tb_list_str_dump(list);

	tb_trace_i("=============================================================");
	tb_trace_i("remove:");
	tb_list_nremove_head(list, 5);
	tb_list_nremove_last(list, 5);
	j = tb_list_remove(list, j);
	j = tb_list_remove(list, j);
	j = tb_list_remove(list, j);
	j = tb_list_remove(list, j);
	j = tb_list_remove(list, j);
	j = tb_list_remove(list, j);
	j = tb_list_remove(list, j);
	j = tb_list_remove(list, j);
	j = tb_list_remove(list, j);
	j = tb_list_remove(list, j);	
	tb_list_str_dump(list);

	tb_trace_i("=============================================================");
	tb_trace_i("replace:");
	tb_list_nreplace_head(list, "TTTTTTTTTT", 10);
	tb_list_nreplace_last(list, "HHHHHHHHHH", 10);
	tb_list_replace_head(list, "OOOOOOOOOO");
	tb_list_replace_last(list, "IIIIIIIIII");
	tb_list_str_dump(list);

	tb_trace_i("=============================================================");
	tb_trace_i("moveto:");
	tb_list_moveto_head(list, tb_iterator_last(list));
	tb_list_moveto_tail(list, tb_iterator_next(list, tb_iterator_head(list)));
	tb_list_str_dump(list);

	tb_trace_i("=============================================================");
	tb_trace_i("clear:");
	tb_list_clear(list);
	tb_list_str_dump(list);

	tb_list_exit(list);
	tb_block_pool_exit(bpool);
}
static tb_void_t tb_list_mem_free(tb_item_func_t* func, tb_pointer_t item)
{
	tb_trace_i("ifm free: %s, priv: %s", item, func->priv);
}
static tb_void_t tb_list_mem_dump(tb_list_t const* list)
{
	tb_trace_i("ifm size: %d, maxn: %d", tb_list_size(list), tb_list_maxn(list));
	tb_for_all (tb_char_t*, item, list)
	{
		tb_trace_i("ifm at[%lx]: %s", item_itor, item);
	}
}
static tb_void_t tb_list_mem_test()
{
	tb_list_t* list = tb_list_init(TB_LIST_GROW_SIZE, tb_item_func_mem(11, tb_list_mem_free, "ifm"));
	tb_assert_and_check_return(list);

	tb_size_t 			i = 0;
	tb_size_t 			j = 0;

	tb_trace_i("=============================================================");
	tb_trace_i("insert:");
	tb_list_ninsert_head(list, "HHHHHHHHHH", 10);
	i = tb_list_ninsert_tail(list, "TTTTTTTTTT", 10);
	j = tb_list_insert_prev(list, i, "0000000000");
	tb_list_insert_prev(list, i, "1111111111");
	tb_list_insert_prev(list, i, "2222222222");
	tb_list_insert_prev(list, i, "3333333333");
	tb_list_insert_prev(list, i, "4444444444");
	tb_list_insert_prev(list, i, "5555555555");
	tb_list_insert_prev(list, i, "6666666666");
	tb_list_insert_prev(list, i, "7777777777");
	tb_list_insert_prev(list, i, "8888888888");
	tb_list_insert_prev(list, i, "9999999999");
	tb_list_insert_head(list, "4444444444");
	tb_list_insert_head(list, "3333333333");
	tb_list_insert_head(list, "2222222222");
	tb_list_insert_head(list, "1111111111");
	tb_list_insert_head(list, "0000000000");
	tb_list_insert_tail(list, "5555555555");
	tb_list_insert_tail(list, "6666666666");
	tb_list_insert_tail(list, "7777777777");
	tb_list_insert_tail(list, "8888888888");
	tb_list_insert_tail(list, "9999999999");
	tb_list_mem_dump(list);

	tb_trace_i("=============================================================");
	tb_trace_i("remove:");
	tb_list_nremove_head(list, 5);
	tb_list_nremove_last(list, 5);
	j = tb_list_remove(list, j);
	j = tb_list_remove(list, j);
	j = tb_list_remove(list, j);
	j = tb_list_remove(list, j);
	j = tb_list_remove(list, j);
	j = tb_list_remove(list, j);
	j = tb_list_remove(list, j);
	j = tb_list_remove(list, j);
	j = tb_list_remove(list, j);
	j = tb_list_remove(list, j);	
	tb_list_mem_dump(list);

	tb_trace_i("=============================================================");
	tb_trace_i("replace:");
	tb_list_nreplace_head(list, "TTTTTTTTTT", 10);
	tb_list_nreplace_last(list, "HHHHHHHHHH", 10);
	tb_list_replace_head(list, "OOOOOOOOOO");
	tb_list_replace_last(list, "IIIIIIIIII");
	tb_list_mem_dump(list);

	tb_trace_i("=============================================================");
	tb_trace_i("moveto:");
	tb_list_moveto_head(list, tb_iterator_last(list));
	tb_list_moveto_tail(list, tb_iterator_next(list, tb_iterator_head(list)));
	tb_list_mem_dump(list);

	tb_trace_i("=============================================================");
	tb_trace_i("clear:");
	tb_list_clear(list);
	tb_list_mem_dump(list);

	tb_list_exit(list);
}
static tb_void_t tb_list_perf_test()
{
	tb_size_t score = 0;
	tb_trace_i("=============================================================");
	tb_trace_i("insert performance:");
	score += tb_list_insert_test();
	score += tb_list_insert_head_test();
	score += tb_list_insert_tail_test();
	score += tb_list_ninsert_test();
	score += tb_list_ninsert_head_test();
	score += tb_list_ninsert_tail_test();

	tb_trace_i("=============================================================");
	tb_trace_i("remove performance:");
	score += tb_list_remove_test();
	score += tb_list_remove_head_test();
	score += tb_list_remove_last_test();
	score += tb_list_nremove_test();
	score += tb_list_nremove_head_test();
	score += tb_list_nremove_last_test();

	tb_trace_i("=============================================================");
	tb_trace_i("replace performance:");
	score += tb_list_replace_test();
	score += tb_list_replace_head_test();
	score += tb_list_replace_last_test();
	score += tb_list_nreplace_test();
	score += tb_list_nreplace_head_test();
	score += tb_list_nreplace_last_test();

	tb_trace_i("=============================================================");
	tb_trace_i("iterator performance:");
	score += tb_list_iterator_next_test();
	score += tb_list_iterator_prev_test();

	tb_trace_i("=============================================================");
	tb_trace_i("score: %d", score / 100);

}
static tb_void_t tb_list_test_itor_perf()
{
	// init list
	tb_list_t* 	list = tb_list_init(TB_LIST_GROW_SIZE, tb_item_func_size());
	tb_assert_and_check_return(list);

	// clear rand
	tb_random_clear(tb_random_generator());

	// add items
	__tb_volatile__ tb_size_t n = 100000;
	while (n--) tb_list_insert_tail(list, (tb_pointer_t)(tb_size_t)tb_random_range(tb_random_generator(), 0, TB_MAXU32)); 

	// performance
	tb_hong_t t = tb_mclock();
	__tb_volatile__ tb_hize_t test[2] = {0};
	__tb_volatile__ tb_size_t 	prev = 0;
	__tb_volatile__ tb_size_t 	itor = tb_iterator_head(list);
	for (; itor != tb_iterator_tail(list); )
	{
		__tb_volatile__ tb_size_t item = (tb_size_t)tb_iterator_item(list, itor);
#if 1
		if (!(((tb_size_t)item >> 25) & 0x1))
		{
			// save 
			tb_size_t next = tb_iterator_next(list, itor);

			// remove
			tb_list_remove_next(list, prev);

			// next
			itor = next;

			// continue 
			continue ;
		}
		else
#endif
		{
			test[0] += (tb_size_t)item;
			test[1]++;
		}

		prev = itor;
		itor = tb_iterator_next(list, itor);
	}
	t = tb_mclock() - t;
	tb_trace_i("item: %llx, size: %llu ?= %u, time: %lld", test[0], test[1], tb_list_size(list), t);

	tb_list_exit(list);
}
static tb_bool_t tb_list_test_walk_item(tb_list_t* list, tb_pointer_t item, tb_bool_t* bdel, tb_pointer_t priv)
{
	// check
	tb_assert_and_check_return_val(list && bdel && priv, tb_false);

	// done
	tb_hize_t* test = priv;
	tb_size_t i = (tb_size_t)item;
	if (!((i >> 25) & 0x1))
//	if (!(i & 0x7))
//	if (1)
//	if (!(tb_random_range(tb_random_generator(), 0, TB_MAXU32) & 0x1))
		*bdel = tb_true;
	else
	{
		test[0] += i;
		test[1]++;
	}

	// ok
	return tb_true;
}
static tb_void_t tb_list_test_walk_perf()
{
	// init list
	tb_list_t* 	list = tb_list_init(TB_LIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return(list);

	// clear rand
	tb_random_clear(tb_random_generator());

	// add items
	__tb_volatile__ tb_size_t n = 100000;
	while (n--) tb_list_insert_tail(list, (tb_pointer_t)(tb_size_t)tb_random_range(tb_random_generator(), 0, TB_MAXU32)); 

	// performance
	tb_hong_t t = tb_mclock();
	__tb_volatile__ tb_hize_t test[2] = {0};
	tb_list_walk(list, tb_list_test_walk_item, (tb_pointer_t)test);
	t = tb_mclock() - t;
	tb_trace_i("item: %llx, size: %llu ?= %u, time: %lld", test[0], test[1], tb_list_size(list), t);

	tb_list_exit(list);
}
/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_container_list_main(tb_int_t argc, tb_char_t** argv)
{
#if 1
	tb_list_int_test();
	tb_list_str_test();
	tb_list_mem_test();
#endif

#if 1
	tb_list_perf_test();
#endif

#if 1
	tb_list_test_itor_perf();
	tb_list_test_walk_perf();
#endif

	return 0;
}
