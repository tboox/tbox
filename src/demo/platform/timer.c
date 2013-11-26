/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdio.h>
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * func
 */
static tb_bool_t tb_timer_task_func(tb_pointer_t data)
{
	// trace
	tb_print("task[%s]: %lld", (tb_char_t const*)data, tb_mclock());

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
	tb_handle_t timer = tb_timer_init(3, tb_true);
//	tb_handle_t timer = tb_timer_init(3, tb_false);
	if (timer)
	{
		// add task: every
		tb_timer_task_run(timer, 1000, tb_timer_task_func, "every");
	
		// add task: one
		tb_timer_task_run(timer, 10000, tb_timer_task_func, "one");
		
		// add task: after
		tb_timer_task_run_after(timer, 10000, 5000, tb_timer_task_func, "after");

		// wait
		tb_timer_loop(timer);

		// exit timer
		tb_timer_exit(timer);
	}

	// exit
	tb_exit();
	return 0;
}
