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
 * \file		eobject.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "eobject.h"
#include "../platform/platform.h"

/* /////////////////////////////////////////////////////////
 * implemention
 */
tb_bool_t tb_eobject_init(tb_eobject_t* object, tb_size_t otype, tb_size_t etype, tb_handle_t handle)
{
	tb_assert_and_check_return_val(object, TB_FALSE);
	object->otype = otype;
	object->etype = etype;
	object->handle = handle;
	return TB_TRUE;
}
tb_void_t tb_eobject_exit(tb_eobject_t* object)
{
	tb_assert_and_check_return(object);

	// clear it
	tb_memset(object, 0, sizeof(tb_eobject_t));
}
tb_size_t tb_eobject_type(tb_eobject_t* object)
{
	tb_assert_and_check_return_val(object, TB_EOTYPE_NULL);
	return object->otype;
}
tb_size_t tb_eobject_gete(tb_eobject_t* object)
{
	tb_assert_and_check_return_val(object, TB_ETYPE_NULL);
	return object->etype;
}
tb_size_t tb_eobject_adde(tb_eobject_t* object, tb_size_t etype)
{
	tb_assert_and_check_return_val(object, TB_EOTYPE_NULL);

	object->etype |= etype;
	return object->etype;
}
tb_size_t tb_eobject_dele(tb_eobject_t* object, tb_size_t etype)
{
	tb_assert_and_check_return_val(object, TB_EOTYPE_NULL);

	object->etype &= ~etype;
	return object->etype;
}
tb_long_t tb_eobject_wait(tb_eobject_t* object, tb_long_t timeout)
{
	tb_assert_and_check_return_val(object && object->handle, 0);

	// the wait funcs
	static tb_long_t (*wait[])(tb_handle_t, tb_size_t, tb_size_t, tb_long_t) =
	{
		TB_NULL

		// for qbuffer
	, 	TB_NULL
	
		// for file
	, 	tb_event_wait_fd

		// for socket
	, 	tb_event_wait_fd

		// for http
	, 	TB_NULL

		// for gstream
	, 	TB_NULL

		// for event
	, 	TB_NULL
	};

	// the fd funcs
	static tb_long_t (*fd[])(tb_handle_t) =
	{
		TB_NULL

		// for qbuffer
	, 	TB_NULL
	
		// for file
	, 	tb_file_fd

		// for socket
	, 	tb_socket_fd

		// for http
	, 	TB_NULL

		// for gstream
	, 	TB_NULL

		// for event
	, 	TB_NULL
	};

	// check
	tb_assert_and_check_return_val(object->otype < tb_arrayn(wait) && object->otype < tb_arrayn(fd), 0);
	tb_assert_and_check_return_val(wait[object->otype] && fd[object->otype], 0);
 
	// wait object
	return wait[object->otype](fd[object->otype](object->handle), object->otype, object->etype, timeout);
}
tb_bool_t tb_eobject_spak(tb_eobject_t* object)
{
	tb_trace_noimpl();
	return TB_FALSE;
}


