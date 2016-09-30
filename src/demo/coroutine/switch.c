/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the stack size
#define STACK       (1024)

// the switch count
#define COUNT       (10000000)

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */ 
static tb_void_t tb_demo_coroutine_switch_test_func(tb_cpointer_t priv)
{
    // loop
    tb_size_t count = (tb_size_t)priv;
    while (count--)
    {
        // trace
        tb_trace_i("[coroutine: %p]: %lu", tb_coroutine_self(), count);

        // yield
        tb_coroutine_yield();
    }
}
static tb_void_t tb_demo_coroutine_switch_test()
{
    // init scheduler
    tb_scheduler_ref_t scheduler = tb_scheduler_init_with_fifo();
    if (scheduler)
    {
        // start coroutine
        tb_coroutine_start(scheduler, tb_demo_coroutine_switch_test_func, (tb_cpointer_t)10, STACK);
        tb_coroutine_start(scheduler, tb_demo_coroutine_switch_test_func, (tb_cpointer_t)10, STACK);
        tb_coroutine_start(scheduler, tb_demo_coroutine_switch_test_func, (tb_cpointer_t)10, STACK);
        tb_coroutine_start(scheduler, tb_demo_coroutine_switch_test_func, (tb_cpointer_t)10, STACK);

        // run scheduler
        tb_scheduler_loop(scheduler);

        // exit scheduler
        tb_scheduler_exit(scheduler);
    }
}
static tb_void_t tb_demo_coroutine_switch_pref_func(tb_cpointer_t priv)
{
    // loop
    tb_size_t count = (tb_size_t)priv;
    while (count--)
    {
        // yield
        tb_coroutine_yield();
    }
}
static tb_void_t tb_demo_coroutine_switch_pref()
{
    // init scheduler
    tb_scheduler_ref_t scheduler = tb_scheduler_init_with_fifo();
    if (scheduler)
    {
        // start coroutine
        tb_coroutine_start(scheduler, tb_demo_coroutine_switch_pref_func, (tb_cpointer_t)(COUNT >> 1), STACK);
        tb_coroutine_start(scheduler, tb_demo_coroutine_switch_pref_func, (tb_cpointer_t)(COUNT >> 1), STACK);

        // init the start time
        tb_hong_t startime = tb_mclock();

        // run scheduler
        tb_scheduler_loop(scheduler);

        // computing time
        tb_hong_t duration = tb_mclock() - startime;

        // trace
        tb_trace_i("%d switches in %lld ms, %lld switches per second", COUNT, duration, (((tb_hong_t)1000 * COUNT) / duration));

        // exit scheduler
        tb_scheduler_exit(scheduler);
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_coroutine_switch_main(tb_int_t argc, tb_char_t** argv)
{
    tb_demo_coroutine_switch_test();
    tb_demo_coroutine_switch_pref();
    return 0;
}
