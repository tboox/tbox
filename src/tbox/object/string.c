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
 * Copyright (C) 2009 - 2017, ruki All rights reserved.
 *
 * @author      ruki
 * @file        string.c
 * @ingroup     object
 *
 */
 
/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME        "object_string"
#define TB_TRACE_MODULE_DEBUG       (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "object.h"
#include "../string/string.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the scache string size
#define TB_OBJECT_STRING_CACHE_SIZE         (64)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the string type
typedef struct __tb_oc_string_t
{
    // the object base
    tb_oc_object_t      base;

    // the string
    tb_string_t         str;

}tb_oc_string_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_oc_string_t* tb_oc_string_cast(tb_oc_object_ref_t object)
{
    // check
    tb_assert_and_check_return_val(object && object->type == TB_OBJECT_TYPE_STRING, tb_null);

    // cast
    return (tb_oc_string_t*)object;
}
static tb_oc_object_ref_t tb_oc_string_copy(tb_oc_object_ref_t object)
{
    return tb_oc_string_init_from_cstr(tb_oc_string_cstr(object));
}
static tb_void_t tb_oc_string_exit(tb_oc_object_ref_t object)
{
    tb_oc_string_t* string = tb_oc_string_cast(object);
    if (string) 
    {
        // exit the string
        tb_string_exit(&string->str);

        // exit the object
        tb_free(object);
    }
}
static tb_void_t tb_oc_string_clear(tb_oc_object_ref_t object)
{
    tb_oc_string_t* string = tb_oc_string_cast(object);
    if (string) 
    {
        // clear the string
        tb_string_clear(&string->str);
    }
}
static tb_oc_string_t* tb_oc_string_init_base()
{
    // done
    tb_bool_t       ok = tb_false;
    tb_oc_string_t* string = tb_null;
    do
    {
        // make string
        string = tb_malloc0_type(tb_oc_string_t);
        tb_assert_and_check_break(string);

        // init string
        if (!tb_oc_object_init((tb_oc_object_ref_t)string, TB_OBJECT_FLAG_NONE, TB_OBJECT_TYPE_STRING)) break;

        // init base
        string->base.copy   = tb_oc_string_copy;
        string->base.exit   = tb_oc_string_exit;
        string->base.clear  = tb_oc_string_clear;
        
        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (string) tb_oc_object_exit((tb_oc_object_ref_t)string);
        string = tb_null;
    }

    // ok?
    return string;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_oc_object_ref_t tb_oc_string_init_from_cstr(tb_char_t const* cstr)
{
    // done
    tb_bool_t       ok = tb_false;
    tb_oc_string_t* string = tb_null;
    do
    {
        // make string
        string = tb_oc_string_init_base();
        tb_assert_and_check_break(string);

        // init str
        if (!tb_string_init(&string->str)) break;

        // copy string
        if (cstr) tb_string_cstrcpy(&string->str, cstr);

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        tb_oc_string_exit((tb_oc_object_ref_t)string);
        string = tb_null;
    }

    // ok?
    return (tb_oc_object_ref_t)string;
}
tb_oc_object_ref_t tb_oc_string_init_from_str(tb_string_ref_t str)
{
    // done
    tb_bool_t       ok = tb_false;
    tb_oc_string_t* string = tb_null;
    do
    {
        // make string
        string = tb_oc_string_init_base();
        tb_assert_and_check_break(string);

        // init str
        if (!tb_string_init(&string->str)) break;

        // copy string
        if (str) tb_string_strcpy(&string->str, str);

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        tb_oc_string_exit((tb_oc_object_ref_t)string);
        string = tb_null;
    }

    // ok?
    return (tb_oc_object_ref_t)string;
}
tb_char_t const* tb_oc_string_cstr(tb_oc_object_ref_t object)
{
    // check
    tb_oc_string_t* string = tb_oc_string_cast(object);
    tb_assert_and_check_return_val(string, tb_null);

    // cstr
    return tb_string_cstr(&string->str);
}
tb_size_t tb_oc_string_cstr_set(tb_oc_object_ref_t object, tb_char_t const* cstr)
{
    // check
    tb_oc_string_t* string = tb_oc_string_cast(object);
    tb_assert_and_check_return_val(string && cstr, 0);

    // copy string
    tb_string_cstrcpy(&string->str, cstr);
 
    // ok?
    return tb_string_size(&string->str);
}
tb_size_t tb_oc_string_size(tb_oc_object_ref_t object)
{
    // check
    tb_oc_string_t* string = tb_oc_string_cast(object);
    tb_assert_and_check_return_val(string, 0);

    // size
    return tb_string_size(&string->str);
}

