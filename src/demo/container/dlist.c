/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

#define TB_DLIST_GROW_SIZE 			(256)

/* ///////////////////////////////////////////////////////////////////////
 * details
 */

static tb_size_t tb_dlist_insert_test()
{
	// init
	tb_dlist_t* dlist = tb_dlist_init(TB_DLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(dlist, 0);

	// insert one first
	tb_size_t itor = tb_dlist_insert_head(dlist, 0xd);

	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 10000;
	tb_hong_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_dlist_insert_prev(dlist, itor, 0xd);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_dlist_insert_prev(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// check
	tb_assert(tb_dlist_size(dlist) == n + 1);
	tb_assert(tb_dlist_head(dlist) == 0xd);
	tb_assert(tb_dlist_last(dlist) == 0xd);

	// clear it
	tb_dlist_clear(dlist);
	tb_assert(!tb_dlist_size(dlist));

	// exit
	tb_dlist_exit(dlist);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_dlist_insert_head_test()
{
	// init
	tb_dlist_t* dlist = tb_dlist_init(TB_DLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(dlist, 0);

	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 1000000;
	tb_hong_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_dlist_insert_head(dlist, 0xd);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_dlist_insert_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// check
	tb_assert(tb_dlist_size(dlist) == n);
	tb_assert(tb_dlist_head(dlist) == 0xd);
	tb_assert(tb_dlist_last(dlist) == 0xd);

	// clear it
	tb_dlist_clear(dlist);
	tb_assert(!tb_dlist_size(dlist));

	// exit
	tb_dlist_exit(dlist);
	
	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_dlist_insert_tail_test()
{
	// init
	tb_dlist_t* dlist = tb_dlist_init(TB_DLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(dlist, 0);

	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 1000000;
	tb_hong_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_dlist_insert_tail(dlist, 0xd);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_dlist_insert_tail(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// check
	tb_assert(tb_dlist_size(dlist) == n);
	tb_assert(tb_dlist_head(dlist) == 0xd);
	tb_assert(tb_dlist_last(dlist) == 0xd);

	// clear it
	tb_dlist_clear(dlist);
	tb_assert(!tb_dlist_size(dlist));

	// exit
	tb_dlist_exit(dlist);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_dlist_ninsert_test()
{
	// init
	tb_dlist_t* dlist = tb_dlist_init(TB_DLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(dlist, 0);

	// insert one first
	tb_size_t itor = tb_dlist_insert_head(dlist, 0xd);

	tb_size_t n = 1000000;
	tb_hong_t t = tb_mclock();
	tb_dlist_ninsert_prev(dlist, itor, 0xd, n);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_dlist_ninsert_prev(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// check
	tb_assert(tb_dlist_size(dlist) == n + 1);
	tb_assert(tb_dlist_head(dlist) == 0xd);
	tb_assert(tb_dlist_last(dlist) == 0xd);

	// clear it
	tb_dlist_clear(dlist);
	tb_assert(!tb_dlist_size(dlist));

	// exit
	tb_dlist_exit(dlist);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_dlist_ninsert_head_test()
{
	// init
	tb_dlist_t* dlist = tb_dlist_init(TB_DLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(dlist, 0);

	tb_size_t n = 1000000;
	tb_hong_t t = tb_mclock();
	tb_dlist_ninsert_head(dlist, 0xd, n);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_dlist_ninsert_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// check
	tb_assert(tb_dlist_size(dlist) == n);
	tb_assert(tb_dlist_head(dlist) == 0xd);
	tb_assert(tb_dlist_last(dlist) == 0xd);

	// clear it
	tb_dlist_clear(dlist);
	tb_assert(!tb_dlist_size(dlist));

	// exit
	tb_dlist_exit(dlist);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_dlist_ninsert_tail_test()
{
	// init
	tb_dlist_t* dlist = tb_dlist_init(TB_DLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(dlist, 0);

	tb_size_t n = 1000000;
	tb_hong_t t = tb_mclock();
	tb_dlist_ninsert_tail(dlist, 0xd, n);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_dlist_ninsert_tail(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// check
	tb_assert(tb_dlist_size(dlist) == n);
	tb_assert(tb_dlist_head(dlist) == 0xd);
	tb_assert(tb_dlist_last(dlist) == 0xd);

	// clear it
	tb_dlist_clear(dlist);
	tb_assert(!tb_dlist_size(dlist));

	// exit
	tb_dlist_exit(dlist);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_dlist_remove_test()
{
	// init
	tb_dlist_t* dlist = tb_dlist_init(TB_DLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(dlist, 0);

	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 1000000;
	tb_size_t itor = tb_dlist_ninsert_head(dlist, 0xd, n);
	tb_hong_t t = tb_mclock();
	for (i = 0; i < n; i++) itor = tb_dlist_remove(dlist, itor);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_dlist_remove(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// check
	tb_assert(!tb_dlist_size(dlist));

	// exit
	tb_dlist_exit(dlist);

	// ok?
	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_dlist_remove_head_test()
{
	// init
	tb_dlist_t* dlist = tb_dlist_init(TB_DLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(dlist, 0);

	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 1000000;
	tb_dlist_ninsert_head(dlist, 0xd, n);
	tb_hong_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_dlist_remove_head(dlist);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_dlist_remove_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// check
	tb_assert(!tb_dlist_size(dlist));

	// exit
	tb_dlist_exit(dlist);

	// ok?
	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_dlist_remove_last_test()
{
	// init
	tb_dlist_t* dlist = tb_dlist_init(TB_DLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(dlist, 0);

	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 10000;
	tb_dlist_ninsert_head(dlist, 0xd, n);
	tb_hong_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_dlist_remove_last(dlist);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_dlist_remove_last(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// check
	tb_assert(!tb_dlist_size(dlist));

	// exit
	tb_dlist_exit(dlist);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_dlist_nremove_test()
{
	// init
	tb_dlist_t* dlist = tb_dlist_init(TB_DLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(dlist, 0);

	tb_size_t n = 1000000;
	tb_dlist_ninsert_head(dlist, 0xd, n);
	tb_hong_t t = tb_mclock();
	tb_dlist_nremove(dlist, tb_iterator_head(dlist), n);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_dlist_nremove(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// check
	tb_assert(!tb_dlist_size(dlist));

	// exit
	tb_dlist_exit(dlist);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_dlist_nremove_head_test()
{
	// init
	tb_dlist_t* dlist = tb_dlist_init(TB_DLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(dlist, 0);

	tb_size_t n = 1000000;
	tb_dlist_ninsert_head(dlist, 0xd, n);
	tb_hong_t t = tb_mclock();
	tb_dlist_nremove_head(dlist, n);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_dlist_nremove_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// check
	tb_assert(!tb_dlist_size(dlist));

	// exit
	tb_dlist_exit(dlist);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_dlist_nremove_last_test()
{
	// init
	tb_dlist_t* dlist = tb_dlist_init(TB_DLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(dlist, 0);

	tb_size_t n = 1000000;
	tb_dlist_ninsert_head(dlist, 0xd, n);
	tb_hong_t t = tb_mclock();
	tb_dlist_nremove_last(dlist, n);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_dlist_nremove_last(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// check
	tb_assert(!tb_dlist_size(dlist));

	// exit
	tb_dlist_exit(dlist);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_dlist_replace_test()
{
	// init
	tb_dlist_t* dlist = tb_dlist_init(TB_DLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(dlist, 0);

	tb_size_t n = 1000000;
	tb_dlist_ninsert_head(dlist, 0xd, n);
	tb_size_t itor = tb_iterator_head(dlist);
	tb_size_t tail = tb_iterator_tail(dlist);
	tb_hong_t t = tb_mclock();
	for (; itor != tail; itor = tb_iterator_next(dlist, itor)) tb_dlist_replace(dlist, itor, 0xe);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_dlist_replace(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// check
	tb_assert(tb_dlist_size(dlist) == n);
	tb_assert(tb_dlist_head(dlist) == 0xe);
	tb_assert(tb_dlist_last(dlist) == 0xe);

	// exit
	tb_dlist_exit(dlist);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_dlist_replace_head_test()
{
	// init
	tb_dlist_t* dlist = tb_dlist_init(TB_DLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(dlist, 0);

	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 1000000;
	tb_dlist_ninsert_head(dlist, 0xd, n);
	tb_hong_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_dlist_replace_head(dlist, 0xe);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_dlist_replace_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// check
	tb_assert(tb_dlist_size(dlist) == n);
	tb_assert(tb_dlist_head(dlist) == 0xe);

	// exit
	tb_dlist_exit(dlist);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_dlist_replace_last_test()
{
	// init
	tb_dlist_t* dlist = tb_dlist_init(TB_DLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(dlist, 0);

	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 1000000;
	tb_dlist_ninsert_head(dlist, 0xd, n);
	tb_hong_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_dlist_replace_last(dlist, 0xe);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_dlist_replace_last(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// check
	tb_assert(tb_dlist_size(dlist) == n);
	tb_assert(tb_dlist_last(dlist) == 0xe);


	// exit
	tb_dlist_exit(dlist);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_dlist_nreplace_test()
{
	// init
	tb_dlist_t* dlist = tb_dlist_init(TB_DLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(dlist, 0);

	tb_size_t n = 1000000;
	tb_dlist_ninsert_head(dlist, 0xd, n);
	tb_hong_t t = tb_mclock();
	tb_dlist_nreplace(dlist, tb_iterator_head(dlist), 0xe, n);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_dlist_nreplace(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// check
	tb_assert(tb_dlist_size(dlist) == n);
	tb_assert(tb_dlist_head(dlist) == 0xe);
	tb_assert(tb_dlist_last(dlist) == 0xe);

	// exit
	tb_dlist_exit(dlist);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_dlist_nreplace_head_test()
{
	// init
	tb_dlist_t* dlist = tb_dlist_init(TB_DLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(dlist, 0);

	tb_size_t n = 1000000;
	tb_dlist_ninsert_head(dlist, 0xd, n);
	tb_hong_t t = tb_mclock();
	tb_dlist_nreplace_head(dlist, 0xe, n);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_dlist_nreplace_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// check
	tb_assert(tb_dlist_size(dlist) == n);
	tb_assert(tb_dlist_head(dlist) == 0xe);
	tb_assert(tb_dlist_last(dlist) == 0xe);

	// exit
	tb_dlist_exit(dlist);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_dlist_nreplace_last_test()
{
	// init
	tb_dlist_t* dlist = tb_dlist_init(TB_DLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(dlist, 0);

	tb_size_t n = 1000000;
	tb_dlist_ninsert_head(dlist, 0xd, n);
	tb_hong_t t = tb_mclock();
	tb_dlist_nreplace_last(dlist, 0xe, n);
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_dlist_nreplace_last(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// check
	tb_assert(tb_dlist_size(dlist) == n);
	tb_assert(tb_dlist_head(dlist) == 0xe);
	tb_assert(tb_dlist_last(dlist) == 0xe);


	// exit
	tb_dlist_exit(dlist);

	return n / ((tb_uint32_t)(t) + 1);
}

static tb_size_t tb_dlist_iterator_next_test()
{
	// init
	tb_dlist_t* dlist = tb_dlist_init(TB_DLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(dlist, 0);

	tb_size_t n = 1000000;
	tb_dlist_ninsert_head(dlist, 0xd, n);
	__tb_volatile__ tb_size_t itor = tb_iterator_head(dlist);
	__tb_volatile__ tb_size_t tail = tb_iterator_tail(dlist);
	tb_hong_t t = tb_mclock();
	for (; itor != tail; itor = tb_iterator_next(dlist, itor))
	{
		__tb_volatile__ tb_byte_t const* item = tb_iterator_item(dlist, itor);
		tb_used(item);
	}
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_dlist_iterator_next(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// exit
	tb_dlist_exit(dlist);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_dlist_iterator_prev_test()
{
	// init
	tb_dlist_t* dlist = tb_dlist_init(TB_DLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return_val(dlist, 0);

	tb_size_t n = 10000;
	tb_dlist_ninsert_head(dlist, 0xd, n);
	__tb_volatile__ tb_size_t itor = tb_iterator_last(dlist);
	__tb_volatile__ tb_size_t head = tb_iterator_head(dlist);
	tb_hong_t t = tb_mclock();
	while (1)
	{
		__tb_volatile__ tb_byte_t const* item = tb_iterator_item(dlist, itor);
		tb_used(item);

		if (itor == head) break;
		itor = tb_iterator_prev(dlist, itor);
	}
	t = tb_mclock() - t;

	// time
	tb_trace_i("tb_dlist_iterator_prev(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// exit
	tb_dlist_exit(dlist);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_void_t tb_dlist_int_dump(tb_dlist_t const* dlist)
{
	tb_trace_i("tb_int_t size: %d, maxn: %d", tb_dlist_size(dlist), tb_dlist_maxn(dlist));
	tb_size_t itor = tb_iterator_head(dlist);
	tb_size_t tail = tb_iterator_tail(dlist);
	for (; itor != tail; itor = tb_iterator_next(dlist, itor))
	{
		tb_char_t const* item = tb_iterator_item(dlist, itor);
		tb_trace_i("tb_int_t at[%lx]: %x", itor, item);
	}
}
static tb_void_t tb_dlist_int_test()
{
	tb_dlist_t* dlist = tb_dlist_init(TB_DLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return(dlist);

	tb_size_t 			i = 0;
	tb_size_t 			j = 0;

	tb_trace_i("=============================================================");
	tb_trace_i("insert:");
	tb_dlist_ninsert_head(dlist, 0xa, 10);
	i = tb_dlist_ninsert_tail(dlist, 0xf, 10);
	j = tb_dlist_insert_prev(dlist, i, 0);
	tb_dlist_insert_prev(dlist, i, 1);
	tb_dlist_insert_prev(dlist, i, 2);
	tb_dlist_insert_prev(dlist, i, 3);
	tb_dlist_insert_prev(dlist, i, 4);
	tb_dlist_insert_prev(dlist, i, 5);
	tb_dlist_insert_prev(dlist, i, 6);
	tb_dlist_insert_prev(dlist, i, 7);
	tb_dlist_insert_prev(dlist, i, 8);
	tb_dlist_insert_prev(dlist, i, 9);
	tb_dlist_insert_head(dlist, 4);
	tb_dlist_insert_head(dlist, 3);
	tb_dlist_insert_head(dlist, 2);
	tb_dlist_insert_head(dlist, 1);
	tb_dlist_insert_head(dlist, 0);
	tb_dlist_insert_tail(dlist, 5);
	tb_dlist_insert_tail(dlist, 6);
	tb_dlist_insert_tail(dlist, 7);
	tb_dlist_insert_tail(dlist, 8);
	tb_dlist_insert_tail(dlist, 9);
	tb_dlist_int_dump(dlist);

	tb_trace_i("=============================================================");
	tb_trace_i("remove:");
	tb_dlist_nremove_head(dlist, 5);
	tb_dlist_nremove_last(dlist, 5);
	j = tb_dlist_remove(dlist, j);
	j = tb_dlist_remove(dlist, j);
	j = tb_dlist_remove(dlist, j);
	j = tb_dlist_remove(dlist, j);
	j = tb_dlist_remove(dlist, j);
	j = tb_dlist_remove(dlist, j);
	j = tb_dlist_remove(dlist, j);
	j = tb_dlist_remove(dlist, j);
	j = tb_dlist_remove(dlist, j);
	j = tb_dlist_remove(dlist, j);	
	tb_dlist_int_dump(dlist);

	tb_trace_i("=============================================================");
	tb_trace_i("replace:");
	tb_dlist_nreplace_head(dlist, 0xf, 10);
	tb_dlist_nreplace_last(dlist, 0xa, 10);
	tb_dlist_replace_head(dlist, 0);
	tb_dlist_replace_last(dlist, 1);
	tb_dlist_int_dump(dlist);

	tb_trace_i("=============================================================");
	tb_trace_i("moveto:");
	tb_dlist_moveto_head(dlist, tb_iterator_last(dlist));
	tb_dlist_moveto_tail(dlist, tb_iterator_next(dlist, tb_iterator_head(dlist)));
	tb_dlist_int_dump(dlist);

	tb_trace_i("=============================================================");
	tb_trace_i("clear:");
	tb_dlist_clear(dlist);
	tb_dlist_int_dump(dlist);

	tb_dlist_exit(dlist);
}
static tb_void_t tb_dlist_str_dump(tb_dlist_t const* dlist)
{
	tb_trace_i("str size: %d, maxn: %d", tb_dlist_size(dlist), tb_dlist_maxn(dlist));
	tb_size_t itor = tb_iterator_head(dlist);
	tb_size_t tail = tb_iterator_tail(dlist);
	for (; itor != tail; itor = tb_iterator_next(dlist, itor))
	{
		tb_char_t const* item = tb_iterator_item(dlist, itor);
		tb_trace_i("str at[%lx]: %s", itor, item);
	}
}
static tb_void_t tb_dlist_str_test()
{
	tb_handle_t spool = tb_spool_init(TB_SPOOL_GROW_SMALL, 0);
	tb_dlist_t* dlist = tb_dlist_init(TB_DLIST_GROW_SIZE, tb_item_func_str(tb_true, spool));
	tb_assert_and_check_return(dlist);

	tb_size_t 			i = 0;
	tb_size_t 			j = 0;

	tb_trace_i("=============================================================");
	tb_trace_i("insert:");
	tb_dlist_ninsert_head(dlist, "HHHHHHHHHH", 10);
	i = tb_dlist_ninsert_tail(dlist, "TTTTTTTTTT", 10);
	j = tb_dlist_insert_prev(dlist, i, "0000000000");
	tb_dlist_insert_prev(dlist, i, "1111111111");
	tb_dlist_insert_prev(dlist, i, "2222222222");
	tb_dlist_insert_prev(dlist, i, "3333333333");
	tb_dlist_insert_prev(dlist, i, "4444444444");
	tb_dlist_insert_prev(dlist, i, "5555555555");
	tb_dlist_insert_prev(dlist, i, "6666666666");
	tb_dlist_insert_prev(dlist, i, "7777777777");
	tb_dlist_insert_prev(dlist, i, "8888888888");
	tb_dlist_insert_prev(dlist, i, "9999999999");
	tb_dlist_insert_head(dlist, "4444444444");
	tb_dlist_insert_head(dlist, "3333333333");
	tb_dlist_insert_head(dlist, "2222222222");
	tb_dlist_insert_head(dlist, "1111111111");
	tb_dlist_insert_head(dlist, "0000000000");
	tb_dlist_insert_tail(dlist, "5555555555");
	tb_dlist_insert_tail(dlist, "6666666666");
	tb_dlist_insert_tail(dlist, "7777777777");
	tb_dlist_insert_tail(dlist, "8888888888");
	tb_dlist_insert_tail(dlist, "9999999999");
	tb_dlist_str_dump(dlist);

	tb_trace_i("=============================================================");
	tb_trace_i("remove:");
	tb_dlist_nremove_head(dlist, 5);
	tb_dlist_nremove_last(dlist, 5);
	j = tb_dlist_remove(dlist, j);
	j = tb_dlist_remove(dlist, j);
	j = tb_dlist_remove(dlist, j);
	j = tb_dlist_remove(dlist, j);
	j = tb_dlist_remove(dlist, j);
	j = tb_dlist_remove(dlist, j);
	j = tb_dlist_remove(dlist, j);
	j = tb_dlist_remove(dlist, j);
	j = tb_dlist_remove(dlist, j);
	j = tb_dlist_remove(dlist, j);	
	tb_dlist_str_dump(dlist);

	tb_trace_i("=============================================================");
	tb_trace_i("replace:");
	tb_dlist_nreplace_head(dlist, "TTTTTTTTTT", 10);
	tb_dlist_nreplace_last(dlist, "HHHHHHHHHH", 10);
	tb_dlist_replace_head(dlist, "OOOOOOOOOO");
	tb_dlist_replace_last(dlist, "IIIIIIIIII");
	tb_dlist_str_dump(dlist);

	tb_trace_i("=============================================================");
	tb_trace_i("moveto:");
	tb_dlist_moveto_head(dlist, tb_iterator_last(dlist));
	tb_dlist_moveto_tail(dlist, tb_iterator_next(dlist, tb_iterator_head(dlist)));
	tb_dlist_str_dump(dlist);

	tb_trace_i("=============================================================");
	tb_trace_i("clear:");
	tb_dlist_clear(dlist);
	tb_dlist_str_dump(dlist);

	tb_dlist_exit(dlist);
	tb_spool_exit(spool);
}
static tb_void_t tb_dlist_efm_dump(tb_dlist_t const* dlist)
{
	tb_trace_i("efm size: %d, maxn: %d", tb_dlist_size(dlist), tb_dlist_maxn(dlist));
	tb_size_t itor = tb_iterator_head(dlist);
	tb_size_t tail = tb_iterator_tail(dlist);
	for (; itor != tail; itor = tb_iterator_next(dlist, itor))
	{
		tb_char_t const* item = tb_iterator_item(dlist, itor);
		tb_trace_i("efm at[%lx]: %s", itor, item);
	}
}
static tb_void_t tb_dlist_efm_test()
{
	tb_handle_t rpool = tb_rpool_init(256, 11, 0);
	tb_dlist_t* dlist = tb_dlist_init(TB_DLIST_GROW_SIZE, tb_item_func_efm(11, rpool));
	tb_assert_and_check_return(dlist);

	tb_size_t 			i = 0;
	tb_size_t 			j = 0;

	tb_trace_i("=============================================================");
	tb_trace_i("insert:");
	tb_dlist_ninsert_head(dlist, "HHHHHHHHHH", 10);
	i = tb_dlist_ninsert_tail(dlist, "TTTTTTTTTT", 10);
	j = tb_dlist_insert_prev(dlist, i, "0000000000");
	tb_dlist_insert_prev(dlist, i, "1111111111");
	tb_dlist_insert_prev(dlist, i, "2222222222");
	tb_dlist_insert_prev(dlist, i, "3333333333");
	tb_dlist_insert_prev(dlist, i, "4444444444");
	tb_dlist_insert_prev(dlist, i, "5555555555");
	tb_dlist_insert_prev(dlist, i, "6666666666");
	tb_dlist_insert_prev(dlist, i, "7777777777");
	tb_dlist_insert_prev(dlist, i, "8888888888");
	tb_dlist_insert_prev(dlist, i, "9999999999");
	tb_dlist_insert_head(dlist, "4444444444");
	tb_dlist_insert_head(dlist, "3333333333");
	tb_dlist_insert_head(dlist, "2222222222");
	tb_dlist_insert_head(dlist, "1111111111");
	tb_dlist_insert_head(dlist, "0000000000");
	tb_dlist_insert_tail(dlist, "5555555555");
	tb_dlist_insert_tail(dlist, "6666666666");
	tb_dlist_insert_tail(dlist, "7777777777");
	tb_dlist_insert_tail(dlist, "8888888888");
	tb_dlist_insert_tail(dlist, "9999999999");
	tb_dlist_efm_dump(dlist);

	tb_trace_i("=============================================================");
	tb_trace_i("remove:");
	tb_dlist_nremove_head(dlist, 5);
	tb_dlist_nremove_last(dlist, 5);
	j = tb_dlist_remove(dlist, j);
	j = tb_dlist_remove(dlist, j);
	j = tb_dlist_remove(dlist, j);
	j = tb_dlist_remove(dlist, j);
	j = tb_dlist_remove(dlist, j);
	j = tb_dlist_remove(dlist, j);
	j = tb_dlist_remove(dlist, j);
	j = tb_dlist_remove(dlist, j);
	j = tb_dlist_remove(dlist, j);
	j = tb_dlist_remove(dlist, j);	
	tb_dlist_efm_dump(dlist);

	tb_trace_i("=============================================================");
	tb_trace_i("replace:");
	tb_dlist_nreplace_head(dlist, "TTTTTTTTTT", 10);
	tb_dlist_nreplace_last(dlist, "HHHHHHHHHH", 10);
	tb_dlist_replace_head(dlist, "OOOOOOOOOO");
	tb_dlist_replace_last(dlist, "IIIIIIIIII");
	tb_dlist_efm_dump(dlist);

	tb_trace_i("=============================================================");
	tb_trace_i("moveto:");
	tb_dlist_moveto_head(dlist, tb_iterator_last(dlist));
	tb_dlist_moveto_tail(dlist, tb_iterator_next(dlist, tb_iterator_head(dlist)));
	tb_dlist_efm_dump(dlist);

	tb_trace_i("=============================================================");
	tb_trace_i("clear:");
	tb_dlist_clear(dlist);
	tb_dlist_efm_dump(dlist);

	tb_dlist_exit(dlist);
	tb_rpool_exit(rpool);
}
static tb_void_t tb_dlist_ifm_free(tb_item_func_t* func, tb_pointer_t item)
{
	tb_trace_i("ifm free: %s, priv: %s", item, func->priv);
}
static tb_void_t tb_dlist_ifm_dump(tb_dlist_t const* dlist)
{
	tb_trace_i("ifm size: %d, maxn: %d", tb_dlist_size(dlist), tb_dlist_maxn(dlist));
	tb_size_t itor = tb_iterator_head(dlist);
	tb_size_t tail = tb_iterator_tail(dlist);
	for (; itor != tail; itor = tb_iterator_next(dlist, itor))
	{
		tb_char_t const* item = tb_iterator_item(dlist, itor);
		tb_trace_i("ifm at[%lx]: %s", itor, item);
	}
}
static tb_void_t tb_dlist_ifm_test()
{
	tb_dlist_t* dlist = tb_dlist_init(TB_DLIST_GROW_SIZE, tb_item_func_ifm(11, tb_dlist_ifm_free, "ifm"));
	tb_assert_and_check_return(dlist);

	tb_size_t 			i = 0;
	tb_size_t 			j = 0;

	tb_trace_i("=============================================================");
	tb_trace_i("insert:");
	tb_dlist_ninsert_head(dlist, "HHHHHHHHHH", 10);
	i = tb_dlist_ninsert_tail(dlist, "TTTTTTTTTT", 10);
	j = tb_dlist_insert_prev(dlist, i, "0000000000");
	tb_dlist_insert_prev(dlist, i, "1111111111");
	tb_dlist_insert_prev(dlist, i, "2222222222");
	tb_dlist_insert_prev(dlist, i, "3333333333");
	tb_dlist_insert_prev(dlist, i, "4444444444");
	tb_dlist_insert_prev(dlist, i, "5555555555");
	tb_dlist_insert_prev(dlist, i, "6666666666");
	tb_dlist_insert_prev(dlist, i, "7777777777");
	tb_dlist_insert_prev(dlist, i, "8888888888");
	tb_dlist_insert_prev(dlist, i, "9999999999");
	tb_dlist_insert_head(dlist, "4444444444");
	tb_dlist_insert_head(dlist, "3333333333");
	tb_dlist_insert_head(dlist, "2222222222");
	tb_dlist_insert_head(dlist, "1111111111");
	tb_dlist_insert_head(dlist, "0000000000");
	tb_dlist_insert_tail(dlist, "5555555555");
	tb_dlist_insert_tail(dlist, "6666666666");
	tb_dlist_insert_tail(dlist, "7777777777");
	tb_dlist_insert_tail(dlist, "8888888888");
	tb_dlist_insert_tail(dlist, "9999999999");
	tb_dlist_ifm_dump(dlist);

	tb_trace_i("=============================================================");
	tb_trace_i("remove:");
	tb_dlist_nremove_head(dlist, 5);
	tb_dlist_nremove_last(dlist, 5);
	j = tb_dlist_remove(dlist, j);
	j = tb_dlist_remove(dlist, j);
	j = tb_dlist_remove(dlist, j);
	j = tb_dlist_remove(dlist, j);
	j = tb_dlist_remove(dlist, j);
	j = tb_dlist_remove(dlist, j);
	j = tb_dlist_remove(dlist, j);
	j = tb_dlist_remove(dlist, j);
	j = tb_dlist_remove(dlist, j);
	j = tb_dlist_remove(dlist, j);	
	tb_dlist_ifm_dump(dlist);

	tb_trace_i("=============================================================");
	tb_trace_i("replace:");
	tb_dlist_nreplace_head(dlist, "TTTTTTTTTT", 10);
	tb_dlist_nreplace_last(dlist, "HHHHHHHHHH", 10);
	tb_dlist_replace_head(dlist, "OOOOOOOOOO");
	tb_dlist_replace_last(dlist, "IIIIIIIIII");
	tb_dlist_ifm_dump(dlist);

	tb_trace_i("=============================================================");
	tb_trace_i("moveto:");
	tb_dlist_moveto_head(dlist, tb_iterator_last(dlist));
	tb_dlist_moveto_tail(dlist, tb_iterator_next(dlist, tb_iterator_head(dlist)));
	tb_dlist_ifm_dump(dlist);

	tb_trace_i("=============================================================");
	tb_trace_i("clear:");
	tb_dlist_clear(dlist);
	tb_dlist_ifm_dump(dlist);

	tb_dlist_exit(dlist);
}
static tb_void_t tb_dlist_perf_test()
{
	tb_size_t score = 0;
	tb_trace_i("=============================================================");
	tb_trace_i("insert performance:");
	score += tb_dlist_insert_test();
	score += tb_dlist_insert_head_test();
	score += tb_dlist_insert_tail_test();
	score += tb_dlist_ninsert_test();
	score += tb_dlist_ninsert_head_test();
	score += tb_dlist_ninsert_tail_test();

	tb_trace_i("=============================================================");
	tb_trace_i("remove performance:");
	score += tb_dlist_remove_test();
	score += tb_dlist_remove_head_test();
	score += tb_dlist_remove_last_test();
	score += tb_dlist_nremove_test();
	score += tb_dlist_nremove_head_test();
	score += tb_dlist_nremove_last_test();

	tb_trace_i("=============================================================");
	tb_trace_i("replace performance:");
	score += tb_dlist_replace_test();
	score += tb_dlist_replace_head_test();
	score += tb_dlist_replace_last_test();
	score += tb_dlist_nreplace_test();
	score += tb_dlist_nreplace_head_test();
	score += tb_dlist_nreplace_last_test();

	tb_trace_i("=============================================================");
	tb_trace_i("iterator performance:");
	score += tb_dlist_iterator_next_test();
	score += tb_dlist_iterator_prev_test();

	tb_trace_i("=============================================================");
	tb_trace_i("score: %d", score / 100);

}
static tb_void_t tb_dlist_test_itor_perf()
{
	// init dlist
	tb_dlist_t* 	dlist = tb_dlist_init(TB_DLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return(dlist);

	// clear rand
	tb_rand_clear();

	// add items
	__tb_volatile__ tb_size_t n = 100000;
	while (n--) tb_dlist_insert_tail(dlist, tb_rand_uint32(0, TB_MAXU32)); 

	// performance
	tb_hong_t t = tb_mclock();
	__tb_volatile__ tb_hize_t test[2] = {0};
	__tb_volatile__ tb_size_t 	prev = 0;
	__tb_volatile__ tb_size_t 	itor = tb_iterator_head(dlist);
	for (; itor != tb_iterator_tail(dlist); )
	{
		__tb_volatile__ tb_size_t item = tb_iterator_item(dlist, itor);
#if 1
		if (!(((tb_size_t)item >> 25) & 0x1))
		{
			// save 
			tb_size_t next = tb_iterator_next(dlist, itor);

			// remove
			tb_dlist_remove_next(dlist, prev);

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
		itor = tb_iterator_next(dlist, itor);
	}
	t = tb_mclock() - t;
	tb_trace_i("item: %llx, size: %llu ?= %u, time: %lld", test[0], test[1], tb_dlist_size(dlist), t);

	tb_dlist_exit(dlist);
}
static tb_bool_t tb_dlist_test_walk_item(tb_dlist_t* dlist, tb_pointer_t* item, tb_bool_t* bdel, tb_pointer_t data)
{
	tb_assert_and_check_return_val(dlist && bdel && data, tb_false);

	tb_hize_t* test = data;
	if (item)
	{
		tb_size_t i = (tb_size_t)*item;
		if (!((i >> 25) & 0x1))
//		if (!(i & 0x7))
//		if (1)
//		if (!(tb_rand_uint32(0, TB_MAXU32) & 0x1))
			*bdel = tb_true;
		else
		{
			test[0] += i;
			test[1]++;
		}
	}

	// ok
	return tb_true;
}
static tb_void_t tb_dlist_test_walk_perf()
{
	// init dlist
	tb_dlist_t* 	dlist = tb_dlist_init(TB_DLIST_GROW_SIZE, tb_item_func_long());
	tb_assert_and_check_return(dlist);

	// clear rand
	tb_rand_clear();

	// add items
	__tb_volatile__ tb_size_t n = 100000;
	while (n--) tb_dlist_insert_tail(dlist, tb_rand_uint32(0, TB_MAXU32)); 

	// performance
	tb_hong_t t = tb_mclock();
	__tb_volatile__ tb_hize_t test[2] = {0};
	tb_dlist_walk(dlist, tb_dlist_test_walk_item, test);
//	tb_dlist_walk(dlist, tb_dlist_test_walk_item, test);
	t = tb_mclock() - t;
	tb_trace_i("item: %llx, size: %llu ?= %u, time: %lld", test[0], test[1], tb_dlist_size(dlist), t);

	tb_dlist_exit(dlist);
}
/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_container_dlist_main(tb_int_t argc, tb_char_t** argv)
{
#if 1
	tb_dlist_int_test();
	tb_dlist_str_test();
	tb_dlist_efm_test();
	tb_dlist_ifm_test();
#endif

#if 1
	tb_dlist_perf_test();
#endif

#if 1
	tb_dlist_test_itor_perf();
	tb_dlist_test_walk_perf();
#endif

	return 0;
}
