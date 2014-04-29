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
 * @ingroup 	database
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
typedef struct __tb_database_sqlite3_t
{
	// the base
	tb_database_t 			base;

	// the database
	sqlite3* 				database;

}tb_database_sqlite3_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_database_sqlite3_t* tb_database_sqlite3_cast(tb_database_t* database)
{
	// check
	tb_assert_and_check_return_val(database && database->type == TB_DATABASE_TYPE_SQLITE3, tb_null);

	// cast
	return (tb_database_sqlite3_t*)database;
}
static tb_bool_t tb_database_sqlite3_open(tb_database_t* database)
{
	// check
	tb_database_sqlite3_t* sqlite3_db = tb_database_sqlite3_cast(database);
	tb_assert_and_check_return_val(sqlite3_db, tb_false);

	// done
	tb_bool_t 			ok = tb_false;
	tb_char_t const* 	path = tb_null;
	do
	{
		// opened?
		tb_check_return_val(!database->bopened, tb_true);

		// the database path
		path = tb_url_path_get(&database->url);
		tb_assert_and_check_break(path);

		// open database
		if (SQLITE_OK != sqlite3_open(path, &sqlite3_db->database) || !sqlite3_db->database) 
		{
			// trace
			if (sqlite3_db->database) tb_trace_e("open: %s failed, error: %s", path, sqlite3_errmsg(sqlite3_db->database));
			break;
		}

		// opened
		database->bopened = tb_true;

		// ok
		ok = tb_true;

	} while (0);

	// trace
	tb_trace_d("open: %s: %s", path, ok? "ok" : "no");

	// ok?
	return ok;
}
static tb_void_t tb_database_sqlite3_clos(tb_database_t* database)
{
	// check
	tb_database_sqlite3_t* sqlite3_db = tb_database_sqlite3_cast(database);
	tb_assert_and_check_return(sqlite3_db);
		
	// opened?
	tb_check_return(database->bopened);

	// close database
	if (sqlite3_db->database) sqlite3_close(sqlite3_db->database);
	sqlite3_db->database = tb_null;

	// closed
	database->bopened = tb_false;
}
static tb_void_t tb_database_sqlite3_exit(tb_database_t* database)
{
	// check
	tb_database_sqlite3_t* sqlite3_db = tb_database_sqlite3_cast(database);
	tb_assert_and_check_return(sqlite3_db);

	// close it first
	tb_database_sqlite3_clos(database);

	// exit url
	tb_url_exit(&database->url);

	// exit it
	tb_free(sqlite3_db);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_size_t tb_database_sqlite3_probe(tb_url_t const* url)
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
tb_database_t* tb_database_sqlite3_init(tb_url_t const* url)
{
	// check
	tb_assert_and_check_return_val(url, tb_null);

	// done
	tb_bool_t 				ok = tb_false;
	tb_database_sqlite3_t* 	sqlite3_db = tb_null;
	do
	{
		// make database
		sqlite3_db = tb_malloc0(sizeof(tb_database_sqlite3_t));
		tb_assert_and_check_break(sqlite3_db);

		// init database
		sqlite3_db->base.type = TB_DATABASE_TYPE_SQLITE3;
		sqlite3_db->base.open = tb_database_sqlite3_open;
		sqlite3_db->base.clos = tb_database_sqlite3_clos;
		sqlite3_db->base.exit = tb_database_sqlite3_exit;

		// init url
		if (!tb_url_init(&sqlite3_db->base.url)) break;

		// copy url
		tb_url_copy(&sqlite3_db->base.url, url);

		// ok
		ok = tb_true;

	} while (0);

	// failed?
	if (!ok) 
	{
		// exit database
		if (sqlite3_db) tb_database_sqlite3_exit((tb_database_t*)sqlite3_db);
		sqlite3_db = tb_null;
	}

	// ok?
	return (tb_database_t*)sqlite3_db;
}

