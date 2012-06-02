/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

#define TB_VECTOR_GROW_SIZE 			(256)

/* ///////////////////////////////////////////////////////////////////////
 * details
 */

static tb_size_t tb_vector_insert_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_item_func_int());
	tb_assert_and_check_return(vector);

	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 100000;
	tb_hong_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_vector_insert(vector, tb_vector_size(vector) >> 1, 0xf);
	t = tb_mclock() - t;

	// time
	tb_print("tb_vector_insert(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	tb_assert(tb_vector_size(vector) == n);
	tb_assert(tb_vector_const_at_head(vector) == 0xf);
	tb_assert(tb_vector_const_at_last(vector) == 0xf);

	// clear it
	tb_vector_clear(vector);
	tb_assert(!tb_vector_size(vector));

	// exit
	tb_vector_exit(vector);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_vector_insert_head_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_item_func_int());
	tb_assert_and_check_return(vector);

	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 100000;
	tb_hong_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_vector_insert_head(vector, 0xf);
	t = tb_mclock() - t;

	// time
	tb_print("tb_vector_insert_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	tb_assert(tb_vector_size(vector) == n);
	tb_assert(tb_vector_const_at_head(vector) == 0xf);
	tb_assert(tb_vector_const_at_last(vector) == 0xf);

	// clear it
	tb_vector_clear(vector);
	tb_assert(!tb_vector_size(vector));

	// exit
	tb_vector_exit(vector);
	
	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_vector_insert_tail_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_item_func_int());
	tb_assert_and_check_return(vector);

	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 1000000;
	tb_hong_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_vector_insert_tail(vector, 0xf);
	t = tb_mclock() - t;

	// time
	tb_print("tb_vector_insert_tail(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	tb_assert(tb_vector_size(vector) == n);
	tb_assert(tb_vector_const_at_head(vector) == 0xf);
	tb_assert(tb_vector_const_at_last(vector) == 0xf);

	// clear it
	tb_vector_clear(vector);
	tb_assert(!tb_vector_size(vector));

	// exit
	tb_vector_exit(vector);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_vector_ninsert_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_item_func_int());
	tb_assert_and_check_return(vector);

	// insert one first
	tb_vector_insert_head(vector, 0xf);

	tb_size_t n = 1000000;
	tb_hong_t t = tb_mclock();
	tb_vector_ninsert(vector, 1, 0xf, n);
	t = tb_mclock() - t;

	// time
	tb_print("tb_vector_ninsert(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	tb_assert(tb_vector_size(vector) == n + 1);
	tb_assert(tb_vector_const_at_head(vector) == 0xf);
	tb_assert(tb_vector_const_at_last(vector) == 0xf);

	// clear it
	tb_vector_clear(vector);
	tb_assert(!tb_vector_size(vector));

	// exit
	tb_vector_exit(vector);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_vector_ninsert_head_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_item_func_int());
	tb_assert_and_check_return(vector);

	tb_size_t n = 1000000;
	tb_hong_t t = tb_mclock();
	tb_vector_ninsert_head(vector, 0xf, n);
	t = tb_mclock() - t;

	// time
	tb_print("tb_vector_ninsert_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	tb_assert(tb_vector_size(vector) == n);
	tb_assert(tb_vector_const_at_head(vector) == 0xf);
	tb_assert(tb_vector_const_at_last(vector) == 0xf);

	// clear it
	tb_vector_clear(vector);
	tb_assert(!tb_vector_size(vector));

	// exit
	tb_vector_exit(vector);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_vector_ninsert_tail_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_item_func_int());
	tb_assert_and_check_return(vector);

	tb_size_t n = 1000000;
	tb_hong_t t = tb_mclock();
	tb_vector_ninsert_tail(vector, 0xf, n);
	t = tb_mclock() - t;

	// time
	tb_print("tb_vector_ninsert_tail(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	tb_assert(tb_vector_size(vector) == n);
	tb_assert(tb_vector_const_at_head(vector) == 0xf);
	tb_assert(tb_vector_const_at_last(vector) == 0xf);

	// clear it
	tb_vector_clear(vector);
	tb_assert(!tb_vector_size(vector));

	// exit
	tb_vector_exit(vector);

	return n / ((tb_uint32_t)(t) + 1);
}

