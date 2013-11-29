/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdio.h>
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * func
 */
static tb_bool_t tb_ltimer_task_func(tb_pointer_t data)
{
	// get the time
	tb_timeval_t tv = {0};
    if (!tb_gettimeofday(&tv, tb_null)) return tb_false;

	// the time value
	tb_hong_t val = ((tb_hong_t)tv.tv_sec * 1000 + tv.tv_usec / 1000);

	// trace
	tb_print("task[%s]: %lld ms", (tb_char_t const*)data, val);

	// continue?
	return !tb_strcmp(data, "one")? tb_false : tb_true;
}

/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// init
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// init timer
//	tb_handle_t timer = tb_ltimer_init(3, TB_LTIMER_PRECISION_MS, tb_false);
	tb_handle_t timer = tb_ltimer_init(3, TB_LTIMER_PRECISION_MS, tb_true);
//	tb_handle_t timer = tb_ltimer_init(3, TB_LTIMER_PRECISION_S, tb_true);
//	tb_handle_t timer = tb_ltimer_init(3, TB_LTIMER_PRECISION_M, tb_true);

	if (timer)
	{
		// the limit and precision
		tb_hong_t limit 		= tb_ltimer_limit(timer);
		tb_size_t precision 	= tb_ltimer_precision(timer);
		switch (precision)
		{
		case TB_LTIMER_PRECISION_MS: 	precision = 1; break;
		case TB_LTIMER_PRECISION_10MS: 	precision = 10; break;
		case TB_LTIMER_PRECISION_100MS: precision = 100; break;
		case TB_LTIMER_PRECISION_S: 	precision = 1000; break;
		case TB_LTIMER_PRECISION_M: 	precision = 60000; break;
		case TB_LTIMER_PRECISION_H: 	precision = 3600000; break;
		default: tb_assert_and_check_return_val(0, 0);
		}

		// trace
		tb_print("limit: %lld, precision: %lu", limit, precision);

		// add task: every
		tb_ltimer_task_run(timer, 1 * precision, tb_ltimer_task_func, "every");
	
		// add task: one
		tb_ltimer_task_run(timer, 10 * precision, tb_ltimer_task_func, "one");
		
		// add task: after
		tb_ltimer_task_run_after(timer, 10 * precision, 5 * precision, tb_ltimer_task_func, "after");

		// wait
		tb_ltimer_loop(timer);

		// exit timer
		tb_ltimer_exit(timer);
	}

	// exit
	tb_exit();
	return 0;
}
