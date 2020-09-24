/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the lock type
typedef struct __tb_demo_lo_lock_t
{
    // the count
    tb_size_t           count;

}tb_demo_lo_lock_t, *tb_demo_lo_lock_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the lock
static tb_lo_lock_t     g_lock;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_demo_lo_coroutine_lock_test_func(tb_lo_coroutine_ref_t coroutine, tb_cpointer_t priv)
{
    // check
    tb_demo_lo_lock_ref_t lock = (tb_demo_lo_lock_ref_t)priv;
    tb_assert(lock);

    // enter coroutine
    tb_lo_coroutine_enter(coroutine)
    {
        // loop
        while (lock->count--)
        {
            // enter lock
            tb_lo_lock_enter(&g_lock);

            // trace
            tb_trace_i("[coroutine: %p]: enter", tb_lo_coroutine_self());

            // wait some time
            tb_lo_coroutine_sleep(1000);

            // trace
            tb_trace_i("[coroutine: %p]: leave", tb_lo_coroutine_self());

            // leave lock
            tb_lo_lock_leave(&g_lock);
        }
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_lo_coroutine_lock_main(tb_int_t argc, tb_char_t** argv)
{
    // init scheduler
    tb_lo_scheduler_ref_t scheduler = tb_lo_scheduler_init();
    if (scheduler)
    {
        // init lock
        tb_lo_lock_init(&g_lock);

        // start coroutines
        tb_size_t count = 10;
        tb_lo_coroutine_start(scheduler, tb_demo_lo_coroutine_lock_test_func, tb_lo_coroutine_pass1(tb_demo_lo_lock_t, count, count));
        tb_lo_coroutine_start(scheduler, tb_demo_lo_coroutine_lock_test_func, tb_lo_coroutine_pass1(tb_demo_lo_lock_t, count, count));
        tb_lo_coroutine_start(scheduler, tb_demo_lo_coroutine_lock_test_func, tb_lo_coroutine_pass1(tb_demo_lo_lock_t, count, count));
        tb_lo_coroutine_start(scheduler, tb_demo_lo_coroutine_lock_test_func, tb_lo_coroutine_pass1(tb_demo_lo_lock_t, count, count));
        tb_lo_coroutine_start(scheduler, tb_demo_lo_coroutine_lock_test_func, tb_lo_coroutine_pass1(tb_demo_lo_lock_t, count, count));

        // run scheduler
        tb_lo_scheduler_loop(scheduler, tb_true);

        // exit scheduler
        tb_lo_scheduler_exit(scheduler);

        // exit lock
        tb_lo_lock_exit(&g_lock);
    }
    return 0;
}