static tb_size_t tb_vector_remove_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_item_func_int());
	tb_assert_and_check_return(vector);

	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 100000;
	tb_vector_ninsert_head(vector, 0xf, n);
	tb_hong_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_vector_remove(vector, tb_vector_size(vector) >> 1);
	t = tb_mclock() - t;

	// time
	tb_print("tb_vector_remove(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	tb_assert(!tb_vector_size(vector));

	// exit
	tb_vector_exit(vector);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_vector_remove_head_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_item_func_int());
	tb_assert_and_check_return(vector);

	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 100000;
	tb_vector_ninsert_head(vector, 0xf, n);
	tb_hong_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_vector_remove_head(vector);
	t = tb_mclock() - t;

	// time
	tb_print("tb_vector_remove_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	tb_assert(!tb_vector_size(vector));

	// exit
	tb_vector_exit(vector);
}
static tb_size_t tb_vector_remove_last_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_item_func_int());
	tb_assert_and_check_return(vector);

	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 1000000;
	tb_vector_ninsert_head(vector, 0xf, n);
	tb_hong_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_vector_remove_last(vector);
	t = tb_mclock() - t;

	// time
	tb_print("tb_vector_remove_last(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	tb_assert(!tb_vector_size(vector));

	// exit
	tb_vector_exit(vector);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_vector_nremove_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_item_func_int());
	tb_assert_and_check_return(vector);

	tb_size_t n = 1000000;
	tb_vector_ninsert_head(vector, 0xf, n);
	tb_hong_t t = tb_mclock();
	tb_vector_nremove(vector, tb_vector_itor_head(vector), n);
	t = tb_mclock() - t;

	// time
	tb_print("tb_vector_nremove(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	tb_assert(!tb_vector_size(vector));

	// exit
	tb_vector_exit(vector);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_vector_nremove_head_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_item_func_int());
	tb_assert_and_check_return(vector);

	tb_size_t n = 1000000;
	tb_vector_ninsert_head(vector, 0xf, n);
	tb_hong_t t = tb_mclock();
	tb_vector_nremove_head(vector, n);
	t = tb_mclock() - t;

	// time
	tb_print("tb_vector_nremove_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	tb_assert(!tb_vector_size(vector));

	// exit
	tb_vector_exit(vector);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_vector_nremove_last_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_item_func_int());
	tb_assert_and_check_return(vector);

	tb_size_t n = 1000000;
	tb_vector_ninsert_head(vector, 0xf, n);
	tb_hong_t t = tb_mclock();
	tb_vector_nremove_last(vector, n);
	t = tb_mclock() - t;

	// time
	tb_print("tb_vector_nremove_last(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	tb_assert(!tb_vector_size(vector));

	// exit
	tb_vector_exit(vector);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_vector_replace_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_item_func_int());
	tb_assert_and_check_return(vector);

	tb_size_t n = 1000000;
	tb_vector_ninsert_head(vector, 0xf, n);
	__tb_volatile__ tb_size_t itor = tb_vector_itor_head(vector);
	__tb_volatile__ tb_size_t tail = tb_vector_itor_tail(vector);
	tb_hong_t t = tb_mclock();
	for (; itor != tail; itor = tb_vector_itor_next(vector, itor)) tb_vector_replace(vector, itor, 0xd);
	t = tb_mclock() - t;

	// time
	tb_print("tb_vector_replace(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	tb_assert(tb_vector_size(vector) == n);
	tb_assert(tb_vector_const_at_head(vector) == 0xd);
	tb_assert(tb_vector_const_at_last(vector) == 0xd);

	// exit
	tb_vector_exit(vector);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_vector_replace_head_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_item_func_int());
	tb_assert_and_check_return(vector);

	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 1000000;
	tb_vector_ninsert_head(vector, 0xf, n);
	tb_hong_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_vector_replace_head(vector, 0xd);
	t = tb_mclock() - t;

	// time
	tb_print("tb_vector_replace_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	tb_assert(tb_vector_size(vector) == n);
	tb_assert(tb_vector_const_at_head(vector) == 0xd);

	// exit
	tb_vector_exit(vector);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_vector_replace_last_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_item_func_int());
	tb_assert_and_check_return(vector);

	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 1000000;
	tb_vector_ninsert_head(vector, 0xf, n);
	tb_hong_t t = tb_mclock();
	for (i = 0; i < n; i++) tb_vector_replace_last(vector, 0xd);
	t = tb_mclock() - t;

	// time
	tb_print("tb_vector_replace_last(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	tb_assert(tb_vector_size(vector) == n);
	tb_assert(tb_vector_const_at_last(vector) == 0xd);


	// exit
	tb_vector_exit(vector);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_vector_nreplace_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_item_func_int());
	tb_assert_and_check_return(vector);

	tb_size_t n = 1000000;
	tb_vector_ninsert_head(vector, 0xf, n);
	tb_hong_t t = tb_mclock();
	tb_vector_nreplace(vector, tb_vector_itor_head(vector), 0xd, n);
	t = tb_mclock() - t;

	// time
	tb_print("tb_vector_nreplace(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	tb_assert(tb_vector_size(vector) == n);
	tb_assert(tb_vector_const_at_head(vector) == 0xd);
	tb_assert(tb_vector_const_at_last(vector) == 0xd);

	// exit
	tb_vector_exit(vector);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_vector_nreplace_head_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_item_func_int());
	tb_assert_and_check_return(vector);

	tb_size_t n = 1000000;
	tb_vector_ninsert_head(vector, 0xf, n);
	tb_hong_t t = tb_mclock();
	tb_vector_nreplace_head(vector, 0xd, n);
	t = tb_mclock() - t;

	// time
	tb_print("tb_vector_nreplace_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	tb_assert(tb_vector_size(vector) == n);
	tb_assert(tb_vector_const_at_head(vector) == 0xd);
	tb_assert(tb_vector_const_at_last(vector) == 0xd);

	// exit
	tb_vector_exit(vector);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_vector_nreplace_last_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_item_func_int());
	tb_assert_and_check_return(vector);

	tb_size_t n = 1000000;
	tb_vector_ninsert_head(vector, 0xf, n);
	tb_hong_t t = tb_mclock();
	tb_vector_nreplace_last(vector, 0xd, n);
	t = tb_mclock() - t;

	// time
	tb_print("tb_vector_nreplace_last(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// check
	tb_assert(tb_vector_size(vector) == n);
	tb_assert(tb_vector_const_at_head(vector) == 0xd);
	tb_assert(tb_vector_const_at_last(vector) == 0xd);


	// exit
	tb_vector_exit(vector);

	return n / ((tb_uint32_t)(t) + 1);
}

