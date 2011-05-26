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

static void tb_vector_char_ctor(void* data, void* priv)
{
	*((tb_char_t*)data) = '_';
	TB_DBG("[ctor]: %s, _", (tb_char_t const*)priv);
}
static void tb_vector_char_dtor(void* data, void* priv)
{
	TB_DBG("[dtor]: %s, %c", (tb_char_t const*)priv, *((tb_char_t*)data));
}
static void tb_vector_char_dump(tb_vector_t const* vector)
{
	TB_DBG("size: %d, maxn: %d", tb_vector_size(vector), tb_vector_maxn(vector));
	tb_size_t itor = tb_vector_head(vector);
	tb_size_t tail = tb_vector_tail(vector);
	for (; itor != tail; itor = tb_vector_next(vector, itor))
	{
		tb_byte_t const* item = tb_vector_const_at(vector, itor);
		if (item)
		{
			TB_DBG("at[%d]: %c", itor, *((tb_char_t const*)item));
		}
	}
}

static tb_size_t tb_vector_insert_test()
{
	// create
	tb_vector_t* vector = tb_vector_create(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN(vector);

	tb_size_t i = 0;
	tb_size_t n = 100000;
	tb_size_t t = (tb_size_t)tplat_clock();
	for (i = 0; i < n; i++) tb_vector_insert(vector, tb_vector_size(vector) >> 1, "F");
	t = (tb_size_t)tplat_clock() - t;

	// time
	TB_DBG("tb_vector_insert(%d): %d ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	TB_ASSERT(tb_vector_size(vector) == n);
	TB_ASSERT(tb_vector_const_at(vector, tb_vector_head(vector))[0] == 'F');
	TB_ASSERT(tb_vector_const_at(vector, tb_vector_last(vector))[0] == 'F');

	// clear it
	tb_vector_clear(vector);
	TB_ASSERT(!tb_vector_size(vector));

	// destroy
	tb_vector_destroy(vector);

	return n / (t + 1);
}
static tb_size_t tb_vector_insert_head_test()
{
	// create
	tb_vector_t* vector = tb_vector_create(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN(vector);

	tb_size_t i = 0;
	tb_size_t n = 100000;
	tb_size_t t = (tb_size_t)tplat_clock();
	for (i = 0; i < n; i++) tb_vector_insert_head(vector, "F");
	t = (tb_size_t)tplat_clock() - t;

	// time
	TB_DBG("tb_vector_insert_head(%d): %d ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	TB_ASSERT(tb_vector_size(vector) == n);
	TB_ASSERT(tb_vector_const_at(vector, tb_vector_head(vector))[0] == 'F');
	TB_ASSERT(tb_vector_const_at(vector, tb_vector_last(vector))[0] == 'F');

	// clear it
	tb_vector_clear(vector);
	TB_ASSERT(!tb_vector_size(vector));

	// destroy
	tb_vector_destroy(vector);
	
	return n / (t + 1);
}
static tb_size_t tb_vector_insert_tail_test()
{
	// create
	tb_vector_t* vector = tb_vector_create(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN(vector);

	tb_size_t i = 0;
	tb_size_t n = 1000000;
	tb_size_t t = (tb_size_t)tplat_clock();
	for (i = 0; i < n; i++) tb_vector_insert_tail(vector, "F");
	t = (tb_size_t)tplat_clock() - t;

	// time
	TB_DBG("tb_vector_insert_tail(%d): %d ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	TB_ASSERT(tb_vector_size(vector) == n);
	TB_ASSERT(tb_vector_const_at(vector, tb_vector_head(vector))[0] == 'F');
	TB_ASSERT(tb_vector_const_at(vector, tb_vector_last(vector))[0] == 'F');

	// clear it
	tb_vector_clear(vector);
	TB_ASSERT(!tb_vector_size(vector));

	// destroy
	tb_vector_destroy(vector);

	return n / (t + 1);
}
static tb_size_t tb_vector_ninsert_test()
{
	// create
	tb_vector_t* vector = tb_vector_create(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN(vector);

	// insert one first
	tb_vector_insert_head(vector, "F");

	tb_size_t n = 1000000;
	tb_size_t t = (tb_size_t)tplat_clock();
	tb_vector_ninsert(vector, 1, "F", n);
	t = (tb_size_t)tplat_clock() - t;

	// time
	TB_DBG("tb_vector_ninsert(%d): %d ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	TB_ASSERT(tb_vector_size(vector) == n + 1);
	TB_ASSERT(tb_vector_const_at(vector, tb_vector_head(vector))[0] == 'F');
	TB_ASSERT(tb_vector_const_at(vector, tb_vector_last(vector))[0] == 'F');

	// clear it
	tb_vector_clear(vector);
	TB_ASSERT(!tb_vector_size(vector));

	// destroy
	tb_vector_destroy(vector);

	return n / (t + 1);
}
static tb_size_t tb_vector_ninsert_head_test()
{
	// create
	tb_vector_t* vector = tb_vector_create(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN(vector);

	tb_size_t n = 1000000;
	tb_size_t t = (tb_size_t)tplat_clock();
	tb_vector_ninsert_head(vector, "F", n);
	t = (tb_size_t)tplat_clock() - t;

	// time
	TB_DBG("tb_vector_ninsert_head(%d): %d ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	TB_ASSERT(tb_vector_size(vector) == n);
	TB_ASSERT(tb_vector_const_at(vector, tb_vector_head(vector))[0] == 'F');
	TB_ASSERT(tb_vector_const_at(vector, tb_vector_last(vector))[0] == 'F');

	// clear it
	tb_vector_clear(vector);
	TB_ASSERT(!tb_vector_size(vector));

	// destroy
	tb_vector_destroy(vector);

	return n / (t + 1);
}
static tb_size_t tb_vector_ninsert_tail_test()
{
	// create
	tb_vector_t* vector = tb_vector_create(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN(vector);

	tb_size_t n = 1000000;
	tb_size_t t = (tb_size_t)tplat_clock();
	tb_vector_ninsert_tail(vector, "F", n);
	t = (tb_size_t)tplat_clock() - t;

	// time
	TB_DBG("tb_vector_ninsert_tail(%d): %d ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	TB_ASSERT(tb_vector_size(vector) == n);
	TB_ASSERT(tb_vector_const_at(vector, tb_vector_head(vector))[0] == 'F');
	TB_ASSERT(tb_vector_const_at(vector, tb_vector_last(vector))[0] == 'F');

	// clear it
	tb_vector_clear(vector);
	TB_ASSERT(!tb_vector_size(vector));

	// destroy
	tb_vector_destroy(vector);

	return n / (t + 1);
}

static tb_size_t tb_vector_remove_test()
{
	// create
	tb_vector_t* vector = tb_vector_create(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN(vector);

	tb_size_t i = 0;
	tb_size_t n = 100000;
	tb_vector_ninsert_head(vector, "F", n);
	tb_size_t t = (tb_size_t)tplat_clock();
	for (i = 0; i < n; i++) tb_vector_remove(vector, tb_vector_size(vector) >> 1);
	t = (tb_size_t)tplat_clock() - t;

	// time
	TB_DBG("tb_vector_remove(%d): %d ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	TB_ASSERT(!tb_vector_size(vector));

	// destroy
	tb_vector_destroy(vector);

	return n / (t + 1);
}
static tb_size_t tb_vector_remove_head_test()
{
	// create
	tb_vector_t* vector = tb_vector_create(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN(vector);

	tb_size_t i = 0;
	tb_size_t n = 100000;
	tb_vector_ninsert_head(vector, "F", n);
	tb_size_t t = (tb_size_t)tplat_clock();
	for (i = 0; i < n; i++) tb_vector_remove_head(vector);
	t = (tb_size_t)tplat_clock() - t;

	// time
	TB_DBG("tb_vector_remove_head(%d): %d ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	TB_ASSERT(!tb_vector_size(vector));

	// destroy
	tb_vector_destroy(vector);
}
static tb_size_t tb_vector_remove_last_test()
{
	// create
	tb_vector_t* vector = tb_vector_create(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN(vector);

	tb_size_t i = 0;
	tb_size_t n = 1000000;
	tb_vector_ninsert_head(vector, "F", n);
	tb_size_t t = (tb_size_t)tplat_clock();
	for (i = 0; i < n; i++) tb_vector_remove_last(vector);
	t = (tb_size_t)tplat_clock() - t;

	// time
	TB_DBG("tb_vector_remove_last(%d): %d ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	TB_ASSERT(!tb_vector_size(vector));

	// destroy
	tb_vector_destroy(vector);

	return n / (t + 1);
}
static tb_size_t tb_vector_nremove_test()
{
	// create
	tb_vector_t* vector = tb_vector_create(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN(vector);

	tb_size_t n = 1000000;
	tb_vector_ninsert_head(vector, "F", n);
	tb_size_t t = (tb_size_t)tplat_clock();
	tb_vector_nremove(vector, tb_vector_head(vector), n);
	t = (tb_size_t)tplat_clock() - t;

	// time
	TB_DBG("tb_vector_nremove(%d): %d ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	TB_ASSERT(!tb_vector_size(vector));

	// destroy
	tb_vector_destroy(vector);

	return n / (t + 1);
}
static tb_size_t tb_vector_nremove_head_test()
{
	// create
	tb_vector_t* vector = tb_vector_create(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN(vector);

	tb_size_t n = 1000000;
	tb_vector_ninsert_head(vector, "F", n);
	tb_size_t t = (tb_size_t)tplat_clock();
	tb_vector_nremove_head(vector, n);
	t = (tb_size_t)tplat_clock() - t;

	// time
	TB_DBG("tb_vector_nremove_head(%d): %d ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	TB_ASSERT(!tb_vector_size(vector));

	// destroy
	tb_vector_destroy(vector);

	return n / (t + 1);
}
static tb_size_t tb_vector_nremove_last_test()
{
	// create
	tb_vector_t* vector = tb_vector_create(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN(vector);

	tb_size_t n = 1000000;
	tb_vector_ninsert_head(vector, "F", n);
	tb_size_t t = (tb_size_t)tplat_clock();
	tb_vector_nremove_last(vector, n);
	t = (tb_size_t)tplat_clock() - t;

	// time
	TB_DBG("tb_vector_nremove_last(%d): %d ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	TB_ASSERT(!tb_vector_size(vector));

	// destroy
	tb_vector_destroy(vector);

	return n / (t + 1);
}
static tb_size_t tb_vector_replace_test()
{
	// create
	tb_vector_t* vector = tb_vector_create(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN(vector);

	tb_size_t n = 1000000;
	tb_vector_ninsert_head(vector, "F", n);
	tb_size_t itor = tb_vector_head(vector);
	tb_size_t tail = tb_vector_tail(vector);
	tb_size_t t = (tb_size_t)tplat_clock();
	for (; itor != tail; itor = tb_vector_next(vector, itor)) tb_vector_replace(vector, itor, "R");
	t = (tb_size_t)tplat_clock() - t;

	// time
	TB_DBG("tb_vector_replace(%d): %d ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	TB_ASSERT(tb_vector_size(vector) == n);
	TB_ASSERT(tb_vector_const_at(vector, tb_vector_head(vector))[0] == 'R');
	TB_ASSERT(tb_vector_const_at(vector, tb_vector_last(vector))[0] == 'R');

	// destroy
	tb_vector_destroy(vector);

	return n / (t + 1);
}
static tb_size_t tb_vector_replace_head_test()
{
	// create
	tb_vector_t* vector = tb_vector_create(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN(vector);

	tb_size_t i = 0;
	tb_size_t n = 1000000;
	tb_vector_ninsert_head(vector, "F", n);
	tb_size_t t = (tb_size_t)tplat_clock();
	for (i = 0; i < n; i++) tb_vector_replace_head(vector, "R");
	t = (tb_size_t)tplat_clock() - t;

	// time
	TB_DBG("tb_vector_replace_head(%d): %d ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	TB_ASSERT(tb_vector_size(vector) == n);
	TB_ASSERT(tb_vector_const_at(vector, tb_vector_head(vector))[0] == 'R');

	// destroy
	tb_vector_destroy(vector);

	return n / (t + 1);
}
static tb_size_t tb_vector_replace_last_test()
{
	// create
	tb_vector_t* vector = tb_vector_create(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN(vector);

	tb_size_t i = 0;
	tb_size_t n = 1000000;
	tb_vector_ninsert_head(vector, "F", n);
	tb_size_t t = (tb_size_t)tplat_clock();
	for (i = 0; i < n; i++) tb_vector_replace_last(vector, "R");
	t = (tb_size_t)tplat_clock() - t;

	// time
	TB_DBG("tb_vector_replace_last(%d): %d ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	TB_ASSERT(tb_vector_size(vector) == n);
	TB_ASSERT(tb_vector_const_at(vector, tb_vector_last(vector))[0] == 'R');


	// destroy
	tb_vector_destroy(vector);

	return n / (t + 1);
}
static tb_size_t tb_vector_nreplace_test()
{
	// create
	tb_vector_t* vector = tb_vector_create(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN(vector);

	tb_size_t n = 1000000;
	tb_vector_ninsert_head(vector, "F", n);
	tb_size_t t = (tb_size_t)tplat_clock();
	tb_vector_nreplace(vector, tb_vector_head(vector), "R", n);
	t = (tb_size_t)tplat_clock() - t;

	// time
	TB_DBG("tb_vector_nreplace(%d): %d ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	TB_ASSERT(tb_vector_size(vector) == n);
	TB_ASSERT(tb_vector_const_at(vector, tb_vector_head(vector))[0] == 'R');
	TB_ASSERT(tb_vector_const_at(vector, tb_vector_last(vector))[0] == 'R');

	// destroy
	tb_vector_destroy(vector);

	return n / (t + 1);
}
static tb_size_t tb_vector_nreplace_head_test()
{
	// create
	tb_vector_t* vector = tb_vector_create(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN(vector);

	tb_size_t n = 1000000;
	tb_vector_ninsert_head(vector, "F", n);
	tb_size_t t = (tb_size_t)tplat_clock();
	tb_vector_nreplace_head(vector, "R", n);
	t = (tb_size_t)tplat_clock() - t;

	// time
	TB_DBG("tb_vector_nreplace_head(%d): %d ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	TB_ASSERT(tb_vector_size(vector) == n);
	TB_ASSERT(tb_vector_const_at(vector, tb_vector_head(vector))[0] == 'R');
	TB_ASSERT(tb_vector_const_at(vector, tb_vector_last(vector))[0] == 'R');

	// destroy
	tb_vector_destroy(vector);

	return n / (t + 1);
}
static tb_size_t tb_vector_nreplace_last_test()
{
	// create
	tb_vector_t* vector = tb_vector_create(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN(vector);

	tb_size_t n = 1000000;
	tb_vector_ninsert_head(vector, "F", n);
	tb_size_t t = (tb_size_t)tplat_clock();
	tb_vector_nreplace_last(vector, "R", n);
	t = (tb_size_t)tplat_clock() - t;

	// time
	TB_DBG("tb_vector_nreplace_last(%d): %d ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	TB_ASSERT(tb_vector_size(vector) == n);
	TB_ASSERT(tb_vector_const_at(vector, tb_vector_head(vector))[0] == 'R');
	TB_ASSERT(tb_vector_const_at(vector, tb_vector_last(vector))[0] == 'R');


	// destroy
	tb_vector_destroy(vector);

	return n / (t + 1);
}

static tb_size_t tb_vector_iterator_next_test()
{
	// create
	tb_vector_t* vector = tb_vector_create(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN(vector);

	tb_size_t n = 1000000;
	tb_vector_ninsert_head(vector, "F", n);
	tb_size_t itor = tb_vector_head(vector);
	tb_size_t tail = tb_vector_tail(vector);
	tb_size_t t = (tb_size_t)tplat_clock();
	for (; itor != tail; itor = tb_vector_next(vector, itor))
	{
		__tplat_volatile__ tb_byte_t const* item = tb_vector_const_at(vector, itor);
	}
	t = (tb_size_t)tplat_clock() - t;

	// time
	TB_DBG("tb_vector_iterator_next(%d): %d ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// destroy
	tb_vector_destroy(vector);

	return n / (t + 1);
}
static tb_size_t tb_vector_iterator_prev_test()
{
	// create
	tb_vector_t* vector = tb_vector_create(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, TB_NULL, TB_NULL, TB_NULL);
	TB_ASSERT_RETURN(vector);

	tb_size_t n = 1000000;
	tb_vector_ninsert_head(vector, "F", n);
	tb_size_t itor = tb_vector_last(vector);
	tb_size_t head = tb_vector_head(vector);
	tb_size_t t = (tb_size_t)tplat_clock();
	while (1)
	{
		__tplat_volatile__ tb_byte_t const* item = tb_vector_const_at(vector, itor);

		if (itor == head) break;
		itor = tb_vector_prev(vector, itor);
	}
	t = (tb_size_t)tplat_clock() - t;

	// time
	TB_DBG("tb_vector_iterator_prev(%d): %d ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// destroy
	tb_vector_destroy(vector);

	return n / (t + 1);
}

/* /////////////////////////////////////////////////////////
 * main
 */
int main(int argc, char** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// create vector
	tb_vector_t* vector = tb_vector_create(sizeof(tb_char_t), TB_VECTOR_GROW_SIZE, tb_vector_char_ctor, tb_vector_char_dtor, "char");
	TB_ASSERT_GOTO(vector, end);

	TB_DBG("=============================================================");
	TB_DBG("insert:");
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

	TB_DBG("=============================================================");
	TB_DBG("remove:");
	tb_vector_nremove_head(vector, 5);
	tb_vector_nremove_last(vector, 5);
	tb_vector_char_dump(vector);

	TB_DBG("=============================================================");
	TB_DBG("replace:");
	tb_vector_nreplace_head(vector, "T", 10);
	tb_vector_nreplace_last(vector, "H", 10);
	tb_vector_replace_head(vector, "O");
	tb_vector_replace_last(vector, "O");
	tb_vector_char_dump(vector);

	TB_DBG("=============================================================");
	TB_DBG("clear:");
	tb_vector_clear(vector);
	tb_vector_char_dump(vector);

	tb_size_t score = 0;
	TB_DBG("=============================================================");
	TB_DBG("insert performance:");
	score += tb_vector_insert_test();
	score += tb_vector_insert_head_test();
	score += tb_vector_insert_tail_test();
	score += tb_vector_ninsert_test();
	score += tb_vector_ninsert_head_test();
	score += tb_vector_ninsert_tail_test();

	TB_DBG("=============================================================");
	TB_DBG("remove performance:");
	score += tb_vector_remove_test();
	score += tb_vector_remove_head_test();
	score += tb_vector_remove_last_test();
	score += tb_vector_nremove_test();
	score += tb_vector_nremove_head_test();
	score += tb_vector_nremove_last_test();

	TB_DBG("=============================================================");
	TB_DBG("replace performance:");
	score += tb_vector_replace_test();
	score += tb_vector_replace_head_test();
	score += tb_vector_replace_last_test();
	score += tb_vector_nreplace_test();
	score += tb_vector_nreplace_head_test();
	score += tb_vector_nreplace_last_test();

	TB_DBG("=============================================================");
	TB_DBG("iterator performance:");
	score += tb_vector_iterator_next_test();
	score += tb_vector_iterator_prev_test();

	TB_DBG("=============================================================");
	TB_DBG("score: %d", score / 100);

	getchar();
end:

	// destroy vector
	tb_vector_destroy(vector);

	return 0;
}
