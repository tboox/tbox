/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * test
 */
static tb_void_t tb_sort_int_test_bubble()
{
	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 30000;

	// init data
	tb_long_t* data = tb_nalloc0(n, sizeof(tb_long_t));
	tb_assert_and_check_return(data);
	
	// init iterator
	tb_iterator_t iterator = tb_iterator_int(data, n);

	// make
	for (i = 0; i < n; i++) data[i] = tb_rand_sint32(TB_MINS32, TB_MAXS32);

	// sort
	tb_size_t itor;
	tb_hong_t time = tb_mclock();
	tb_bubble_sort_all(&iterator);
	time = tb_mclock() - time;

	// time
	tb_print("tb_bubble_sort_all: %lld ms", time);

	// check
	for (i = 1; i < n; i++) tb_assert_and_check_break(data[i - 1] <= data[i]);

	// free
	tb_free(data);
}
static tb_void_t tb_sort_int_test_insert()
{
	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 30000;

	// init data
	tb_long_t* data = tb_nalloc0(n, sizeof(tb_long_t));
	tb_assert_and_check_return(data);
	
	// init iterator
	tb_iterator_t iterator = tb_iterator_int(data, n);

	// make
	for (i = 0; i < n; i++) data[i] = tb_rand_sint32(TB_MINS32, TB_MAXS32);

	// sort
	tb_size_t itor;
	tb_hong_t time = tb_mclock();
	tb_insert_sort_all(&iterator);
	time = tb_mclock() - time;

	// time
	tb_print("tb_insert_sort_all: %lld ms", time);

	// check
	for (i = 1; i < n; i++) tb_assert_and_check_break(data[i - 1] <= data[i]);

	// free
	tb_free(data);
}
static tb_void_t tb_sort_int_test_quick()
{
	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 30000;

	// init data
	tb_long_t* data = tb_nalloc0(n, sizeof(tb_long_t));
	tb_assert_and_check_return(data);
	
	// init iterator
	tb_iterator_t iterator = tb_iterator_int(data, n);

	// make
	for (i = 0; i < n; i++) data[i] = tb_rand_sint32(TB_MINS32, TB_MAXS32);

	// sort
	tb_size_t itor;
	tb_hong_t time = tb_mclock();
	tb_quick_sort_all(&iterator);
	time = tb_mclock() - time;

	// time
	tb_print("tb_quick_sort_all: %lld ms", time);

	// check
	for (i = 1; i < n; i++) tb_assert_and_check_break(data[i - 1] <= data[i]);

	// free
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
	tb_sort_int_test_bubble();
	tb_sort_int_test_insert();
	tb_sort_int_test_quick();


	// exit tbox
	tb_exit();
	return 0;
}
