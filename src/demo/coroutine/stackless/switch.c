/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the switch count
#define COUNT       (10000000)

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_demo_lo_coroutine_switch_test_func(tb_lo_coroutine_ref_t coroutine, tb_cpointer_t priv)
{
    // check
    tb_size_t* count = (tb_size_t*)priv;
    tb_assert(count);

    // enter coroutine
    tb_lo_coroutine_enter(coroutine)
    {
        // loop
        while ((*count)--)
        {
            // trace
            tb_trace_i("[coroutine: %p]: %lu", tb_lo_coroutine_self(), *count);

            // yield
            tb_lo_coroutine_yield();
        }
    }
}
static tb_void_t tb_demo_lo_coroutine_switch_test()
{
    // init scheduler
    tb_lo_scheduler_ref_t scheduler = tb_lo_scheduler_init();
    if (scheduler)
    {
        // start coroutines
        tb_size_t counts[] = {10, 10, 10, 10};
        tb_lo_coroutine_start(scheduler, tb_demo_lo_coroutine_switch_test_func, &counts[0], tb_null);
        tb_lo_coroutine_start(scheduler, tb_demo_lo_coroutine_switch_test_func, &counts[1], tb_null);
        tb_lo_coroutine_start(scheduler, tb_demo_lo_coroutine_switch_test_func, &counts[2], tb_null);
        tb_lo_coroutine_start(scheduler, tb_demo_lo_coroutine_switch_test_func, &counts[3], tb_null);

        // run scheduler
        tb_lo_scheduler_loop(scheduler, tb_true);

        // exit scheduler
        tb_lo_scheduler_exit(scheduler);
    }
}
static tb_void_t tb_demo_lo_coroutine_switch_perf_func(tb_lo_coroutine_ref_t coroutine, tb_cpointer_t priv)
{
    // check
    tb_size_t* count = (tb_size_t*)priv;
    tb_assert(count);

    // enter coroutine
    tb_lo_coroutine_enter(coroutine)
    {
        // loop
        while ((*count)--)
        {
            // yield
            tb_lo_coroutine_yield();
        }
    }
}
static tb_void_t tb_demo_lo_coroutine_switch_perf()
{
    // init scheduler
    tb_lo_scheduler_ref_t scheduler = tb_lo_scheduler_init();
    if (scheduler)
    {
        // start coroutine
        tb_size_t counts[] = {COUNT >> 1, COUNT >> 1};
        tb_lo_coroutine_start(scheduler, tb_demo_lo_coroutine_switch_perf_func, &counts[0], tb_null);
        tb_lo_coroutine_start(scheduler, tb_demo_lo_coroutine_switch_perf_func, &counts[1], tb_null);

        // init the start time
        tb_hong_t startime = tb_mclock();

        // run scheduler
        tb_lo_scheduler_loop(scheduler, tb_true);

        // computing time
        tb_hong_t duration = tb_mclock() - startime;

        // trace
        tb_trace_i("%d switches in %lld ms, %lld switches per second", COUNT, duration, (((tb_hong_t)1000 * COUNT) / duration));

        // exit scheduler
        tb_lo_scheduler_exit(scheduler);
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_lo_coroutine_switch_main(tb_int_t argc, tb_char_t** argv)
{
    tb_demo_lo_coroutine_switch_test();
    tb_demo_lo_coroutine_switch_perf();
    return 0;
}
