/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_int_t tb_demo_thread_func(tb_cpointer_t priv)
{
    // self
    tb_size_t self = tb_thread_self();

    // trace
    tb_trace_i("thread[%lx: %zu]: init", self, priv);

    // wait some time
    tb_sleep(1);

    // get cpu core
    tb_cpuset_t cpuset;
    TB_CPUSET_ZERO(&cpuset);
    if (tb_thread_getaffinity(tb_null, &cpuset))
    {
        tb_size_t i;
        for (i = 0; i < TB_CPUSET_SIZE; i++)
        {
            if (TB_CPUSET_ISSET(i, &cpuset))
                tb_trace_i("thread[%lx: %zu]: get cpu core: %d", self, priv, i);
        }
    }

    // reset thread affinity
    tb_size_t cpu = tb_min(1, tb_cpu_count() - 1);
    TB_CPUSET_ZERO(&cpuset);
    TB_CPUSET_SET(cpu, &cpuset);
    if (!tb_thread_setaffinity(tb_null, &cpuset))
    {
        tb_trace_e("thread[%lx: %zu]: set cpu core(%zu) failed!", self, priv, 1);
    }

    // get cpu core again
    TB_CPUSET_ZERO(&cpuset);
    if (tb_thread_getaffinity(tb_null, &cpuset))
    {
        tb_size_t i;
        for (i = 0; i < TB_CPUSET_SIZE; i++)
        {
            if (TB_CPUSET_ISSET(i, &cpuset))
                tb_trace_i("thread[%lx: %zu]: get cpu core again: %d", self, priv, i);
        }
    }

    // exit
    tb_thread_return(-1);

    // trace
    tb_trace_i("thread[%lx: %zu]: exit", self, priv);

    // ok
    return 0;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_platform_thread_main(tb_int_t argc, tb_char_t** argv)
{
    // get cpu count
    tb_size_t cpu_count = tb_cpu_count();
    tb_trace_i("cpu count: %lu", cpu_count);

    // init threads
    tb_size_t i = 0;
    tb_cpuset_t cpuset;
    tb_thread_ref_t threads[64] = {0};
    for (i = 0; i < cpu_count; i++)
    {
        // init thread
        threads[i] = tb_thread_init(tb_null, tb_demo_thread_func, tb_u2p(i), 0);
        tb_assert_and_check_break(threads[i]);

        // set thread affinity
        TB_CPUSET_ZERO(&cpuset);
        TB_CPUSET_SET(i, &cpuset);
        if (!tb_thread_setaffinity(threads[i], &cpuset))
        {
            tb_trace_e("set cpu core(%zu) failed for thread(%zu)", i, i);
        }
    }

    // wait threads
    for (i = 0; i < cpu_count; i++)
    {
        tb_thread_ref_t thread = threads[i];
        if (thread)
        {
            // wait thread
            tb_int_t retval = 0;
            if (tb_thread_wait(thread, -1, &retval) > 0)
            {
                // trace
                tb_trace_i("wait: %zu ok, retval: %d", i, retval);
            }

            // exit thread
            tb_thread_exit(thread);
        }
    }
    return 0;
}
