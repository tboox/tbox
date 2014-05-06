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
#include "sql.h"
#include "mysql.h"
#include "../libc/libc.h"
#include "../utils/utils.h"
#include <mysql/mysql.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the mysql result row type
typedef struct __tb_database_mysql_result_row_t
{
	// the iterator
	tb_iterator_t 						itor;

	// the row
	MYSQL_ROW 							row;

	// the lengths
	tb_ulong_t* 						lengths;

	// the col count
	tb_size_t 							count;

	// the col value
	tb_database_sql_value_t 			value;

}tb_database_mysql_result_row_t;

// the mysql result type
typedef struct __tb_database_mysql_result_t
{
	// the iterator
	tb_iterator_t 						itor;

	// the result
	MYSQL_RES* 							result;

	// the fields
	MYSQL_FIELD* 						fields;

	// the row count
	tb_size_t 							count;

	// load all?
	tb_bool_t 							ball;

	// the row
	tb_database_mysql_result_row_t 		row;

}tb_database_mysql_result_t;

// the mysql type
typedef struct __tb_database_mysql_t
{
	// the base
	tb_database_sql_t 					base;

	// the result
	tb_database_mysql_result_t 			result;

	// the database
	MYSQL* 								database;

	// the bind list
	MYSQL_BIND* 						bind_list;

	// the bind maxn
	tb_size_t 							bind_maxn;

}tb_database_mysql_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * library implementation
 */
