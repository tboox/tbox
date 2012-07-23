/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * test
 */
static tb_void_t tb_find_int_test()
{
	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 100000;

	// init data
	tb_long_t* data = tb_nalloc0(n, sizeof(tb_long_t));
	tb_assert_and_check_return(data);
	
	// init iterator
	tb_iterator_t iterator = tb_iterator_int(data, n);

	// make
	for (i = 0; i < n; i++) data[i] = i;

	// find
	tb_size_t itor;
	tb_hong_t time = tb_mclock();
	for (i = 0; i < n; i++) itor = tb_find_all(&iterator, &data[8000]);
	time = tb_mclock() - time;

	// item
	tb_long_t* item = itor != tb_iterator_tail(&iterator)? (tb_long_t*)tb_iterator_item(&iterator, itor) : TB_NULL;

	// time
	tb_print("tb_find_int_all[%ld ?= %ld]: %lld ms", item? *item : 0, data[8000], time);

	// free
	tb_free(data);
}
static tb_void_t tb_find_int_test_binary()
{
	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 100000;

	// init data
	tb_long_t* data = tb_nalloc0(n, sizeof(tb_long_t));
	tb_assert_and_check_return(data);
	
	// init iterator
	tb_iterator_t iterator = tb_iterator_int(data, n);

	// make
	for (i = 0; i < n; i++) data[i] = i;

	// find
	tb_size_t itor;
	tb_hong_t time = tb_mclock();
	for (i = 0; i < n; i++) itor = tb_binary_find_all(&iterator, &data[8000]);
	time = tb_mclock() - time;

	// item
	tb_long_t* item = itor != tb_iterator_tail(&iterator)? (tb_long_t*)tb_iterator_item(&iterator, itor) : TB_NULL;

	// time
	tb_print("tb_binary_find_int_all[%ld ?= %ld]: %lld ms", item? *item : 0, data[8000], time);

	// free
	tb_free(data);
}
static tb_void_t tb_find_str_test()
{
	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 10000;

	// init data
	tb_char_t** data = tb_nalloc0(n, sizeof(tb_char_t*));
	tb_assert_and_check_return(data);

	// init pool
	tb_handle_t pool = tb_spool_init(TB_SPOOL_GROW_DEFAULT, 0);
	tb_assert_and_check_return(pool);
	
	// init iterator
	tb_iterator_t iterator = tb_iterator_str(data, n, TB_TRUE);

	// make
	tb_char_t s[256] = {0};
	for (i = 0; i < n; i++) 
	{
		tb_long_t r = tb_snprintf(s, 256, "%04lu", i); 
		s[r] == '\0'; 
		data[i] = tb_spool_strdup(pool, s);
	}

	// find
	tb_size_t itor;
	tb_hong_t time = tb_mclock();
	for (i = 0; i < n; i++) itor = tb_find_all(&iterator, data[8000]);
	time = tb_mclock() - time;

	// item
	tb_char_t* item = itor != tb_iterator_tail(&iterator)? (tb_char_t*)tb_iterator_item(&iterator, itor) : TB_NULL;

	// time
	tb_print("tb_find_str_all[%s ?= %s]: %lld ms", item, data[8000], time);

	// exit pool
	tb_spool_exit(pool);

	// free data
	tb_free(data);
}
static tb_void_t tb_find_str_test_binary()
{
	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 10000;

	// init data
	tb_char_t** data = tb_nalloc0(n, sizeof(tb_char_t*));
	tb_assert_and_check_return(data);

	// init pool
	tb_handle_t pool = tb_spool_init(TB_SPOOL_GROW_DEFAULT, 0);
	tb_assert_and_check_return(pool);
	
	// init iterator
	tb_iterator_t iterator = tb_iterator_str(data, n, TB_TRUE);

	// make
	tb_char_t s[256] = {0};
	for (i = 0; i < n; i++) 
	{
		tb_long_t r = tb_snprintf(s, 256, "%04lu", i); 
		s[r] == '\0'; 
		data[i] = tb_spool_strdup(pool, s);
	}

	// find
	tb_size_t itor;
	tb_hong_t time = tb_mclock();
	for (i = 0; i < n; i++) itor = tb_binary_find_all(&iterator, data[8000]);
	time = tb_mclock() - time;

	// item
	tb_char_t* item = itor != tb_iterator_tail(&iterator)? (tb_char_t*)tb_iterator_item(&iterator, itor) : TB_NULL;

	// time
	tb_print("tb_binary_find_str_all[%s ?= %s]: %lld ms", item, data[8000], time);

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
//	tb_find_int_test();
//	tb_find_int_test_binary();
//	tb_find_str_test();
	tb_find_str_test_binary();


	// exit tbox
	tb_exit();
	return 0;
}
