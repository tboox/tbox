/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * test
 */ 
static tb_void_t tb_demo_task_60s_done(tb_cpointer_t priv)
{
	// trace
	tb_trace_i("done: 60s");
	
	// wait some time
	tb_sleep(60);
}
static tb_void_t tb_demo_task_10s_done(tb_cpointer_t priv)
{
	// trace
	tb_trace_i("done: 10s");
	
	// wait some time
	tb_sleep(10);
}
static tb_void_t tb_demo_task_1s_done(tb_cpointer_t priv)
{
	// trace
	tb_trace_i("done: 1s");
	
	// wait some time
	tb_sleep(1);
}
static tb_void_t tb_demo_task_time_done(tb_cpointer_t priv)
{
	// trace
	tb_trace_i("done: %u ms", tb_p2u32(priv));
	
	// wait some time
	tb_msleep(tb_p2u32(priv));
}
static tb_void_t tb_demo_task_time_exit(tb_cpointer_t priv)
{
	// trace
	tb_trace_i("exit: %u ms", tb_p2u32(priv));
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_platform_thread_pool_main(tb_int_t argc, tb_char_t** argv)
{
	// post task: 60s
	tb_thread_pool_task_post(tb_thread_pool_instance(), "60s", tb_demo_task_60s_done, tb_null, tb_null, tb_false);

	// post task: 10s
	tb_thread_pool_task_post(tb_thread_pool_instance(), "10s", tb_demo_task_10s_done, tb_null, tb_null, tb_false);

	// post task: 1s
	tb_thread_pool_task_post(tb_thread_pool_instance(), "1s", tb_demo_task_1s_done, tb_null, tb_null, tb_false);

	getchar();
	// done
	tb_size_t count = tb_rand_uint32(1, 16);
	tb_size_t total = count;
	while (count-- && total < 100)
	{
		// the time
		tb_size_t time = tb_rand_uint32(0, 10000);

		// trace
		tb_trace_i("post: %lu ms", time);
	
		// post task: time ms
		tb_thread_pool_task_post(tb_thread_pool_instance(), tb_null, tb_demo_task_time_done, tb_demo_task_time_exit, (tb_pointer_t)time, tb_false);

		// finished? wait some time and update count
		if (!count) 
		{
			// wait some time
			tb_sleep(5);

			// update count
			count = tb_rand_uint32(1, 16);
			total += count;
		}
	}

	return 0;
}
