/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_demo_coroutine_sleep_func(tb_cpointer_t priv)
{
    // the interval
    tb_size_t interval = (tb_size_t)priv;

    // loop
    tb_size_t count = 10;
    while (count--)
    {
        // get the start time
        tb_hong_t time = tb_mclock();

        // sleep it
        tb_msleep(interval);

        // update the interval time
        time = tb_mclock() - time;

        // trace
        tb_trace_i("[coroutine: %lu]: count: %lu, interval: %lld ms", interval, count, time);
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_coroutine_sleep_main(tb_int_t argc, tb_char_t** argv)
{
    // init scheduler
    tb_co_scheduler_ref_t scheduler = tb_co_scheduler_init();
    if (scheduler)
    {
        // start coroutines
        tb_coroutine_start(scheduler, tb_demo_coroutine_sleep_func, (tb_cpointer_t)1000, 0);
        tb_coroutine_start(scheduler, tb_demo_coroutine_sleep_func, (tb_cpointer_t)2000, 0);
        tb_coroutine_start(scheduler, tb_demo_coroutine_sleep_func, (tb_cpointer_t)10, 0);
        tb_coroutine_start(scheduler, tb_demo_coroutine_sleep_func, (tb_cpointer_t)100, 0);

        // run scheduler
        tb_co_scheduler_loop(scheduler, tb_true);

        // exit scheduler
        tb_co_scheduler_exit(scheduler);
    }
    return 0;
}
