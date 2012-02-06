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
 * \author		ruki
 * \file		aioo.c
 *
 */
/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "aioo.h"
#include "../../stream/stream.h"
#include "../../network/network.h"

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_long_t tb_aioo_reactor_http_wait(tb_aioo_t* object, tb_long_t timeout)
{
	tb_assert_and_check_return_val(object && object->handle, -1);
	tb_handle_t http = object->handle;
	return tb_http_wait(http, object->etype, timeout);
}
tb_long_t tb_aioo_reactor_gstm_wait(tb_aioo_t* object, tb_long_t timeout)
{
	tb_assert_and_check_return_val(object && object->handle, -1);
	tb_gstream_t* gst = object->handle;
	return tb_gstream_wait(gst, object->etype, timeout);
}
