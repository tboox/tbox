/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_memory_fixed_pool_main(tb_int_t argc, tb_char_t** argv)
{
    // init pool
    tb_fixed_pool_ref_t pool = tb_fixed_pool_init(0, 64, 0);
    tb_assert_and_check_return_val(pool, 0);

    __tb_volatile__ tb_hong_t   time = tb_mclock();
    __tb_volatile__ tb_byte_t*  data = tb_null;
    __tb_volatile__ tb_size_t   maxn = 100000;
    while (maxn--)
    {
        data = (__tb_volatile__ tb_byte_t*)tb_fixed_pool_malloc(pool);
        tb_check_break(data);
    }
    time = tb_mclock() - time;

    // dump
#ifdef __tb_debug__
//  tb_fixed_pool_dump(pool);
#endif

    // trace
    tb_trace_i("pool: %lld ms", time);
    
    // exit pool
    tb_fixed_pool_exit(pool);

    return 0;
}
