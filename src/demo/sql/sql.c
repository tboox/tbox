/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_sql_main(tb_int_t argc, tb_char_t** argv)
{
	// init sql
	tb_handle_t sql = tb_sql_init(argv[1]);
	if (sql)
	{
		// open sql
		if (tb_sql_open(sql))
		{

		}

		// exit sql
		tb_sql_exit(sql);
	}
	return 0;
}
