/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

#define TB_SLIST_GROW_SIZE 			(256)

/* //////////////////////////////////////////////////////////////////////////////////////
 * details
 */
static tb_size_t tb_slist_insert_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(TB_SLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(slist, 0);

	// insert one first
	tb_size_t itor = tb_slist_insert_head(slist, (tb_pointer_t)0xd);

	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 10000;
	tb_hong_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_slist_insert_prev(slist, itor, (tb_pointer_t)0xd);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_slist_insert_prev(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	tb_assert(tb_slist_size(slist) == n + 1);
	tb_assert(tb_slist_head(slist) == (tb_pointer_t)0xd);
	tb_assert(tb_slist_last(slist) == (tb_pointer_t)0xd);

	// clear it
	tb_slist_clear(slist);
	tb_assert(!tb_slist_size(slist));

	// exit
	tb_slist_exit(slist);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_slist_insert_head_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(TB_SLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(slist, 0);

	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 1000000;
	tb_hong_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_slist_insert_head(slist, (tb_pointer_t)0xd);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_slist_insert_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	tb_assert(tb_slist_size(slist) == n);
	tb_assert(tb_slist_head(slist) == (tb_pointer_t)0xd);
	tb_assert(tb_slist_last(slist) == (tb_pointer_t)0xd);

	// clear it
	tb_slist_clear(slist);
	tb_assert(!tb_slist_size(slist));

	// exit
	tb_slist_exit(slist);
	
	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_slist_insert_tail_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(TB_SLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(slist, 0);

	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 1000000;
	tb_hong_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_slist_insert_tail(slist, (tb_pointer_t)0xd);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_slist_insert_tail(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	tb_assert(tb_slist_size(slist) == n);
	tb_assert(tb_slist_head(slist) == (tb_pointer_t)0xd);
	tb_assert(tb_slist_last(slist) == (tb_pointer_t)0xd);

	// clear it
	tb_slist_clear(slist);
	tb_assert(!tb_slist_size(slist));

	// exit
	tb_slist_exit(slist);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_slist_ninsert_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(TB_SLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(slist, 0);

	// insert one first
	tb_size_t itor = tb_slist_insert_head(slist, (tb_pointer_t)0xd);

	tb_size_t n = 1000000;
	tb_hong_t t = tb_mclock();
	tb_slist_ninsert_prev(slist, itor, (tb_pointer_t)0xd, n);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_slist_ninsert_prev(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	tb_assert(tb_slist_size(slist) == n + 1);
	tb_assert(tb_slist_head(slist) == (tb_pointer_t)0xd);
	tb_assert(tb_slist_last(slist) == (tb_pointer_t)0xd);

	// clear it
	tb_slist_clear(slist);
	tb_assert(!tb_slist_size(slist));

	// exit
	tb_slist_exit(slist);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_slist_ninsert_head_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(TB_SLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(slist, 0);

	tb_size_t n = 1000000;
	tb_hong_t t = tb_mclock();
	tb_slist_ninsert_head(slist, (tb_pointer_t)0xd, n);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_slist_ninsert_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	tb_assert(tb_slist_size(slist) == n);
	tb_assert(tb_slist_head(slist) == (tb_pointer_t)0xd);
	tb_assert(tb_slist_last(slist) == (tb_pointer_t)0xd);

	// clear it
	tb_slist_clear(slist);
	tb_assert(!tb_slist_size(slist));

	// exit
	tb_slist_exit(slist);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_slist_ninsert_tail_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(TB_SLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(slist, 0);

	tb_size_t n = 1000000;
	tb_hong_t t = tb_mclock();
	tb_slist_ninsert_tail(slist, (tb_pointer_t)0xd, n);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_slist_ninsert_tail(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	tb_assert(tb_slist_size(slist) == n);
	tb_assert(tb_slist_head(slist) == (tb_pointer_t)0xd);
	tb_assert(tb_slist_last(slist) == (tb_pointer_t)0xd);

	// clear it
	tb_slist_clear(slist);
	tb_assert(!tb_slist_size(slist));

	// exit
	tb_slist_exit(slist);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_slist_remove_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(TB_SLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(slist, 0);

	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 1000000;
	tb_size_t itor = tb_slist_ninsert_head(slist, (tb_pointer_t)0xd, n);
	tb_hong_t t = tb_mclock();
	for (i = 0; i < n; i++) itor = tb_slist_remove(slist, itor);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_slist_remove(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	tb_assert(!tb_slist_size(slist));

	// exit
	tb_slist_exit(slist);

	// ok?
	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_slist_remove_head_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(TB_SLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(slist, 0);

	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 1000000;
	tb_slist_ninsert_head(slist, (tb_pointer_t)0xd, n);
	tb_hong_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_slist_remove_head(slist);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_slist_remove_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	tb_assert(!tb_slist_size(slist));

	// exit
	tb_slist_exit(slist);

	// ok?
	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_slist_remove_last_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(TB_SLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(slist, 0);

	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 10000;
	tb_slist_ninsert_head(slist, (tb_pointer_t)0xd, n);
	tb_hong_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_slist_remove_last(slist);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_slist_remove_last(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	tb_assert(!tb_slist_size(slist));

	// exit
	tb_slist_exit(slist);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_slist_nremove_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(TB_SLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(slist, 0);

	tb_size_t n = 1000000;
	tb_slist_ninsert_head(slist, (tb_pointer_t)0xd, n);
	tb_hong_t t = tb_mclock();
	tb_slist_nremove(slist, tb_iterator_head(slist), n);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_slist_nremove(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	tb_assert(!tb_slist_size(slist));

	// exit
	tb_slist_exit(slist);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_slist_nremove_head_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(TB_SLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(slist, 0);

	tb_size_t n = 1000000;
	tb_slist_ninsert_head(slist, (tb_pointer_t)0xd, n);
	tb_hong_t t = tb_mclock();
	tb_slist_nremove_head(slist, n);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_slist_nremove_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	tb_assert(!tb_slist_size(slist));

	// exit
	tb_slist_exit(slist);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_slist_nremove_last_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(TB_SLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(slist, 0);

	tb_size_t n = 1000000;
	tb_slist_ninsert_head(slist, (tb_pointer_t)0xd, n);
	tb_hong_t t = tb_mclock();
	tb_slist_nremove_last(slist, n);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_slist_nremove_last(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	tb_assert(!tb_slist_size(slist));

	// exit
	tb_slist_exit(slist);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_slist_replace_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(TB_SLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(slist, 0);

	tb_size_t n = 1000000;
	tb_slist_ninsert_head(slist, (tb_pointer_t)0xd, n);
	tb_size_t itor = tb_iterator_head(slist);
	tb_size_t tail = tb_iterator_tail(slist);
	tb_hong_t t = tb_mclock();
	for (; itor != tail; itor = tb_iterator_next(slist, itor)) tb_slist_replace(slist, itor, (tb_pointer_t)0xe);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_slist_replace(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	tb_assert(tb_slist_size(slist) == n);
	tb_assert(tb_slist_head(slist) == (tb_pointer_t)0xe);
	tb_assert(tb_slist_last(slist) == (tb_pointer_t)0xe);

	// exit
	tb_slist_exit(slist);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_slist_replace_head_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(TB_SLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(slist, 0);

	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 1000000;
	tb_slist_ninsert_head(slist, (tb_pointer_t)0xd, n);
	tb_hong_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_slist_replace_head(slist, (tb_pointer_t)0xe);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_slist_replace_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	tb_assert(tb_slist_size(slist) == n);
	tb_assert(tb_slist_head(slist) == (tb_pointer_t)0xe);

	// exit
	tb_slist_exit(slist);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_slist_replace_last_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(TB_SLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(slist, 0);

	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 1000000;
	tb_slist_ninsert_head(slist, (tb_pointer_t)0xd, n);
	tb_hong_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_slist_replace_last(slist, (tb_pointer_t)0xe);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_slist_replace_last(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	tb_assert(tb_slist_size(slist) == n);
	tb_assert(tb_slist_last(slist) == (tb_pointer_t)0xe);


	// exit
	tb_slist_exit(slist);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_slist_nreplace_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(TB_SLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(slist, 0);

	tb_size_t n = 1000000;
	tb_slist_ninsert_head(slist, (tb_pointer_t)0xd, n);
	tb_hong_t t = tb_mclock();
	tb_slist_nreplace(slist, tb_iterator_head(slist), (tb_pointer_t)0xe, n);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_slist_nreplace(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	tb_assert(tb_slist_size(slist) == n);
	tb_assert(tb_slist_head(slist) == (tb_pointer_t)0xe);
	tb_assert(tb_slist_last(slist) == (tb_pointer_t)0xe);

	// exit
	tb_slist_exit(slist);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_slist_nreplace_head_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(TB_SLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(slist, 0);

	tb_size_t n = 1000000;
	tb_slist_ninsert_head(slist, (tb_pointer_t)0xd, n);
	tb_hong_t t = tb_mclock();
	tb_slist_nreplace_head(slist, (tb_pointer_t)0xe, n);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_slist_nreplace_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	tb_assert(tb_slist_size(slist) == n);
	tb_assert(tb_slist_head(slist) == (tb_pointer_t)0xe);
	tb_assert(tb_slist_last(slist) == (tb_pointer_t)0xe);

	// exit
	tb_slist_exit(slist);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_slist_nreplace_last_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(TB_SLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(slist, 0);

	tb_size_t n = 1000000;
	tb_slist_ninsert_head(slist, (tb_pointer_t)0xd, n);
	tb_hong_t t = tb_mclock();
	tb_slist_nreplace_last(slist, (tb_pointer_t)0xe, n);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_slist_nreplace_last(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	tb_assert(tb_slist_size(slist) == n);
	tb_assert(tb_slist_head(slist) == (tb_pointer_t)0xe);
	tb_assert(tb_slist_last(slist) == (tb_pointer_t)0xe);


	// exit
	tb_slist_exit(slist);

	return n / ((tb_uint32_t)(t) + 1);
}

static tb_size_t tb_slist_iterator_next_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(TB_SLIST_GROW_SIZE, tb_item_func_size());
	tb_assert_and_check_return_val(slist, 0);

	tb_size_t n = 1000000;
	tb_slist_ninsert_head(slist, (tb_pointer_t)0xd, n);
	tb_hong_t t = tb_mclock();
	tb_for_all(tb_size_t, item, slist) tb_used(item);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_slist_iterator_next(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// exit
	tb_slist_exit(slist);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_slist_iterator_prev_test()
{
	// init
	tb_slist_t* slist = tb_slist_init(TB_SLIST_GROW_SIZE, tb_item_func_size());
	tb_assert_and_check_return_val(slist, 0);

	tb_size_t n = 10000;
	tb_slist_ninsert_head(slist, (tb_pointer_t)0xd, n);
	tb_hong_t t = tb_mclock();
	tb_rfor_all(tb_size_t, item, slist) tb_used(item);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_slist_iterator_prev(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// exit
	tb_slist_exit(slist);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_void_t tb_slist_int_dump(tb_slist_t const* slist)
{
	tb_trace_i("tb_int_t size: %d, maxn: %d", tb_slist_size(slist), tb_slist_maxn(slist));
	tb_for_all(tb_char_t*, item, slist)
	{
		tb_trace_i("tb_int_t at[%lx]: %x", item_itor, item);
	}
}
static tb_void_t tb_slist_int_test()
{
	tb_slist_t* slist = tb_slist_init(TB_SLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return(slist);

	tb_size_t 			i = 0;
	tb_size_t 			j = 0;

	tb_trace_i("=============================================================");
	tb_trace_i("insert:");
	tb_slist_ninsert_head(slist, (tb_pointer_t)0xa, 10);
	i = tb_slist_ninsert_tail(slist, (tb_pointer_t)0xf, 10);
	j = tb_slist_insert_prev(slist, i, (tb_pointer_t)0);
	tb_slist_insert_prev(slist, i, (tb_pointer_t)1);
	tb_slist_insert_prev(slist, i, (tb_pointer_t)2);
	tb_slist_insert_prev(slist, i, (tb_pointer_t)3);
	tb_slist_insert_prev(slist, i, (tb_pointer_t)4);
	tb_slist_insert_prev(slist, i, (tb_pointer_t)5);
	tb_slist_insert_prev(slist, i, (tb_pointer_t)6);
	tb_slist_insert_prev(slist, i, (tb_pointer_t)7);
	tb_slist_insert_prev(slist, i, (tb_pointer_t)8);
	tb_slist_insert_prev(slist, i, (tb_pointer_t)9);
	tb_slist_insert_head(slist, (tb_pointer_t)4);
	tb_slist_insert_head(slist, (tb_pointer_t)3);
	tb_slist_insert_head(slist, (tb_pointer_t)2);
	tb_slist_insert_head(slist, (tb_pointer_t)1);
	tb_slist_insert_head(slist, (tb_pointer_t)0);
	tb_slist_insert_tail(slist, (tb_pointer_t)5);
	tb_slist_insert_tail(slist, (tb_pointer_t)6);
	tb_slist_insert_tail(slist, (tb_pointer_t)7);
	tb_slist_insert_tail(slist, (tb_pointer_t)8);
	tb_slist_insert_tail(slist, (tb_pointer_t)9);
	tb_slist_int_dump(slist);

	tb_trace_i("=============================================================");
	tb_trace_i("remove:");
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
	tb_slist_int_dump(slist);

	tb_trace_i("=============================================================");
	tb_trace_i("replace:");
	tb_slist_nreplace_head(slist, (tb_pointer_t)0xf, 10);
	tb_slist_nreplace_last(slist, (tb_pointer_t)0xa, 10);
	tb_slist_replace_head(slist, (tb_pointer_t)0);
	tb_slist_replace_last(slist, (tb_pointer_t)1);
	tb_slist_int_dump(slist);

	tb_trace_i("=============================================================");
	tb_trace_i("moveto:");
	tb_slist_moveto_head(slist, tb_iterator_last(slist));
	tb_slist_moveto_tail(slist, tb_iterator_next(slist, tb_iterator_head(slist)));
	tb_slist_int_dump(slist);

	tb_trace_i("=============================================================");
	tb_trace_i("clear:");
	tb_slist_clear(slist);
	tb_slist_int_dump(slist);

	tb_slist_exit(slist);
}
static tb_void_t tb_slist_str_dump(tb_slist_t const* slist)
{
	tb_trace_i("str size: %d, maxn: %d", tb_slist_size(slist), tb_slist_maxn(slist));
	tb_for_all (tb_char_t*, item, slist)
	{
		tb_trace_i("str at[%lx]: %s", item_itor, item);
	}
}
static tb_void_t tb_slist_str_test()
{
	tb_handle_t bpool = tb_block_pool_init(TB_BLOCK_POOL_GROW_SMALL, 0);
	tb_slist_t* slist = tb_slist_init(TB_SLIST_GROW_SIZE, tb_item_func_str(tb_true, bpool));
	tb_assert_and_check_return(slist);

	tb_size_t 			i = 0;
	tb_size_t 			j = 0;

	tb_trace_i("=============================================================");
	tb_trace_i("insert:");
	tb_slist_ninsert_head(slist, "HHHHHHHHHH", 10);
	i = tb_slist_ninsert_tail(slist, "TTTTTTTTTT", 10);
	j = tb_slist_insert_prev(slist, i, "0000000000");
	tb_slist_insert_prev(slist, i, "1111111111");
	tb_slist_insert_prev(slist, i, "2222222222");
	tb_slist_insert_prev(slist, i, "3333333333");
	tb_slist_insert_prev(slist, i, "4444444444");
	tb_slist_insert_prev(slist, i, "5555555555");
	tb_slist_insert_prev(slist, i, "6666666666");
	tb_slist_insert_prev(slist, i, "7777777777");
	tb_slist_insert_prev(slist, i, "8888888888");
	tb_slist_insert_prev(slist, i, "9999999999");
	tb_slist_insert_head(slist, "4444444444");
	tb_slist_insert_head(slist, "3333333333");
	tb_slist_insert_head(slist, "2222222222");
	tb_slist_insert_head(slist, "1111111111");
	tb_slist_insert_head(slist, "0000000000");
	tb_slist_insert_tail(slist, "5555555555");
	tb_slist_insert_tail(slist, "6666666666");
	tb_slist_insert_tail(slist, "7777777777");
	tb_slist_insert_tail(slist, "8888888888");
	tb_slist_insert_tail(slist, "9999999999");
	tb_slist_str_dump(slist);

	tb_trace_i("=============================================================");
	tb_trace_i("remove:");
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
	tb_slist_str_dump(slist);

	tb_trace_i("=============================================================");
	tb_trace_i("replace:");
	tb_slist_nreplace_head(slist, "TTTTTTTTTT", 10);
	tb_slist_nreplace_last(slist, "HHHHHHHHHH", 10);
	tb_slist_replace_head(slist, "OOOOOOOOOO");
	tb_slist_replace_last(slist, "IIIIIIIIII");
	tb_slist_str_dump(slist);

	tb_trace_i("=============================================================");
	tb_trace_i("moveto:");
	tb_slist_moveto_head(slist, tb_iterator_last(slist));
	tb_slist_moveto_tail(slist, tb_iterator_next(slist, tb_iterator_head(slist)));
	tb_slist_str_dump(slist);

	tb_trace_i("=============================================================");
	tb_trace_i("clear:");
	tb_slist_clear(slist);
	tb_slist_str_dump(slist);

	tb_slist_exit(slist);
	tb_block_pool_exit(bpool);
}
static tb_void_t tb_slist_mem_free(tb_item_func_t* func, tb_pointer_t item)
{
	tb_trace_i("ifm free: %s, priv: %s", item, func->priv);
}
static tb_void_t tb_slist_mem_dump(tb_slist_t const* slist)
{
	tb_trace_i("ifm size: %d, maxn: %d", tb_slist_size(slist), tb_slist_maxn(slist));
	tb_for_all (tb_char_t*, item, slist)
	{
		tb_trace_i("ifm at[%lx]: %s", item_itor, item);
	}
}
static tb_void_t tb_slist_mem_test()
{
	tb_slist_t* slist = tb_slist_init(TB_SLIST_GROW_SIZE, tb_item_func_mem(11, tb_slist_mem_free, "ifm"));
	tb_assert_and_check_return(slist);

	tb_size_t 			i = 0;
	tb_size_t 			j = 0;

	tb_trace_i("=============================================================");
	tb_trace_i("insert:");
	tb_slist_ninsert_head(slist, "HHHHHHHHHH", 10);
	i = tb_slist_ninsert_tail(slist, "TTTTTTTTTT", 10);
	j = tb_slist_insert_prev(slist, i, "0000000000");
	tb_slist_insert_prev(slist, i, "1111111111");
	tb_slist_insert_prev(slist, i, "2222222222");
	tb_slist_insert_prev(slist, i, "3333333333");
	tb_slist_insert_prev(slist, i, "4444444444");
	tb_slist_insert_prev(slist, i, "5555555555");
	tb_slist_insert_prev(slist, i, "6666666666");
	tb_slist_insert_prev(slist, i, "7777777777");
	tb_slist_insert_prev(slist, i, "8888888888");
	tb_slist_insert_prev(slist, i, "9999999999");
	tb_slist_insert_head(slist, "4444444444");
	tb_slist_insert_head(slist, "3333333333");
	tb_slist_insert_head(slist, "2222222222");
	tb_slist_insert_head(slist, "1111111111");
	tb_slist_insert_head(slist, "0000000000");
	tb_slist_insert_tail(slist, "5555555555");
	tb_slist_insert_tail(slist, "6666666666");
	tb_slist_insert_tail(slist, "7777777777");
	tb_slist_insert_tail(slist, "8888888888");
	tb_slist_insert_tail(slist, "9999999999");
	tb_slist_mem_dump(slist);

	tb_trace_i("=============================================================");
	tb_trace_i("remove:");
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
	tb_slist_mem_dump(slist);

	tb_trace_i("=============================================================");
	tb_trace_i("replace:");
	tb_slist_nreplace_head(slist, "TTTTTTTTTT", 10);
	tb_slist_nreplace_last(slist, "HHHHHHHHHH", 10);
	tb_slist_replace_head(slist, "OOOOOOOOOO");
	tb_slist_replace_last(slist, "IIIIIIIIII");
	tb_slist_mem_dump(slist);

	tb_trace_i("=============================================================");
	tb_trace_i("moveto:");
	tb_slist_moveto_head(slist, tb_iterator_last(slist));
	tb_slist_moveto_tail(slist, tb_iterator_next(slist, tb_iterator_head(slist)));
	tb_slist_mem_dump(slist);

	tb_trace_i("=============================================================");
	tb_trace_i("clear:");
	tb_slist_clear(slist);
	tb_slist_mem_dump(slist);

	tb_slist_exit(slist);
}
static tb_void_t tb_slist_perf_test()
{
	tb_size_t score = 0;
	tb_trace_i("=============================================================");
	tb_trace_i("insert performance:");
	score += tb_slist_insert_test();
	score += tb_slist_insert_head_test();
	score += tb_slist_insert_tail_test();
	score += tb_slist_ninsert_test();
	score += tb_slist_ninsert_head_test();
	score += tb_slist_ninsert_tail_test();

	tb_trace_i("=============================================================");
	tb_trace_i("remove performance:");
	score += tb_slist_remove_test();
	score += tb_slist_remove_head_test();
	score += tb_slist_remove_last_test();
	score += tb_slist_nremove_test();
	score += tb_slist_nremove_head_test();
	score += tb_slist_nremove_last_test();

	tb_trace_i("=============================================================");
	tb_trace_i("replace performance:");
	score += tb_slist_replace_test();
	score += tb_slist_replace_head_test();
	score += tb_slist_replace_last_test();
	score += tb_slist_nreplace_test();
	score += tb_slist_nreplace_head_test();
	score += tb_slist_nreplace_last_test();

	tb_trace_i("=============================================================");
	tb_trace_i("iterator performance:");
	score += tb_slist_iterator_next_test();
	score += tb_slist_iterator_prev_test();

	tb_trace_i("=============================================================");
	tb_trace_i("score: %d", score / 100);

}
static tb_void_t tb_slist_test_itor_perf()
{
	// init slist
	tb_slist_t* 	slist = tb_slist_init(TB_SLIST_GROW_SIZE, tb_item_func_size());
	tb_assert_and_check_return(slist);

	// clear rand
	tb_rand_clear();

	// add items
	__tb_volatile__ tb_size_t n = 100000;
	while (n--) tb_slist_insert_tail(slist, (tb_pointer_t)(tb_size_t)tb_rand_uint32(0, TB_MAXU32)); 

	// performance
	tb_hong_t t = tb_mclock();
	__tb_volatile__ tb_hize_t test[2] = {0};
	__tb_volatile__ tb_size_t 	prev = 0;
	__tb_volatile__ tb_size_t 	itor = tb_iterator_head(slist);
	for (; itor != tb_iterator_tail(slist); )
	{
		__tb_volatile__ tb_size_t item = (tb_size_t)tb_iterator_item(slist, itor);
#if 1
		if (!(((tb_size_t)item >> 25) & 0x1))
		{
			// save 
			tb_size_t next = tb_iterator_next(slist, itor);

			// remove
			tb_slist_remove_next(slist, prev);

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
		itor = tb_iterator_next(slist, itor);
	}
	t = tb_mclock() - t;
	tb_trace_i("item: %llx, size: %llu ?= %u, time: %lld", test[0], test[1], tb_slist_size(slist), t);

	tb_slist_exit(slist);
}
static tb_bool_t tb_slist_test_walk_item(tb_slist_t* slist, tb_pointer_t item, tb_bool_t* bdel, tb_pointer_t priv)
{
	// check
	tb_assert_and_check_return_val(slist && bdel && priv, tb_false);

	// done
	tb_hize_t* test = priv;
	tb_size_t i = (tb_size_t)item;
	if (!((i >> 25) & 0x1))
//	if (!(i & 0x7))
//	if (1)
//	if (!(tb_rand_uint32(0, TB_MAXU32) & 0x1))
		*bdel = tb_true;
	else
	{
		test[0] += i;
		test[1]++;
	}

	// ok
	return tb_true;
}
static tb_void_t tb_slist_test_walk_perf()
{
	// init slist
	tb_slist_t* 	slist = tb_slist_init(TB_SLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return(slist);

	// clear rand
	tb_rand_clear();

	// add items
	__tb_volatile__ tb_size_t n = 100000;
	while (n--) tb_slist_insert_tail(slist, (tb_pointer_t)(tb_size_t)tb_rand_uint32(0, TB_MAXU32)); 

	// performance
	tb_hong_t t = tb_mclock();
	__tb_volatile__ tb_hize_t test[2] = {0};
	tb_slist_walk(slist, tb_slist_test_walk_item, (tb_pointer_t)test);
	t = tb_mclock() - t;
	tb_trace_i("item: %llx, size: %llu ?= %u, time: %lld", test[0], test[1], tb_slist_size(slist), t);

	tb_slist_exit(slist);
}
/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_container_slist_main(tb_int_t argc, tb_char_t** argv)
{
#if 1
	tb_slist_int_test();
	tb_slist_str_test();
	tb_slist_mem_test();
#endif

#if 1
	tb_slist_perf_test();
#endif

#if 1
	tb_slist_test_itor_perf();
	tb_slist_test_walk_perf();
#endif

	return 0;
}
