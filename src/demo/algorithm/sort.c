/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * test
 */
static tb_void_t tb_sort_int_test_perf(tb_size_t n)
{
    __tb_volatile__ tb_size_t i = 0;

    // init data
    tb_long_t* data = (tb_long_t*)tb_nalloc0(n, sizeof(tb_long_t));
    tb_assert_and_check_return(data);

    // init iterator
    tb_array_iterator_t array_iterator;
    tb_iterator_ref_t   iterator = tb_array_iterator_init_long(&array_iterator, data, n);

    // make
    for (i = 0; i < n; i++) data[i] = tb_random_range(TB_MINS16, TB_MAXS16);

    // sort
    tb_hong_t time = tb_mclock();
    tb_sort_all(iterator, tb_null);
    time = tb_mclock() - time;

    // time
    tb_trace_i("tb_sort_int_all: %lld ms", time);

    // check
    for (i = 1; i < n; i++) tb_assert_and_check_break(data[i - 1] <= data[i]);

    // free
    tb_free(data);
}
static tb_void_t tb_sort_int_test_perf_bubble(tb_size_t n)
{
    __tb_volatile__ tb_size_t i = 0;

    // init data
    tb_long_t* data = (tb_long_t*)tb_nalloc0(n, sizeof(tb_long_t));
    tb_assert_and_check_return(data);

    // init iterator
    tb_array_iterator_t array_iterator;
    tb_iterator_ref_t   iterator = tb_array_iterator_init_long(&array_iterator, data, n);

    // make
    for (i = 0; i < n; i++) data[i] = tb_random_range(TB_MINS16, TB_MAXS16);

    // sort
    tb_hong_t time = tb_mclock();
    tb_bubble_sort_all(iterator, tb_null);
    time = tb_mclock() - time;

    // time
    tb_trace_i("tb_bubble_sort_int_all: %lld ms", time);

    // check
    for (i = 1; i < n; i++) tb_assert_and_check_break(data[i - 1] <= data[i]);

    // free
    tb_free(data);
}
static tb_void_t tb_sort_int_test_func_bubble()
{
    // init
    __tb_volatile__ tb_size_t i = 0;
    __tb_volatile__ tb_size_t n = 20;

    // init data
    tb_long_t* data = (tb_long_t*)tb_nalloc0(n, sizeof(tb_long_t));
    tb_assert_and_check_return(data);

    // init iterator
    tb_array_iterator_t array_iterator;
    tb_iterator_ref_t   iterator = tb_array_iterator_init_long(&array_iterator, data, n);

    // trace
    tb_trace_i("");

    // put
    for (i = 0; i < n; i++)
    {
        data[i] = tb_random_range(TB_MINS16, TB_MAXS16);
        tb_trace_i("bubble_put: %ld", data[i]);
    }

    // sort
    tb_heap_sort_all(iterator, tb_null);

    // trace
    tb_trace_i("");

    // pop
    for (i = 0; i < n; i++) tb_trace_i("bubble_pop: %ld", data[i]);

    // free
    tb_free(data);
}
static tb_void_t tb_sort_int_test_perf_insert(tb_size_t n)
{
    __tb_volatile__ tb_size_t i = 0;

    // init data
    tb_long_t* data = (tb_long_t*)tb_nalloc0(n, sizeof(tb_long_t));
    tb_assert_and_check_return(data);

    // init iterator
    tb_array_iterator_t array_iterator;
    tb_iterator_ref_t   iterator = tb_array_iterator_init_long(&array_iterator, data, n);

    // make
    for (i = 0; i < n; i++) data[i] = tb_random_range(TB_MINS16, TB_MAXS16);

    // sort
    tb_hong_t time = tb_mclock();
    tb_insert_sort_all(iterator, tb_null);
    time = tb_mclock() - time;

    // time
    tb_trace_i("tb_insert_sort_int_all: %lld ms", time);

    // check
    for (i = 1; i < n; i++) tb_assert_and_check_break(data[i - 1] <= data[i]);

    // free
    tb_free(data);
}
static tb_void_t tb_sort_int_test_func_insert()
{
    // init
    __tb_volatile__ tb_size_t i = 0;
    __tb_volatile__ tb_size_t n = 20;

    // init data
    tb_long_t* data = (tb_long_t*)tb_nalloc0(n, sizeof(tb_long_t));
    tb_assert_and_check_return(data);

    // init iterator
    tb_array_iterator_t array_iterator;
    tb_iterator_ref_t   iterator = tb_array_iterator_init_long(&array_iterator, data, n);

    // trace
    tb_trace_i("");

    // put
    for (i = 0; i < n; i++)
    {
        data[i] = tb_random_range(TB_MINS16, TB_MAXS16);
        tb_trace_i("insert_put: %ld", data[i]);
    }

    // sort
    tb_heap_sort_all(iterator, tb_null);

    // trace
    tb_trace_i("");

    // pop
    for (i = 0; i < n; i++) tb_trace_i("insert_pop: %ld", data[i]);

    // free
    tb_free(data);
}
static tb_void_t tb_sort_int_test_perf_quick(tb_size_t n)
{
    __tb_volatile__ tb_size_t i = 0;

    // init data
    tb_long_t* data = (tb_long_t*)tb_nalloc0(n, sizeof(tb_long_t));
    tb_assert_and_check_return(data);

    // init iterator
    tb_array_iterator_t array_iterator;
    tb_iterator_ref_t   iterator = tb_array_iterator_init_long(&array_iterator, data, n);

    // make
    for (i = 0; i < n; i++) data[i] = tb_random_range(TB_MINS16, TB_MAXS16);

    // sort
    tb_hong_t time = tb_mclock();
    tb_quick_sort_all(iterator, tb_null);
    time = tb_mclock() - time;

    // time
    tb_trace_i("tb_quick_sort_int_all: %lld ms", time);

    // check
    for (i = 1; i < n; i++) tb_assert_and_check_break(data[i - 1] <= data[i]);

    // free
    tb_free(data);
}
static tb_void_t tb_sort_int_test_func_quick()
{
    // init
    __tb_volatile__ tb_size_t i = 0;
    __tb_volatile__ tb_size_t n = 20;

    // init data
    tb_long_t* data = (tb_long_t*)tb_nalloc0(n, sizeof(tb_long_t));
    tb_assert_and_check_return(data);

    // init iterator
    tb_array_iterator_t array_iterator;
    tb_iterator_ref_t   iterator = tb_array_iterator_init_long(&array_iterator, data, n);

    // trace
    tb_trace_i("");

    // put
    for (i = 0; i < n; i++)
    {
        data[i] = tb_random_range(TB_MINS16, TB_MAXS16);
        tb_trace_i("quick_put: %ld", data[i]);
    }

    // sort
    tb_heap_sort_all(iterator, tb_null);

    // trace
    tb_trace_i("");

    // pop
    for (i = 0; i < n; i++) tb_trace_i("quick_pop: %ld", data[i]);

    // free
    tb_free(data);
}
static tb_void_t tb_sort_int_test_perf_heap(tb_size_t n)
{
    __tb_volatile__ tb_size_t i = 0;

    // init data
    tb_long_t* data = (tb_long_t*)tb_nalloc0(n, sizeof(tb_long_t));
    tb_assert_and_check_return(data);

    // init iterator
    tb_array_iterator_t array_iterator;
    tb_iterator_ref_t   iterator = tb_array_iterator_init_long(&array_iterator, data, n);

    // make
    for (i = 0; i < n; i++) data[i] = tb_random_range(TB_MINS16, TB_MAXS16);

    // sort
    tb_hong_t time = tb_mclock();
    tb_heap_sort_all(iterator, tb_null);
    time = tb_mclock() - time;

    // time
    tb_trace_i("tb_heap_sort_int_all: %lld ms", time);

    // check
    for (i = 1; i < n; i++) tb_assert_and_check_break(data[i - 1] <= data[i]);

    // free
    tb_free(data);
}
static tb_void_t tb_sort_int_test_func_heap()
{
    // init
    __tb_volatile__ tb_size_t i = 0;
    __tb_volatile__ tb_size_t n = 20;

    // init data
    tb_long_t* data = (tb_long_t*)tb_nalloc0(n, sizeof(tb_long_t));
    tb_assert_and_check_return(data);

    // init iterator
    tb_array_iterator_t array_iterator;
    tb_iterator_ref_t   iterator = tb_array_iterator_init_long(&array_iterator, data, n);

    // trace
    tb_trace_i("");

    // put
    for (i = 0; i < n; i++)
    {
        data[i] = tb_random_range(TB_MINS16, TB_MAXS16);
        tb_trace_i("heap_put: %ld", data[i]);
    }

    // sort
    tb_heap_sort_all(iterator, tb_null);

    // trace
    tb_trace_i("");

    // pop
    for (i = 0; i < n; i++) tb_trace_i("heap_pop: %ld", data[i]);

    // free
    tb_free(data);
}
static tb_void_t tb_sort_str_test_perf(tb_size_t n)
{
    __tb_volatile__ tb_size_t i = 0;

    // init data
    tb_char_t** data = (tb_char_t**)tb_nalloc0(n, sizeof(tb_char_t*));
    tb_assert_and_check_return(data);

    // init iterator
    tb_array_iterator_t array_iterator;
    tb_iterator_ref_t   iterator = tb_array_iterator_init_str(&array_iterator, data, n);

    // make
    tb_char_t s[256] = {0};
    for (i = 0; i < n; i++)
    {
        tb_long_t r = tb_snprintf(s, 256, "%ld", tb_random_value());
        s[r] = '\0';
        data[i] = tb_strdup(s);
    }

    // sort
    tb_hong_t time = tb_mclock();
    tb_sort_all(iterator, tb_null);
    time = tb_mclock() - time;

    // time
    tb_trace_i("tb_sort_str_all: %lld ms", time);

    // check
    for (i = 1; i < n; i++) tb_assert_and_check_break(tb_strcmp(data[i - 1], data[i]) <= 0);

    // free data
    for (i = 0; i < n; i++) tb_free(data[i]);
    tb_free(data);
}
static tb_void_t tb_sort_str_test_perf_bubble(tb_size_t n)
{
    __tb_volatile__ tb_size_t i = 0;

    // init data
    tb_char_t** data = (tb_char_t**)tb_nalloc0(n, sizeof(tb_char_t*));
    tb_assert_and_check_return(data);

    // init iterator
    tb_array_iterator_t array_iterator;
    tb_iterator_ref_t   iterator = tb_array_iterator_init_str(&array_iterator, data, n);

    // make
    tb_char_t s[256] = {0};
    for (i = 0; i < n; i++)
    {
        tb_long_t r = tb_snprintf(s, 256, "%ld", tb_random_value());
        s[r] = '\0';
        data[i] = tb_strdup(s);
    }

    // sort
    tb_hong_t time = tb_mclock();
    tb_bubble_sort_all(iterator, tb_null);
    time = tb_mclock() - time;

    // time
    tb_trace_i("tb_bubble_sort_str_all: %lld ms", time);

    // check
    for (i = 1; i < n; i++) tb_assert_and_check_break(tb_strcmp(data[i - 1], data[i]) <= 0);

    // free data
    for (i = 0; i < n; i++) tb_free(data[i]);
    tb_free(data);
}
static tb_void_t tb_sort_str_test_perf_insert(tb_size_t n)
{
    __tb_volatile__ tb_size_t i = 0;

    // init data
    tb_char_t** data = (tb_char_t**)tb_nalloc0(n, sizeof(tb_char_t*));
    tb_assert_and_check_return(data);

    // init iterator
    tb_array_iterator_t array_iterator;
    tb_iterator_ref_t   iterator = tb_array_iterator_init_str(&array_iterator, data, n);

    // make
    tb_char_t s[256] = {0};
    for (i = 0; i < n; i++)
    {
        tb_long_t r = tb_snprintf(s, 256, "%ld", tb_random_value());
        s[r] = '\0';
        data[i] = tb_strdup(s);
    }

    // sort
    tb_hong_t time = tb_mclock();
    tb_insert_sort_all(iterator, tb_null);
    time = tb_mclock() - time;

    // time
    tb_trace_i("tb_insert_sort_str_all: %lld ms", time);

    // check
    for (i = 1; i < n; i++) tb_assert_and_check_break(tb_strcmp(data[i - 1], data[i]) <= 0);

    // free data
    for (i = 0; i < n; i++) tb_free(data[i]);
    tb_free(data);
}
static tb_void_t tb_sort_str_test_perf_quick(tb_size_t n)
{
    __tb_volatile__ tb_size_t i = 0;

    // init data
    tb_char_t** data = (tb_char_t**)tb_nalloc0(n, sizeof(tb_char_t*));
    tb_assert_and_check_return(data);

    // init iterator
    tb_array_iterator_t array_iterator;
    tb_iterator_ref_t   iterator = tb_array_iterator_init_str(&array_iterator, data, n);

    // make
    tb_char_t s[256] = {0};
    for (i = 0; i < n; i++)
    {
        tb_long_t r = tb_snprintf(s, 256, "%ld", tb_random_value());
        s[r] = '\0';
        data[i] = tb_strdup(s);
    }

    // sort
    tb_hong_t time = tb_mclock();
    tb_quick_sort_all(iterator, tb_null);
    time = tb_mclock() - time;

    // time
    tb_trace_i("tb_quick_sort_str_all: %lld ms", time);

    // check
    for (i = 1; i < n; i++) tb_assert_and_check_break(tb_strcmp(data[i - 1], data[i]) <= 0);

    // free data
    for (i = 0; i < n; i++) tb_free(data[i]);
    tb_free(data);
}
static tb_void_t tb_sort_str_test_perf_heap(tb_size_t n)
{
    __tb_volatile__ tb_size_t i = 0;

    // init data
    tb_char_t** data = (tb_char_t**)tb_nalloc0(n, sizeof(tb_char_t*));
    tb_assert_and_check_return(data);

    // init iterator
    tb_array_iterator_t array_iterator;
    tb_iterator_ref_t   iterator = tb_array_iterator_init_str(&array_iterator, data, n);

    // make
    tb_char_t s[256] = {0};
    for (i = 0; i < n; i++)
    {
        tb_long_t r = tb_snprintf(s, 256, "%ld", tb_random_value());
        s[r] = '\0';
        data[i] = tb_strdup(s);
    }

    // sort
    tb_hong_t time = tb_mclock();
    tb_heap_sort_all(iterator, tb_null);
    time = tb_mclock() - time;

    // time
    tb_trace_i("tb_heap_sort_str_all: %lld ms", time);

    // check
    for (i = 1; i < n; i++) tb_assert_and_check_break(tb_strcmp(data[i - 1], data[i]) <= 0);

    // free data
    for (i = 0; i < n; i++) tb_free(data[i]);
    tb_free(data);
}
/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_algorithm_sort_main(tb_int_t argc, tb_char_t** argv)
{
    // func
    tb_sort_int_test_func_heap();
    tb_sort_int_test_func_quick();
    tb_sort_int_test_func_bubble();
    tb_sort_int_test_func_insert();

    // perf
    tb_sort_int_test_perf(1000);
    tb_sort_int_test_perf_heap(1000);
    tb_sort_int_test_perf_quick(1000);
    tb_sort_int_test_perf_bubble(1000);
    tb_sort_int_test_perf_insert(1000);
    tb_sort_str_test_perf(1000);
    tb_sort_str_test_perf_heap(1000);
    tb_sort_str_test_perf_quick(1000);
    tb_sort_str_test_perf_bubble(1000);
    tb_sort_str_test_perf_insert(1000);

    return 0;
}
