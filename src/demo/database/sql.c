/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * test
 */
static tb_void_t tb_demo_database_sql_test_done(tb_handle_t database, tb_char_t const* sql)
{
	// check
	tb_assert_and_check_return(database && sql);

	// done sql
	if (!tb_database_sql_done(database, sql))
	{
		// trace
		tb_trace_e("done sql failed, error: %s", tb_state_cstr(tb_database_sql_state(database)));
		return ;
	}

	// load result
	tb_iterator_t* result = tb_database_sql_result_load(database, tb_true);
//	tb_iterator_t* result = tb_database_sql_result_load(database, tb_false);
	if (result)
	{
		// trace
		tb_trace_i("==============================================================================");
		tb_trace_i("row: size: %lu", tb_iterator_size(result));

		// walk result
		tb_for_all_if (tb_iterator_t*, row, result, row)
		{
			// trace
			tb_tracef_i("[row: %lu, col: size: %lu]: ", row_itor, tb_iterator_size(row));

			// walk items
			tb_for_all_if (tb_database_sql_value_t*, value, row, value)
			{
				// trace
				tb_tracet_i("[%s:%s] ", tb_database_sql_value_name(value), tb_database_sql_value_text(value));
			}

			// trace
			tb_tracet_i(__tb_newline__);
		}

		// exit result
		tb_database_sql_result_exit(database, result);
	}
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_database_sql_main(tb_int_t argc, tb_char_t** argv)
{
	// init database
	tb_handle_t database = tb_database_sql_init(argv[1]);
	if (database)
	{
		// open database
		if (tb_database_sql_open(database))
		{
			// done tests 
			tb_demo_database_sql_test_done(database, "drop table if exists table1");
			tb_demo_database_sql_test_done(database, "create table table1(id int, name text, number int)");
			tb_demo_database_sql_test_done(database, "insert into table1 values(1, 'name1', 52642)");
			tb_demo_database_sql_test_done(database, "insert into table1 values(2, 'name2', 57127)");
			tb_demo_database_sql_test_done(database, "insert into table1 values(3, 'name3', 9000)");
			tb_demo_database_sql_test_done(database, "insert into table1 values(4, 'name4', 29000)");
			tb_demo_database_sql_test_done(database, "insert into table1 values(5, 'name5', 350000)");
			tb_demo_database_sql_test_done(database, "insert into table1 values(6, 'name6', 21000)");
			tb_demo_database_sql_test_done(database, "insert into table1 values(7, 'name7', 21600)");
			tb_demo_database_sql_test_done(database, "select * from table1");
		}

		// exit database
		tb_database_sql_exit(database);
	}
	return 0;
}
