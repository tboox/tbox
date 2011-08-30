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

static tb_void_t tb_slist_char_ctor(tb_void_t* data, tb_void_t* priv)
{
	*((tb_char_t*)data) = '_';
	TB_DBG("[ctor]: %s, _", (tb_char_t const*)priv);
}
static tb_void_t tb_slist_char_dtor(tb_void_t* data, tb_void_t* priv)
{
	TB_DBG("[dtor]: %s, %c", (tb_char_t const*)priv, *((tb_char_t*)data));
}
static tb_void_t tb_slist_char_dump(tb_slist_t const* slist)
{
	TB_DBG("size: %d, maxn: %d", tb_slist_size(slist), tb_slist_maxn(slist));
	tb_size_t itor = tb_slist_head(slist);
	tb_size_t tail = tb_slist_tail(slist);
	for (; itor != tail; itor = tb_slist_next(slist, itor))
	{
		tb_byte_t const* item = tb_slist_const_at(slist, itor);
		if (item)
		{
			TB_DBG("at[%d]: %c", itor, *((tb_char_t const*)item));
		}
	}
}
static tb_size_t tb_slist_insert_test()
{
	// create
	tb_slist_t* slist = tb_slist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(slist, 0);

	// insert one first
	tb_size_t index = tb_slist_insert_head(slist, "F");

	tb_size_t i = 0;
	tb_size_t n = 10000;
	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_slist_insert(slist, index, "F");
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_slist_insert(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	TB_ASSERT(tb_slist_size(slist) == n + 1);
	TB_ASSERT(tb_slist_const_at(slist, tb_slist_head(slist))[0] == 'F');
	TB_ASSERT(!tb_strcmp("F", tb_slist_const_at(slist, tb_slist_last(slist))));

	// clear it
	tb_slist_clear(slist);
	TB_ASSERT(!tb_slist_size(slist));

	// destroy
	tb_slist_destroy(slist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_slist_insert_head_test()
{
	// create
	tb_slist_t* slist = tb_slist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(slist, 0);

	tb_size_t i = 0;
	tb_size_t n = 1000000;
	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_slist_insert_head(slist, "F");
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_slist_insert_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	TB_ASSERT(tb_slist_size(slist) == n);
	TB_ASSERT(tb_slist_const_at(slist, tb_slist_head(slist))[0] == 'F');
	TB_ASSERT(!tb_strcmp("F", tb_slist_const_at(slist, tb_slist_last(slist))));

	// clear it
	tb_slist_clear(slist);
	TB_ASSERT(!tb_slist_size(slist));

	// destroy
	tb_slist_destroy(slist);
	
	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_slist_insert_tail_test()
{
	// create
	tb_slist_t* slist = tb_slist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(slist, 0);

	tb_size_t i = 0;
	tb_size_t n = 1000000;
	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_slist_insert_tail(slist, "F");
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_slist_insert_tail(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	TB_ASSERT(tb_slist_size(slist) == n);
	TB_ASSERT(tb_slist_const_at(slist, tb_slist_head(slist))[0] == 'F');
	TB_ASSERT(!tb_strcmp("F", tb_slist_const_at(slist, tb_slist_last(slist))));

	// clear it
	tb_slist_clear(slist);
	TB_ASSERT(!tb_slist_size(slist));

	// destroy
	tb_slist_destroy(slist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_slist_ninsert_test()
{
	// create
	tb_slist_t* slist = tb_slist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(slist, 0);

	// insert one first
	tb_size_t index = tb_slist_insert_head(slist, "F");

	tb_size_t n = 1000000;
	tb_int64_t t = tb_mclock();
	tb_slist_ninsert(slist, index, "F", n);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_slist_ninsert(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	TB_ASSERT(tb_slist_size(slist) == n + 1);
	TB_ASSERT(tb_slist_const_at(slist, tb_slist_head(slist))[0] == 'F');
	TB_ASSERT(!tb_strcmp("F", tb_slist_const_at(slist, tb_slist_last(slist))));

	// clear it
	tb_slist_clear(slist);
	TB_ASSERT(!tb_slist_size(slist));

	// destroy
	tb_slist_destroy(slist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_slist_ninsert_head_test()
{
	// create
	tb_slist_t* slist = tb_slist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(slist, 0);

	tb_size_t n = 1000000;
	tb_int64_t t = tb_mclock();
	tb_slist_ninsert_head(slist, "F", n);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_slist_ninsert_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	TB_ASSERT(tb_slist_size(slist) == n);
	TB_ASSERT(tb_slist_const_at(slist, tb_slist_head(slist))[0] == 'F');
	TB_ASSERT(!tb_strcmp("F", tb_slist_const_at(slist, tb_slist_last(slist))));

	// clear it
	tb_slist_clear(slist);
	TB_ASSERT(!tb_slist_size(slist));

	// destroy
	tb_slist_destroy(slist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_slist_ninsert_tail_test()
{
	// create
	tb_slist_t* slist = tb_slist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(slist, 0);

	tb_size_t n = 1000000;
	tb_int64_t t = tb_mclock();
	tb_slist_ninsert_tail(slist, "F", n);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_slist_ninsert_tail(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	TB_ASSERT(tb_slist_size(slist) == n);
	TB_ASSERT(tb_slist_const_at(slist, tb_slist_head(slist))[0] == 'F');
	TB_ASSERT(!tb_strcmp("F", tb_slist_const_at(slist, tb_slist_last(slist))));

	// clear it
	tb_slist_clear(slist);
	TB_ASSERT(!tb_slist_size(slist));

	// destroy
	tb_slist_destroy(slist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_slist_remove_test()
{
	// create
	tb_slist_t* slist = tb_slist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(slist, 0);

	tb_size_t i = 0;
	tb_size_t n = 1000000;
	tb_size_t index = tb_slist_ninsert_head(slist, "F", n);
	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) index = tb_slist_remove(slist, index);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_slist_remove(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	TB_ASSERT(!tb_slist_size(slist));

	// destroy
	tb_slist_destroy(slist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_slist_remove_head_test()
{
	// create
	tb_slist_t* slist = tb_slist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(slist, 0);

	tb_size_t i = 0;
	tb_size_t n = 1000000;
	tb_size_t index = tb_slist_ninsert_head(slist, "F", n);
	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_slist_remove_head(slist);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_slist_remove_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	TB_ASSERT(!tb_slist_size(slist));

	// destroy
	tb_slist_destroy(slist);
}
static tb_size_t tb_slist_remove_last_test()
{
	// create
	tb_slist_t* slist = tb_slist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(slist, 0);

	tb_size_t i = 0;
	tb_size_t n = 10000;
	tb_size_t index = tb_slist_ninsert_head(slist, "F", n);
	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_slist_remove_last(slist);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_slist_remove_last(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	TB_ASSERT(!tb_slist_size(slist));

	// destroy
	tb_slist_destroy(slist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_slist_nremove_test()
{
	// create
	tb_slist_t* slist = tb_slist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(slist, 0);

	tb_size_t n = 1000000;
	tb_size_t index = tb_slist_ninsert_head(slist, "F", n);
	tb_int64_t t = tb_mclock();
	tb_slist_nremove(slist, tb_slist_head(slist), n);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_slist_nremove(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	TB_ASSERT(!tb_slist_size(slist));

	// destroy
	tb_slist_destroy(slist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_slist_nremove_head_test()
{
	// create
	tb_slist_t* slist = tb_slist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(slist, 0);

	tb_size_t n = 1000000;
	tb_size_t index = tb_slist_ninsert_head(slist, "F", n);
	tb_int64_t t = tb_mclock();
	tb_slist_nremove_head(slist, n);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_slist_nremove_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	TB_ASSERT(!tb_slist_size(slist));

	// destroy
	tb_slist_destroy(slist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_slist_nremove_last_test()
{
	// create
	tb_slist_t* slist = tb_slist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(slist, 0);

	tb_size_t n = 1000000;
	tb_size_t index = tb_slist_ninsert_head(slist, "F", n);
	tb_int64_t t = tb_mclock();
	tb_slist_nremove_last(slist, n);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_slist_nremove_last(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	TB_ASSERT(!tb_slist_size(slist));

	// destroy
	tb_slist_destroy(slist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_slist_replace_test()
{
	// create
	tb_slist_t* slist = tb_slist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(slist, 0);

	tb_size_t n = 1000000;
	tb_size_t index = tb_slist_ninsert_head(slist, "F", n);
	tb_size_t itor = tb_slist_head(slist);
	tb_size_t tail = tb_slist_tail(slist);
	tb_int64_t t = tb_mclock();
	for (; itor != tail; itor = tb_slist_next(slist, itor)) tb_slist_replace(slist, itor, "R");
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_slist_replace(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	TB_ASSERT(tb_slist_size(slist) == n);
	TB_ASSERT(tb_slist_const_at(slist, tb_slist_head(slist))[0] == 'R');
	TB_ASSERT(tb_slist_const_at(slist, tb_slist_last(slist))[0] == 'R');

	// destroy
	tb_slist_destroy(slist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_slist_replace_head_test()
{
	// create
	tb_slist_t* slist = tb_slist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(slist, 0);

	tb_size_t i = 0;
	tb_size_t n = 1000000;
	tb_size_t index = tb_slist_ninsert_head(slist, "F", n);
	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_slist_replace_head(slist, "R");
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_slist_replace_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	TB_ASSERT(tb_slist_size(slist) == n);
	TB_ASSERT(tb_slist_const_at(slist, tb_slist_head(slist))[0] == 'R');

	// destroy
	tb_slist_destroy(slist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_slist_replace_last_test()
{
	// create
	tb_slist_t* slist = tb_slist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(slist, 0);

	tb_size_t i = 0;
	tb_size_t n = 1000000;
	tb_size_t index = tb_slist_ninsert_head(slist, "F", n);
	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_slist_replace_last(slist, "R");
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_slist_replace_last(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	TB_ASSERT(tb_slist_size(slist) == n);
	TB_ASSERT(tb_slist_const_at(slist, tb_slist_last(slist))[0] == 'R');


	// destroy
	tb_slist_destroy(slist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_slist_nreplace_test()
{
	// create
	tb_slist_t* slist = tb_slist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(slist, 0);

	tb_size_t n = 1000000;
	tb_size_t index = tb_slist_ninsert_head(slist, "F", n);
	tb_int64_t t = tb_mclock();
	tb_slist_nreplace(slist, tb_slist_head(slist), "R", n);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_slist_nreplace(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	TB_ASSERT(tb_slist_size(slist) == n);
	TB_ASSERT(tb_slist_const_at(slist, tb_slist_head(slist))[0] == 'R');
	TB_ASSERT(tb_slist_const_at(slist, tb_slist_last(slist))[0] == 'R');

	// destroy
	tb_slist_destroy(slist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_slist_nreplace_head_test()
{
	// create
	tb_slist_t* slist = tb_slist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(slist, 0);

	tb_size_t n = 1000000;
	tb_size_t index = tb_slist_ninsert_head(slist, "F", n);
	tb_int64_t t = tb_mclock();
	tb_slist_nreplace_head(slist, "R", n);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_slist_nreplace_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	TB_ASSERT(tb_slist_size(slist) == n);
	TB_ASSERT(tb_slist_const_at(slist, tb_slist_head(slist))[0] == 'R');
	TB_ASSERT(tb_slist_const_at(slist, tb_slist_last(slist))[0] == 'R');

	// destroy
	tb_slist_destroy(slist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_slist_nreplace_last_test()
{
	// create
	tb_slist_t* slist = tb_slist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(slist, 0);

	tb_size_t n = 1000000;
	tb_size_t index = tb_slist_ninsert_head(slist, "F", n);
	tb_int64_t t = tb_mclock();
	tb_slist_nreplace_last(slist, "R", n);
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_slist_nreplace_last(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// check
	TB_ASSERT(tb_slist_size(slist) == n);
	TB_ASSERT(tb_slist_const_at(slist, tb_slist_head(slist))[0] == 'R');
	TB_ASSERT(tb_slist_const_at(slist, tb_slist_last(slist))[0] == 'R');


	// destroy
	tb_slist_destroy(slist);

	return n / (tb_int64_to_int32(t) + 1);
}

static tb_size_t tb_slist_iterator_next_test()
{
	// create
	tb_slist_t* slist = tb_slist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(slist, 0);

	tb_size_t n = 1000000;
	tb_slist_ninsert_head(slist, "F", n);
	tb_size_t itor = tb_slist_head(slist);
	tb_size_t tail = tb_slist_tail(slist);
	tb_int64_t t = tb_mclock();
	for (; itor != tail; itor = tb_slist_next(slist, itor))
	{
		__tb_volatile__ tb_byte_t const* item = tb_slist_const_at(slist, itor);
	}
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_slist_iterator_next(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// destroy
	tb_slist_destroy(slist);

	return n / (tb_int64_to_int32(t) + 1);
}
static tb_size_t tb_slist_iterator_prev_test()
{
	// create
	tb_slist_t* slist = tb_slist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN_VAL(slist, 0);

	tb_size_t n = 10000;
	tb_slist_ninsert_head(slist, "F", n);
	tb_size_t itor = tb_slist_last(slist);
	tb_size_t head = tb_slist_head(slist);
	tb_int64_t t = tb_mclock();
	while (1)
	{
		__tb_volatile__ tb_byte_t const* item = tb_slist_const_at(slist, itor);

		if (itor == head) break;
		itor = tb_slist_prev(slist, itor);
	}
	t = tb_int64_sub(tb_mclock(), t);

	// time
	TB_DBG("tb_slist_iterator_prev(%d): %lld ms, size: %d, maxn: %d", n, t, tb_slist_size(slist), tb_slist_maxn(slist));

	// destroy
	tb_slist_destroy(slist);

	return n / (tb_int64_to_int32(t) + 1);
}
/* /////////////////////////////////////////////////////////
 * main
 */
int main(int argc, char** argv)
{
	if (!tb_init(malloc(30 * 1024 * 1024), 30 * 1024 * 1024)) return 0;

	// create slist
	tb_slist_t* slist = tb_slist_create(sizeof(tb_char_t), TB_SLIST_GROW_SIZE, tb_slist_char_ctor, tb_slist_char_dtor, "char");
	TB_ASSERT_GOTO(slist, end);

	tb_size_t 			i = 0;
	tb_size_t 			j = 0;

	TB_DBG("=============================================================");
	TB_DBG("insert:");
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

	TB_DBG("=============================================================");
	TB_DBG("remove:");
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

	TB_DBG("=============================================================");
	TB_DBG("replace:");
	tb_slist_nreplace_head(slist, "T", 10);
	tb_slist_nreplace_last(slist, "H", 10);
	tb_slist_replace_head(slist, "O");
	tb_slist_replace_last(slist, "O");
	tb_slist_char_dump(slist);

	TB_DBG("=============================================================");
	TB_DBG("clear:");
	tb_slist_clear(slist);
	tb_slist_char_dump(slist);

	tb_size_t score = 0;
	TB_DBG("=============================================================");
	TB_DBG("insert performance:");
	score += tb_slist_insert_test();
	score += tb_slist_insert_head_test();
	score += tb_slist_insert_tail_test();
	score += tb_slist_ninsert_test();
	score += tb_slist_ninsert_head_test();
	score += tb_slist_ninsert_tail_test();

	TB_DBG("=============================================================");
	TB_DBG("remove performance:");
	score += tb_slist_remove_test();
	score += tb_slist_remove_head_test();
	score += tb_slist_remove_last_test();
	score += tb_slist_nremove_test();
	score += tb_slist_nremove_head_test();
	score += tb_slist_nremove_last_test();

	TB_DBG("=============================================================");
	TB_DBG("replace performance:");
	score += tb_slist_replace_test();
	score += tb_slist_replace_head_test();
	score += tb_slist_replace_last_test();
	score += tb_slist_nreplace_test();
	score += tb_slist_nreplace_head_test();
	score += tb_slist_nreplace_last_test();

	TB_DBG("=============================================================");
	TB_DBG("iterator performance:");
	score += tb_slist_iterator_next_test();
	score += tb_slist_iterator_prev_test();

	TB_DBG("=============================================================");
	TB_DBG("score: %d", score / 100);

	getchar();
end:

	// destroy slist
	tb_slist_destroy(slist);

	return 0;
}
