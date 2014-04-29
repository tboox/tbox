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
 * @file		mysql.c
 * @ingroup 	database
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME 			"mysql"
#define TB_TRACE_MODULE_DEBUG 			(1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "mysql.h"
#include "../libc/libc.h"
#include <mysql/mysql.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the mysql type
typedef struct __tb_database_mysql_t
{
	// the base
	tb_database_t 			base;

	// the database
	MYSQL* 					database;

}tb_database_mysql_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_database_mysql_t* tb_database_mysql_cast(tb_database_t* database)
{
	// check
	tb_assert_and_check_return_val(database && database->type == TB_DATABASE_TYPE_MYSQL, tb_null);

	// cast
	return (tb_database_mysql_t*)database;
}
static tb_bool_t tb_database_mysql_open(tb_database_t* database)
{
	// check
	tb_database_mysql_t* mysql = tb_database_mysql_cast(database);
	tb_assert_and_check_return_val(mysql, tb_false);

	// done
	tb_bool_t 			ok = tb_false;
	tb_char_t const* 	host = tb_null;
	tb_size_t 			port = 0;
	tb_char_t 			username[64] = {0};
	tb_char_t 			password[64] = {0};
	tb_char_t 			database_name[64] = {0};
	do
	{
		// opened?
		tb_check_return_val(!database->bopened, tb_true);

		// the database host
		host = tb_url_host_get(&database->url);
		tb_assert_and_check_break(host);

		// the database port
		port = tb_url_port_get(&database->url);

		// the database args
		tb_char_t const* args = tb_url_args_get(&database->url);
		if (args)
		{
			// the args size
			tb_size_t argn = tb_strlen(args);

			// the database username
			tb_char_t const* p = tb_stristr(args, "username=");
			if (p)
			{
				// skip to value
				p += 9;

				// the value end
				tb_char_t const* e = tb_strchr(p, '&');
				if (!e) e = args + argn;

				// save username
				if (p < e) tb_strlcpy(username, p, tb_min(e - p, sizeof(username) - 1));
			}
	
			// the database password
			p = tb_stristr(args, "password=");
			if (p)
			{
				// skip to value
				p += 9;

				// the value end
				tb_char_t const* e = tb_strchr(p, '&');
				if (!e) e = args + argn;

				// save password
				if (p < e) tb_strlcpy(password, p, tb_min(e - p, sizeof(password) - 1));
			}
	
			// the database name
			p = tb_stristr(args, "database=");
			if (p)
			{
				// skip to value
				p += 9;

				// the value end
				tb_char_t const* e = tb_strchr(p, '&');
				if (!e) e = args + argn;

				// save database name
				if (p < e) tb_strlcpy(database_name, p, tb_min(e - p, sizeof(database_name) - 1));
			}
		}

		// init mysql database
		mysql->database = mysql_init(tb_null);
		tb_assert_and_check_break(mysql->database);

		// connect it
		if (!mysql_real_connect(mysql->database, host, username[0]? username : tb_null, password[0]? password : tb_null, database_name[0]? database_name : tb_null, (tb_uint_t)port, tb_null, 0)) break;

		// opened
		database->bopened = tb_true;

		// ok
		ok = tb_true;

	} while (0);

	// trace
	tb_trace_d("open: host: %s, port: %lu, username: %s, password: %s, database: %s : %s", host, port, username, password, database_name, ok? "ok" : "no");

	// ok?
	return ok;
}
static tb_void_t tb_database_mysql_clos(tb_database_t* database)
{
	// check
	tb_database_mysql_t* mysql = tb_database_mysql_cast(database);
	tb_assert_and_check_return(mysql);
		
	// opened?
	tb_check_return(database->bopened);

	// close database
	if (mysql->database) mysql_close(mysql->database);
	mysql->database = tb_null;

	// closed
	database->bopened = tb_false;
}
static tb_void_t tb_database_mysql_exit(tb_database_t* database)
{
	// check
	tb_database_mysql_t* mysql = tb_database_mysql_cast(database);
	tb_assert_and_check_return(mysql);

	// close it first
	tb_database_mysql_clos(database);

	// exit url
	tb_url_exit(&database->url);

	// exit it
	tb_free(mysql);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_size_t tb_database_mysql_probe(tb_url_t const* url)
{
	// check
	tb_assert_and_check_return_val(url, 0);

	// done
	tb_size_t score = 0;
	do
	{
		// the url arguments
		tb_char_t const* args = tb_url_args_get(url);
		if (args)
		{
			// find the database type
			tb_char_t const* ptype = tb_stristr(args, "type=");
			if (ptype && !tb_strnicmp(ptype + 5, "mysql", 5))
			{
				// ok
				score = 100;
				break;
			}
		}

		// the database port, the default port: 3306 
		if (tb_url_port_get(url) == 3306) score += 20;

		// is sql url? 
		if (tb_url_protocol_get(url) == TB_URL_PROTOCOL_SQL) 
			score += 5;

	} while (0);

	// trace
	tb_trace_d("probe: %s, score: %lu", tb_url_get((tb_url_t*)url), score);

	// ok?
	return score;
}
tb_database_t* tb_database_mysql_init(tb_url_t const* url)
{
	// check
	tb_assert_and_check_return_val(url, tb_null);

	// done
	tb_bool_t 				ok = tb_false;
	tb_database_mysql_t* 	mysql = tb_null;
	do
	{
		// make database
		mysql = tb_malloc0(sizeof(tb_database_mysql_t));
		tb_assert_and_check_break(mysql);

		// init database
		mysql->base.type = TB_DATABASE_TYPE_MYSQL;
		mysql->base.open = tb_database_mysql_open;
		mysql->base.clos = tb_database_mysql_clos;
		mysql->base.exit = tb_database_mysql_exit;

		// init url
		if (!tb_url_init(&mysql->base.url)) break;

		// copy url
		tb_url_copy(&mysql->base.url, url);

		// ok
		ok = tb_true;

	} while (0);

	// failed?
	if (!ok) 
	{
		// exit database
		if (mysql) tb_database_mysql_exit((tb_database_t*)mysql);
		mysql = tb_null;
	}

	// ok?
	return (tb_database_t*)mysql;
}

