/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * func
 */
static tb_void_t tb_demo_timer_task_func(tb_bool_t killed, tb_cpointer_t priv)
{
    // get the time
    tb_timeval_t tv = {0};
    if (tb_gettimeofday(&tv, tb_null))
    {
        // the time value
        tb_hong_t val = ((tb_hong_t)tv.tv_sec * 1000 + tv.tv_usec / 1000);

        // trace
        tb_trace_i("task[%s]: %lld ms, killed: %d", (tb_char_t const*)priv, val, killed);
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_platform_timer_main(tb_int_t argc, tb_char_t** argv)
{
    // add task: every
    tb_timer_task_post(tb_timer(), 1000, tb_true, tb_demo_timer_task_func, "every");

    // add task: one
    tb_timer_task_ref_t one = tb_timer_task_init(tb_timer(), 10000, tb_false, tb_demo_timer_task_func, "one");

    // add task: after
    tb_timer_task_ref_t after = tb_timer_task_init_after(tb_timer(), 10000, 5000, tb_true, tb_demo_timer_task_func, "after");

    // wait some time
    tb_getchar();

    // kil the task
    if (one) tb_timer_task_kill(tb_timer(), one);
    if (after) tb_timer_task_kill(tb_timer(), after);

    // wait some time
    tb_getchar();

    // del the task
    if (one) tb_timer_task_exit(tb_timer(), one);
    if (after) tb_timer_task_exit(tb_timer(), after);

    // ok
    return 0;
}
