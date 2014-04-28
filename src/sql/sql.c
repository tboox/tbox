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
 * @file		sql.c
 * @defgroup 	sql
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME 			"sql"
#define TB_TRACE_MODULE_DEBUG 			(1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "sql.h"
#include "mysql.h"
#include "sqlite3.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_handle_t tb_sql_init(tb_char_t const* url)
{
	// check
	tb_assert_and_check_return_val(url, tb_null);

	// done
	tb_bool_t 	ok = tb_false;
	tb_sql_t* 	sql = tb_null;
	tb_url_t 	sql_url;
	do
	{
		// trace
		tb_trace_d("init: %s: ..", url);

		// init url
		if (!tb_url_init(&sql_url)) break;

		// make url
		if (!tb_url_set(&sql_url, url)) break;

		// check protocol
		tb_size_t protocol = tb_url_protocol_get(&sql_url);
		tb_assert_and_check_break(protocol == TB_URL_PROTOCOL_SQL || protocol == TB_URL_PROTOCOL_FILE);

		// the probe func
		static tb_size_t (*s_probe[])(tb_url_t const*) = 
		{
			tb_null
#ifdef TB_CONFIG_HAVE_MYSQL
		,	tb_sql_mysql_probe
#endif
#ifdef TB_CONFIG_HAVE_SQLITE3
		,	tb_sql_sqlite3_probe
#endif
		};

		// the init func
		static tb_sql_t* (*s_init[])(tb_url_t const*) = 
		{
			tb_null
#ifdef TB_CONFIG_HAVE_MYSQL
		,	tb_sql_mysql_init
#endif
#ifdef TB_CONFIG_HAVE_SQLITE3
		,	tb_sql_sqlite3_init
#endif
		};

		// probe the sql type
		tb_size_t i = 1;
		tb_size_t n = tb_arrayn(s_probe);
		tb_size_t s = 0;
		tb_size_t m = 0;
		for (; i < n; i++)
		{
			if (s_probe[i]) 
			{
				// probe it
				tb_size_t score = s_probe[i](&sql_url);
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
		sql = s_init[m](&sql_url);
		tb_assert_and_check_break(sql);

		// trace
		tb_trace_d("init: %s: ok", url);

		// ok
		ok = tb_true;

	} while (0);

	// exit url
	tb_url_exit(&sql_url);

	// failed?
	if (!ok)
	{
		// trace
		tb_trace_d("init: %s: no", url);

		// exit sql
		tb_sql_exit((tb_handle_t)sql);
		sql = tb_null;
	}

	// ok?
	return sql;
}
tb_void_t tb_sql_exit(tb_handle_t handle)
{
	// check
	tb_sql_t* sql = (tb_sql_t*)handle;
	tb_assert_and_check_return(sql);
		
	// trace
	tb_trace_d("exit: ..");

	// exit it
	if (sql->exit) sql->exit(sql);

	// trace
	tb_trace_d("exit: ok");
}
tb_size_t tb_sql_type(tb_handle_t handle)
{
	// check
	tb_sql_t* sql = (tb_sql_t*)handle;
	tb_assert_and_check_return_val(sql, TB_SQL_DATABASE_TYPE_NONE);

	// the database type
	return sql->type;
}
tb_bool_t tb_sql_open(tb_handle_t handle)
{
	// check
	tb_sql_t* sql = (tb_sql_t*)handle;
	tb_assert_and_check_return_val(sql, tb_false);
	
	// open it
	return sql->open(sql);
}
tb_void_t tb_sql_clos(tb_handle_t handle)
{
	// check
	tb_sql_t* sql = (tb_sql_t*)handle;
	tb_assert_and_check_return(sql);
	
	// clos it
	if (sql->clos) sql->clos(sql);
}


