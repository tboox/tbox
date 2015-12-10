/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * demo
 */ 
tb_void_t tb_demo_pool_leak(tb_noarg_t);
tb_void_t tb_demo_pool_leak()
{
    // done
    tb_pool_ref_t       pool        = tb_null;
    tb_large_pool_ref_t large_pool  = tb_null;
    do
    {
        // init pool
        pool = tb_pool_init(tb_null);
        tb_assert_and_check_break(pool);

        // init large pool
        large_pool = tb_large_pool_init(tb_null, 0);
        tb_assert_and_check_break(large_pool);

        // make data0
        tb_pointer_t data0 = tb_pool_malloc(pool, 10);
        tb_assert_and_check_break(data0);
    
        // make data1
        tb_pointer_t data1 = tb_pool_malloc(pool, 10);
        tb_assert_and_check_break(data1);
    
#ifdef __tb_debug__
        // dump pool
        tb_pool_dump(pool);
#endif

    } while (0);

    // exit pool
    if (pool) tb_pool_exit(pool);
    pool = tb_null;

    // exit large pool
    if (large_pool) tb_large_pool_exit(large_pool);
    large_pool = tb_null;
}
tb_void_t tb_demo_pool_free2(tb_noarg_t);
tb_void_t tb_demo_pool_free2()
{
    // done
    tb_pool_ref_t       pool        = tb_null;
    tb_large_pool_ref_t large_pool  = tb_null;
    do
    {
        // init pool
        pool = tb_pool_init(tb_null);
        tb_assert_and_check_break(pool);

        // init large pool
        large_pool = tb_large_pool_init(tb_null, 0);
        tb_assert_and_check_break(large_pool);

        // make data
        tb_pointer_t data = tb_pool_malloc(pool, 10);
        tb_assert_and_check_break(data);
    
        // exit data
        tb_pool_free(pool, data);
        tb_pool_free(pool, data);
     
#ifdef __tb_debug__
        // dump pool
        tb_pool_dump(pool);
#endif
    } while (0);

    // exit pool
    if (pool) tb_pool_exit(pool);
    pool = tb_null;

    // exit large pool
    if (large_pool) tb_large_pool_exit(large_pool);
    large_pool = tb_null;
}
tb_void_t tb_demo_pool_underflow(tb_noarg_t);
tb_void_t tb_demo_pool_underflow()
{
    // done
    tb_pool_ref_t       pool        = tb_null;
    tb_large_pool_ref_t large_pool  = tb_null;
    do
    {
        // init pool
        pool = tb_pool_init(tb_null);
        tb_assert_and_check_break(pool);

        // init large pool
        large_pool = tb_large_pool_init(tb_null, 0);
        tb_assert_and_check_break(large_pool);

        // make data
        tb_pointer_t data = tb_pool_malloc(pool, 10);
        tb_assert_and_check_break(data);
    
        // done underflow
        tb_memset(data, 0, 10 + 1);

        // exit data
        tb_pool_free(pool, data);
 
#ifdef __tb_debug__
        // dump pool
        tb_pool_dump(pool);
#endif
    } while (0);

    // exit pool
    if (pool) tb_pool_exit(pool);
    pool = tb_null;

    // exit large pool
    if (large_pool) tb_large_pool_exit(large_pool);
    large_pool = tb_null;
}
tb_void_t tb_demo_pool_underflow2(tb_noarg_t);
tb_void_t tb_demo_pool_underflow2()
{
    // done
    tb_pool_ref_t       pool        = tb_null;
    tb_large_pool_ref_t large_pool  = tb_null;
    do
    {
        // init pool
        pool = tb_pool_init(tb_null);
        tb_assert_and_check_break(pool);

        // init large pool
        large_pool = tb_large_pool_init(tb_null, 0);
        tb_assert_and_check_break(large_pool);

        // make data
        tb_pointer_t data = tb_pool_malloc(pool, 10);
        tb_assert_and_check_break(data);
    
        // done underflow
        tb_memset(data, 0, 10 + 1);

        // make data2
        data = tb_pool_malloc(pool, 10);
        tb_assert_and_check_break(data);
 
#ifdef __tb_debug__
        // dump pool
        tb_pool_dump(pool);
#endif
    } while (0);

    // exit pool
    if (pool) tb_pool_exit(pool);
    pool = tb_null;

    // exit large pool
    if (large_pool) tb_large_pool_exit(large_pool);
    large_pool = tb_null;
}
tb_void_t tb_demo_pool_perf(tb_noarg_t);
tb_void_t tb_demo_pool_perf()
{
    // done
    tb_pool_ref_t       pool        = tb_null;
    tb_large_pool_ref_t large_pool  = tb_null;
    do
    {
        // init pool
        pool = tb_pool_init(tb_null);
        tb_assert_and_check_break(pool);

        // init large pool
        large_pool = tb_large_pool_init(tb_null, 0);
        tb_assert_and_check_break(large_pool);

        // make data list
        tb_size_t       maxn = 10000;
        tb_pointer_t*   list = (tb_pointer_t*)tb_pool_nalloc0(pool, maxn, sizeof(tb_pointer_t));
        tb_assert_and_check_break(list);

        // done 
        __tb_volatile__ tb_size_t indx = 0;
        __tb_volatile__ tb_hong_t time = tb_mclock();
        __tb_volatile__ tb_size_t rand = 0xbeaf;
        for (indx = 0; indx < maxn; indx++)
        {
            // make data
            list[indx] = tb_pool_malloc0(pool, (rand & 65535) + 1);
            tb_assert_and_check_break(list[indx]);

            // make rand
            rand = (rand * 10807 + 1) & 0xffffffff;

            // re-make data
            if (!(indx & 31)) 
            {
                list[indx] = tb_pool_ralloc(pool, list[indx], (rand & 65535) + 1);
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
                    if (list[free_indx]) tb_pool_free(pool, list[free_indx]);
                    list[free_indx] = tb_null;
                }
            }
        }
        time = tb_mclock() - time;

        // exit list
        tb_pool_free(pool, list);

#ifdef __tb_debug__
        // dump pool
        tb_pool_dump(pool);
#endif

        // trace
        tb_trace_i("time: %lld ms", time);

    } while (0);

    // exit pool
    if (pool) tb_pool_exit(pool);
    pool = tb_null;

    // exit large pool
    if (large_pool) tb_large_pool_exit(large_pool);
    large_pool = tb_null;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_memory_pool_main(tb_int_t argc, tb_char_t** argv)
{
#if 1
    tb_demo_pool_perf();
#endif

#if 0
    tb_demo_pool_leak();
#endif

#if 0
    tb_demo_pool_free2();
#endif

#if 0
    tb_demo_pool_underflow();
#endif

#if 0
    tb_demo_pool_underflow2();
#endif

    return 0;
}
