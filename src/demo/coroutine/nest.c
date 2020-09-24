/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_demo_coroutine_nest_next(tb_size_t start, tb_size_t end)
{
    // loop
    for (; start < end; start++)
    {
        // trace
        tb_trace_i("[coroutine: %p]:   %lu", tb_coroutine_self(), start);

        // yield
        tb_coroutine_yield();
    }
}
static tb_void_t tb_demo_coroutine_nest_func(tb_cpointer_t priv)
{
    // loop
    tb_size_t count = (tb_size_t)priv;
    while (count--)
    {
        // trace
        tb_trace_i("[coroutine: %p]: %lu", tb_coroutine_self(), count);

        // call next level function
        tb_demo_coroutine_nest_next(count * 10, count * 10 + 5);

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

        // run scheduler
        tb_co_scheduler_loop(scheduler, tb_true);

        // exit scheduler
        tb_co_scheduler_exit(scheduler);
    }
    return 0;
}
