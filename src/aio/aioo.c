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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		aioo.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "aioo.h"
#include "reactor/aioo.h"

/* ///////////////////////////////////////////////////////////////////////
 * implemention
 */
tb_void_t tb_aioo_seto(tb_aioo_t* object, tb_handle_t handle, tb_size_t otype, tb_size_t etype, tb_pointer_t odata)
{
	tb_assert_and_check_return(object);
	object->otype = otype;
	object->etype = etype;
	object->odata = odata;
	object->handle = handle;
}
tb_size_t tb_aioo_type(tb_aioo_t* object)
{
	tb_assert_and_check_return_val(object, TB_AIOO_OTYPE_NULL);
	return object->otype;
}
tb_size_t tb_aioo_gete(tb_aioo_t* object)
{
	tb_assert_and_check_return_val(object, TB_AIOO_ETYPE_NULL);
	return object->etype;
}
tb_size_t tb_aioo_sete(tb_aioo_t* object, tb_size_t etype)
{
	tb_assert_and_check_return_val(object, TB_AIOO_OTYPE_NULL);

	object->etype = etype;
	return object->etype;
}
tb_size_t tb_aioo_adde(tb_aioo_t* object, tb_size_t etype)
{
	tb_assert_and_check_return_val(object, TB_AIOO_OTYPE_NULL);

	object->etype |= etype;
	return object->etype;
}
tb_size_t tb_aioo_dele(tb_aioo_t* object, tb_size_t etype)
{
	tb_assert_and_check_return_val(object, TB_AIOO_OTYPE_NULL);

	object->etype &= ~etype;
	return object->etype;
}
tb_long_t tb_aioo_wait(tb_aioo_t* object, tb_long_t timeout)
{
	tb_assert_and_check_return_val(object && object->handle, 0);

	// the reactor 
	static tb_long_t (*wait[])(tb_aioo_t*, tb_long_t) =
	{
		TB_NULL

		// for file
	, 	tb_aioo_reactor_file_wait

		// for socket
	, 	tb_aioo_reactor_sock_wait

		// for http
	, 	tb_aioo_reactor_http_wait

		// for gstream
	, 	tb_aioo_reactor_gstm_wait

	};

	// check
	tb_assert_and_check_return_val(object->otype < tb_arrayn(wait), 0);
	tb_assert_and_check_return_val(wait[object->otype], 0);
 
	// wait object
	return wait[object->otype](object, timeout);
}

