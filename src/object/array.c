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
 * @file		array.c
 * @ingroup 	object
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "object.h"
#include "../container/container.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the array type
typedef struct __tb_array_t
{
	// the object base
	tb_object_t 		base;

	// the vector
	tb_vector_t* 		vector;

}tb_array_t;

/* ///////////////////////////////////////////////////////////////////////
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
	return tb_null;
}
static tb_void_t tb_array_exit(tb_object_t* object)
{
	tb_array_t* array = tb_array_cast(object);
	if (array) 
	{
		// exit vector
		if (array->vector) tb_vector_exit(array->vector);
		array->vector = tb_null;

		// exit it
		tb_free(array);
	}
}
static tb_array_t* tb_array_init_base()
{
	// make
	tb_array_t* array = tb_malloc0(sizeof(tb_array_t));
	tb_assert_and_check_return_val(array, tb_null);

	// init object
	if (!tb_object_init(array, TB_OBJECT_FLAG_NONE, TB_OBJECT_TYPE_ARRAY)) goto fail;

	// init base
	array->base.copy = tb_array_copy;
	array->base.exit = tb_array_exit;

	// ok
	return array;

	// no
fail:
	if (array) tb_free(array);
	return tb_null;
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

