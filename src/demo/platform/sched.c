/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_platform_sched_main(tb_int_t argc, tb_char_t** argv)
{
    // trace
    tb_trace_i("cpu count: %lu", tb_processor_count());

    tb_cpuset_t cpuset;
    TB_CPUSET_ZERO(&cpuset);

    return 0;
}
