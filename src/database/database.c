/*!The Treasure Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author		ruki
 * @file		database.c
 * @defgroup 	database
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME 			"database"
#define TB_TRACE_MODULE_DEBUG 			(1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "database.h"
#include "mysql.h"
#include "sqlite3.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_handle_t tb_database_init(tb_char_t const* url)
{
	// check
	tb_assert_and_check_return_val(url, tb_null);

	// done
	tb_bool_t 		ok = tb_false;
	tb_database_t* 	database = tb_null;
	tb_url_t 		database_url;
	do
	{
		// trace
		tb_trace_d("init: %s: ..", url);

		// init url
		if (!tb_url_init(&database_url)) break;

		// make url
		if (!tb_url_set(&database_url, url)) break;

		// check protocol
		tb_size_t protocol = tb_url_protocol_get(&database_url);
		tb_assert_and_check_break(protocol == TB_URL_PROTOCOL_SQL || protocol == TB_URL_PROTOCOL_FILE);

		// the probe func
		static tb_size_t (*s_probe[])(tb_url_t const*) = 
		{
			tb_null
#ifdef TB_CONFIG_HAVE_MYSQL
		,	tb_database_mysql_probe
#endif
#ifdef TB_CONFIG_HAVE_SQLITE3
		,	tb_database_sqlite3_probe
#endif
		};

		// the init func
		static tb_database_t* (*s_init[])(tb_url_t const*) = 
		{
			tb_null
#ifdef TB_CONFIG_HAVE_MYSQL
		,	tb_database_mysql_init
#endif
#ifdef TB_CONFIG_HAVE_SQLITE3
		,	tb_database_sqlite3_init
#endif
		};

		// probe the database type
		tb_size_t i = 1;
		tb_size_t n = tb_arrayn(s_probe);
		tb_size_t s = 0;
		tb_size_t m = 0;
		for (; i < n; i++)
		{
			if (s_probe[i]) 
			{
				// probe it
				tb_size_t score = s_probe[i](&database_url);
				if (score > s) 
				{
					// save the max score
					s = score;
					m = i;

					// ok?
					if (score == 100) break;
				}
			}
		}
		tb_check_break(m < n && s_init[m]);

		// init it
		database = s_init[m](&database_url);
		tb_assert_and_check_break(database);

		// trace
		tb_trace_d("init: %s: ok", url);

		// ok
		ok = tb_true;

	} while (0);

	// exit url
	tb_url_exit(&database_url);

	// failed?
	if (!ok)
	{
		// trace
		tb_trace_d("init: %s: no", url);

		// exit database
		tb_database_exit((tb_handle_t)database);
		database = tb_null;
	}

	// ok?
	return database;
}
tb_void_t tb_database_exit(tb_handle_t handle)
{
	// check
	tb_database_t* database = (tb_database_t*)handle;
	tb_assert_and_check_return(database);
		
	// trace
	tb_trace_d("exit: ..");

	// exit it
	if (database->exit) database->exit(database);

	// trace
	tb_trace_d("exit: ok");
}
tb_size_t tb_database_type(tb_handle_t handle)
{
	// check
	tb_database_t* database = (tb_database_t*)handle;
	tb_assert_and_check_return_val(database, TB_DATABASE_TYPE_NONE);

	// the database type
	return database->type;
}
tb_bool_t tb_database_open(tb_handle_t handle)
{
	// check
	tb_database_t* database = (tb_database_t*)handle;
	tb_assert_and_check_return_val(database, tb_false);
	
	// open it
	return database->open(database);
}
tb_void_t tb_database_clos(tb_handle_t handle)
{
	// check
	tb_database_t* database = (tb_database_t*)handle;
	tb_assert_and_check_return(database);
	
	// clos it
	if (database->clos) database->clos(database);
}


