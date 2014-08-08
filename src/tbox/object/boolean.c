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
 * @author      ruki
 * @file        boolean.c
 * @ingroup     object
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME        "object_boolean"
#define TB_TRACE_MODULE_DEBUG       (0)
 
/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "object.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the boolean type
typedef struct __tb_object_boolean_t
{
    // the object base
    tb_object_t         base;

    // the boolean value
    tb_bool_t           value;

}tb_object_boolean_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_object_boolean_t* tb_object_boolean_cast(tb_object_ref_t object)
{
    // check
    tb_assert_and_check_return_val(object && object->type == TB_OBJECT_TYPE_BOOLEAN, tb_null);

    // cast
    return (tb_object_boolean_t*)object;
}

static tb_object_ref_t tb_object_boolean_copy(tb_object_ref_t object)
{
    // check
    tb_object_boolean_t* boolean = (tb_object_boolean_t*)object;
    tb_assert_and_check_return_val(boolean, tb_null);

    // copy
    return object;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */
  
// true
static tb_object_boolean_t const g_boolean_true = 
{
    {
        TB_OBJECT_FLAG_READONLY | TB_OBJECT_FLAG_SINGLETON
    ,   TB_OBJECT_TYPE_BOOLEAN
    ,   1
    ,   tb_null
    ,   tb_object_boolean_copy
    ,   tb_null
    ,   tb_null
    }
,   tb_true

};

// false
static tb_object_boolean_t const g_boolean_false = 
{
    {
        TB_OBJECT_FLAG_READONLY | TB_OBJECT_FLAG_SINGLETON
    ,   TB_OBJECT_TYPE_BOOLEAN
    ,   1
    ,   tb_null
    ,   tb_object_boolean_copy
    ,   tb_null
    ,   tb_null
    }
,   tb_false

};

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_object_ref_t tb_object_boolean_init(tb_bool_t value)
{
    return value? tb_object_boolean_true() : tb_object_boolean_false();
}
tb_object_ref_t tb_object_boolean_true()
{
    return (tb_object_ref_t)&g_boolean_true;
}
tb_object_ref_t tb_object_boolean_false()
{
    return (tb_object_ref_t)&g_boolean_false;
}
tb_bool_t tb_object_boolean_bool(tb_object_ref_t object)
{
    tb_object_boolean_t* boolean = tb_object_boolean_cast(object);
    tb_assert_and_check_return_val(boolean, tb_false);

    return boolean->value;
}

