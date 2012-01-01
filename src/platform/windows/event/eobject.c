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
#include "prefix.h"
#include "../../event.h"
#include "winsock2.h"
#include "windows.h"

/* /////////////////////////////////////////////////////////
 * implemention
 */
#ifdef TB_CONFIG_EVENT_HAVE_WAITO
# 	include "eobject/waito.c"
#endif

#ifdef TB_CONFIG_EVENT_HAVE_SELECT
# 	include "eobject/select.c"
#endif

/* /////////////////////////////////////////////////////////
 * selector
 */
tb_long_t tb_eobject_wait_impl(tb_eobject_t* object, tb_long_t timeout)
{
	tb_assert_and_check_return_val(object, -1);

	switch (object->otype)
	{
#ifdef TB_CONFIG_EVENT_HAVE_WAITO
	case TB_EOTYPE_FILE:
	case TB_EOTYPE_EVET:
		return tb_eobject_waito(object, timeout);
#endif

#ifdef TB_CONFIG_EVENT_HAVE_SELECT
	case TB_EOTYPE_SOCK:
		return tb_eobject_select(object, timeout);
#endif
	default:
		break;
	}
	tb_trace_noimpl();
	return 0;
}
