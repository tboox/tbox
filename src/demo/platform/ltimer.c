/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdio.h>
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * func
 */
static tb_void_t tb_ltimer_task_func(tb_pointer_t data)
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
//	tb_handle_t timer = tb_ltimer_init(3, TB_LTIMER_TICK_S, tb_true);
//	tb_handle_t timer = tb_ltimer_init(3, TB_LTIMER_TICK_S, tb_false);
	tb_handle_t timer = tb_ltimer_init(3, TB_LTIMER_TICK_100MS, tb_false);
//	tb_handle_t timer = tb_ltimer_init(3, TB_LTIMER_TICK_M, tb_false);

	if (timer)
	{
		// the limit and timeout
		tb_size_t limit 		= tb_ltimer_limit(timer);
		tb_size_t timeout 		= tb_ltimer_timeout(timer);

		// trace
		tb_print("limit: %lu, timeout: %lu", limit, timeout);

		// add task: every
		tb_ltimer_task_run(timer, 1 * timeout, tb_true, tb_ltimer_task_func, "every");
	
		// add task: one
		tb_ltimer_task_run(timer, 10 * timeout, tb_false, tb_ltimer_task_func, "one");
		
		// add task: after
		tb_ltimer_task_run_after(timer, 10 * timeout, 5 * timeout, tb_true, tb_ltimer_task_func, "after");

		// wait
		tb_ltimer_loop(timer);

		// exit timer
		tb_ltimer_exit(timer);
	}

	// exit
	tb_exit();
	return 0;
}
