/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * test
 */
static tb_void_t tb_tstore_free(tb_tstore_data_t* data)
{
	if (data) tb_free(data);
}
static tb_pointer_t tb_tstore_test(tb_cpointer_t data)
{
	// self
	tb_size_t self = tb_thread_self();
	
	// trace
	tb_trace_i("thread[%lu]: init", self);

	// done
	tb_tstore_data_t* item = tb_null;
	while (1)
	{
		if (!(item = tb_tstore_getp()))
		{
			item = tb_malloc0(sizeof(tb_tstore_data_t));
			if (item)
			{
				item->type = (tb_size_t)self;
				item->free = tb_tstore_free;
				tb_tstore_setp(item);
			}
		}
		else 
		{
			tb_trace_i("getp: %lu", item->type);
		}
		tb_sleep(1);
	}

	// trace
	tb_trace_i("thread[%lu]: exit", self);
	tb_thread_return(tb_null);
	return tb_null;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_platform_tstore_main(tb_int_t argc, tb_char_t** argv)
{
	// done
	tb_thread_init(tb_null, tb_tstore_test, tb_null, 0);
	tb_thread_init(tb_null, tb_tstore_test, tb_null, 0);
	tb_thread_init(tb_null, tb_tstore_test, tb_null, 0);
	tb_thread_init(tb_null, tb_tstore_test, tb_null, 0);

	// wait
	getchar();

	return 0;
}
