/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */
static tb_size_t        g_func_indx = 0;
static tb_element_t     g_func_prev;

/* //////////////////////////////////////////////////////////////////////////////////////
 * test
 */
static tb_size_t tb_demo_test_hash_func(tb_element_ref_t element, tb_cpointer_t priv, tb_size_t mask, tb_size_t index)
{
    return g_func_prev.hash(element, priv, mask, g_func_indx);
}
static tb_void_t tb_demo_test_cstr_h(tb_size_t index)
{
    // the count
    tb_size_t count = 1000000;

    // save element
    g_func_indx = index;
    g_func_prev = tb_element_str(tb_true);

    // the element
    tb_element_t element = g_func_prev;
    element.hash = tb_demo_test_hash_func;

    // init filter
    tb_bloom_filter_ref_t filter = tb_bloom_filter_init(TB_BLOOM_FILTER_PROBABILITY_0_001, 1, count, element);
    if (filter)
    {
        // done
        tb_size_t i = 0;
        tb_size_t r = 0;
        tb_char_t s[256] = {0};
        tb_hong_t t = tb_mclock();
        for (i = 0; i < count; i++)
        {
            // the value
            tb_long_t value = tb_random();

            // format it
            tb_snprintf(s, sizeof(s) - 1, "%ld", value);

            // set value to filter
            if (!tb_bloom_filter_set(filter, s))
            {
                // repeat++
                r++;
            }
        }
        t = tb_mclock() - t;

        // trace
        tb_trace_i("cstr: index: %lu, repeat: %lu, time: %lld ms", index, r, t);

        // exit filter
        tb_bloom_filter_exit(filter);
    }
}
static tb_void_t tb_demo_test_cstr_p()
{
    // the count
    tb_size_t count = 10000000;

    // init filter
    tb_bloom_filter_ref_t filter = tb_bloom_filter_init(TB_BLOOM_FILTER_PROBABILITY_0_01, 3, count, tb_element_str(tb_true));
    if (filter)
    {
        // done
        tb_size_t i = 0;
        tb_size_t r = 0;
        tb_char_t s[256] = {0};
        for (i = 0; i < count; i++)
        {
            // the value
            tb_long_t value = tb_random();

            // format it
            tb_snprintf(s, sizeof(s) - 1, "%ld", value);

            // set value to filter
            if (!tb_bloom_filter_set(filter, s))
            {
                // repeat++
                r++;
            }
        }

        // trace
#ifdef TB_CONFIG_TYPE_HAVE_FLOAT
        tb_trace_i("cstr: count: %lu, repeat: %lu, repeat_p ~= p: %lf", count, r, (tb_double_t)r / count);
#else
        tb_trace_i("cstr: count: %lu, repeat: %lu", count, r);
#endif

        // exit filter
        tb_bloom_filter_exit(filter);
    }
}
static tb_void_t tb_demo_test_uint8_h(tb_size_t index)
{
    // the count
    tb_size_t count = TB_MAXU8;

    // save element
    g_func_indx = index;
    g_func_prev = tb_element_uint8();

    // the element
    tb_element_t element = g_func_prev;
    element.hash = tb_demo_test_hash_func;

    // init filter
    tb_bloom_filter_ref_t filter = tb_bloom_filter_init(TB_BLOOM_FILTER_PROBABILITY_0_001, 1, count, element);
    if (filter)
    {
        // done
        tb_size_t i = 0;
        tb_size_t r = 0;
        tb_hong_t t = tb_mclock();
        for (i = 0; i < count; i++)
        {
            // the value
            tb_long_t value = tb_random_range(0, TB_MAXU8);

            // set value to filter
            if (!tb_bloom_filter_set(filter, (tb_cpointer_t)value))
            {
                // repeat++
                r++;
            }
        }
        t = tb_mclock() - t;

        // trace
        tb_trace_i("uint8: index: %lu, repeat: %lu, time: %lld ms", index, r, t);

        // exit filter
        tb_bloom_filter_exit(filter);
    }
}
static tb_void_t tb_demo_test_uint16_h(tb_size_t index)
{
    // the count
    tb_size_t count = TB_MAXU16;

    // save element
    g_func_indx = index;
    g_func_prev = tb_element_uint16();

    // the element
    tb_element_t element = g_func_prev;
    element.hash = tb_demo_test_hash_func;

    // init filter
    tb_bloom_filter_ref_t filter = tb_bloom_filter_init(TB_BLOOM_FILTER_PROBABILITY_0_001, 1, count, element);
    if (filter)
    {
        // done
        tb_size_t i = 0;
        tb_size_t r = 0;
        tb_hong_t t = tb_mclock();
        for (i = 0; i < count; i++)
        {
            // the value
            tb_long_t value = tb_random_range(0, TB_MAXU16);

            // set value to filter
            if (!tb_bloom_filter_set(filter, (tb_cpointer_t)value))
            {
                // repeat++
                r++;
            }
        }
        t = tb_mclock() - t;

        // trace
        tb_trace_i("uint16: index: %lu, repeat: %lu, time: %lld ms", index, r, t);

        // exit filter
        tb_bloom_filter_exit(filter);
    }
}
static tb_void_t tb_demo_test_uint32_h(tb_size_t index)
{
    // the count
    tb_size_t count = 1000000;

    // save element
    g_func_indx = index;
    g_func_prev = tb_element_uint32();

    // the element
    tb_element_t element = g_func_prev;
    element.hash = tb_demo_test_hash_func;

    // init filter
    tb_bloom_filter_ref_t filter = tb_bloom_filter_init(TB_BLOOM_FILTER_PROBABILITY_0_001, 1, count, element);
    if (filter)
    {
        // done
        tb_size_t i = 0;
        tb_size_t r = 0;
        tb_hong_t t = tb_mclock();
        for (i = 0; i < count; i++)
        {
            // the value
            tb_long_t value = tb_random_value();

            // set value to filter
            if (!tb_bloom_filter_set(filter, (tb_cpointer_t)value))
            {
                // repeat++
                r++;
            }
        }
        t = tb_mclock() - t;

        // trace
        tb_trace_i("uint32: index: %lu, repeat: %lu, time: %lld ms", index, r, t);

        // exit filter
        tb_bloom_filter_exit(filter);
    }
}
static tb_void_t tb_demo_test_long_h(tb_size_t index)
{
    // the count
    tb_size_t count = 1000000;

    // save element
    g_func_indx = index;
    g_func_prev = tb_element_long();

    // the element
    tb_element_t element = g_func_prev;
    element.hash = tb_demo_test_hash_func;

    // init filter
    tb_bloom_filter_ref_t filter = tb_bloom_filter_init(TB_BLOOM_FILTER_PROBABILITY_0_001, 1, count, element);
    if (filter)
    {
        // done
        tb_size_t i = 0;
        tb_size_t r = 0;
        tb_hong_t t = tb_mclock();
        for (i = 0; i < count; i++)
        {
            // the value
            tb_long_t value = tb_random();

            // set value to filter
            if (!tb_bloom_filter_set(filter, (tb_cpointer_t)value))
            {
                // repeat++
                r++;
            }
        }
        t = tb_mclock() - t;

        // trace
        tb_trace_i("long: index: %lu, repeat: %lu, time: %lld ms", index, r, t);

        // exit filter
        tb_bloom_filter_exit(filter);
    }
}
static tb_void_t tb_demo_test_long_p()
{
    // the count
    tb_size_t count = 10000000;

    // init filter
    tb_bloom_filter_ref_t filter = tb_bloom_filter_init(TB_BLOOM_FILTER_PROBABILITY_0_01, 3, count, tb_element_long());
    if (filter)
    {
        // done
        tb_size_t i = 0;
        tb_size_t r = 0;
        for (i = 0; i < count; i++)
        {
            // the value
            tb_long_t value = tb_random();

            // set value to filter
            if (!tb_bloom_filter_set(filter, (tb_cpointer_t)value))
            {
                // repeat++
                r++;
            }
        }

        // trace
#ifdef TB_CONFIG_TYPE_HAVE_FLOAT
        tb_trace_i("long: count: %lu, repeat: %lu, repeat_p ~= p: %lf", count, r, (tb_double_t)r / count);
#else
        tb_trace_i("long: count: %lu, repeat: %lu", count, r);
#endif

        // exit filter
        tb_bloom_filter_exit(filter);
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_container_bloom_filter_main(tb_int_t argc, tb_char_t** argv)
{
    tb_trace_i("===========================================================");
    tb_demo_test_uint8_h(0);
    tb_demo_test_uint8_h(1);
    tb_demo_test_uint8_h(2);
    tb_demo_test_uint8_h(3);
    tb_demo_test_uint8_h(4);
    tb_demo_test_uint8_h(5);
    tb_demo_test_uint8_h(6);
    tb_demo_test_uint8_h(7);
    tb_demo_test_uint8_h(8);
    tb_demo_test_uint8_h(9);
    tb_demo_test_uint8_h(10);
    tb_demo_test_uint8_h(11);
    tb_demo_test_uint8_h(12);
    tb_demo_test_uint8_h(13);
    tb_demo_test_uint8_h(14);
    tb_demo_test_uint8_h(15);

    tb_trace_i("===========================================================");
    tb_demo_test_uint16_h(0);
    tb_demo_test_uint16_h(1);
    tb_demo_test_uint16_h(2);
    tb_demo_test_uint16_h(3);
    tb_demo_test_uint16_h(4);
    tb_demo_test_uint16_h(5);
    tb_demo_test_uint16_h(6);
    tb_demo_test_uint16_h(7);
    tb_demo_test_uint16_h(8);
    tb_demo_test_uint16_h(9);
    tb_demo_test_uint16_h(10);
    tb_demo_test_uint16_h(11);
    tb_demo_test_uint16_h(12);
    tb_demo_test_uint16_h(13);
    tb_demo_test_uint16_h(14);
    tb_demo_test_uint16_h(15);

    tb_trace_i("===========================================================");
    tb_demo_test_uint32_h(0);
    tb_demo_test_uint32_h(1);
    tb_demo_test_uint32_h(2);
    tb_demo_test_uint32_h(3);
    tb_demo_test_uint32_h(4);
    tb_demo_test_uint32_h(5);
    tb_demo_test_uint32_h(6);
    tb_demo_test_uint32_h(7);
    tb_demo_test_uint32_h(8);
    tb_demo_test_uint32_h(9);
    tb_demo_test_uint32_h(10);
    tb_demo_test_uint32_h(11);
    tb_demo_test_uint32_h(12);
    tb_demo_test_uint32_h(13);
    tb_demo_test_uint32_h(14);
    tb_demo_test_uint32_h(15);

    tb_trace_i("===========================================================");
    tb_demo_test_long_h(0);
    tb_demo_test_long_h(1);
    tb_demo_test_long_h(2);
    tb_demo_test_long_h(3);
    tb_demo_test_long_h(4);
    tb_demo_test_long_h(5);
    tb_demo_test_long_h(6);
    tb_demo_test_long_h(7);
    tb_demo_test_long_h(8);
    tb_demo_test_long_h(9);
    tb_demo_test_long_h(10);
    tb_demo_test_long_h(11);
    tb_demo_test_long_h(12);
    tb_demo_test_long_h(13);
    tb_demo_test_long_h(14);
    tb_demo_test_long_h(15);

    tb_trace_i("===========================================================");
    tb_demo_test_cstr_h(0);
    tb_demo_test_cstr_h(1);
    tb_demo_test_cstr_h(2);
    tb_demo_test_cstr_h(3);
    tb_demo_test_cstr_h(4);
    tb_demo_test_cstr_h(5);
    tb_demo_test_cstr_h(6);
    tb_demo_test_cstr_h(7);
    tb_demo_test_cstr_h(8);
    tb_demo_test_cstr_h(9);
    tb_demo_test_cstr_h(10);
    tb_demo_test_cstr_h(11);
    tb_demo_test_cstr_h(12);
//  tb_demo_test_cstr_h(13);
//  tb_demo_test_cstr_h(14);
//  tb_demo_test_cstr_h(15);

    tb_trace_i("===========================================================");
    tb_demo_test_long_p();
    tb_demo_test_cstr_p();

    return 0;
}
