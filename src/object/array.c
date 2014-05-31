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
 * @file        array.c
 * @ingroup     object
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME        "object_array"
#define TB_TRACE_MODULE_DEBUG       (0)
 
/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "object.h"
#include "../algorithm/algorithm.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the array type
typedef struct __tb_array_t
{
    // the object base
    tb_object_t         base;

    // the vector
    tb_vector_t*        vector;

    // is increase refn?
    tb_bool_t           incr;

}tb_array_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_array_t* tb_array_cast(tb_object_t* object)
{
    // check
    tb_assert_and_check_return_val(object && object->type == TB_OBJECT_TYPE_ARRAY, tb_null);

    // cast
    return (tb_array_t*)object;
}
static tb_object_t* tb_array_copy(tb_object_t* object)
{
    // check
    tb_array_t* array = tb_array_cast(object);
    tb_assert_and_check_return_val(array && array->vector, tb_null);

    // init copy
    tb_array_t* copy = (tb_array_t*)tb_array_init(tb_vector_grow(array->vector), array->incr);
    tb_assert_and_check_return_val(copy && copy->vector, tb_null);

    // refn++
    tb_for_all (tb_object_t*, item, array->vector)
    {
        if (item) tb_object_inc(item);
    }

    // copy
    tb_vector_copy(copy->vector, array->vector);

    // ok
    return (tb_object_t*)copy;
}
static tb_void_t tb_array_exit(tb_object_t* object)
{
    // check
    tb_array_t* array = tb_array_cast(object);
    tb_assert_and_check_return(array);

    // exit vector
    if (array->vector) tb_vector_exit(array->vector);
    array->vector = tb_null;

    // exit it
    tb_object_pool_del(tb_object_pool(), object);
}
static tb_void_t tb_array_cler(tb_object_t* object)
{
    tb_array_t* array = tb_array_cast(object);
    tb_assert_and_check_return(array && array->vector);

    // clear vector
    tb_vector_clear(array->vector);
}
static tb_array_t* tb_array_init_base()
{
    // make
    tb_array_t* array = (tb_array_t*)tb_object_pool_get(tb_object_pool(), sizeof(tb_array_t), TB_OBJECT_FLAG_NONE, TB_OBJECT_TYPE_ARRAY);
    tb_assert_and_check_return_val(array, tb_null);

    // init base
    array->base.copy = tb_array_copy;
    array->base.cler = tb_array_cler;
    array->base.exit = tb_array_exit;

    // ok
    return array;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_object_t* tb_array_init(tb_size_t grow, tb_bool_t incr)
{
    // make
    tb_array_t* array = tb_array_init_base();
    tb_assert_and_check_return_val(array, tb_null);

    // init item func
    tb_item_func_t func = tb_item_func_obj();

    // init vector
    array->vector = tb_vector_init(grow, func);
    tb_assert_and_check_goto(array->vector, fail);

    // init incr
    array->incr = incr;

    // ok
    return (tb_object_t*)array;

fail:
    // no
    tb_array_exit((tb_object_t*)array);
    return tb_null;
}
tb_size_t tb_array_size(tb_object_t* object)
{
    tb_array_t* array = tb_array_cast(object);
    tb_assert_and_check_return_val(array && array->vector, 0);

    // size
    return tb_vector_size(array->vector);
}
tb_object_t* tb_array_item(tb_object_t* object, tb_size_t index)
{
    tb_array_t* array = tb_array_cast(object);
    tb_assert_and_check_return_val(array && array->vector, tb_null);

    // item
    return (tb_object_t*)tb_iterator_item(array->vector, index);
}
tb_iterator_t* tb_array_itor(tb_object_t* object)
{
    tb_array_t* array = tb_array_cast(object);
    tb_assert_and_check_return_val(array, tb_null);

    // iterator
    return (tb_iterator_t*)array->vector;
}
tb_void_t tb_array_remove(tb_object_t* object, tb_size_t index)
{
    tb_array_t* array = tb_array_cast(object);
    tb_assert_and_check_return(array && array->vector);

    // remove
    tb_vector_remove(array->vector, index);
}
tb_void_t tb_array_append(tb_object_t* object, tb_object_t* item)
{
    tb_array_t* array = tb_array_cast(object);
    tb_assert_and_check_return(array && array->vector && item);

    // insert
    tb_vector_insert_tail(array->vector, item);

    // refn--
    if (!array->incr) tb_object_dec(item);
}
tb_void_t tb_array_insert(tb_object_t* object, tb_size_t index, tb_object_t* item)
{
    tb_array_t* array = tb_array_cast(object);
    tb_assert_and_check_return(array && array->vector && item);

    // insert
    tb_vector_insert_prev(array->vector, index, item);

    // refn--
    if (!array->incr) tb_object_dec(item);
}
tb_void_t tb_array_replace(tb_object_t* object, tb_size_t index, tb_object_t* item)
{
    tb_array_t* array = tb_array_cast(object);
    tb_assert_and_check_return(array && array->vector && item);

    // replace
    tb_vector_replace(array->vector, index, item);

    // refn--
    if (!array->incr) tb_object_dec(item);
}
tb_void_t tb_array_incr(tb_object_t* object, tb_bool_t incr)
{
    tb_array_t* array = tb_array_cast(object);
    tb_assert_and_check_return(array);

    array->incr = incr;
}
