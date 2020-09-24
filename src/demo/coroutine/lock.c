/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_demo_coroutine_lock_test_func(tb_cpointer_t priv)
{
    // check
    tb_co_lock_ref_t lock = (tb_co_lock_ref_t)priv;
    tb_assert_and_check_return(lock);

    // loop
    tb_size_t count = 10;
    while (count--)
    {
        // enter lock
        tb_co_lock_enter(lock);

        // trace
        tb_trace_i("[coroutine: %p]: enter", tb_coroutine_self());

        // wait some time
        tb_msleep(1000);

        // trace
        tb_trace_i("[coroutine: %p]: leave", tb_coroutine_self());

        // leave lock
        tb_co_lock_leave(lock);
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_coroutine_lock_main(tb_int_t argc, tb_char_t** argv)
{
    // init scheduler
    tb_co_scheduler_ref_t scheduler = tb_co_scheduler_init();
    if (scheduler)
    {
        // init lock
        tb_co_lock_ref_t lock = tb_co_lock_init();
        tb_assert(lock);

        // start coroutines
        tb_coroutine_start(scheduler, tb_demo_coroutine_lock_test_func, lock, 0);
        tb_coroutine_start(scheduler, tb_demo_coroutine_lock_test_func, lock, 0);
        tb_coroutine_start(scheduler, tb_demo_coroutine_lock_test_func, lock, 0);
        tb_coroutine_start(scheduler, tb_demo_coroutine_lock_test_func, lock, 0);
        tb_coroutine_start(scheduler, tb_demo_coroutine_lock_test_func, lock, 0);

        // run scheduler
        tb_co_scheduler_loop(scheduler, tb_true);

        // exit lock
        tb_co_lock_exit(lock);

        // exit scheduler
        tb_co_scheduler_exit(scheduler);
    }
    return 0;
}
