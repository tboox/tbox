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
 * @file		sqlite3.c
 * @ingroup 	sql
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME 			"sqlite3"
#define TB_TRACE_MODULE_DEBUG 			(1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "sqlite3.h"
#include "../libc/libc.h"
#include "../stream/stream.h"
#include <sqlite3.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the sqlite3 type
typedef struct __tb_sql_sqlite3_t
{
	// the base
	tb_sql_t 			base;

	// the database
	sqlite3* 			database;

}tb_sql_sqlite3_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_sql_sqlite3_t* tb_sql_sqlite3_cast(tb_sql_t* sql)
{
	// check
	tb_assert_and_check_return_val(sql && sql->type == TB_SQL_DATABASE_TYPE_SQLITE3, tb_null);

	// cast
	return (tb_sql_sqlite3_t*)sql;
}
static tb_bool_t tb_sql_sqlite3_open(tb_sql_t* sql)
{
	// check
	tb_sql_sqlite3_t* sql3 = tb_sql_sqlite3_cast(sql);
	tb_assert_and_check_return_val(sql3, tb_false);

	// done
	tb_bool_t 			ok = tb_false;
	tb_char_t const* 	path = tb_null;
	do
	{
		// opened?
		tb_check_return_val(!sql->bopened, tb_true);

		// the database path
		path = tb_url_path_get(&sql->url);
		tb_assert_and_check_break(path);

		// open database
		if (SQLITE_OK != sqlite3_open(path, &sql3->database) || !sql3->database) 
		{
			// trace
			if (sql3->database) tb_trace_e("open: %s failed, error: %s", path, sqlite3_errmsg(sql3->database));
			break;
		}

		// opened
		sql->bopened = tb_true;

		// ok
		ok = tb_true;

	} while (0);

	// trace
	tb_trace_d("open: %s: %s", path, ok? "ok" : "no");

	// ok?
	return ok;
}
static tb_void_t tb_sql_sqlite3_clos(tb_sql_t* sql)
{
	// check
	tb_sql_sqlite3_t* sql3 = tb_sql_sqlite3_cast(sql);
	tb_assert_and_check_return(sql3);
		
	// opened?
	tb_check_return(sql->bopened);

	// close database
	if (sql3->database) sqlite3_close(sql3->database);
	sql3->database = tb_null;

	// closed
	sql->bopened = tb_false;
}
static tb_void_t tb_sql_sqlite3_exit(tb_sql_t* sql)
{
	// check
	tb_sql_sqlite3_t* sql3 = tb_sql_sqlite3_cast(sql);
	tb_assert_and_check_return(sql3);

	// close it first
	tb_sql_sqlite3_clos(sql);

	// exit url
	tb_url_exit(&sql->url);

	// exit it
	tb_free(sql3);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_size_t tb_sql_sqlite3_probe(tb_url_t const* url)
{
	// check
	tb_assert_and_check_return_val(url, 0);

	// done
	tb_size_t 			score = 0;
	tb_basic_stream_t* 	stream = tb_null;
	do
	{
		// the url arguments
		tb_char_t const* args = tb_url_args_get(url);
		if (args)
		{
			// find the database type
			tb_char_t const* ptype = tb_stristr(args, "type=");
			if (ptype && !tb_strnicmp(ptype + 5, "sqlite3", 7))
			{
				// ok
				score = 100;
				break;
			}
		}

		// the database path
		tb_char_t const* path = tb_url_path_get(url);
		tb_assert_and_check_break(path);

		// init stream
		stream = tb_basic_stream_init_from_url(path);
		tb_assert_and_check_break(stream);

		// open stream
		if (!tb_basic_stream_open(stream)) break;

		// read head
		tb_char_t head[16] = {0};
		if (!tb_basic_stream_bread(stream, (tb_byte_t*)head, sizeof(head))) break;

		// is sqlite3?
		if (!tb_stricmp(head, "SQLite format 3")) score = 100;

	} while (0);

	// exit stream
	if (stream) tb_basic_stream_exit(stream);
	stream = tb_null;

	// trace
	tb_trace_d("probe: %s, score: %lu", tb_url_get((tb_url_t*)url), score);

	// ok?
	return score;
}
tb_sql_t* tb_sql_sqlite3_init(tb_url_t const* url)
{
	// check
	tb_assert_and_check_return_val(url, tb_null);

	// done
	tb_bool_t 			ok = tb_false;
	tb_sql_sqlite3_t* 	sql = tb_null;
	do
	{
		// make sql
		sql = tb_malloc0(sizeof(tb_sql_sqlite3_t));
		tb_assert_and_check_break(sql);

		// init sql
		sql->base.type = TB_SQL_DATABASE_TYPE_SQLITE3;
		sql->base.open = tb_sql_sqlite3_open;
		sql->base.clos = tb_sql_sqlite3_clos;
		sql->base.exit = tb_sql_sqlite3_exit;

		// init url
		if (!tb_url_init(&sql->base.url)) break;

		// copy url
		tb_url_copy(&sql->base.url, url);

		// ok
		ok = tb_true;

	} while (0);

	// failed?
	if (!ok) 
	{
		// exit sql
		if (sql) tb_sql_sqlite3_exit((tb_sql_t*)sql);
		sql = tb_null;
	}

	// ok?
	return (tb_sql_t*)sql;
}

