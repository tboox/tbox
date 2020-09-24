/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_demo_coroutine_semaphore_wait_func(tb_cpointer_t priv)
{
    // check
    tb_co_semaphore_ref_t semaphore = (tb_co_semaphore_ref_t)priv;
    tb_assert(semaphore);

    // loop
    while (1)
    {
        // wait it
        tb_long_t ok = tb_co_semaphore_wait(semaphore, -1);
        tb_assert_and_check_break(ok > 0);

        // trace
        tb_trace_i("[coroutine: %p]: wait ok", tb_coroutine_self());
    }
}
static tb_void_t tb_demo_coroutine_semaphore_post_func(tb_cpointer_t priv)
{
    // check
    tb_co_semaphore_ref_t semaphore = (tb_co_semaphore_ref_t)priv;
    tb_assert(semaphore);

    // loop
    while (1)
    {
        // trace
        tb_trace_i("[coroutine: %p]: post", tb_coroutine_self());

        // post it
        tb_co_semaphore_post(semaphore, 2);

        // wait some time
        tb_msleep(1000);
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_coroutine_semaphore_main(tb_int_t argc, tb_char_t** argv)
{
    // init scheduler
    tb_co_scheduler_ref_t scheduler = tb_co_scheduler_init();
    if (scheduler)
    {
        // init semaphore
        tb_co_semaphore_ref_t semaphore = tb_co_semaphore_init(0);
        tb_assert(semaphore);

        // start coroutines
        tb_coroutine_start(scheduler, tb_demo_coroutine_semaphore_wait_func, semaphore, 0);
        tb_coroutine_start(scheduler, tb_demo_coroutine_semaphore_wait_func, semaphore, 0);
        tb_coroutine_start(scheduler, tb_demo_coroutine_semaphore_wait_func, semaphore, 0);
        tb_coroutine_start(scheduler, tb_demo_coroutine_semaphore_wait_func, semaphore, 0);
        tb_coroutine_start(scheduler, tb_demo_coroutine_semaphore_post_func, semaphore, 0);

        // run scheduler
        tb_co_scheduler_loop(scheduler, tb_true);

        // exit semaphore
        tb_co_semaphore_exit(semaphore);

        // exit scheduler
        tb_co_scheduler_exit(scheduler);
    }
    return 0;
}
