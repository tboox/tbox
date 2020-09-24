/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../../demo.h"
#include "../../../tbox/memory/impl/static_fixed_pool.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * demo
 */
tb_void_t tb_demo_static_fixed_pool_leak(tb_noarg_t);
tb_void_t tb_demo_static_fixed_pool_leak()
{
    // done
    tb_byte_t                   buff[8192];
    tb_static_fixed_pool_ref_t  pool = tb_null;
    do
    {
        // init pool
        pool = tb_static_fixed_pool_init(buff, sizeof(buff), sizeof(tb_size_t), tb_false);
        tb_assert_and_check_break(pool);

        // make data0
        tb_pointer_t data0 = tb_static_fixed_pool_malloc(pool __tb_debug_vals__);
        tb_assert_and_check_break(data0);

        // make data1
        tb_pointer_t data1 = tb_static_fixed_pool_malloc(pool __tb_debug_vals__);
        tb_assert_and_check_break(data1);

#ifdef __tb_debug__
        // dump pool
        tb_static_fixed_pool_dump(pool);
#endif

    } while (0);

    // exit pool
    if (pool) tb_static_fixed_pool_exit(pool);
}
tb_void_t tb_demo_static_fixed_pool_free2(tb_noarg_t);
tb_void_t tb_demo_static_fixed_pool_free2()
{
    // done
    tb_byte_t                   buff[8192];
    tb_static_fixed_pool_ref_t  pool = tb_null;
    do
    {
        // init pool
        pool = tb_static_fixed_pool_init(buff, sizeof(buff), sizeof(tb_size_t), tb_false);
        tb_assert_and_check_break(pool);

        // make data
        tb_pointer_t data = tb_static_fixed_pool_malloc(pool __tb_debug_vals__);
        tb_assert_and_check_break(data);

        // exit data
        tb_static_fixed_pool_free(pool, data __tb_debug_vals__);
        tb_static_fixed_pool_free(pool, data __tb_debug_vals__);

#ifdef __tb_debug__
        // dump pool
        tb_static_fixed_pool_dump(pool);
#endif
    } while (0);

    // exit pool
    if (pool) tb_static_fixed_pool_exit(pool);
}
tb_void_t tb_demo_static_fixed_pool_underflow(tb_noarg_t);
tb_void_t tb_demo_static_fixed_pool_underflow()
{
    // done
    tb_byte_t                   buff[8192];
    tb_static_fixed_pool_ref_t  pool = tb_null;
    do
    {
        // init pool
        pool = tb_static_fixed_pool_init(buff, sizeof(buff), sizeof(tb_size_t), tb_false);
        tb_assert_and_check_break(pool);

        // make data
        tb_pointer_t data = tb_static_fixed_pool_malloc(pool __tb_debug_vals__);
        tb_assert_and_check_break(data);

        // done underflow
        tb_memset(data, 0, sizeof(tb_size_t) + 1);

        // exit data
        tb_static_fixed_pool_free(pool, data __tb_debug_vals__);

#ifdef __tb_debug__
        // dump pool
        tb_static_fixed_pool_dump(pool);
#endif
    } while (0);

    // exit pool
    if (pool) tb_static_fixed_pool_exit(pool);
}
tb_void_t tb_demo_static_fixed_pool_underflow2(tb_noarg_t);
tb_void_t tb_demo_static_fixed_pool_underflow2()
{
    // done
    tb_byte_t                   buff[8192];
    tb_static_fixed_pool_ref_t  pool = tb_null;
    do
    {
        // init pool
        pool = tb_static_fixed_pool_init(buff, sizeof(buff), sizeof(tb_size_t), tb_false);
        tb_assert_and_check_break(pool);

        // make data
        tb_pointer_t data = tb_static_fixed_pool_malloc(pool __tb_debug_vals__);
        tb_assert_and_check_break(data);

        // done underflow
        tb_memset(data, 0, sizeof(tb_size_t) + 1);

        // make data2
        data = tb_static_fixed_pool_malloc(pool __tb_debug_vals__);
        tb_assert_and_check_break(data);

#ifdef __tb_debug__
        // dump pool
        tb_static_fixed_pool_dump(pool);
#endif
    } while (0);

    // exit pool
    if (pool) tb_static_fixed_pool_exit(pool);
}
tb_void_t tb_demo_static_fixed_pool_perf(tb_size_t item_size);
tb_void_t tb_demo_static_fixed_pool_perf(tb_size_t item_size)
{
    // done
    tb_static_fixed_pool_ref_t pool = tb_null;
    do
    {
        // init pool
        pool = tb_static_fixed_pool_init((tb_byte_t*)malloc(50 * 1024 * 1024), 50 * 1024 * 1024, item_size, tb_false);
        tb_assert_and_check_break(pool);

        // make data list
        tb_size_t       maxn = 10000;
        tb_pointer_t*   list = (tb_pointer_t*)calloc(maxn, sizeof(tb_pointer_t));
        tb_assert_and_check_break(list);

        // done
        __tb_volatile__ tb_size_t indx = 0;
        __tb_volatile__ tb_hong_t time = tb_mclock();
        __tb_volatile__ tb_size_t rand = 0xbeaf;
        for (indx = 0; indx < maxn; indx++)
        {
            // make data
            list[indx] = tb_static_fixed_pool_malloc(pool __tb_debug_vals__);
            tb_assert_and_check_break(list[indx]);

            // make rand
            rand = (rand * 10807 + 1) & 0xffffffff;

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
                    if (list[free_indx]) tb_static_fixed_pool_free(pool, list[free_indx] __tb_debug_vals__);
                    list[free_indx] = tb_null;
                }
            }
        }
        time = tb_mclock() - time;

#ifdef __tb_debug__
        // dump pool
        tb_static_fixed_pool_dump(pool);
#endif

        // trace
        tb_trace_i("time: %lld ms", time);

        // clear pool
        tb_static_fixed_pool_clear(pool);

    } while (0);

    // exit pool
    if (pool) tb_static_fixed_pool_exit(pool);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_memory_impl_static_fixed_pool_main(tb_int_t argc, tb_char_t** argv)
{
#if 1
    tb_demo_static_fixed_pool_perf(16);
    tb_demo_static_fixed_pool_perf(32);
    tb_demo_static_fixed_pool_perf(64);
    tb_demo_static_fixed_pool_perf(96);
    tb_demo_static_fixed_pool_perf(128);
    tb_demo_static_fixed_pool_perf(192);
    tb_demo_static_fixed_pool_perf(256);
    tb_demo_static_fixed_pool_perf(384);
    tb_demo_static_fixed_pool_perf(512);
    tb_demo_static_fixed_pool_perf(1024);
    tb_demo_static_fixed_pool_perf(2048);
    tb_demo_static_fixed_pool_perf(3072);
#endif

#if 0
    tb_demo_static_fixed_pool_leak();
#endif

#if 0
    tb_demo_static_fixed_pool_free2();
#endif

#if 0
    tb_demo_static_fixed_pool_underflow();
#endif

#if 0
    tb_demo_static_fixed_pool_underflow2();
#endif

    return 0;
}
