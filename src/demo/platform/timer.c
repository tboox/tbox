/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* ///////////////////////////////////////////////////////////////////////
 * func
 */
static tb_void_t tb_demo_timer_task_func(tb_bool_t killed, tb_pointer_t data)
{
	// get the time
	tb_timeval_t tv = {0};
    if (tb_gettimeofday(&tv, tb_null))
	{
		// the time value
		tb_hong_t val = ((tb_hong_t)tv.tv_sec * 1000 + tv.tv_usec / 1000);

		// trace
		tb_trace_i("task[%s]: %lld ms, killed: %d", (tb_char_t const*)data, val, killed);
	}
}
static tb_pointer_t tb_demo_timer_loop(tb_pointer_t data)
{
	// the timer
	tb_handle_t timer = (tb_handle_t)data;

	// wait
	if (timer) tb_timer_loop(timer);

	// exit it
	tb_thread_return(tb_null);
	return tb_null;
}

/* ///////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_platform_timer_main(tb_int_t argc, tb_char_t** argv)
{
	// init timer
//	tb_handle_t timer = tb_timer_init(3, tb_true);
	tb_handle_t timer = tb_timer_init(3, tb_false);

	if (timer)
	{
		// add task: every
		tb_timer_task_run(timer, 1000, tb_true, tb_demo_timer_task_func, "every");
	
		// add task: one
		tb_handle_t one = tb_timer_task_add(timer, 10000, tb_false, tb_demo_timer_task_func, "one");
		
		// add task: after
		tb_handle_t after = tb_timer_task_add_after(timer, 10000, 5000, tb_true, tb_demo_timer_task_func, "after");

		// init loop
		tb_thread_init(tb_null, tb_demo_timer_loop, timer, 0);

		// wait some time
		getchar();

		// kil the task
		if (one) tb_timer_task_kil(timer, one);
		if (after) tb_timer_task_kil(timer, after);

		// wait some time
		getchar();

		// del the task
		if (one) tb_timer_task_del(timer, one);
		if (after) tb_timer_task_del(timer, after);

		// exit timer
		tb_timer_exit(timer);
	}

	return 0;
}
