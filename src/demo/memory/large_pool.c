/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */ 

// init pool
#if 1
#   define tb_demo_init_pool()      tb_large_pool_init((tb_byte_t*)malloc(500 * 1024 * 1024), 500 * 1024 * 1024)
#else
#   define tb_demo_init_pool()      tb_large_pool_init(tb_null, 0)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * demo
 */ 
tb_void_t tb_demo_large_pool_leak(tb_noarg_t);
tb_void_t tb_demo_large_pool_leak()
{
    // done
    tb_large_pool_ref_t pool = tb_null;
    do
    {
        // init pool
        pool = tb_demo_init_pool();
        tb_assert_and_check_break(pool);

        // make data0
        tb_pointer_t data0 = tb_large_pool_malloc(pool, 10, tb_null);
        tb_assert_and_check_break(data0);
    
        // make data1
        tb_pointer_t data1 = tb_large_pool_malloc(pool, 10, tb_null);
        tb_assert_and_check_break(data1);
    
#ifdef __tb_debug__
        // dump pool
        tb_large_pool_dump(pool);
#endif

    } while (0);

    // exit pool
    if (pool) tb_large_pool_exit(pool);
}
tb_void_t tb_demo_large_pool_free2(tb_noarg_t);
tb_void_t tb_demo_large_pool_free2()
{
    // done
    tb_large_pool_ref_t pool = tb_null;
    do
    {
        // init pool
        pool = tb_demo_init_pool();
        tb_assert_and_check_break(pool);

        // make data
        tb_pointer_t data = tb_large_pool_malloc(pool, 10, tb_null);
        tb_assert_and_check_break(data);
    
        // exit data
        tb_large_pool_free(pool, data);
        tb_large_pool_free(pool, data);
    
    } while (0);

    // exit pool
    if (pool) tb_large_pool_exit(pool);
}
tb_void_t tb_demo_large_pool_underflow(tb_noarg_t);
tb_void_t tb_demo_large_pool_underflow()
{
    // done
    tb_large_pool_ref_t pool = tb_null;
    do
    {
        // init pool
        pool = tb_demo_init_pool();
        tb_assert_and_check_break(pool);

        // make data
        tb_pointer_t data = tb_large_pool_malloc(pool, 10, tb_null);
        tb_assert_and_check_break(data);
    
        // done underflow
        tb_memset(data, 0, 10 + 1);

        // exit data
        tb_large_pool_free(pool, data);

    } while (0);

    // exit pool
    if (pool) tb_large_pool_exit(pool);
}
tb_void_t tb_demo_large_pool_underflow2(tb_noarg_t);
tb_void_t tb_demo_large_pool_underflow2()
{
    // done
    tb_large_pool_ref_t pool = tb_null;
    do
    {
        // init pool
        pool = tb_demo_init_pool();
        tb_assert_and_check_break(pool);

        // make data
        tb_pointer_t data = tb_large_pool_malloc(pool, 10, tb_null);
        tb_assert_and_check_break(data);
    
        // done underflow
        tb_memset(data, 0, 10 + 1);

        // make data2
        data = tb_large_pool_malloc(pool, 10, tb_null);
        tb_assert_and_check_break(data);

    } while (0);

    // exit pool
    if (pool) tb_large_pool_exit(pool);
}
tb_void_t tb_demo_large_pool_real(tb_size_t size);
tb_void_t tb_demo_large_pool_real(tb_size_t size)
{
    // done
    tb_large_pool_ref_t pool = tb_null;
    do
    {
        // init pool
        pool = tb_demo_init_pool();
        tb_assert_and_check_break(pool);

        // make data
        tb_size_t       real = 0;
        tb_pointer_t    data = tb_large_pool_malloc(pool, size, &real);
        tb_assert_and_check_break(data);
   
        // trace
        tb_trace_i("size: %lu => real: %lu", size, real);

    } while (0);

    // exit pool
    if (pool) tb_large_pool_exit(pool);
}
tb_void_t tb_demo_large_pool_perf(tb_noarg_t);
tb_void_t tb_demo_large_pool_perf()
{
    // done
    tb_large_pool_ref_t pool = tb_null;
    do
    {
        // init pool
        pool = tb_demo_init_pool();
        tb_assert_and_check_break(pool);

        // clear random
        tb_random_clear(tb_random_generator());

        // done 
        __tb_volatile__ tb_byte_t*  data = tb_null;
        __tb_volatile__ tb_size_t   maxn = 10000;
        __tb_volatile__ tb_size_t   size = 0;
        __tb_volatile__ tb_size_t   pagesize = tb_page_size();
        __tb_volatile__ tb_hong_t   time = tb_mclock();
        while (maxn--)
        {
            size = tb_random_range(tb_random_generator(), 0, pagesize << 4);
            data = (__tb_volatile__ tb_byte_t*)tb_large_pool_malloc0(pool, size, tb_null);
            if (!(maxn & 31)) 
            {
                size = tb_random_range(tb_random_generator(), 0, pagesize << 4);
                data = (__tb_volatile__ tb_byte_t*)tb_large_pool_ralloc(pool, (tb_pointer_t)data, size, tb_null);
            }
            if (!(maxn & 15)) tb_large_pool_free(pool, (tb_pointer_t)data);
            tb_check_break(data);
        }
        time = tb_mclock() - time;

#ifdef __tb_debug__
        // dump pool
        tb_large_pool_dump(pool);
#endif

        // trace
        tb_trace_i("time: %lld ms", time);

        // clear pool
        tb_large_pool_clear(pool);

    } while (0);

    // exit pool
    if (pool) tb_large_pool_exit(pool);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_memory_large_pool_main(tb_int_t argc, tb_char_t** argv)
{
#if 1
    tb_demo_large_pool_perf();
#endif

#if 0
    tb_demo_large_pool_leak();
#endif

#if 0
    tb_demo_large_pool_free2();
#endif

#if 0
    tb_demo_large_pool_underflow();
#endif

#if 0
    tb_demo_large_pool_underflow2();
#endif

#if 1
    tb_demo_large_pool_real(1 << 1);
    tb_demo_large_pool_real(1 << 2);
    tb_demo_large_pool_real(1 << 3);
    tb_demo_large_pool_real(1 << 4);
    tb_demo_large_pool_real(1 << 5);
    tb_demo_large_pool_real(1 << 6);
    tb_demo_large_pool_real(1 << 7);
    tb_demo_large_pool_real(1 << 8);
    tb_demo_large_pool_real(1 << 9);
    tb_demo_large_pool_real(1 << 10);
    tb_demo_large_pool_real(1 << 11);
    tb_demo_large_pool_real(1 << 12);
    tb_demo_large_pool_real(1 << 13);
    tb_demo_large_pool_real(1 << 14);
    tb_demo_large_pool_real(1 << 15);
    tb_demo_large_pool_real(1 << 16);
#endif
    return 0;
}
