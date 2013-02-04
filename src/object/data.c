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
 * @file		data.c
 * @ingroup 	object
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "object.h"
#include "../memory/memory.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the data type
typedef struct __tb_data_t
{
	// the object base
	tb_object_t 		base;

	// the data buffer
	tb_pbuffer_t 		buff;

}tb_data_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_data_t* tb_data_cast(tb_object_t* object)
{
	// check
	tb_assert_and_check_return_val(object && object->type == TB_OBJECT_TYPE_DATA, tb_null);

	// cast
	return (tb_data_t*)object;
}
static tb_object_t* tb_data_copy(tb_object_t* object)
{
	return tb_data_init_from_data(tb_data_addr(object), tb_data_size(object));
}
static tb_void_t tb_data_exit(tb_object_t* object)
{
	tb_data_t* data = tb_data_cast(object);
	if (data) 
	{
		tb_pbuffer_exit(&data->buff);
		tb_free(data);
	}
}
static tb_void_t tb_data_cler(tb_object_t* object)
{
	tb_data_t* data = tb_data_cast(object);
	if (data) tb_pbuffer_clear(&data->buff);
}
static tb_data_t* tb_data_init_base()
{
	// make
	tb_data_t* data = tb_malloc0(sizeof(tb_data_t));
	tb_assert_and_check_return_val(data, tb_null);

	// init object
	if (!tb_object_init(data, TB_OBJECT_FLAG_NONE, TB_OBJECT_TYPE_DATA)) goto fail;

	// init base
	data->base.copy = tb_data_copy;
	data->base.cler = tb_data_cler;
	data->base.exit = tb_data_exit;

	// ok
	return data;

	// no
fail:
	if (data) tb_free(data);
	return tb_null;
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_object_t* tb_data_init_from_data(tb_pointer_t addr, tb_size_t size)
{
	// make
	tb_data_t* data = tb_data_init_base();
	tb_assert_and_check_return_val(data, tb_null);

	// init buff
	if (!tb_pbuffer_init(&data->buff)) goto fail;

	// copy data
	if (addr && size) tb_pbuffer_memncpy(&data->buff, addr, size);

	// ok
	return data;

	// no
fail:
	tb_data_exit(data);
	return tb_null;
}
tb_object_t* tb_data_init_from_pbuf(tb_pbuffer_t* pbuf)
{	
	// make
	tb_data_t* data = tb_data_init_base();
	tb_assert_and_check_return_val(data, tb_null);

	// init buff
	if (!tb_pbuffer_init(&data->buff)) goto fail;

	// copy data
	if (pbuf) tb_pbuffer_memcpy(&data->buff, pbuf);

	// ok
	return data;

	// no
fail:
	tb_data_exit(data);
	return tb_null;
}
tb_pointer_t tb_data_addr(tb_object_t* object)
{
	// check
	tb_data_t* data = tb_data_cast(object);
	tb_assert_and_check_return_val(data, tb_null);

	// data
	return tb_pbuffer_data(&data->buff);
}
tb_size_t tb_data_size(tb_object_t* object)
{
	// check
	tb_data_t* data = tb_data_cast(object);
	tb_assert_and_check_return_val(data, 0);

	// data
	return tb_pbuffer_size(&data->buff);
}
tb_pbuffer_t* tb_data_buff(tb_object_t* object)
{
	// check
	tb_data_t* data = tb_data_cast(object);
	tb_assert_and_check_return_val(data, tb_null);

	// buff
	return &data->buff;
}