static tb_handle_t tb_database_mysql_library_init(tb_cpointer_t* ppriv)
{
	// init it
	if (mysql_library_init(0, tb_null, tb_null))
	{
		// trace
		tb_trace_e("init: mysql library failed!");
		return tb_null;
	}

	// ok
	return ppriv;
}
static tb_void_t tb_database_mysql_library_exit(tb_handle_t handle, tb_cpointer_t priv)
{
	// exit it
	mysql_library_end();
}
static tb_handle_t tb_database_mysql_library_load()
{
	return tb_singleton_instance(TB_SINGLETON_TYPE_LIBRARY_MYSQL, tb_database_mysql_library_init, tb_database_mysql_library_exit, tb_null);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * iterator implementation
 */
static tb_size_t tb_mysql_result_row_iterator_size(tb_iterator_t* iterator)
{
	// check
	tb_database_mysql_result_t* result = (tb_database_mysql_result_t*)iterator;
	tb_assert_return_val(result, 0);

	// size
	return result->count;
}
static tb_size_t tb_mysql_result_row_iterator_head(tb_iterator_t* iterator)
{
	// check
	tb_database_mysql_result_t* result = (tb_database_mysql_result_t*)iterator;
	tb_assert_return_val(result, 0);

	// not load all? try fetching it
	if (!result->ball)
	{
		// fetch the row
		result->row.row = mysql_fetch_row(result->result);
		tb_check_return_val(result->row.row, result->count);

		// fetch the lengths
		result->row.lengths = mysql_fetch_lengths(result->result);
		tb_assert_and_check_return_val(result->row.lengths, result->count);
	}

	// head
	return 0;
}
static tb_size_t tb_mysql_result_row_iterator_tail(tb_iterator_t* iterator)
{
	// check
	tb_database_mysql_result_t* result = (tb_database_mysql_result_t*)iterator;
	tb_assert_return_val(result, 0);

	// tail
	return result->count;
}
static tb_size_t tb_mysql_result_row_iterator_prev(tb_iterator_t* iterator, tb_size_t itor)
{
	// check
	tb_database_mysql_result_t* result = (tb_database_mysql_result_t*)iterator;
	tb_assert_return_val(result, 0);
	tb_assert_and_check_return_val(itor && itor <= result->count, result->count);

	// load all? 
	tb_assert_and_check_return_val(result->ball, result->count);

	// prev
	return itor - 1;
}
static tb_size_t tb_mysql_result_row_iterator_next(tb_iterator_t* iterator, tb_size_t itor)
{
	// check
	tb_database_mysql_result_t* result = (tb_database_mysql_result_t*)iterator;
	tb_assert_return_val(result, 0);
	tb_assert_and_check_return_val(itor < result->count, result->count);

	// not load all? try fetching it
	if (!result->ball)
	{
		// fetch the row
		result->row.row = mysql_fetch_row(result->result);
		tb_check_return_val(result->row.row, (tb_size_t)-1);

		// fetch the lengths
		result->row.lengths = mysql_fetch_lengths(result->result);
		tb_assert_and_check_return_val(result->row.lengths, (tb_size_t)-1);
	}

	// next
	return itor + 1;
}
static tb_pointer_t tb_mysql_result_row_iterator_item(tb_iterator_t* iterator, tb_size_t itor)
{
	// check
	tb_database_mysql_result_t* result = (tb_database_mysql_result_t*)iterator;
	tb_assert_and_check_return_val(result && result->result && itor < result->count, tb_null);

	// load all?
	if (result->ball)
	{
		// seek to the row number
		mysql_data_seek(result->result, itor);
		
		// fetch the row
		result->row.row = mysql_fetch_row(result->result);
		tb_assert_and_check_return_val(result->row.row, tb_null);

		// fetch the lengths
		result->row.lengths = mysql_fetch_lengths(result->result);
		tb_assert_and_check_return_val(result->row.lengths, tb_null);
	}

	// the row iterator
	return (tb_pointer_t)&result->row;
}
static tb_size_t tb_mysql_result_col_iterator_size(tb_iterator_t* iterator)
{
	// check
	tb_database_mysql_result_row_t* row = (tb_database_mysql_result_row_t*)iterator;
	tb_assert_and_check_return_val(row, 0);

	// size
	return row->count;
}
static tb_size_t tb_mysql_result_col_iterator_head(tb_iterator_t* iterator)
{
	// check
	tb_database_mysql_result_row_t* row = (tb_database_mysql_result_row_t*)iterator;
	tb_assert_and_check_return_val(row, 0);

	// head
	return 0;
}
static tb_size_t tb_mysql_result_col_iterator_tail(tb_iterator_t* iterator)
{
	// check
	tb_database_mysql_result_row_t* row = (tb_database_mysql_result_row_t*)iterator;
	tb_assert_and_check_return_val(row, 0);

	// tail
	return row->count;
}
static tb_size_t tb_mysql_result_col_iterator_prev(tb_iterator_t* iterator, tb_size_t itor)
{
	// check
	tb_database_mysql_result_row_t* row = (tb_database_mysql_result_row_t*)iterator;
	tb_assert_and_check_return_val(row && itor && itor <= row->count, 0);

	// prev
	return itor - 1;
}
static tb_size_t tb_mysql_result_col_iterator_next(tb_iterator_t* iterator, tb_size_t itor)
{
	// check
	tb_database_mysql_result_row_t* row = (tb_database_mysql_result_row_t*)iterator;
	tb_assert_and_check_return_val(row && itor < row->count, row->count);

	// next
	return itor + 1;
}
static tb_pointer_t tb_mysql_result_col_iterator_item(tb_iterator_t* iterator, tb_size_t itor)
{
	// check
	tb_database_mysql_result_row_t* row = (tb_database_mysql_result_row_t*)iterator;
	tb_assert_and_check_return_val(row && row->row && row->lengths && itor < row->count, tb_null);

	// the mysql
	tb_database_mysql_t* mysql = (tb_database_mysql_t*)iterator->priv;
	tb_assert_and_check_return_val(mysql && mysql->result.fields, tb_null);

	// init value
	tb_database_sql_value_name_set(&row->value, (tb_char_t const*)mysql->result.fields[itor].name);
	tb_database_sql_value_set_text(&row->value, (tb_char_t const*)row->row[itor], (tb_size_t)row->lengths[itor]);

	// the col item
	return (tb_pointer_t)&row->value;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_database_mysql_t* tb_database_mysql_cast(tb_database_sql_t* database)
{
	// check
	tb_assert_and_check_return_val(database && database->type == TB_DATABASE_SQL_TYPE_MYSQL, tb_null);

	// cast
	return (tb_database_mysql_t*)database;
}
static tb_bool_t tb_database_mysql_open(tb_database_sql_t* database)
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
	tb_char_t 			database_sql_name[64] = {0};
	do
	{
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
				if (p < e) tb_strlcpy(database_sql_name, p, tb_min(e - p, sizeof(database_sql_name) - 1));
			}
		}

		// load mysql library
		if (!tb_database_mysql_library_load()) break;

		// init mysql database
		mysql->database = mysql_init(tb_null);
		tb_assert_and_check_break(mysql->database);

		// connect it
		if (!mysql_real_connect(mysql->database, host, username[0]? username : tb_null, password[0]? password : tb_null, database_sql_name[0]? database_sql_name : tb_null, (tb_uint_t)port, tb_null, 0))
		{
			// trace
			tb_trace_e("open: host: %s failed, error: %s", host, mysql_error(mysql->database));
			break;
		}

		// ok
		ok = tb_true;

	} while (0);

	// trace
	tb_trace_d("open: host: %s, port: %lu, username: %s, password: %s, database: %s : %s", host, port, username, password, database_sql_name, ok? "ok" : "no");

	// ok?
	return ok;
}
static tb_void_t tb_database_mysql_clos(tb_database_sql_t* database)
{
	// check
	tb_database_mysql_t* mysql = tb_database_mysql_cast(database);
	tb_assert_and_check_return(mysql);

	// close database
	if (mysql->database) mysql_close(mysql->database);
	mysql->database = tb_null;
}
static tb_void_t tb_database_mysql_exit(tb_database_sql_t* database)
{
	// check
	tb_database_mysql_t* mysql = tb_database_mysql_cast(database);
	tb_assert_and_check_return(mysql);

	// close it first
	tb_database_mysql_clos(database);

	// exit bind list
	if (mysql->bind_list) tb_free(mysql->bind_list);
	mysql->bind_list = tb_null;
	mysql->bind_maxn = 0;

	// exit url
	tb_url_exit(&database->url);

	// exit it
	tb_free(mysql);
}
static tb_bool_t tb_database_mysql_done(tb_database_sql_t* database, tb_char_t const* sql)
{
	// check
	tb_database_mysql_t* mysql = tb_database_mysql_cast(database);
	tb_assert_and_check_return_val(mysql && mysql->database && sql, tb_false);

	// done query
	if (mysql_query(mysql->database, sql))
	{
		// trace
		tb_trace_e("done: sql: %s failed, error: %s", sql, mysql_error(mysql->database));
		return tb_false;
	}

	// trace
	tb_trace_d("done: sql: %s: ok", sql);

	// ok
	return tb_true;
}
static tb_void_t tb_database_mysql_result_exit(tb_database_sql_t* database, tb_iterator_t* result)
{
	// check
	tb_database_mysql_result_t* mysql_result = (tb_database_mysql_result_t*)result;
	tb_assert_and_check_return(mysql_result);

	// exit result
	if (mysql_result->result) mysql_free_result(mysql_result->result);
	mysql_result->result = tb_null;
	mysql_result->fields = tb_null;

	// clear result
	mysql_result->count = 0;
	mysql_result->row.count = 0;
}
static tb_iterator_t* tb_database_mysql_result_load(tb_database_sql_t* database, tb_bool_t ball)
{
	// check
	tb_database_mysql_t* mysql = tb_database_mysql_cast(database);
	tb_assert_and_check_return_val(mysql && mysql->database, tb_null);

	// done
	tb_bool_t ok = tb_false;
	do
	{
		// load result
		mysql->result.result = ball? mysql_store_result(mysql->database) : mysql_use_result(mysql->database);
		tb_check_break(mysql->result.result);

		// load result fields
		mysql->result.fields = mysql_fetch_fields(mysql->result.result);
		tb_assert_and_check_break(mysql->result.fields);

		// save result row count
		mysql->result.count = ball? (tb_size_t)mysql_num_rows(mysql->result.result) : -1;

		// save result col count
		mysql->result.row.count = (tb_size_t)mysql_num_fields(mysql->result.result);

		// load all?
		mysql->result.ball = ball;

		// init mode
		mysql->result.itor.mode = (ball? TB_ITERATOR_MODE_RACCESS : TB_ITERATOR_MODE_FORWARD) | TB_ITERATOR_MODE_READONLY;

		// ok
		ok = tb_true;

	} while (0);

	// ok?
	return ok? (tb_iterator_t*)&mysql->result : tb_null;
}
static tb_handle_t tb_database_mysql_stmt_init(tb_database_sql_t* database, tb_char_t const* sql)
{
	// check
	tb_database_mysql_t* mysql = tb_database_mysql_cast(database);
	tb_assert_and_check_return_val(mysql && mysql->database && sql, tb_null);

	// done
	tb_bool_t 	ok = tb_false;
	MYSQL_STMT* stmt = tb_null;
	do
	{
		// init stmt
		stmt = mysql_stmt_init(mysql->database);
		if (!stmt)
		{
			// trace
			tb_trace_e("stmt: init: %s failed, error: %s", sql, mysql_error(mysql->database));
			break;
		}

		// prepare stmt
		if (mysql_stmt_prepare(stmt, sql, tb_strlen(sql)))
		{
			// trace
			tb_trace_e("stmt: prepare: %s failed, error: %s", sql, mysql_stmt_error(stmt));
			break;
		}

		// ok
		ok = tb_true;

	} while (0);

	// failed?
	if (!ok)
	{
		// exit it
		if (stmt) mysql_stmt_close(stmt);
		stmt = tb_null;
	}

	// ok?
	return (tb_handle_t)stmt;
}
static tb_void_t tb_database_mysql_stmt_exit(tb_database_sql_t* database, tb_handle_t stmt)
{
	// exit it
	if (stmt) mysql_stmt_close((MYSQL_STMT*)stmt);
}
static tb_bool_t tb_database_mysql_stmt_done(tb_database_sql_t* database, tb_handle_t stmt)
{
	// check
	tb_database_mysql_t* mysql = tb_database_mysql_cast(database);
	tb_assert_and_check_return_val(mysql && mysql->database && stmt, tb_false);

	// done
	tb_bool_t ok = tb_false;
	do
	{
		// done stmt
		if (mysql_stmt_execute((MYSQL_STMT*)stmt))
		{
			// trace
			tb_trace_e("stmt: done failed, error: %s", mysql_stmt_error(stmt));
			break;
		}

		// ok
		ok = tb_true;

	} while (0);

	// ok?
	return ok;
}
static tb_bool_t tb_database_mysql_stmt_bind(tb_database_sql_t* database, tb_handle_t stmt, tb_database_sql_value_t const* list, tb_size_t size)
{
	// check
	tb_database_mysql_t* mysql = tb_database_mysql_cast(database);
	tb_assert_and_check_return_val(mysql && mysql->database && stmt && list && size, tb_false);

	// done
	tb_bool_t ok = tb_false;
	do
	{
		// check the param count
		tb_size_t param_count = mysql_stmt_param_count((MYSQL_STMT*)stmt);
		tb_assert_and_check_break(size <= param_count);

		// make bind list
		if (!mysql->bind_list)
		{
			mysql->bind_maxn = size + 16;
			mysql->bind_list = (MYSQL_BIND*)tb_nalloc(mysql->bind_maxn, sizeof(MYSQL_BIND));
		}
		// grow bind list
		else if (size > mysql->bind_maxn)
		{
			mysql->bind_maxn = size + 16;
			mysql->bind_list = (MYSQL_BIND*)tb_ralloc(mysql->bind_list, mysql->bind_maxn * sizeof(MYSQL_BIND));
		}

		// check
		tb_assert_and_check_break(mysql->bind_list && size <= mysql->bind_maxn);

		// clear bind list
		tb_memset(mysql->bind_list, 0, mysql->bind_maxn * sizeof(MYSQL_BIND));

		// init bind list
		tb_size_t i = 0;
		for (i = 0; i < size; i++)
		{
			// the value
			tb_database_sql_value_t const* value = &list[i];
			switch (value->type)
			{
			case TB_DATABASE_SQL_VALUE_TYPE_TEXT:
				mysql->bind_list[i].buffer_type 	= MYSQL_TYPE_STRING;   
				mysql->bind_list[i].buffer 			= (tb_char_t*)tb_database_sql_value_text(value);   
				mysql->bind_list[i].buffer_length 	= tb_database_sql_value_size(value) + 1;   
				break;
			case TB_DATABASE_SQL_VALUE_TYPE_INT64:
				mysql->bind_list[i].buffer_type 	= MYSQL_TYPE_LONGLONG;   
				mysql->bind_list[i].buffer 			= (tb_char_t*)&value->i64;
				break;
			case TB_DATABASE_SQL_VALUE_TYPE_INT32:
				mysql->bind_list[i].buffer_type 	= MYSQL_TYPE_LONG;   
				mysql->bind_list[i].buffer 			= (tb_char_t*)&value->i32;
				break;
			case TB_DATABASE_SQL_VALUE_TYPE_INT16:
				mysql->bind_list[i].buffer_type 	= MYSQL_TYPE_SHORT;   
				mysql->bind_list[i].buffer 			= (tb_char_t*)&value->i16;
				break;
			case TB_DATABASE_SQL_VALUE_TYPE_INT8:
				mysql->bind_list[i].buffer_type 	= MYSQL_TYPE_TINY;   
				mysql->bind_list[i].buffer 			= (tb_char_t*)&value->i8;
				break;
			case TB_DATABASE_SQL_VALUE_TYPE_BLOB32:
				mysql->bind_list[i].buffer_type 	= MYSQL_TYPE_LONG_BLOB;   
				mysql->bind_list[i].buffer 			= (tb_char_t*)tb_database_sql_value_blob(value);   
				mysql->bind_list[i].buffer_length 	= tb_database_sql_value_size(value);   
				mysql->bind_list[i].length 			= &mysql->bind_list[i].buffer_length; 
				break;
			case TB_DATABASE_SQL_VALUE_TYPE_BLOB16:
				mysql->bind_list[i].buffer_type 	= MYSQL_TYPE_BLOB;   
				mysql->bind_list[i].buffer 			= (tb_char_t*)tb_database_sql_value_blob(value);   
				mysql->bind_list[i].buffer_length 	= tb_database_sql_value_size(value);   
				mysql->bind_list[i].length 			= &mysql->bind_list[i].buffer_length; 
				break;
			case TB_DATABASE_SQL_VALUE_TYPE_BLOB8:
				mysql->bind_list[i].buffer_type 	= MYSQL_TYPE_TINY_BLOB;   
				mysql->bind_list[i].buffer 			= (tb_char_t*)tb_database_sql_value_blob(value);   
				mysql->bind_list[i].buffer_length 	= tb_database_sql_value_size(value);   
				mysql->bind_list[i].length 			= &mysql->bind_list[i].buffer_length; 
				break;
#ifdef TB_CONFIG_TYPE_FLOAT
			case TB_DATABASE_SQL_VALUE_TYPE_FLOAT:
				mysql->bind_list[i].buffer_type 	= MYSQL_TYPE_FLOAT;   
				mysql->bind_list[i].buffer 			= (tb_char_t*)&value->f;
				break;
			case TB_DATABASE_SQL_VALUE_TYPE_DOUBLE:
				mysql->bind_list[i].buffer_type 	= MYSQL_TYPE_DOUBLE;   
				mysql->bind_list[i].buffer 			= (tb_char_t*)&value->d;
				break;
#endif
			case TB_DATABASE_SQL_VALUE_TYPE_NULL:
				mysql->bind_list[i].buffer_type 	= MYSQL_TYPE_NULL;   
				break;
			default:
				tb_trace_e("stmt: bind: unknown value type: %lu", value->type);
				break;
			}
		}

		// bind it
		if (mysql_stmt_bind_param((MYSQL_STMT*)stmt, mysql->bind_list))
		{
			// trace
			tb_trace_e("stmt: bind failed, error: %s", mysql_stmt_error(stmt));
			break;
		}

		// ok
		ok = tb_true;

	} while (0);

	// ok?
	return ok;
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
tb_database_sql_t* tb_database_mysql_init(tb_url_t const* url)
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
		mysql->base.type 			= TB_DATABASE_SQL_TYPE_MYSQL;
		mysql->base.open 			= tb_database_mysql_open;
		mysql->base.clos 			= tb_database_mysql_clos;
		mysql->base.exit 			= tb_database_mysql_exit;
		mysql->base.done 			= tb_database_mysql_done;
		mysql->base.result_load 	= tb_database_mysql_result_load;
		mysql->base.result_exit 	= tb_database_mysql_result_exit;
		mysql->base.stmt_init 		= tb_database_mysql_stmt_init;
		mysql->base.stmt_exit 		= tb_database_mysql_stmt_exit;
		mysql->base.stmt_done 		= tb_database_mysql_stmt_done;
		mysql->base.stmt_bind 		= tb_database_mysql_stmt_bind;


		// init result row iterator
		mysql->result.itor.mode 	= TB_ITERATOR_MODE_RACCESS | TB_ITERATOR_MODE_READONLY;
		mysql->result.itor.priv 	= (tb_pointer_t)mysql;
		mysql->result.itor.step 	= 0;
		mysql->result.itor.size 	= tb_mysql_result_row_iterator_size;
		mysql->result.itor.head 	= tb_mysql_result_row_iterator_head;
		mysql->result.itor.tail 	= tb_mysql_result_row_iterator_tail;
		mysql->result.itor.prev 	= tb_mysql_result_row_iterator_prev;
		mysql->result.itor.next 	= tb_mysql_result_row_iterator_next;
		mysql->result.itor.item 	= tb_mysql_result_row_iterator_item;
		mysql->result.itor.copy 	= tb_null;
		mysql->result.itor.comp 	= tb_null;

		// init result col iterator
		mysql->result.row.itor.mode = TB_ITERATOR_MODE_RACCESS | TB_ITERATOR_MODE_READONLY;
		mysql->result.row.itor.priv = (tb_pointer_t)mysql;
		mysql->result.row.itor.step = 0;
		mysql->result.row.itor.size = tb_mysql_result_col_iterator_size;
		mysql->result.row.itor.head = tb_mysql_result_col_iterator_head;
		mysql->result.row.itor.tail = tb_mysql_result_col_iterator_tail;
		mysql->result.row.itor.prev = tb_mysql_result_col_iterator_prev;
		mysql->result.row.itor.next = tb_mysql_result_col_iterator_next;
		mysql->result.row.itor.item = tb_mysql_result_col_iterator_item;
		mysql->result.row.itor.copy = tb_null;
		mysql->result.row.itor.comp = tb_null;

		// init url
		if (!tb_url_init(&mysql->base.url)) break;

		// copy url
		tb_url_copy(&mysql->base.url, url);

		// init state
		mysql->base.state = TB_STATE_OK;

		// ok
		ok = tb_true;

	} while (0);

	// failed?
	if (!ok) 
	{
		// exit database
		if (mysql) tb_database_mysql_exit((tb_database_sql_t*)mysql);
		mysql = tb_null;
	}

	// ok?
	return (tb_database_sql_t*)mysql;
}

