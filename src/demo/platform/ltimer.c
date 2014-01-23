/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* ///////////////////////////////////////////////////////////////////////
 * func
 */
static tb_void_t tb_demo_ltimer_task_func(tb_bool_t killed, tb_pointer_t data)
{
	// get the time
	tb_timeval_t tv = {0};
    if (tb_gettimeofday(&tv, tb_null))
	{
		// the time value
		tb_hong_t val = ((tb_hong_t)tv.tv_sec * 1000 + tv.tv_usec / 1000);

		// trace
		tb_print("task[%s]: %lld ms, killed: %d", (tb_char_t const*)data, val, killed);
	}
}
static tb_pointer_t tb_demo_ltimer_loop(tb_pointer_t data)
{
	// the timer
	tb_handle_t timer = (tb_handle_t)data;

	// wait
	if (timer) tb_ltimer_loop(timer);

	// exit it
	tb_thread_return(tb_null);
	return tb_null;
}

/* ///////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_platform_ltimer_main(tb_int_t argc, tb_char_t** argv)
{
	// init timer
//	tb_handle_t timer = tb_ltimer_init(3, TB_LTIMER_TICK_S, tb_true);
	tb_handle_t timer = tb_ltimer_init(3, TB_LTIMER_TICK_S, tb_false);
//	tb_handle_t timer = tb_ltimer_init(3, TB_LTIMER_TICK_100MS, tb_false);
//	tb_handle_t timer = tb_ltimer_init(3, TB_LTIMER_TICK_M, tb_false);

	if (timer)
	{
		// the limit and delay
		tb_size_t limit 		= tb_ltimer_limit(timer);
		tb_size_t delay 		= tb_ltimer_delay(timer);

		// trace
		tb_print("limit: %lu, delay: %lu", limit, delay);

		// add task: every
		tb_ltimer_task_run(timer, 1 * delay, tb_true, tb_demo_ltimer_task_func, "every");
	
		// add task: one
		tb_handle_t one = tb_ltimer_task_add(timer, 10 * delay, tb_false, tb_demo_ltimer_task_func, "one");
		
		// add task: after
		tb_handle_t after = tb_ltimer_task_add_after(timer, 10 * delay, 5 * delay, tb_true, tb_demo_ltimer_task_func, "after");

		// init loop
		tb_thread_init(tb_null, tb_demo_ltimer_loop, timer, 0);

		// wait some time
		getchar();

		// kil the task
		if (one) tb_ltimer_task_kil(timer, one);
		if (after) tb_ltimer_task_kil(timer, after);

		// wait some time
		getchar();

		// del the task
		if (one) tb_ltimer_task_del(timer, one);
		if (after) tb_ltimer_task_del(timer, after);

		// exit timer
		tb_ltimer_exit(timer);
	}

	return 0;
}
