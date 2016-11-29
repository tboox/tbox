/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */ 
static tb_void_t tb_demo_lo_coroutine_nest_func(tb_lo_coroutine_ref_t coroutine, tb_cpointer_t priv)
{
    // check
    tb_size_t* count = (tb_size_t*)priv;
    tb_assert_and_check_return(count);

    // enter coroutine
    tb_lo_coroutine_enter(coroutine);

    // the count
    if (*count == 40)
    {
        // start coroutines
        static tb_size_t counts[] = {5, 15, 25, 35};
        tb_lo_coroutine_start(tb_lo_scheduler_self(), tb_demo_lo_coroutine_nest_func, &counts[0], tb_null);
        tb_lo_coroutine_start(tb_lo_scheduler_self(), tb_demo_lo_coroutine_nest_func, &counts[1], tb_null);
        tb_lo_coroutine_start(tb_lo_scheduler_self(), tb_demo_lo_coroutine_nest_func, &counts[2], tb_null);
        tb_lo_coroutine_start(tb_lo_scheduler_self(), tb_demo_lo_coroutine_nest_func, &counts[3], tb_null);
    }

    // loop
    while ((*count)--)
    {
        // trace
        tb_trace_i("[coroutine: %p]: %lu", tb_lo_coroutine_self(), *count);

        // yield
        tb_lo_coroutine_yield();
    }
 
    // leave coroutine
    tb_lo_coroutine_leave();
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_lo_coroutine_nest_main(tb_int_t argc, tb_char_t** argv)
{
    // init scheduler
    tb_lo_scheduler_ref_t scheduler = tb_lo_scheduler_init();
    if (scheduler)
    {
        // start coroutines
        tb_size_t counts[] = {10, 20, 30, 40};
        tb_lo_coroutine_start(scheduler, tb_demo_lo_coroutine_nest_func, &counts[0], tb_null);
        tb_lo_coroutine_start(scheduler, tb_demo_lo_coroutine_nest_func, &counts[1], tb_null);
        tb_lo_coroutine_start(scheduler, tb_demo_lo_coroutine_nest_func, &counts[2], tb_null);
        tb_lo_coroutine_start(scheduler, tb_demo_lo_coroutine_nest_func, &counts[3], tb_null);

        // run scheduler
        tb_lo_scheduler_loop(scheduler);

        // exit scheduler
        tb_lo_scheduler_exit(scheduler);
    }
    return 0;
}
