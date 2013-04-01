/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * test
 */
static tb_void_t tb_sort_int_test(tb_size_t n)
{
	__tb_volatile__ tb_size_t i = 0;

	// init data
	tb_long_t* data = tb_nalloc0(n, sizeof(tb_long_t));
	tb_assert_and_check_return(data);
	
	// init iterator
	tb_iterator_t iterator = tb_iterator_int(data, n);

	// make
	tb_rand_clear();
	for (i = 0; i < n; i++) data[i] = tb_rand_sint32(TB_MINS16, TB_MAXS16);

	// sort
	tb_hong_t time = tb_mclock();
	tb_sort_all(&iterator);
	time = tb_mclock() - time;

	// time
	tb_print("tb_sort_int_all: %lld ms", time);

	// check
	for (i = 1; i < n; i++) tb_assert_and_check_break(data[i - 1] <= data[i]);

	// free
	tb_free(data);
}
static tb_void_t tb_sort_int_test_bubble(tb_size_t n)
{
	__tb_volatile__ tb_size_t i = 0;

	// init data
	tb_long_t* data = tb_nalloc0(n, sizeof(tb_long_t));
	tb_assert_and_check_return(data);
	
	// init iterator
	tb_iterator_t iterator = tb_iterator_int(data, n);

	// make
	tb_rand_clear();
	for (i = 0; i < n; i++) data[i] = tb_rand_sint32(TB_MINS16, TB_MAXS16);

	// sort
	tb_hong_t time = tb_mclock();
	tb_bubble_sort_all(&iterator);
	time = tb_mclock() - time;

	// time
	tb_print("tb_bubble_sort_int_all: %lld ms", time);

	// check
	for (i = 1; i < n; i++) tb_assert_and_check_break(data[i - 1] <= data[i]);

	// free
	tb_free(data);
}
static tb_void_t tb_sort_int_test_insert(tb_size_t n)
{
	__tb_volatile__ tb_size_t i = 0;

	// init data
	tb_long_t* data = tb_nalloc0(n, sizeof(tb_long_t));
	tb_assert_and_check_return(data);
	
	// init iterator
	tb_iterator_t iterator = tb_iterator_int(data, n);

	// make
	tb_rand_clear();
	for (i = 0; i < n; i++) data[i] = tb_rand_sint32(TB_MINS16, TB_MAXS16);

	// sort
	tb_hong_t time = tb_mclock();
	tb_insert_sort_all(&iterator);
	time = tb_mclock() - time;

	// time
	tb_print("tb_insert_sort_int_all: %lld ms", time);

	// check
	for (i = 1; i < n; i++) tb_assert_and_check_break(data[i - 1] <= data[i]);

	// free
	tb_free(data);
}
static tb_void_t tb_sort_int_test_quick(tb_size_t n)
{
	__tb_volatile__ tb_size_t i = 0;

	// init data
	tb_long_t* data = tb_nalloc0(n, sizeof(tb_long_t));
	tb_assert_and_check_return(data);
	
	// init iterator
	tb_iterator_t iterator = tb_iterator_int(data, n);

	// make
	tb_rand_clear();
	for (i = 0; i < n; i++) data[i] = tb_rand_sint32(TB_MINS16, TB_MAXS16);
	
	// sort
	tb_hong_t time = tb_mclock();
	tb_quick_sort_all(&iterator);
	time = tb_mclock() - time;

	// time
	tb_print("tb_quick_sort_int_all: %lld ms", time);

	// check
	for (i = 1; i < n; i++) tb_assert_and_check_break(data[i - 1] <= data[i]);

	// free
	tb_free(data);
}
static tb_void_t tb_sort_int_test_heap(tb_size_t n)
{
	__tb_volatile__ tb_size_t i = 0;

	// init data
	tb_long_t* data = tb_nalloc0(n, sizeof(tb_long_t));
	tb_assert_and_check_return(data);
	
	// init iterator
	tb_iterator_t iterator = tb_iterator_int(data, n);

	// make
	tb_rand_clear();
	for (i = 0; i < n; i++) data[i] = tb_rand_sint32(TB_MINS16, TB_MAXS16);
	
	// sort
	tb_hong_t time = tb_mclock();
	tb_heap_sort_all(&iterator);
	time = tb_mclock() - time;

	// time
	tb_print("tb_heap_sort_int_all: %lld ms", time);

	// check
	for (i = 1; i < n; i++) tb_assert_and_check_break(data[i - 1] <= data[i]);

	// free
	tb_free(data);
}
static tb_void_t tb_sort_str_test(tb_size_t n)
{
	__tb_volatile__ tb_size_t i = 0;

	// init data
	tb_char_t** data = tb_nalloc0(n, sizeof(tb_char_t*));
	tb_assert_and_check_return(data);

	// init pool
	tb_handle_t pool = tb_spool_init(TB_SPOOL_GROW_DEFAULT, 0);
	tb_assert_and_check_return(pool);
	
	// init iterator
	tb_iterator_t iterator = tb_iterator_str(data, n, tb_true);

	// make
	tb_rand_clear();
	tb_char_t s[256] = {0};
	for (i = 0; i < n; i++) 
	{
		tb_long_t r = tb_snprintf(s, 256, "%x", tb_rand_uint32(0, TB_MAXU32)); 
		s[r] == '\0'; 
		data[i] = tb_spool_strdup(pool, s);
	}

	// sort
	tb_hong_t time = tb_mclock();
	tb_sort_all(&iterator);
	time = tb_mclock() - time;

	// time
	tb_print("tb_sort_str_all: %lld ms", time);

	// check
	for (i = 1; i < n; i++) tb_assert_and_check_break(tb_strcmp(data[i - 1], data[i]) <= 0);

	// exit pool
	tb_spool_exit(pool);

	// free data
	tb_free(data);
}
static tb_void_t tb_sort_str_test_bubble(tb_size_t n)
{
	__tb_volatile__ tb_size_t i = 0;

	// init data
	tb_char_t** data = tb_nalloc0(n, sizeof(tb_char_t*));
	tb_assert_and_check_return(data);

	// init pool
	tb_handle_t pool = tb_spool_init(TB_SPOOL_GROW_DEFAULT, 0);
	tb_assert_and_check_return(pool);
	
	// init iterator
	tb_iterator_t iterator = tb_iterator_str(data, n, tb_true);

	// make
	tb_rand_clear();
	tb_char_t s[256] = {0};
	for (i = 0; i < n; i++) 
	{
		tb_long_t r = tb_snprintf(s, 256, "%x", tb_rand_uint32(0, TB_MAXU32)); 
		s[r] == '\0'; 
		data[i] = tb_spool_strdup(pool, s);
	}

	// sort
	tb_hong_t time = tb_mclock();
	tb_bubble_sort_all(&iterator);
	time = tb_mclock() - time;

	// time
	tb_print("tb_bubble_sort_str_all: %lld ms", time);

	// check
	for (i = 1; i < n; i++) tb_assert_and_check_break(tb_strcmp(data[i - 1], data[i]) <= 0);

	// exit pool
	tb_spool_exit(pool);

	// free data
	tb_free(data);
}
static tb_void_t tb_sort_str_test_insert(tb_size_t n)
{
	__tb_volatile__ tb_size_t i = 0;

	// init data
	tb_char_t** data = tb_nalloc0(n, sizeof(tb_char_t*));
	tb_assert_and_check_return(data);

	// init pool
	tb_handle_t pool = tb_spool_init(TB_SPOOL_GROW_DEFAULT, 0);
	tb_assert_and_check_return(pool);
	
	// init iterator
	tb_iterator_t iterator = tb_iterator_str(data, n, tb_true);

	// make
	tb_rand_clear();
	tb_char_t s[256] = {0};
	for (i = 0; i < n; i++) 
	{
		tb_long_t r = tb_snprintf(s, 256, "%x", tb_rand_uint32(0, TB_MAXU32)); 
		s[r] == '\0'; 
		data[i] = tb_spool_strdup(pool, s);
	}

	// sort
	tb_hong_t time = tb_mclock();
	tb_insert_sort_all(&iterator);
	time = tb_mclock() - time;

	// time
	tb_print("tb_insert_sort_str_all: %lld ms", time);

	// check
	for (i = 1; i < n; i++) tb_assert_and_check_break(tb_strcmp(data[i - 1], data[i]) <= 0);

	// exit pool
	tb_spool_exit(pool);

	// free data
	tb_free(data);
}
static tb_void_t tb_sort_str_test_quick(tb_size_t n)
{
	__tb_volatile__ tb_size_t i = 0;

	// init data
	tb_char_t** data = tb_nalloc0(n, sizeof(tb_char_t*));
	tb_assert_and_check_return(data);

	// init pool
	tb_handle_t pool = tb_spool_init(TB_SPOOL_GROW_DEFAULT, 0);
	tb_assert_and_check_return(pool);
	
	// init iterator
	tb_iterator_t iterator = tb_iterator_str(data, n, tb_true);

	// make
	tb_rand_clear();
	tb_char_t s[256] = {0};
	for (i = 0; i < n; i++) 
	{
		tb_long_t r = tb_snprintf(s, 256, "%x", tb_rand_uint32(0, TB_MAXU32)); 
		s[r] == '\0'; 
		data[i] = tb_spool_strdup(pool, s);
	}

	// sort
	tb_hong_t time = tb_mclock();
	tb_quick_sort_all(&iterator);
	time = tb_mclock() - time;

	// time
	tb_print("tb_quick_sort_str_all: %lld ms", time);

	// check
	for (i = 1; i < n; i++) tb_assert_and_check_break(tb_strcmp(data[i - 1], data[i]) <= 0);

	// exit pool
	tb_spool_exit(pool);

	// free data
	tb_free(data);
}
static tb_void_t tb_sort_str_test_heap(tb_size_t n)
{
	__tb_volatile__ tb_size_t i = 0;

	// init data
	tb_char_t** data = tb_nalloc0(n, sizeof(tb_char_t*));
	tb_assert_and_check_return(data);

	// init pool
	tb_handle_t pool = tb_spool_init(TB_SPOOL_GROW_DEFAULT, 0);
	tb_assert_and_check_return(pool);
	
	// init iterator
	tb_iterator_t iterator = tb_iterator_str(data, n, tb_true);

	// make
	tb_rand_clear();
	tb_char_t s[256] = {0};
	for (i = 0; i < n; i++) 
	{
		tb_long_t r = tb_snprintf(s, 256, "%x", tb_rand_uint32(0, TB_MAXU32)); 
		s[r] == '\0'; 
		data[i] = tb_spool_strdup(pool, s);
	}

	// sort
	tb_hong_t time = tb_mclock();
	tb_heap_sort_all(&iterator);
	time = tb_mclock() - time;

	// time
	tb_print("tb_heap_sort_str_all: %lld ms", time);

	// check
	for (i = 1; i < n; i++) tb_assert_and_check_break(tb_strcmp(data[i - 1], data[i]) <= 0);

	// exit pool
	tb_spool_exit(pool);

	// free data
	tb_free(data);
}
/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// init tbox
	if (!tb_init(malloc(30 * 1024 * 1024), 30 * 1024 * 1024)) return 0;

	// test
	tb_sort_int_test(30000);
	tb_sort_int_test_heap(30000);
	tb_sort_int_test_quick(30000);
	tb_sort_int_test_bubble(30000);
	tb_sort_int_test_insert(30000);
	tb_sort_str_test(30000);
	tb_sort_str_test_heap(30000);
	tb_sort_str_test_quick(30000);
	tb_sort_str_test_bubble(30000);
	tb_sort_str_test_insert(30000);

	// exit tbox
	tb_exit();
	return 0;
}
