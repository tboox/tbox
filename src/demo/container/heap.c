/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * test
 */

static tb_long_t tb_test_heap_max_comp(tb_element_ref_t element, tb_cpointer_t ldata, tb_cpointer_t rdata)
{
    return ((tb_p2u32(ldata) > tb_p2u32(rdata))? -1 : (tb_p2u32(ldata) < tb_p2u32(rdata)));
}
static tb_void_t tb_test_heap_min_func()
{
    // init heap
    tb_heap_ref_t heap = tb_heap_init(16, tb_element_uint32());
    tb_assert_and_check_return(heap);

    // reset rand
    tb_random_reset(tb_true);

    // make heap
    tb_size_t i = 0;
    for (i = 0; i < 100; i++)
    {
        // the value
        tb_uint32_t val = (tb_uint32_t)tb_random_range(0, 50);

        // trace
//      tb_trace_i("heap_min: put: %u", val);

        // put it
        tb_heap_put(heap, tb_u2p(val));
    }

    // reset rand
    tb_random_reset(tb_true);

    // remove some values
    for (i = 0; i < 100; i++)
    {
        // the value
        tb_uint32_t val = (tb_uint32_t)tb_random_range(0, 50);

        // remove it?
        if (!(i & 3))
        {
            tb_size_t itor = tb_find_all(heap, tb_u2p(val));
            if (itor != tb_iterator_tail(heap)) tb_heap_remove(heap, itor);
        }
    }

    // append heap
    for (i = 0; i < 30; i++)
    {
        // the value
        tb_uint32_t val = (tb_uint32_t)tb_random_range(0, 50);

        // put it
        tb_heap_put(heap, tb_u2p(val));
    }

    // trace
    tb_trace_i("");

    // dump heap
    while (tb_heap_size(heap))
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
    tb_heap_ref_t heap = tb_heap_init(4096, tb_element_uint32());
    tb_assert_and_check_return(heap);

    // reset rand
    tb_random_reset(tb_true);

    // init time
    tb_hong_t time = tb_mclock();

    // profile
    __tb_volatile__ tb_size_t i = 0;
    __tb_volatile__ tb_size_t n = 100000;
    __tb_volatile__ tb_size_t p; tb_used(&p);
    for (i = 0; i < n; i++) tb_heap_put(heap, (tb_pointer_t)(tb_size_t)tb_random_range(0, 50));
    for (i = 0; tb_heap_size(heap); i++)
    {
        // get the top value
        tb_size_t v = (tb_size_t)tb_heap_top(heap);

        // check order
        tb_assert(!i || p <= v);

        // save the previous value
        p = v;

        // pop it
        tb_heap_pop(heap);
    }

    // exit time
    time = tb_mclock() - time;

    // trace
    tb_trace_i("heap_min: %lld ms", time);

    // exit heap
    tb_heap_exit(heap);
}
static tb_void_t tb_test_heap_max_func()
{
    // init element
    tb_element_t element = tb_element_uint32(); element.comp = tb_test_heap_max_comp;

    // init heap
    tb_heap_ref_t heap = tb_heap_init(16, element);
    tb_assert_and_check_return(heap);

    // reset rand
    tb_random_reset(tb_true);

    // make heap
    tb_size_t i = 0;
    for (i = 0; i < 100; i++)
    {
        // the value
        tb_uint32_t val = (tb_uint32_t)tb_random_range(0, 50);

        // trace
//      tb_trace_i("heap_max: put: %u", val);

        // put it
        tb_heap_put(heap, tb_u2p(val));
    }

    // remove some values
    for (i = 0; i < 100; i++)
    {
        // the value
        tb_uint32_t val = (tb_uint32_t)tb_random_range(0, 50);

        // remove it?
        if (!(i & 3))
        {
            tb_size_t itor = tb_find_all(heap, tb_u2p(val));
            if (itor != tb_iterator_tail(heap)) tb_heap_remove(heap, itor);
        }
    }

    // append heap
    for (i = 0; i < 30; i++)
    {
        // the value
        tb_uint32_t val = (tb_uint32_t)tb_random_range(0, 50);

        // put it
        tb_heap_put(heap, tb_u2p(val));
    }

    // trace
    tb_trace_i("");

    // dump heap
    while (tb_heap_size(heap))
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
    // init element
    tb_element_t element = tb_element_uint32(); element.comp = tb_test_heap_max_comp;

    // init heap
    tb_heap_ref_t heap = tb_heap_init(4096, element);
    tb_assert_and_check_return(heap);

    // init time
    tb_hong_t time = tb_mclock();

    // profile
    __tb_volatile__ tb_size_t i = 0;
    __tb_volatile__ tb_size_t n = 100000;
    __tb_volatile__ tb_size_t p; tb_used(&p);
    for (i = 0; i < n; i++) tb_heap_put(heap, (tb_pointer_t)(tb_size_t)tb_random_range(0, 50));
    for (i = 0; tb_heap_size(heap); i++)
    {
        // get the top value
        tb_size_t v = (tb_size_t)tb_heap_top(heap);

        // check order
        tb_assert(!i || p >= v);

        // save the previous value
        p = v;

        // pop it
        tb_heap_pop(heap);
    }

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
    // element
    tb_test_heap_min_func();
    tb_test_heap_max_func();

    // performance
    tb_test_heap_min_perf();
    tb_test_heap_max_perf();
    return 0;
}
