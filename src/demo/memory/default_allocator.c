/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * demo
 */
tb_void_t tb_demo_default_allocator_leak(tb_noarg_t);
tb_void_t tb_demo_default_allocator_leak()
{
    // done
    tb_allocator_ref_t allocator = tb_null;
    do
    {
        // init allocator
        allocator = tb_default_allocator_init(tb_null);
        tb_assert_and_check_break(allocator);

        // make data0
        tb_pointer_t data0 = tb_allocator_malloc(allocator, 10);
        tb_assert_and_check_break(data0);

        // make data1
        tb_pointer_t data1 = tb_allocator_malloc(allocator, 10);
        tb_assert_and_check_break(data1);

#ifdef __tb_debug__
        // dump allocator
        tb_allocator_dump(allocator);
#endif

    } while (0);

    // exit allocator
    if (allocator) tb_allocator_exit(allocator);
    allocator = tb_null;
}
tb_void_t tb_demo_default_allocator_free2(tb_noarg_t);
tb_void_t tb_demo_default_allocator_free2()
{
    // done
    tb_allocator_ref_t allocator = tb_null;
    do
    {
        // init allocator
        allocator = tb_default_allocator_init(tb_null);
        tb_assert_and_check_break(allocator);

        // make data
        tb_pointer_t data = tb_allocator_malloc(allocator, 10);
        tb_assert_and_check_break(data);

        // exit data
        tb_allocator_free(allocator, data);
        tb_allocator_free(allocator, data);

#ifdef __tb_debug__
        // dump allocator
        tb_allocator_dump(allocator);
#endif
    } while (0);

    // exit allocator
    if (allocator) tb_allocator_exit(allocator);
    allocator = tb_null;
}
tb_void_t tb_demo_default_allocator_underflow(tb_noarg_t);
tb_void_t tb_demo_default_allocator_underflow()
{
    // done
    tb_allocator_ref_t allocator = tb_null;
    do
    {
        // init allocator
        allocator = tb_default_allocator_init(tb_null);
        tb_assert_and_check_break(allocator);

        // make data
        tb_pointer_t data = tb_allocator_malloc(allocator, 10);
        tb_assert_and_check_break(data);

        // done underflow
        tb_memset(data, 0, 10 + 1);

        // exit data
        tb_allocator_free(allocator, data);

#ifdef __tb_debug__
        // dump allocator
        tb_allocator_dump(allocator);
#endif
    } while (0);

    // exit allocator
    if (allocator) tb_allocator_exit(allocator);
    allocator = tb_null;
}
tb_void_t tb_demo_default_allocator_underflow2(tb_noarg_t);
tb_void_t tb_demo_default_allocator_underflow2()
{
    // done
    tb_allocator_ref_t allocator = tb_null;
    do
    {
        // init allocator
        allocator = tb_default_allocator_init(tb_null);
        tb_assert_and_check_break(allocator);

        // make data
        tb_pointer_t data = tb_allocator_malloc(allocator, 10);
        tb_assert_and_check_break(data);

        // done underflow
        tb_memset(data, 0, 10 + 1);

        // make data2
        data = tb_allocator_malloc(allocator, 10);
        tb_assert_and_check_break(data);

#ifdef __tb_debug__
        // dump allocator
        tb_allocator_dump(allocator);
#endif
    } while (0);

    // exit allocator
    if (allocator) tb_allocator_exit(allocator);
    allocator = tb_null;
}
tb_void_t tb_demo_default_allocator_perf(tb_noarg_t);
tb_void_t tb_demo_default_allocator_perf()
{
    // done
    tb_allocator_ref_t allocator = tb_null;
    tb_allocator_ref_t large_allocator = tb_null;
    do
    {
        // init large allocator
        large_allocator = tb_large_allocator_init(tb_null, 0);
        tb_assert_and_check_break(large_allocator);

        // init allocator
        allocator = tb_default_allocator_init(large_allocator);
        tb_assert_and_check_break(allocator);

        // make data list
        tb_size_t       maxn = 10000;
        tb_pointer_t*   list = (tb_pointer_t*)tb_allocator_nalloc0(allocator, maxn, sizeof(tb_pointer_t));
        tb_assert_and_check_break(list);

        // done
        __tb_volatile__ tb_size_t indx = 0;
        __tb_volatile__ tb_hong_t time = tb_mclock();
        __tb_volatile__ tb_size_t rand = 0xbeaf;
        for (indx = 0; indx < maxn; indx++)
        {
            // make data
            list[indx] = tb_allocator_malloc0(allocator, (rand & 65535) + 1);
            tb_assert_and_check_break(list[indx]);

            // make rand
            rand = (rand * 10807 + 1) & 0xffffffff;

            // re-make data
            if (!(indx & 31))
            {
                list[indx] = tb_allocator_ralloc(allocator, list[indx], (rand & 65535) + 1);
                tb_assert_and_check_break(list[indx]);
            }

            // free data
            __tb_volatile__ tb_size_t size = rand & 15;
            if (size > 5 && indx)
            {
                size -= 5;
                while (size--)
                {
                    // the free index
                    tb_size_t free_indx = rand % indx;

                    // free it
                    if (list[free_indx]) tb_allocator_free(allocator, list[free_indx]);
                    list[free_indx] = tb_null;
                }
            }
        }
        time = tb_mclock() - time;

        // exit list
        tb_allocator_free(allocator, list);

#ifdef __tb_debug__
        // dump allocator
        tb_allocator_dump(allocator);
#endif

        // trace
        tb_trace_i("time: %lld ms", time);

    } while (0);

    // exit allocator
    if (allocator) tb_allocator_exit(allocator);
    allocator = tb_null;

#ifdef __tb_debug__
    // dump large allocator
    if (large_allocator) tb_allocator_dump(large_allocator);
#endif

    // exit large allocator
    if (large_allocator) tb_allocator_exit(large_allocator);
    large_allocator = tb_null;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_memory_default_allocator_main(tb_int_t argc, tb_char_t** argv)
{
#if 1
    tb_demo_default_allocator_perf();
#endif

#if 0
    tb_demo_default_allocator_leak();
#endif

#if 0
    tb_demo_default_allocator_free2();
#endif

#if 0
    tb_demo_default_allocator_underflow();
#endif

#if 0
    tb_demo_default_allocator_underflow2();
#endif

    return 0;
}
