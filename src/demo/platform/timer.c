/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdio.h>
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * func
 */
static tb_void_t tb_timer_task_func(tb_pointer_t data)
{
	// get the time
	tb_timeval_t tv = {0};
    if (tb_gettimeofday(&tv, tb_null))
	{
		// the time value
		tb_hong_t val = ((tb_hong_t)tv.tv_sec * 1000 + tv.tv_usec / 1000);

		// trace
		tb_print("task[%s]: %lld ms", (tb_char_t const*)data, val);
	}
}

/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// init
	if (!tb_init(malloc(10 * 1024 * 1024), 10 * 1024 * 1024)) return 0;

	// init timer
//	tb_handle_t timer = tb_timer_init(3, tb_true);
	tb_handle_t timer = tb_timer_init(3, tb_false);

	if (timer)
	{
		// add task: every
		tb_timer_task_run(timer, 1000, tb_true, tb_timer_task_func, "every");
	
		// add task: one
		tb_timer_task_run(timer, 10000, tb_false, tb_timer_task_func, "one");
		
		// add task: after
		tb_timer_task_run_after(timer, 10000, 5000, tb_true, tb_timer_task_func, "after");

		// wait
		tb_timer_loop(timer);

		// exit timer
		tb_timer_exit(timer);
	}

	// exit
	tb_exit();
	return 0;
}
