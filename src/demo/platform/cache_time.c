/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_platform_cache_time_main(tb_int_t argc, tb_char_t** argv)
{
    tb_trace_i("%lld %lld", tb_cache_time_spak(), tb_cache_time_mclock());
    tb_sleep(1);
    tb_trace_i("%lld %lld", tb_cache_time_spak(), tb_cache_time_mclock());
    tb_sleep(1);
    tb_trace_i("%lld %lld", tb_cache_time_spak(), tb_cache_time_mclock());
    return 0;
}
