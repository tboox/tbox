/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// init pool
#if 1
#   define tb_demo_init_pool()      tb_large_allocator_init((tb_byte_t*)malloc(500 * 1024 * 1024), 500 * 1024 * 1024)
#else
#   define tb_demo_init_pool()      tb_large_allocator_init(tb_null, 0)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * demo
 */
tb_void_t tb_demo_large_allocator_leak(tb_noarg_t);
tb_void_t tb_demo_large_allocator_leak()
{
    // done
    tb_allocator_ref_t pool = tb_null;
    do
    {
        // init pool
        pool = tb_demo_init_pool();
        tb_assert_and_check_break(pool);

        // make data0
        tb_pointer_t data0 = tb_allocator_large_malloc(pool, 10, tb_null);
        tb_assert_and_check_break(data0);

        // make data1
        tb_pointer_t data1 = tb_allocator_large_malloc(pool, 10, tb_null);
        tb_assert_and_check_break(data1);

#ifdef __tb_debug__
        // dump pool
        tb_allocator_dump(pool);
#endif

    } while (0);

    // exit pool
    if (pool) tb_allocator_exit(pool);
}
tb_void_t tb_demo_large_allocator_free2(tb_noarg_t);
tb_void_t tb_demo_large_allocator_free2()
{
    // done
    tb_allocator_ref_t pool = tb_null;
    do
    {
        // init pool
        pool = tb_demo_init_pool();
        tb_assert_and_check_break(pool);

        // make data
        tb_pointer_t data = tb_allocator_large_malloc(pool, 10, tb_null);
        tb_assert_and_check_break(data);

        // exit data
        tb_allocator_large_free(pool, data);
        tb_allocator_large_free(pool, data);

#ifdef __tb_debug__
        // dump pool
        tb_allocator_dump(pool);
#endif
    } while (0);

    // exit pool
    if (pool) tb_allocator_exit(pool);
}
tb_void_t tb_demo_large_allocator_underflow(tb_noarg_t);
tb_void_t tb_demo_large_allocator_underflow()
{
    // done
    tb_allocator_ref_t pool = tb_null;
    do
    {
        // init pool
        pool = tb_demo_init_pool();
        tb_assert_and_check_break(pool);

        // make data
        tb_pointer_t data = tb_allocator_large_malloc(pool, 10, tb_null);
        tb_assert_and_check_break(data);

        // done underflow
        tb_memset(data, 0, 10 + 1);

        // exit data
        tb_allocator_large_free(pool, data);

#ifdef __tb_debug__
        // dump pool
        tb_allocator_dump(pool);
#endif
    } while (0);

    // exit pool
    if (pool) tb_allocator_exit(pool);
}
tb_void_t tb_demo_large_allocator_underflow2(tb_noarg_t);
tb_void_t tb_demo_large_allocator_underflow2()
{
    // done
    tb_allocator_ref_t pool = tb_null;
    do
    {
        // init pool
        pool = tb_demo_init_pool();
        tb_assert_and_check_break(pool);

        // make data
        tb_pointer_t data = tb_allocator_large_malloc(pool, 10, tb_null);
        tb_assert_and_check_break(data);

        // done underflow
        tb_memset(data, 0, 10 + 1);

        // make data2
        data = tb_allocator_large_malloc(pool, 10, tb_null);
        tb_assert_and_check_break(data);

#ifdef __tb_debug__
        // dump pool
        tb_allocator_dump(pool);
#endif
    } while (0);

    // exit pool
    if (pool) tb_allocator_exit(pool);
}
tb_void_t tb_demo_large_allocator_real(tb_size_t size);
tb_void_t tb_demo_large_allocator_real(tb_size_t size)
{
    // done
    tb_allocator_ref_t pool = tb_null;
    do
    {
        // init pool
        pool = tb_demo_init_pool();
        tb_assert_and_check_break(pool);

        // make data
        tb_size_t       real = 0;
        tb_pointer_t    data = tb_allocator_large_malloc(pool, size, &real);
        tb_assert_and_check_break(data);

        // trace
        tb_trace_i("size: %lu => real: %lu", size, real);

    } while (0);

    // exit pool
    if (pool) tb_allocator_exit(pool);
}
tb_void_t tb_demo_large_allocator_perf(tb_noarg_t);
tb_void_t tb_demo_large_allocator_perf()
{
    // done
    tb_allocator_ref_t pool = tb_null;
    do
    {
        // init pool
        pool = tb_demo_init_pool();
        tb_assert_and_check_break(pool);

        // make data list
        tb_size_t       maxn = 10000;
        tb_pointer_t*   list = (tb_pointer_t*)tb_allocator_large_nalloc0(pool, maxn, sizeof(tb_pointer_t), tb_null);
        tb_assert_and_check_break(list);

        // done
        __tb_volatile__ tb_size_t indx = 0;
        __tb_volatile__ tb_size_t pagesize = tb_page_size();
        __tb_volatile__ tb_hong_t time = tb_mclock();
        for (indx = 0; indx < maxn; indx++)
        {
            // make data
            list[indx] = tb_allocator_large_malloc0(pool, tb_random_range(1, pagesize << 4), tb_null);
            tb_assert_and_check_break(list[indx]);

            // re-make data
            if (!(indx & 31))
            {
                list[indx] = tb_allocator_large_ralloc(pool, list[indx], tb_random_range(1, pagesize << 4), tb_null);
                tb_assert_and_check_break(list[indx]);
            }

            // free data
            __tb_volatile__ tb_size_t size = tb_random_range(0, 10);
            if (size > 5 && indx)
            {
                size -= 5;
                while (size--)
                {
                    // the free index
                    tb_size_t free_indx = tb_random_range(0, indx);

                    // free it
                    if (list[free_indx]) tb_allocator_large_free(pool, list[free_indx]);
                    list[free_indx] = tb_null;
                }
            }
        }
        time = tb_mclock() - time;

#ifdef __tb_debug__
        // dump pool
        tb_allocator_dump(pool);
#endif

        // trace
        tb_trace_i("time: %lld ms", time);

        // clear pool
        tb_allocator_clear(pool);

    } while (0);

    // exit pool
    if (pool) tb_allocator_exit(pool);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_memory_large_allocator_main(tb_int_t argc, tb_char_t** argv)
{
#if 1
    tb_demo_large_allocator_perf();
#endif

#if 0
    tb_demo_large_allocator_leak();
#endif

#if 0
    tb_demo_large_allocator_free2();
#endif

#if 0
    tb_demo_large_allocator_underflow();
#endif

#if 0
    tb_demo_large_allocator_underflow2();
#endif

#if 1
    tb_demo_large_allocator_real(16 * 256);
    tb_demo_large_allocator_real(32 * 256);
    tb_demo_large_allocator_real(64 * 256);
    tb_demo_large_allocator_real(96 * 256);
    tb_demo_large_allocator_real(128 * 256);
    tb_demo_large_allocator_real(192 * 256);
    tb_demo_large_allocator_real(256 * 256);
    tb_demo_large_allocator_real(384 * 256);
    tb_demo_large_allocator_real(512 * 256);
    tb_demo_large_allocator_real(1024 * 256);
    tb_demo_large_allocator_real(2048 * 256);
    tb_demo_large_allocator_real(3072 * 256);
#endif

    return 0;
}
