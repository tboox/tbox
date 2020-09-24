/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_platform_sched_main(tb_int_t argc, tb_char_t** argv)
{
    // get cpu count
    tb_size_t cpu_count = tb_cpu_count();
    tb_trace_i("cpu count: %lu", cpu_count);

    // get the previous cpu core
    tb_cpuset_t cpuset;
    TB_CPUSET_ZERO(&cpuset);
    if (tb_sched_getaffinity(0, &cpuset))
    {
        tb_size_t i;
        for (i = 0; i < cpu_count; i++)
        {
            if (TB_CPUSET_ISSET(i, &cpuset))
                tb_trace_i("get previous cpu core: %d", i);
        }
    }

    // trace
    tb_int_t cpu = argv[1]? tb_atoi(argv[1]) : 0;
    tb_trace_i("set cpu core: %d", cpu);

    // set cpu affinity to the given cpu core
    TB_CPUSET_ZERO(&cpuset);
    TB_CPUSET_SET(cpu, &cpuset);
    if (tb_sched_setaffinity(0, &cpuset))
    {
        TB_CPUSET_ZERO(&cpuset);
        if (tb_sched_getaffinity(0, &cpuset))
        {
            tb_size_t i;
            for (i = 0; i < cpu_count; i++)
            {
                if (TB_CPUSET_ISSET(i, &cpuset))
                    tb_trace_i("get cpu core: %d", i);
            }
        }
    }
    else tb_trace_i("set cpu core failed!");
    return 0;
}
