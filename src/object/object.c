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
 * @file		object.c
 * @ingroup 	object
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "object.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_object_init(tb_object_t* object, tb_size_t flag, tb_size_t type)
{
	// check
	tb_assert_and_check_return_val(object, tb_false);

	// init
	tb_memset(object, 0, sizeof(tb_object_t));
	object->flag = flag;
	object->type = type;
	object->refn = 1;
}
tb_void_t tb_object_exit(tb_object_t* object)
{
	// check
	tb_assert_and_check_return(object);

	// readonly?
	tb_check_return(!(object->flag & TB_OBJECT_FLAG_READONLY));

	// refn must be 1
	tb_size_t refn = tb_object_ref(object);
	tb_assert_and_check_return(refn == 1);

	// exit
	tb_object_dec(object);
}
tb_object_t* tb_object_copy(tb_object_t* object)
{
	// check
	tb_assert_and_check_return_val(object && object->copy, tb_null);

	// copy
	return object->copy(object);
}
tb_size_t tb_object_type(tb_object_t* object)
{
	tb_assert_and_check_return_val(object, TB_OBJECT_TYPE_NONE);
	return object->type;
}
tb_size_t tb_object_ref(tb_object_t* object)
{
	tb_assert_and_check_return_val(object, 0);
	return object->refn;
}
tb_void_t tb_object_inc(tb_object_t* object)
{
	// check
	tb_assert_and_check_return(object);

	// readonly?
	tb_check_return(!(object->flag & TB_OBJECT_FLAG_READONLY));

	// refn++
	object->refn++;
}
tb_void_t tb_object_dec(tb_object_t* object)
{
	// check
	tb_assert_and_check_return(object);

	// readonly?
	tb_check_return(!(object->flag & TB_OBJECT_FLAG_READONLY));

	// refn--
	if (object->refn > 1) object->refn--;
	else if (object->exit) object->exit(object);
}

