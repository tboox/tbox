/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */ 
static tb_void_t tb_demo_lo_coroutine_sleep_func(tb_lo_coroutine_ref_t coroutine, tb_cpointer_t priv)
{
    // the interval
    tb_size_t interval = (tb_size_t)priv;

    // enter coroutine
    tb_lo_coroutine_enter(coroutine);

    // loop
    tb_size_t count = 10;
    while (count--)
    {
        // get the start time
        tb_hong_t time = tb_mclock();

        // sleep it
        tb_lo_coroutine_sleep(interval);

        // update the interval time
        time = tb_mclock() - time;

        // trace
        tb_trace_i("[coroutine: %lu]: count: %lu, interval: %lld ms", interval, count, time);
    }

    // leave coroutine
    tb_lo_coroutine_leave();
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_lo_coroutine_sleep_main(tb_int_t argc, tb_char_t** argv)
{
    // init scheduler
    tb_lo_scheduler_ref_t scheduler = tb_lo_scheduler_init();
    if (scheduler)
    {
        // start coroutines
        tb_lo_coroutine_start(scheduler, tb_demo_lo_coroutine_sleep_func, (tb_cpointer_t)1000);
        tb_lo_coroutine_start(scheduler, tb_demo_lo_coroutine_sleep_func, (tb_cpointer_t)2000);
        tb_lo_coroutine_start(scheduler, tb_demo_lo_coroutine_sleep_func, (tb_cpointer_t)10);
        tb_lo_coroutine_start(scheduler, tb_demo_lo_coroutine_sleep_func, (tb_cpointer_t)100);

        // run scheduler
        tb_lo_scheduler_loop(scheduler);

        // exit scheduler
        tb_lo_scheduler_exit(scheduler);
    }
    return 0;
}
