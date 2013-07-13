/* ///////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "tbox.h"
#include <stdio.h>
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * test
 */
static tb_void_t tb_tstore_free(tb_tstore_data_t* data)
{
	if (data) tb_free(data);
}
static tb_cpointer_t tb_tstore_test(tb_cpointer_t data)
{
	// self
	tb_handle_t self = tb_thread_self();
	
	// trace
	tb_print("thread[%p]: init", self);

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
			tb_print("getp: %lu", item->type);
		}
		tb_sleep(1);
	}

	// trace
	tb_print("thread[%p]: exit", self);
	tb_thread_return(tb_null);
	return tb_null;
}
/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// init
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// done
	tb_thread_init(tb_null, tb_tstore_test, tb_null, 0);
	tb_thread_init(tb_null, tb_tstore_test, tb_null, 0);
	tb_thread_init(tb_null, tb_tstore_test, tb_null, 0);
	tb_thread_init(tb_null, tb_tstore_test, tb_null, 0);

	// wait
	getchar();

	// exit
	tb_exit();
	return 0;
}
