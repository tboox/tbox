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
 * @file        date.c
 * @ingroup     object
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME        "object_date"
#define TB_TRACE_MODULE_DEBUG       (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "object.h"
#include "../utils/utils.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */
 
// the date type
typedef struct __tb_object_date_t
{
    // the object base
    tb_object_t         base;

    // the date time
    tb_time_t           time;

}tb_object_date_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_object_date_t* tb_object_date_cast(tb_object_t* object)
{
    // check
    tb_assert_and_check_return_val(object && object->type == TB_OBJECT_TYPE_DATE, tb_object_null);

    // cast
    return (tb_object_date_t*)object;
}
static tb_object_t* tb_object_date_copy(tb_object_t* object)
{
    return tb_object_date_init_from_time(tb_object_date_time(object));
}
static tb_void_t tb_object_date_exit(tb_object_t* object)
{
    if (object) tb_object_pool_del(tb_object_pool(), object);
}
static tb_void_t tb_object_date_cler(tb_object_t* object)
{
    tb_object_date_t* date = tb_object_date_cast(object);
    if (date) date->time = 0;
}
static tb_object_date_t* tb_object_date_init_base()
{
    // make
    tb_object_date_t* date = (tb_object_date_t*)tb_object_pool_get(tb_object_pool(), sizeof(tb_object_date_t), TB_OBJECT_FLAG_NONE, TB_OBJECT_TYPE_DATE);
    tb_assert_and_check_return_val(date, tb_object_null);

    // init base
    date->base.copy = tb_object_date_copy;
    date->base.cler = tb_object_date_cler;
    date->base.exit = tb_object_date_exit;

    // ok
    return date;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_object_t* tb_object_date_init_from_now()
{
    // make
    tb_object_date_t* date = tb_object_date_init_base();
    tb_assert_and_check_return_val(date, tb_object_null);

    // init time
    date->time = tb_time();

    // ok
    return (tb_object_t*)date;
}
tb_object_t* tb_object_date_init_from_time(tb_time_t time)
{
    // make
    tb_object_date_t* date = tb_object_date_init_base();
    tb_assert_and_check_return_val(date, tb_object_null);

    // init time
    if (time > 0) date->time = time;

    // ok
    return (tb_object_t*)date;
}
tb_time_t tb_object_date_time(tb_object_t* object)
{
    // check
    tb_object_date_t* date = tb_object_date_cast(object);
    tb_assert_and_check_return_val(date, -1);

    // time
    return date->time;
}
tb_bool_t tb_object_date_time_set(tb_object_t* object, tb_time_t time)
{
    // check
    tb_object_date_t* date = tb_object_date_cast(object);
    tb_assert_and_check_return_val(date, tb_false);

    // set time
    date->time = time;

    // ok
    return tb_true;
}
tb_bool_t tb_object_date_time_set_now(tb_object_t* object)
{
    // check
    tb_object_date_t* date = tb_object_date_cast(object);
    tb_assert_and_check_return_val(date, tb_false);

    // set time
    date->time = tb_time();

    // ok
    return tb_true;
}
