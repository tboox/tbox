/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */ 
#if 0
static tb_void_t tb_demo_lo_coroutine_nest_next(tb_lo_coroutine_ref_t coroutine, tb_size_t start, tb_size_t end)
{
    // enter coroutine
    tb_lo_coroutine_enter(coroutine)
    {
        // loop
        for (; start < end; start++)
        {
            // trace
            tb_trace_i("[coroutine: %p]:   %lu", tb_lo_coroutine_self(), start);

            // yield
            tb_lo_coroutine_yield();
        }
    }
}
#endif
static tb_void_t tb_demo_lo_coroutine_nest_func(tb_lo_coroutine_ref_t coroutine, tb_cpointer_t priv)
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

            // call next level function
//            tb_demo_lo_coroutine_nest_next(coroutine, *count * 10, *count * 10 + 5);

            // yield
            tb_lo_coroutine_yield();
        }
    }
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
        tb_size_t counts[] = {10, 10};
        tb_lo_coroutine_start(scheduler, tb_demo_lo_coroutine_nest_func, &counts[0], tb_null);
        tb_lo_coroutine_start(scheduler, tb_demo_lo_coroutine_nest_func, &counts[1], tb_null);

        // run scheduler
        tb_lo_scheduler_loop(scheduler);

        // exit scheduler
        tb_lo_scheduler_exit(scheduler);
    }

    return 0;
}
