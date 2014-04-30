/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * test
 */
static tb_void_t tb_demo_database_test_done(tb_handle_t database, tb_char_t const* sql)
{
	// check
	tb_assert_and_check_return(database && sql);

	// done sql
	if (!tb_database_done(database, sql))
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
			tb_demo_database_test_done(database, "drop table if exists table1");
			tb_demo_database_test_done(database, "create table table1(id int, name text, number int)");
			tb_demo_database_test_done(database, "insert into table1 values(1,'name1',52642)");
			tb_demo_database_test_done(database, "insert into table1 values(2,'name2',57127)");
			tb_demo_database_test_done(database, "insert into table1 values(3,'name3',9000)");
			tb_demo_database_test_done(database, "insert into table1 values(4,'name4',29000)");
			tb_demo_database_test_done(database, "insert into table1 values(5,'name5',350000)");
			tb_demo_database_test_done(database, "insert into table1 values(6,'name6',21000)");
			tb_demo_database_test_done(database, "insert into table1 values(8,'name7',21600)");
		}

		// exit database
		tb_database_exit(database);
	}
	return 0;
}
