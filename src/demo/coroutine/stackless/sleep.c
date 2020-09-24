/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the sleep local type
typedef struct __tb_demo_lo_sleep_t
{
    // the interval
    tb_size_t       interval;

    // the count
    tb_size_t       count;

    // the time
    tb_hong_t       time;

}tb_demo_lo_sleep_t, *tb_demo_lo_sleep_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_demo_lo_coroutine_sleep_func(tb_lo_coroutine_ref_t coroutine, tb_cpointer_t priv)
{
    // the local
    tb_demo_lo_sleep_ref_t local = (tb_demo_lo_sleep_ref_t)priv;

    // enter coroutine
    tb_lo_coroutine_enter(coroutine)
    {
        // loop
        while (local->count--)
        {
            // get the start time
            local->time = tb_mclock();

            // sleep it
            tb_lo_coroutine_sleep(local->interval);

            // update the interval time
            local->time = tb_mclock() - local->time;

            // trace
            tb_trace_i("[coroutine: %lu]: count: %lu, interval: %lld ms", local->interval, local->count, local->time);
        }
    }
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
        tb_demo_lo_sleep_t sleeps[] =
        {
            {1000,  10}
        ,   {2000,  10}
        ,   {10,    10}
        ,   {100,   10}
        };
        tb_lo_coroutine_start(scheduler, tb_demo_lo_coroutine_sleep_func, &sleeps[0], tb_null);
        tb_lo_coroutine_start(scheduler, tb_demo_lo_coroutine_sleep_func, &sleeps[1], tb_null);
        tb_lo_coroutine_start(scheduler, tb_demo_lo_coroutine_sleep_func, &sleeps[2], tb_null);
        tb_lo_coroutine_start(scheduler, tb_demo_lo_coroutine_sleep_func, &sleeps[3], tb_null);

        // run scheduler
        tb_lo_scheduler_loop(scheduler, tb_true);

        // exit scheduler
        tb_lo_scheduler_exit(scheduler);
    }
    return 0;
}
