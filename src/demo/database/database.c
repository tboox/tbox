/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * test
 */
static tb_void_t tb_demo_database_test_select_1(tb_handle_t database)
{
	// check
	tb_assert_and_check_return(database);

	// done sql
	if (!tb_database_done(database, "select * from table"))
	{
		// trace
		tb_trace_e("done sql failed, error: %s", tb_state_cstr(tb_database_state(database)));
		return ;
	}

	// load results
	tb_iterator_t* results = tb_database_results_load(database);
	if (results)
	{
		// walk results
		tb_for_all_if (tb_iterator_t*, row, results, row)
		{
			// walk items
			tb_for_all_if (tb_database_results_item_t*, item, row, item)
			{
				// trace
				tb_trace_i("name: %s, data: %s, size: %lu, at: %lux%lu", item->name, item->data, item->size, row_itor, item_itor);
			}
		}

		// exit results
		tb_database_results_exit(database, results);
	}
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_database_main(tb_int_t argc, tb_char_t** argv)
{
	// init database
	tb_handle_t database = tb_database_init(argv[1]);
	if (database)
	{
		// open database
		if (tb_database_open(database))
		{
			// done tests 
			tb_demo_database_test_select_1(database);
		}

		// exit database
		tb_database_exit(database);
	}
	return 0;
}
