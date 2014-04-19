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
 * @file		string.c
 * @ingroup 	object
 *
 */
 
/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME 		"object_string"
#define TB_TRACE_MODULE_DEBUG 		(0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "object.h"
#include "../string/string.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the scache string size
#define TB_STRING_STRING_CACHE_SIZE 		(64)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the string type
typedef struct __tb_string_t
{
	// the object base
	tb_object_t 		base;

	// the pstring
	tb_scoped_string_t 		pstr;

	// the cache data
	tb_char_t const* 	cdata;

	// the cache size
	tb_size_t 			csize;

}tb_string_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_string_t* tb_string_cast(tb_object_t* object)
{
	// check
	tb_assert_and_check_return_val(object && object->type == TB_OBJECT_TYPE_STRING, tb_null);

	// cast
	return (tb_string_t*)object;
}
static tb_object_t* tb_string_copy(tb_object_t* object)
{
	return tb_string_init_from_cstr(tb_string_cstr(object));
}
static tb_void_t tb_string_exit(tb_object_t* object)
{
	tb_string_t* string = tb_string_cast(object);
	if (string) 
	{
		if (string->cdata) tb_string_cache_del(string->cdata);
		tb_scoped_string_exit(&string->pstr);
		tb_object_pool_del(tb_object_pool_instance(), object);
	}
}
static tb_void_t tb_string_cler(tb_object_t* object)
{
	tb_string_t* string = tb_string_cast(object);
	if (string) 
	{
		if (string->cdata) tb_string_cache_del(string->cdata);
		string->cdata = tb_null;
		string->csize = 0;
		tb_scoped_string_clear(&string->pstr);
	}
}
static tb_string_t* tb_string_init_base()
{
	// make
	tb_string_t* string = (tb_string_t*)tb_object_pool_get(tb_object_pool_instance(), sizeof(tb_string_t), TB_OBJECT_FLAG_NONE, TB_OBJECT_TYPE_STRING);
	tb_assert_and_check_return_val(string, tb_null);

	// init base
	string->base.copy = tb_string_copy;
	string->base.cler = tb_string_cler;
	string->base.exit = tb_string_exit;

	// ok
	return string;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_object_t* tb_string_init_from_cstr(tb_char_t const* cstr)
{
	// make
	tb_string_t* string = tb_string_init_base();
	tb_assert_and_check_return_val(string, tb_null);

	// init pstr
	if (!tb_scoped_string_init(&string->pstr)) goto fail;

	// copy string
	if (cstr) 
	{
		tb_size_t size = tb_strlen(cstr);
		if (size)
		{
			if (size < TB_STRING_STRING_CACHE_SIZE) 
			{
				// put string to scache
				string->cdata = tb_string_cache_put(cstr);
				tb_assert_and_check_goto(string->cdata, fail);

				// the string size
				string->csize = size;
			}
			else tb_scoped_string_cstrncpy(&string->pstr, cstr, size);
		}
	}

	// ok
	return (tb_object_t*)string;

	// no
fail:
	tb_string_exit((tb_object_t*)string);
	return tb_null;
}
tb_object_t* tb_string_init_from_pstr(tb_scoped_string_t* pstr)
{
	// make
	tb_string_t* string = tb_string_init_base();
	tb_assert_and_check_return_val(string, tb_null);

	// init pstr
	if (!tb_scoped_string_init(&string->pstr)) goto fail;

	// copy string
	if (pstr) 
	{
		tb_size_t size = tb_scoped_string_size(&string->pstr);
		if (size < TB_STRING_STRING_CACHE_SIZE) 
		{
			// put string to scache
			string->cdata = tb_string_cache_put(tb_scoped_string_cstr(pstr));
			tb_assert_and_check_goto(string->cdata, fail);

			// the string size
			string->csize = size;
		}
		else tb_scoped_string_strcpy(&string->pstr, pstr);
	}

	// ok
	return (tb_object_t*)string;

	// no
fail:
	tb_string_exit((tb_object_t*)string);
	return tb_null;
}
tb_char_t const* tb_string_cstr(tb_object_t* object)
{
	// check
	tb_string_t* string = tb_string_cast(object);
	tb_assert_and_check_return_val(string, tb_null);

	// cstr
	return string->cdata? string->cdata : tb_scoped_string_cstr(&string->pstr);
}
tb_size_t tb_string_cstr_set(tb_object_t* object, tb_char_t const* cstr)
{
	// check
	tb_string_t* string = tb_string_cast(object);
	tb_assert_and_check_return_val(string && cstr, 0);

	tb_size_t size = 0;
	if (cstr) 
	{
		size = tb_strlen(cstr);
		if (size)
		{
			if (size < TB_STRING_STRING_CACHE_SIZE) 
			{
				// put string to scache
				tb_char_t const* cdata = tb_string_cache_put(cstr);
				if (cdata)
				{
					// save string
					if (string->cdata) tb_string_cache_del(string->cdata);
					string->cdata = cdata;
					string->csize = size;
				}
			}
			else 
			{
				// copy string
				tb_scoped_string_cstrncpy(&string->pstr, cstr, size);
				size = tb_scoped_string_size(&string->pstr);

				// remove string from scache
				if (string->cdata) tb_string_cache_del(string->cdata);
				string->cdata = tb_null;
				string->csize = 0;
			}
		}
		else
		{
			// clear string
			tb_scoped_string_clear(&string->pstr);

			// remove string from scache
			if (string->cdata) tb_string_cache_del(string->cdata);
			string->cdata = tb_null;
			string->csize = 0;
		}
	}

	// ok?
	return size;
}
tb_size_t tb_string_size(tb_object_t* object)
{
	// check
	tb_string_t* string = tb_string_cast(object);
	tb_assert_and_check_return_val(string, 0);

	// size
	return string->cdata? string->csize : tb_scoped_string_size(&string->pstr);
}

