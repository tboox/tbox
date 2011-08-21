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

static tb_void_t tb_dlist_char_ctor(tb_void_t* data, tb_void_t* priv)
{
	*((tb_char_t*)data) = '_';
	TB_DBG("[ctor]: %s, _", (tb_char_t const*)priv);
}
static tb_void_t tb_dlist_char_dtor(tb_void_t* data, tb_void_t* priv)
{
	TB_DBG("[dtor]: %s, %c", (tb_char_t const*)priv, *((tb_char_t*)data));
}
static tb_void_t tb_dlist_char_dump(tb_dlist_t const* dlist)
{
	TB_DBG("size: %d, maxn: %d", tb_dlist_size(dlist), tb_dlist_maxn(dlist));
	tb_size_t itor = tb_dlist_head(dlist);
	tb_size_t tail = tb_dlist_tail(dlist);
	for (; itor != tail; itor = tb_dlist_next(dlist, itor))
	{
		tb_byte_t const* item = tb_dlist_const_at(dlist, itor);
		if (item)
		{
			TB_DBG("at[%d]: %c", itor, *((tb_char_t const*)item));
		}
	}
}
static tb_size_t tb_dlist_insert_test()
{
	// create
	tb_dlist_t* dlist = tb_dlist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(dlist, 0);

	// insert one first
	tb_size_t index = tb_dlist_insert_head(dlist, "F");

	tb_size_t i = 0;
	tb_size_t n = 10000;
	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_dlist_insert(dlist, index, "F");
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_dlist_insert(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// check
	TB_ASSERT(tb_dlist_size(dlist) == n + 1);
	TB_ASSERT(tb_dlist_const_at(dlist, tb_dlist_head(dlist))[0] == 'F');
	TB_ASSERT(!tb_cstring_compare("F", tb_dlist_const_at(dlist, tb_dlist_last(dlist))));

	// clear it
	tb_dlist_clear(dlist);
	TB_ASSERT(!tb_dlist_size(dlist));

	// destroy
	tb_dlist_destroy(dlist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_dlist_insert_head_test()
{
	// create
	tb_dlist_t* dlist = tb_dlist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(dlist, 0);

	tb_size_t i = 0;
	tb_size_t n = 1000000;
	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_dlist_insert_head(dlist, "F");
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_dlist_insert_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// check
	TB_ASSERT(tb_dlist_size(dlist) == n);
	TB_ASSERT(tb_dlist_const_at(dlist, tb_dlist_head(dlist))[0] == 'F');
	TB_ASSERT(!tb_cstring_compare("F", tb_dlist_const_at(dlist, tb_dlist_last(dlist))));

	// clear it
	tb_dlist_clear(dlist);
	TB_ASSERT(!tb_dlist_size(dlist));

	// destroy
	tb_dlist_destroy(dlist);
	
	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_dlist_insert_tail_test()
{
	// create
	tb_dlist_t* dlist = tb_dlist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(dlist, 0);

	tb_size_t i = 0;
	tb_size_t n = 1000000;
	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_dlist_insert_tail(dlist, "F");
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_dlist_insert_tail(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// check
	TB_ASSERT(tb_dlist_size(dlist) == n);
	TB_ASSERT(tb_dlist_const_at(dlist, tb_dlist_head(dlist))[0] == 'F');
	TB_ASSERT(!tb_cstring_compare("F", tb_dlist_const_at(dlist, tb_dlist_last(dlist))));

	// clear it
	tb_dlist_clear(dlist);
	TB_ASSERT(!tb_dlist_size(dlist));

	// destroy
	tb_dlist_destroy(dlist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_dlist_ninsert_test()
{
	// create
	tb_dlist_t* dlist = tb_dlist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(dlist, 0);

	// insert one first
	tb_size_t index = tb_dlist_insert_head(dlist, "F");

	tb_size_t n = 1000000;
	tb_int64_t t = tb_mclock();
	tb_dlist_ninsert(dlist, index, "F", n);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_dlist_ninsert(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// check
	TB_ASSERT(tb_dlist_size(dlist) == n + 1);
	TB_ASSERT(tb_dlist_const_at(dlist, tb_dlist_head(dlist))[0] == 'F');
	TB_ASSERT(!tb_cstring_compare("F", tb_dlist_const_at(dlist, tb_dlist_last(dlist))));

	// clear it
	tb_dlist_clear(dlist);
	TB_ASSERT(!tb_dlist_size(dlist));

	// destroy
	tb_dlist_destroy(dlist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_dlist_ninsert_head_test()
{
	// create
	tb_dlist_t* dlist = tb_dlist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(dlist, 0);

	tb_size_t n = 1000000;
	tb_int64_t t = tb_mclock();
	tb_dlist_ninsert_head(dlist, "F", n);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_dlist_ninsert_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// check
	TB_ASSERT(tb_dlist_size(dlist) == n);
	TB_ASSERT(tb_dlist_const_at(dlist, tb_dlist_head(dlist))[0] == 'F');
	TB_ASSERT(!tb_cstring_compare("F", tb_dlist_const_at(dlist, tb_dlist_last(dlist))));

	// clear it
	tb_dlist_clear(dlist);
	TB_ASSERT(!tb_dlist_size(dlist));

	// destroy
	tb_dlist_destroy(dlist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_dlist_ninsert_tail_test()
{
	// create
	tb_dlist_t* dlist = tb_dlist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(dlist, 0);

	tb_size_t n = 1000000;
	tb_int64_t t = tb_mclock();
	tb_dlist_ninsert_tail(dlist, "F", n);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_dlist_ninsert_tail(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// check
	TB_ASSERT(tb_dlist_size(dlist) == n);
	TB_ASSERT(tb_dlist_const_at(dlist, tb_dlist_head(dlist))[0] == 'F');
	TB_ASSERT(!tb_cstring_compare("F", tb_dlist_const_at(dlist, tb_dlist_last(dlist))));

	// clear it
	tb_dlist_clear(dlist);
	TB_ASSERT(!tb_dlist_size(dlist));

	// destroy
	tb_dlist_destroy(dlist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_dlist_remove_test()
{
	// create
	tb_dlist_t* dlist = tb_dlist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(dlist, 0);

	tb_size_t i = 0;
	tb_size_t n = 1000000;
	tb_size_t index = tb_dlist_ninsert_head(dlist, "F", n);
	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) index = tb_dlist_remove(dlist, index);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_dlist_remove(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// check
	TB_ASSERT(!tb_dlist_size(dlist));

	// destroy
	tb_dlist_destroy(dlist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_dlist_remove_head_test()
{
	// create
	tb_dlist_t* dlist = tb_dlist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(dlist, 0);

	tb_size_t i = 0;
	tb_size_t n = 1000000;
	tb_size_t index = tb_dlist_ninsert_head(dlist, "F", n);
	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_dlist_remove_head(dlist);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_dlist_remove_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// check
	TB_ASSERT(!tb_dlist_size(dlist));

	// destroy
	tb_dlist_destroy(dlist);
}
static tb_size_t tb_dlist_remove_last_test()
{
	// create
	tb_dlist_t* dlist = tb_dlist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(dlist, 0);

	tb_size_t i = 0;
	tb_size_t n = 10000;
	tb_size_t index = tb_dlist_ninsert_head(dlist, "F", n);
	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_dlist_remove_last(dlist);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_dlist_remove_last(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// check
	TB_ASSERT(!tb_dlist_size(dlist));

	// destroy
	tb_dlist_destroy(dlist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_dlist_nremove_test()
{
	// create
	tb_dlist_t* dlist = tb_dlist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(dlist, 0);

	tb_size_t n = 1000000;
	tb_size_t index = tb_dlist_ninsert_head(dlist, "F", n);
	tb_int64_t t = tb_mclock();
	tb_dlist_nremove(dlist, tb_dlist_head(dlist), n);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_dlist_nremove(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// check
	TB_ASSERT(!tb_dlist_size(dlist));

	// destroy
	tb_dlist_destroy(dlist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_dlist_nremove_head_test()
{
	// create
	tb_dlist_t* dlist = tb_dlist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(dlist, 0);

	tb_size_t n = 1000000;
	tb_size_t index = tb_dlist_ninsert_head(dlist, "F", n);
	tb_int64_t t = tb_mclock();
	tb_dlist_nremove_head(dlist, n);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_dlist_nremove_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// check
	TB_ASSERT(!tb_dlist_size(dlist));

	// destroy
	tb_dlist_destroy(dlist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_dlist_nremove_last_test()
{
	// create
	tb_dlist_t* dlist = tb_dlist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(dlist, 0);

	tb_size_t n = 1000000;
	tb_size_t index = tb_dlist_ninsert_head(dlist, "F", n);
	tb_int64_t t = tb_mclock();
	tb_dlist_nremove_last(dlist, n);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_dlist_nremove_last(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// check
	TB_ASSERT(!tb_dlist_size(dlist));

	// destroy
	tb_dlist_destroy(dlist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_dlist_replace_test()
{
	// create
	tb_dlist_t* dlist = tb_dlist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(dlist, 0);

	tb_size_t n = 1000000;
	tb_size_t index = tb_dlist_ninsert_head(dlist, "F", n);
	tb_size_t itor = tb_dlist_head(dlist);
	tb_size_t tail = tb_dlist_tail(dlist);
	tb_int64_t t = tb_mclock();
	for (; itor != tail; itor = tb_dlist_next(dlist, itor)) tb_dlist_replace(dlist, itor, "R");
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_dlist_replace(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// check
	TB_ASSERT(tb_dlist_size(dlist) == n);
	TB_ASSERT(tb_dlist_const_at(dlist, tb_dlist_head(dlist))[0] == 'R');
	TB_ASSERT(tb_dlist_const_at(dlist, tb_dlist_last(dlist))[0] == 'R');

	// destroy
	tb_dlist_destroy(dlist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_dlist_replace_head_test()
{
	// create
	tb_dlist_t* dlist = tb_dlist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(dlist, 0);

	tb_size_t i = 0;
	tb_size_t n = 1000000;
	tb_size_t index = tb_dlist_ninsert_head(dlist, "F", n);
	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_dlist_replace_head(dlist, "R");
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_dlist_replace_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// check
	TB_ASSERT(tb_dlist_size(dlist) == n);
	TB_ASSERT(tb_dlist_const_at(dlist, tb_dlist_head(dlist))[0] == 'R');

	// destroy
	tb_dlist_destroy(dlist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_dlist_replace_last_test()
{
	// create
	tb_dlist_t* dlist = tb_dlist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(dlist, 0);

	tb_size_t i = 0;
	tb_size_t n = 1000000;
	tb_size_t index = tb_dlist_ninsert_head(dlist, "F", n);
	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_dlist_replace_last(dlist, "R");
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_dlist_replace_last(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// check
	TB_ASSERT(tb_dlist_size(dlist) == n);
	TB_ASSERT(tb_dlist_const_at(dlist, tb_dlist_last(dlist))[0] == 'R');


	// destroy
	tb_dlist_destroy(dlist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_dlist_nreplace_test()
{
	// create
	tb_dlist_t* dlist = tb_dlist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(dlist, 0);

	tb_size_t n = 1000000;
	tb_size_t index = tb_dlist_ninsert_head(dlist, "F", n);
	tb_int64_t t = tb_mclock();
	tb_dlist_nreplace(dlist, tb_dlist_head(dlist), "R", n);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_dlist_nreplace(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// check
	TB_ASSERT(tb_dlist_size(dlist) == n);
	TB_ASSERT(tb_dlist_const_at(dlist, tb_dlist_head(dlist))[0] == 'R');
	TB_ASSERT(tb_dlist_const_at(dlist, tb_dlist_last(dlist))[0] == 'R');

	// destroy
	tb_dlist_destroy(dlist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_dlist_nreplace_head_test()
{
	// create
	tb_dlist_t* dlist = tb_dlist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(dlist, 0);

	tb_size_t n = 1000000;
	tb_size_t index = tb_dlist_ninsert_head(dlist, "F", n);
	tb_int64_t t = tb_mclock();
	tb_dlist_nreplace_head(dlist, "R", n);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_dlist_nreplace_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// check
	TB_ASSERT(tb_dlist_size(dlist) == n);
	TB_ASSERT(tb_dlist_const_at(dlist, tb_dlist_head(dlist))[0] == 'R');
	TB_ASSERT(tb_dlist_const_at(dlist, tb_dlist_last(dlist))[0] == 'R');

	// destroy
	tb_dlist_destroy(dlist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_dlist_nreplace_last_test()
{
	// create
	tb_dlist_t* dlist = tb_dlist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(dlist, 0);

	tb_size_t n = 1000000;
	tb_size_t index = tb_dlist_ninsert_head(dlist, "F", n);
	tb_int64_t t = tb_mclock();
	tb_dlist_nreplace_last(dlist, "R", n);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_dlist_nreplace_last(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// check
	TB_ASSERT(tb_dlist_size(dlist) == n);
	TB_ASSERT(tb_dlist_const_at(dlist, tb_dlist_head(dlist))[0] == 'R');
	TB_ASSERT(tb_dlist_const_at(dlist, tb_dlist_last(dlist))[0] == 'R');


	// destroy
	tb_dlist_destroy(dlist);

	return n / (tb_int64_to_int32(t) + 1);
}

static tb_size_t tb_dlist_iterator_next_test()
{
	// create
	tb_dlist_t* dlist = tb_dlist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(dlist, 0);

	tb_size_t n = 1000000;
	tb_dlist_ninsert_head(dlist, "F", n);
	tb_size_t itor = tb_dlist_head(dlist);
	tb_size_t tail = tb_dlist_tail(dlist);
	tb_int64_t t = tb_mclock();
	for (; itor != tail; itor = tb_dlist_next(dlist, itor))
	{
		__tb_volatile__ tb_byte_t const* item = tb_dlist_const_at(dlist, itor);
	}
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_dlist_iterator_next(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// destroy
	tb_dlist_destroy(dlist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_dlist_iterator_prev_test()
{
	// create
	tb_dlist_t* dlist = tb_dlist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(dlist, 0);

	tb_size_t n = 10000;
	tb_dlist_ninsert_head(dlist, "F", n);
	tb_size_t itor = tb_dlist_last(dlist);
	tb_size_t head = tb_dlist_head(dlist);
	tb_int64_t t = tb_mclock();
	while (1)
	{
		__tb_volatile__ tb_byte_t const* item = tb_dlist_const_at(dlist, itor);

		if (itor == head) break;
		itor = tb_dlist_prev(dlist, itor);
	}
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_dlist_iterator_prev(%d): %lld ms, size: %d, maxn: %d", n, t, tb_dlist_size(dlist), tb_dlist_maxn(dlist));

	// destroy
	tb_dlist_destroy(dlist);

	return n / (tb_int64_to_int32(t) + 1);
}
/* /////////////////////////////////////////////////////////
 * main
 */
int main(int argc, char** argv)
{
	if (!tb_init(malloc(30 * 1024 * 1024), 30 * 1024 * 1024)) return 0;

	// create dlist
	tb_dlist_t* dlist = tb_dlist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, tb_dlist_char_ctor, tb_dlist_char_dtor, "char");
	TB_ASSERT_GOTO(dlist, end);

	tb_size_t 			i = 0;
	tb_size_t 			j = 0;

	TB_DBG("=============================================================");
	TB_DBG("insert:");
	tb_dlist_ninsert_head(dlist, "H", 10);
	i = tb_dlist_ninsert_tail(dlist, "T", 10);
	j = tb_dlist_insert(dlist, i, "0");
	tb_dlist_insert(dlist, i, "1");
	tb_dlist_insert(dlist, i, "2");
	tb_dlist_insert(dlist, i, "3");
	tb_dlist_insert(dlist, i, "4");
	tb_dlist_insert(dlist, i, "5");
	tb_dlist_insert(dlist, i, "6");
	tb_dlist_insert(dlist, i, "7");
	tb_dlist_insert(dlist, i, "8");
	tb_dlist_insert(dlist, i, "9");
	tb_dlist_insert_head(dlist, "4");
	tb_dlist_insert_head(dlist, "3");
	tb_dlist_insert_head(dlist, "2");
	tb_dlist_insert_head(dlist, "1");
	tb_dlist_insert_head(dlist, "0");
	tb_dlist_insert_tail(dlist, "5");
	tb_dlist_insert_tail(dlist, "6");
	tb_dlist_insert_tail(dlist, "7");
	tb_dlist_insert_tail(dlist, "8");
	tb_dlist_insert_tail(dlist, "9");
	tb_dlist_char_dump(dlist);

	TB_DBG("=============================================================");
	TB_DBG("remove:");
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
	tb_dlist_char_dump(dlist);

	TB_DBG("=============================================================");
	TB_DBG("replace:");
	tb_dlist_nreplace_head(dlist, "T", 10);
	tb_dlist_nreplace_last(dlist, "H", 10);
	tb_dlist_replace_head(dlist, "O");
	tb_dlist_replace_last(dlist, "O");
	tb_dlist_char_dump(dlist);

	TB_DBG("=============================================================");
	TB_DBG("clear:");
	tb_dlist_clear(dlist);
	tb_dlist_char_dump(dlist);

	tb_size_t score = 0;
	TB_DBG("=============================================================");
	TB_DBG("insert performance:");
	score += tb_dlist_insert_test();
	score += tb_dlist_insert_head_test();
	score += tb_dlist_insert_tail_test();
	score += tb_dlist_ninsert_test();
	score += tb_dlist_ninsert_head_test();
	score += tb_dlist_ninsert_tail_test();

	TB_DBG("=============================================================");
	TB_DBG("remove performance:");
	score += tb_dlist_remove_test();
	score += tb_dlist_remove_head_test();
	score += tb_dlist_remove_last_test();
	score += tb_dlist_nremove_test();
	score += tb_dlist_nremove_head_test();
	score += tb_dlist_nremove_last_test();

	TB_DBG("=============================================================");
	TB_DBG("replace performance:");
	score += tb_dlist_replace_test();
	score += tb_dlist_replace_head_test();
	score += tb_dlist_replace_last_test();
	score += tb_dlist_nreplace_test();
	score += tb_dlist_nreplace_head_test();
	score += tb_dlist_nreplace_last_test();

	TB_DBG("=============================================================");
	TB_DBG("iterator performance:");
	score += tb_dlist_iterator_next_test();
	score += tb_dlist_iterator_prev_test();

	TB_DBG("=============================================================");
	TB_DBG("score: %d", score / 100);

	getchar();
end:

	// destroy dlist
	tb_dlist_destroy(dlist);

	return 0;
}
