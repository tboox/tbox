/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the nest type
typedef struct __tb_demo_lo_nest_t
{
    // the count
    tb_size_t           count;

    // the start
    tb_size_t           start;

    // the end
    tb_size_t           end;

}tb_demo_lo_nest_t, *tb_demo_lo_nest_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#if 0
static tb_void_t tb_demo_lo_coroutine_nest_next(tb_lo_coroutine_ref_t coroutine, tb_demo_lo_nest_ref_t nest)
{
    // enter coroutine
    tb_lo_coroutine_enter(coroutine)
    {
        // loop
        nest->start = nest->count * 10;
        nest->end   = nest->count * 10 + 5;
        for (; nest->start < nest->end; nest->start++)
        {
            // trace
            tb_trace_i("[coroutine: %p]:   %lu", tb_lo_coroutine_self(), nest->start);

            // trace
            tb_trace_i("yield: 1");

            // yield
            tb_lo_coroutine_yield();

            // trace
            tb_trace_i("yield: 2");

            // yield
            tb_lo_coroutine_yield();

            // trace
            tb_trace_i("yield: 3");
        }
    }
}
#endif
static tb_void_t tb_demo_lo_coroutine_nest_func(tb_lo_coroutine_ref_t coroutine, tb_cpointer_t priv)
{
    // check
    tb_demo_lo_nest_ref_t nest = (tb_demo_lo_nest_ref_t)priv;
    tb_assert(nest);

    // enter coroutine
    tb_lo_coroutine_enter(coroutine)
    {
        // loop
        while (nest->count--)
        {
            // trace
            tb_trace_i("[coroutine: %p]: %lu", tb_lo_coroutine_self(), nest->count);

#if 0
            // TODO need implementation
            // fork child coroutine
            tb_lo_coroutine_fork()
            {
                // call next level function
                tb_demo_lo_coroutine_nest_next(coroutine, nest);
            }
#endif

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
        tb_size_t count = 10;
        tb_lo_coroutine_start(scheduler, tb_demo_lo_coroutine_nest_func, tb_lo_coroutine_pass1(tb_demo_lo_nest_t, count, count));
        tb_lo_coroutine_start(scheduler, tb_demo_lo_coroutine_nest_func, tb_lo_coroutine_pass1(tb_demo_lo_nest_t, count, count));

        // run scheduler
        tb_lo_scheduler_loop(scheduler, tb_true);

        // exit scheduler
        tb_lo_scheduler_exit(scheduler);
    }

    return 0;
}
