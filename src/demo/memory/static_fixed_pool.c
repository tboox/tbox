/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_memory_static_fixed_pool_main(tb_int_t argc, tb_char_t** argv)
{
    // init
    tb_handle_t fpool = tb_static_fixed_pool_init(malloc(50 * 1024 * 1024), 50 * 1024 * 1024, 64, 0);
    tb_assert_and_check_return_val(fpool, 0);

    __tb_volatile__ tb_hong_t   time = tb_mclock();
    __tb_volatile__ tb_byte_t*  data = tb_null;
    __tb_volatile__ tb_size_t   maxn = 100000;
    while (maxn--)
    {
        data = tb_static_fixed_pool_malloc(fpool);
        tb_check_break(data);
    }
    time = tb_mclock() - time;

    // dump
#ifdef __tb_debug__
    tb_static_fixed_pool_dump(fpool);
#endif

    // trace
    tb_trace_i("fpool: %lld ms", time);
    
    // exit
    tb_static_fixed_pool_exit(fpool);
    return 0;
}
