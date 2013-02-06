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
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * @author		ruki
 * @file		string.c
 * @ingroup 	object
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "object.h"
#include "../string/string.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the string type
typedef struct __tb_string_t
{
	// the object base
	tb_object_t 		base;

	// the pstring
	tb_pstring_t 		pstr;

}tb_string_t;

/* ///////////////////////////////////////////////////////////////////////
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
	return tb_string_init_cstr(tb_string_cstr(object));
}
static tb_void_t tb_string_exit(tb_object_t* object)
{
	tb_string_t* string = tb_string_cast(object);
	if (string) 
	{
		tb_pstring_exit(&string->pstr);
		tb_free(string);
	}
}
static tb_string_t* tb_string_init_base()
{
	// make
	tb_string_t* string = tb_malloc0(sizeof(tb_string_t));
	tb_assert_and_check_return_val(string, tb_null);

	// init object
	if (!tb_object_init(string, TB_OBJECT_FLAG_NONE, TB_OBJECT_TYPE_STRING)) goto fail;

	// init base
	string->base.copy = tb_string_copy;
	string->base.exit = tb_string_exit;

	// ok
	return string;

	// no
fail:
	if (string) tb_free(string);
	return tb_null;
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_object_t* tb_string_init_cstr(tb_char_t const* cstr)
{
	// make
	tb_string_t* string = tb_string_init_base();
	tb_assert_and_check_return_val(string, tb_null);

	// init pstr
	if (!tb_pstring_init(&string->pstr)) goto fail;

	// copy string
	if (cstr) tb_pstring_cstrcpy(&string->pstr, cstr);

	// ok
	return string;

	// no
fail:
	tb_string_exit(string);
	return tb_null;
}
tb_char_t* tb_string_cstr(tb_object_t* object)
{
	// check
	tb_string_t* string = tb_string_cast(object);
	tb_assert_and_check_return_val(string, tb_null);

	// cstr
	return tb_pstring_cstr(&string->pstr);
}
tb_size_t tb_string_size(tb_object_t* object)
{
	// check
	tb_string_t* string = tb_string_cast(object);
	tb_assert_and_check_return_val(string, 0);

	// size
	return tb_pstring_size(&string->pstr);
}
tb_pstring_t* tb_string_pstr(tb_object_t* object)
{
	// check
	tb_string_t* string = tb_string_cast(object);
	tb_assert_and_check_return_val(string, tb_null);

	// pstr
	return &string->pstr;
}

