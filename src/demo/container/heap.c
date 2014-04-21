/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * test
 */
static tb_long_t tb_test_heap_max_comp(tb_item_func_t* func, tb_cpointer_t ldata, tb_cpointer_t rdata)
{
	return ((tb_uint32_t)(tb_size_t)ldata < (tb_uint32_t)(tb_size_t)rdata? 1 : ((tb_uint32_t)(tb_size_t)ldata > (tb_uint32_t)(tb_size_t)rdata? -1 : 0));
}
static tb_void_t tb_test_heap_min_func()
{
	// init heap
	tb_heap_t* heap = tb_heap_init(16, tb_item_func_uint32());
	tb_assert_and_check_return(heap);

	// clear rand
	tb_rand_clear();

	// walk
	tb_size_t i = 0;
	for (i = 0; i < 100; i++) 
	{
		// the value
		tb_uint32_t val = tb_rand_uint32(0, TB_MAXU32);

		// trace
//		tb_trace_i("heap_min: put: %u", val);

		// put it
		tb_heap_put(heap, (tb_pointer_t)(tb_size_t)val);
	}

	// trace
	tb_trace_i("");

	// walk
	for (i = 0; i < 100; i++) 
	{
		// put it
		tb_uint32_t val = (tb_uint32_t)(tb_size_t)tb_heap_top(heap);

		// trace
		tb_trace_i("heap_min: pop: %u", val);

		// pop it
		tb_heap_pop(heap);
	}

	// exit heap
	tb_heap_exit(heap);
}
static tb_void_t tb_test_heap_min_perf()
{
	// init heap
	tb_heap_t* heap = tb_heap_init(4096, tb_item_func_uint32());
	tb_assert_and_check_return(heap);

	// clear rand
	tb_rand_clear();

	// init time
	tb_hong_t time = tb_mclock();

	// walk
	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 10000;
	for (i = 0; i < n; i++) tb_heap_put(heap, (tb_pointer_t)(tb_size_t)tb_rand_uint32(0, TB_MAXU32));
	for (i = 0; i < n; i++) tb_heap_pop(heap);

	// exit time
	time = tb_mclock() - time;

	// trace
	tb_trace_i("heap_min: %lld ms", time);

	// exit heap
	tb_heap_exit(heap);
}
static tb_void_t tb_test_heap_max_func()
{
	// init func
	tb_item_func_t func = tb_item_func_uint32(); func.comp = tb_test_heap_max_comp;

	// init heap
	tb_heap_t* heap = tb_heap_init(16, func);
	tb_assert_and_check_return(heap);

	// clear rand
	tb_rand_clear();

	// walk
	tb_size_t i = 0;
	for (i = 0; i < 100; i++) 
	{
		// the value
		tb_uint32_t val = tb_rand_uint32(0, TB_MAXU32);

		// trace
//		tb_trace_i("heap_max: put: %u", val);

		// put it
		tb_heap_put(heap, (tb_pointer_t)(tb_size_t)val);
	}

	// trace
	tb_trace_i("");

	// walk
	for (i = 0; i < 100; i++) 
	{
		// put it
		tb_uint32_t val = (tb_uint32_t)(tb_size_t)tb_heap_top(heap);

		// trace
		tb_trace_i("heap_max: pop: %u", val);

		// pop it
		tb_heap_pop(heap);
	}

	// exit heap
	tb_heap_exit(heap);
}
static tb_void_t tb_test_heap_max_perf()
{
	// init func
	tb_item_func_t func = tb_item_func_uint32(); func.comp = tb_test_heap_max_comp;

	// init heap
	tb_heap_t* heap = tb_heap_init(4096, func);
	tb_assert_and_check_return(heap);

	// clear rand
	tb_rand_clear();

	// init time
	tb_hong_t time = tb_mclock();

	// walk
	__tb_volatile__ tb_size_t i = 0;
	__tb_volatile__ tb_size_t n = 10000;
	for (i = 0; i < n; i++) tb_heap_put(heap, (tb_pointer_t)(tb_size_t)tb_rand_uint32(0, TB_MAXU32));
	for (i = 0; i < n; i++) tb_heap_pop(heap);

	// exit time
	time = tb_mclock() - time;

	// trace
	tb_trace_i("heap_max: %lld ms", time);

	// exit heap
	tb_heap_exit(heap);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_container_heap_main(tb_int_t argc, tb_char_t** argv)
{
	// func
	tb_test_heap_min_func();
	tb_test_heap_max_func();

	// perf
	tb_test_heap_min_perf();
	tb_test_heap_max_perf();

	return 0;
}
