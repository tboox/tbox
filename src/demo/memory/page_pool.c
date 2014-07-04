/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_memory_page_pool_main(tb_int_t argc, tb_char_t** argv)
{
    // init
    tb_page_pool_ref_t pool = tb_page_pool_init((tb_byte_t*)malloc(50 * 1024 * 1024), 50 * 1024 * 1024);
    tb_assert_and_check_return_val(pool, 0);

#if 0
    __tb_volatile__ tb_hong_t   time = tb_mclock();
    __tb_volatile__ tb_byte_t*  data = tb_null;
    __tb_volatile__ tb_size_t   maxn = 100000;
    while (maxn--)
    {
        data = tb_page_pool_malloc(pool, 64);
        tb_check_break(data);
    }
    time = tb_mclock() - time;
#elif 1
    __tb_volatile__ tb_hong_t   time = tb_mclock();
    __tb_volatile__ tb_byte_t*  data = tb_null;
    __tb_volatile__ tb_size_t   maxn = 100000;
    __tb_volatile__ tb_size_t   size = 0xdeaf;
    __tb_volatile__ tb_size_t   lmod = 511;
    while (maxn--)
    {
        size = (size * 10807 + 1) & 0xffffffff;
        data = (__tb_volatile__ tb_byte_t*)tb_page_pool_malloc(pool, (size & lmod)? (size & lmod) : 1);
        tb_check_break(data);
    }
    time = tb_mclock() - time;
#endif

    // dump
#ifdef __tb_debug__
    tb_page_pool_dump(pool);
#endif

    // trace
    tb_trace_i("pool: %lld ms", time);
    
    // exit
    tb_page_pool_exit(pool);
    return 0;
}
