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
/* /////////////////////////////////////////////////////////
 * includes
 */
#include "aioo.h"
#include "../../stream/stream.h"

/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_long_t tb_aioo_reactor_http_wait(tb_aioo_t* object, tb_long_t timeout)
{
	return tb_aioo_reactor_sock_wait(object, timeout);
}
tb_long_t tb_aioo_reactor_gstm_wait(tb_aioo_t* object, tb_long_t timeout)
{
	tb_gstream_t* gst = object->handle;
	switch (gst->type)
	{
	case TB_GSTREAM_TYPE_FILE:
		return tb_aioo_reactor_file_wait(object, timeout);
	case TB_GSTREAM_TYPE_SOCK:
		return tb_aioo_reactor_sock_wait(object, timeout);
	case TB_GSTREAM_TYPE_HTTP:
		return tb_aioo_reactor_sock_wait(object, timeout);
	case TB_GSTREAM_TYPE_DATA:
		{
			tb_long_t 	e = 0;
			tb_uint64_t o = tb_gstream_offset(gst);
			tb_uint64_t n = tb_gstream_size(gst);
			if (o <= n) 
			{
				if (object->etype & TB_AIOO_ETYPE_READ) e |= TB_AIOO_ETYPE_READ;
				if (object->etype & TB_AIOO_ETYPE_WRIT) e |= TB_AIOO_ETYPE_WRIT;
			}
			else e = -1;
			return e;
		}
	case TB_GSTREAM_TYPE_TRAN:
		{
			tb_tstream_t* tst = (tb_tstream_t*)gst;
			tb_assert_and_check_return_val(tst, -1);

			// wait the next gstream
			tb_aioo_t o = *object;
			o.handle = (tb_handle_t)tst->gst;
			return tb_aioo_reactor_gstm_wait(&o, timeout);
		}
	default:
		tb_warning("the gstream type %x is not supported for aioo", gst->type);
		break;
	}
	return -1;
}