static tb_size_t tb_vector_iterator_next_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_item_func_int());
	tb_assert_and_check_return(vector);

	tb_size_t n = 1000000;
	tb_vector_ninsert_head(vector, 0xf, n);
	__tb_volatile__ tb_size_t itor = tb_vector_itor_head(vector);
	__tb_volatile__ tb_size_t tail = tb_vector_itor_tail(vector);
	tb_hong_t t = tb_mclock();
	for (; itor != tail; itor = tb_vector_itor_next(vector, itor))
	{
		__tb_volatile__ tb_byte_t const* item = tb_vector_itor_const_at(vector, itor);
	}
	t = tb_mclock() - t;

	// time
	tb_print("tb_vector_iterator_next(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// exit
	tb_vector_exit(vector);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_vector_iterator_prev_test()
{
	// init
	tb_vector_t* vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_item_func_int());
	tb_assert_and_check_return(vector);

	tb_size_t n = 1000000;
	tb_vector_ninsert_head(vector, 0xf, n);
	__tb_volatile__ tb_size_t itor = tb_vector_itor_last(vector);
	__tb_volatile__ tb_size_t head = tb_vector_itor_head(vector);
	tb_hong_t t = tb_mclock();
	while (1)
	{
		__tb_volatile__ tb_byte_t const* item = tb_vector_itor_const_at(vector, itor);

		if (itor == head) break;
		itor = tb_vector_itor_prev(vector, itor);
	}
	t = tb_mclock() - t;

	// time
	tb_print("tb_vector_iterator_prev(%d): %lld ms, size: %d, maxn: %d", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

	// exit
	tb_vector_exit(vector);

	return n / ((tb_uint32_t)(t) + 1);
}
static tb_void_t tb_vector_int_dump(tb_vector_t const* vector)
{
	tb_print("int size: %d, maxn: %d", tb_vector_size(vector), tb_vector_maxn(vector));
	tb_size_t itor = tb_vector_itor_head(vector);
	tb_size_t tail = tb_vector_itor_tail(vector);
	for (; itor != tail; itor = tb_vector_itor_next(vector, itor))
		tb_print("int at[%d]: %x", itor, tb_vector_itor_const_at(vector, itor));
}
static tb_void_t tb_vector_int_test()
{
	tb_vector_t* vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_item_func_int());
	tb_assert_and_check_return(vector);

	tb_print("=============================================================");
	tb_print("insert:");
	tb_vector_ninsert_head(vector, 0xa, 10); 
	tb_vector_ninsert_tail(vector, 0xf, 10);
	tb_vector_insert(vector, 10, 0);
	tb_vector_insert(vector, 10, 1);
	tb_vector_insert(vector, 10, 2);
	tb_vector_insert(vector, 10, 3);
	tb_vector_insert(vector, 10, 4);
	tb_vector_insert(vector, 10, 5);
	tb_vector_insert(vector, 10, 6);
	tb_vector_insert(vector, 10, 7);
	tb_vector_insert(vector, 10, 8);
	tb_vector_insert(vector, 10, 9);
	tb_vector_insert_head(vector, 4);
	tb_vector_insert_head(vector, 3);
	tb_vector_insert_head(vector, 2);
	tb_vector_insert_head(vector, 1);
	tb_vector_insert_head(vector, 0);
	tb_vector_insert_tail(vector, 5);
	tb_vector_insert_tail(vector, 6);
	tb_vector_insert_tail(vector, 7);
	tb_vector_insert_tail(vector, 8);
	tb_vector_insert_tail(vector, 9);
	tb_vector_int_dump(vector);

	tb_print("=============================================================");
	tb_print("remove:");
	tb_vector_nremove_head(vector, 5);
	tb_vector_nremove_last(vector, 5);
	tb_vector_int_dump(vector);

	tb_print("=============================================================");
	tb_print("replace:");
	tb_vector_nreplace_head(vector, 0xf, 10);
	tb_vector_nreplace_last(vector, 0xa, 10);
	tb_vector_replace_head(vector, 0);
	tb_vector_replace_last(vector, 0);
	tb_vector_int_dump(vector);

	tb_print("=============================================================");
	tb_print("clear:");
	tb_vector_clear(vector);
	tb_vector_int_dump(vector);

	tb_vector_exit(vector);
}
static tb_void_t tb_vector_str_dump(tb_vector_t const* vector)
{
	tb_print("str size: %d, maxn: %d", tb_vector_size(vector), tb_vector_maxn(vector));
	tb_size_t itor = tb_vector_itor_head(vector);
	tb_size_t tail = tb_vector_itor_tail(vector);
	for (; itor != tail; itor = tb_vector_itor_next(vector, itor))
		tb_print("str at[%d]: %s", itor, tb_vector_itor_const_at(vector, itor));
}
static tb_void_t tb_vector_str_test()
{
	tb_vector_t* vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_item_func_str(TB_TRUE, tb_spool_init(TB_SPOOL_GROW_SMALL, 0)));
	tb_assert_and_check_return(vector);

	tb_print("=============================================================");
	tb_print("insert:");
	tb_vector_ninsert_head(vector, "HHHHHHHHHH", 10); 
	tb_vector_ninsert_tail(vector, "TTTTTTTTTT", 10);
	tb_vector_insert(vector, 10, "0000000000");
	tb_vector_insert(vector, 10, "1111111111");
	tb_vector_insert(vector, 10, "2222222222");
	tb_vector_insert(vector, 10, "3333333333");
	tb_vector_insert(vector, 10, "4444444444");
	tb_vector_insert(vector, 10, "5555555555");
	tb_vector_insert(vector, 10, "6666666666");
	tb_vector_insert(vector, 10, "7777777777");
	tb_vector_insert(vector, 10, "8888888888");
	tb_vector_insert(vector, 10, "9999999999");
	tb_vector_insert_head(vector, "4444444444");
	tb_vector_insert_head(vector, "3333333333");
	tb_vector_insert_head(vector, "2222222222");
	tb_vector_insert_head(vector, "1111111111");
	tb_vector_insert_head(vector, "0000000000");
	tb_vector_insert_tail(vector, "5555555555");
	tb_vector_insert_tail(vector, "6666666666");
	tb_vector_insert_tail(vector, "7777777777");
	tb_vector_insert_tail(vector, "8888888888");
	tb_vector_insert_tail(vector, "9999999999");
	tb_vector_str_dump(vector);

	tb_print("=============================================================");
	tb_print("remove:");
	tb_vector_nremove_head(vector, 5);
	tb_vector_nremove_last(vector, 5);
	tb_vector_str_dump(vector);

	tb_print("=============================================================");
	tb_print("replace:");
	tb_vector_nreplace_head(vector, "TTTTTTTTTT", 10);
	tb_vector_nreplace_last(vector, "HHHHHHHHHH", 10);
	tb_vector_replace_head(vector, "OOOOOOOOOO");
	tb_vector_replace_last(vector, "OOOOOOOOOO");
	tb_vector_str_dump(vector);

	tb_print("=============================================================");
	tb_print("clear:");
	tb_vector_clear(vector);
	tb_vector_str_dump(vector);

	tb_vector_exit(vector);
}
static tb_void_t tb_vector_efm_dump(tb_vector_t const* vector)
{
	tb_print("efm size: %d, maxn: %d", tb_vector_size(vector), tb_vector_maxn(vector));
	tb_size_t itor = tb_vector_itor_head(vector);
	tb_size_t tail = tb_vector_itor_tail(vector);
	for (; itor != tail; itor = tb_vector_itor_next(vector, itor))
		tb_print("efm at[%d]: %s", itor, tb_vector_itor_const_at(vector, itor));
}
static tb_void_t tb_vector_efm_test()
{
	tb_vector_t* vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_item_func_efm(11, tb_rpool_init(256, 11, 0)));
	tb_assert_and_check_return(vector);

	tb_print("=============================================================");
	tb_print("insert:");
	tb_vector_ninsert_head(vector, "HHHHHHHHHH", 10); 
	tb_vector_ninsert_tail(vector, "TTTTTTTTTT", 10);
	tb_vector_insert(vector, 10, "0000000000");
	tb_vector_insert(vector, 10, "1111111111");
	tb_vector_insert(vector, 10, "2222222222");
	tb_vector_insert(vector, 10, "3333333333");
	tb_vector_insert(vector, 10, "4444444444");
	tb_vector_insert(vector, 10, "5555555555");
	tb_vector_insert(vector, 10, "6666666666");
	tb_vector_insert(vector, 10, "7777777777");
	tb_vector_insert(vector, 10, "8888888888");
	tb_vector_insert(vector, 10, "9999999999");
	tb_vector_insert_head(vector, "4444444444");
	tb_vector_insert_head(vector, "3333333333");
	tb_vector_insert_head(vector, "2222222222");
	tb_vector_insert_head(vector, "1111111111");
	tb_vector_insert_head(vector, "0000000000");
	tb_vector_insert_tail(vector, "5555555555");
	tb_vector_insert_tail(vector, "6666666666");
	tb_vector_insert_tail(vector, "7777777777");
	tb_vector_insert_tail(vector, "8888888888");
	tb_vector_insert_tail(vector, "9999999999");
	tb_vector_efm_dump(vector);

	tb_print("=============================================================");
	tb_print("remove:");
	tb_vector_nremove_head(vector, 5);
	tb_vector_nremove_last(vector, 5);
	tb_vector_efm_dump(vector);

	tb_print("=============================================================");
	tb_print("replace:");
	tb_vector_nreplace_head(vector, "TTTTTTTTTT", 10);
	tb_vector_nreplace_last(vector, "HHHHHHHHHH", 10);
	tb_vector_replace_head(vector, "OOOOOOOOOO");
	tb_vector_replace_last(vector, "OOOOOOOOOO");
	tb_vector_efm_dump(vector);

	tb_print("=============================================================");
	tb_print("clear:");
	tb_vector_clear(vector);
	tb_vector_efm_dump(vector);

	tb_vector_exit(vector);
}
static tb_void_t tb_vector_ifm_dump(tb_vector_t const* vector)
{
	tb_print("ifm size: %d, maxn: %d", tb_vector_size(vector), tb_vector_maxn(vector));
	tb_size_t itor = tb_vector_itor_head(vector);
	tb_size_t tail = tb_vector_itor_tail(vector);
	for (; itor != tail; itor = tb_vector_itor_next(vector, itor))
		tb_print("ifm at[%d]: %s", itor, tb_vector_itor_const_at(vector, itor));
}
static tb_void_t tb_vector_ifm_free(tb_item_func_t* func, tb_pointer_t item)
{
	tb_print("ifm free: %s, priv: %s", item, func->priv);
}
static tb_void_t tb_vector_ifm_test()
{
	tb_vector_t* vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_item_func_ifm(11, tb_vector_ifm_free, "ifm"));
	tb_assert_and_check_return(vector);

	tb_print("=============================================================");
	tb_print("insert:");
	tb_vector_ninsert_head(vector, "HHHHHHHHHH", 10); 
	tb_vector_ninsert_tail(vector, "TTTTTTTTTT", 10);
	tb_vector_insert(vector, 10, "0000000000");
	tb_vector_insert(vector, 10, "1111111111");
	tb_vector_insert(vector, 10, "2222222222");
	tb_vector_insert(vector, 10, "3333333333");
	tb_vector_insert(vector, 10, "4444444444");
	tb_vector_insert(vector, 10, "5555555555");
	tb_vector_insert(vector, 10, "6666666666");
	tb_vector_insert(vector, 10, "7777777777");
	tb_vector_insert(vector, 10, "8888888888");
	tb_vector_insert(vector, 10, "9999999999");
	tb_vector_insert_head(vector, "4444444444");
	tb_vector_insert_head(vector, "3333333333");
	tb_vector_insert_head(vector, "2222222222");
	tb_vector_insert_head(vector, "1111111111");
	tb_vector_insert_head(vector, "0000000000");
	tb_vector_insert_tail(vector, "5555555555");
	tb_vector_insert_tail(vector, "6666666666");
	tb_vector_insert_tail(vector, "7777777777");
	tb_vector_insert_tail(vector, "8888888888");
	tb_vector_insert_tail(vector, "9999999999");
	tb_vector_ifm_dump(vector);

	tb_print("=============================================================");
	tb_print("remove:");
	tb_vector_nremove_head(vector, 5);
	tb_vector_nremove_last(vector, 5);
	tb_vector_ifm_dump(vector);

	tb_print("=============================================================");
	tb_print("replace:");
	tb_vector_nreplace_head(vector, "TTTTTTTTTT", 10);
	tb_vector_nreplace_last(vector, "HHHHHHHHHH", 10);
	tb_vector_replace_head(vector, "OOOOOOOOOO");
	tb_vector_replace_last(vector, "OOOOOOOOOO");
	tb_vector_ifm_dump(vector);

	tb_print("=============================================================");
	tb_print("clear:");
	tb_vector_clear(vector);
	tb_vector_ifm_dump(vector);

	tb_vector_exit(vector);
}
static tb_void_t tb_vector_perf_test()
{
	tb_size_t score = 0;
	tb_print("=============================================================");
	tb_print("insert performance:");
	score += tb_vector_insert_test();
	score += tb_vector_insert_head_test();
	score += tb_vector_insert_tail_test();
	score += tb_vector_ninsert_test();
	score += tb_vector_ninsert_head_test();
	score += tb_vector_ninsert_tail_test();

	tb_print("=============================================================");
	tb_print("remove performance:");
	score += tb_vector_remove_test();
	score += tb_vector_remove_head_test();
	score += tb_vector_remove_last_test();
	score += tb_vector_nremove_test();
	score += tb_vector_nremove_head_test();
	score += tb_vector_nremove_last_test();

	tb_print("=============================================================");
	tb_print("replace performance:");
	score += tb_vector_replace_test();
	score += tb_vector_replace_head_test();
	score += tb_vector_replace_last_test();
	score += tb_vector_nreplace_test();
	score += tb_vector_nreplace_head_test();
	score += tb_vector_nreplace_last_test();

	tb_print("=============================================================");
	tb_print("iterator performance:");
	score += tb_vector_iterator_next_test();
	score += tb_vector_iterator_prev_test();

	tb_print("=============================================================");
	tb_print("score: %d", score / 100);

}
static tb_void_t tb_vector_test_itor_perf()
{
	// init vector
	tb_vector_t* 	vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_item_func_int());
	tb_assert_and_check_return(vector);

	// clear rand
	tb_rand_clear();

	// add items
	__tb_volatile__ tb_size_t n = 100000;
	while (n--) tb_vector_insert_tail(vector, tb_rand_uint32(0, TB_MAXU32)); 

	// performance
	tb_hong_t t = tb_mclock();
	__tb_volatile__ tb_hize_t test[2] = {0};
	__tb_volatile__ tb_size_t 	itor = tb_vector_itor_head(vector);
	for (; itor != tb_vector_itor_tail(vector); )
	{
		__tb_volatile__ tb_size_t item = tb_vector_itor_const_at(vector, itor);
		if (!(((tb_size_t)item >> 25) & 0x1))
		{
			// remove, hack: the itor of the same item is mutable
			tb_vector_remove(vector, itor);

			// continue 
			continue ;
		}
		else
		{
			test[0] += (tb_size_t)item;
			test[1]++;
		}

		itor = tb_vector_itor_next(vector, itor);
	}
	t = tb_mclock() - t;
	tb_print("item: %llx, size: %llu ?= %u, time: %lld", test[0], test[1], tb_vector_size(vector), t);

	tb_vector_exit(vector);
}
static tb_bool_t tb_vector_test_walk_item(tb_vector_t* vector, tb_pointer_t* item, tb_bool_t* bdel, tb_pointer_t data)
{
	tb_assert_and_check_return_val(vector && bdel && data, TB_FALSE);

	tb_hize_t* test = data;
	if (item)
	{
		tb_size_t i = (tb_size_t)*item;
		if (!((i >> 25) & 0x1))
//		if (!(i & 0x7))
//		if (1)
//		if (!(tb_rand_uint32(0, TB_MAXU32) & 0x1))
			*bdel = TB_TRUE;
		else
		{
			test[0] += i;
			test[1]++;
		}
	}

	// ok
	return TB_TRUE;
}
static tb_void_t tb_vector_test_walk_perf()
{
	// init vector
	tb_vector_t* 	vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_item_func_int());
	tb_assert_and_check_return(vector);

	// clear rand
	tb_rand_clear();

	// add items
	__tb_volatile__ tb_size_t n = 100000;
	while (n--) tb_vector_insert_tail(vector, tb_rand_uint32(0, TB_MAXU32)); 

	// performance
	tb_hong_t t = tb_mclock();
	__tb_volatile__ tb_hize_t test[2] = {0};
	tb_vector_walk(vector, tb_vector_test_walk_item, test);
	t = tb_mclock() - t;
	tb_print("item: %llx, size: %llu ?= %u, time: %lld", test[0], test[1], tb_vector_size(vector), t);

	tb_vector_exit(vector);
}
/* ///////////////////////////////////////////////////////////////////////
 * main
 */
int main(int argc, char** argv)
{
	if (!tb_init(malloc(30 * 1024 * 1024), 30 * 1024 * 1024)) return 0;

#if 0
	tb_vector_int_test();
	tb_vector_str_test();
	tb_vector_efm_test();
	tb_vector_ifm_test();
#endif

#if 0
	tb_vector_perf_test();
#endif

#if 1
	tb_vector_test_itor_perf();
	tb_vector_test_walk_perf();
#endif

	return 0;
}
