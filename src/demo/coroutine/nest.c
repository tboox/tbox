/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the switch count
#define COUNT       (10000000)

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */ 
static tb_void_t tb_demo_coroutine_nest_func(tb_cpointer_t priv)
{
    // the count
    tb_size_t count = (tb_size_t)priv;
    if (count == 10)
    {
        // start coroutines
        tb_coroutine_start(tb_null, tb_demo_coroutine_nest_func, (tb_cpointer_t)5, 0);
        tb_coroutine_start(tb_null, tb_demo_coroutine_nest_func, (tb_cpointer_t)15, 0);
        tb_coroutine_start(tb_null, tb_demo_coroutine_nest_func, (tb_cpointer_t)25, 0);
        tb_coroutine_start(tb_null, tb_demo_coroutine_nest_func, (tb_cpointer_t)35, 0);
    }

    // loop
    while (count--)
    {
        // trace
        tb_trace_i("[coroutine: %p]: %lu", tb_coroutine_self(), count);

        // yield
        tb_coroutine_yield();
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_coroutine_nest_main(tb_int_t argc, tb_char_t** argv)
{
    // init scheduler
    tb_co_scheduler_ref_t scheduler = tb_co_scheduler_init();
    if (scheduler)
    {
        // start coroutines
        tb_coroutine_start(scheduler, tb_demo_coroutine_nest_func, (tb_cpointer_t)10, 0);
        tb_coroutine_start(scheduler, tb_demo_coroutine_nest_func, (tb_cpointer_t)10, 0);
        tb_coroutine_start(scheduler, tb_demo_coroutine_nest_func, (tb_cpointer_t)10, 0);
        tb_coroutine_start(scheduler, tb_demo_coroutine_nest_func, (tb_cpointer_t)10, 0);

        // run scheduler
        tb_co_scheduler_loop(scheduler, tb_true);

        // exit scheduler
        tb_co_scheduler_exit(scheduler);
    }
    return 0;
}
