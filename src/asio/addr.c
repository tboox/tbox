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
 * @file		addr.c
 * @ingroup 	asio
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "addr.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_aicp_post_addr(tb_aicp_t* aicp, tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->ptor && aicp->ptor->post, tb_false);
	tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_ADDR && aice->u.addr.host, tb_false);
	
	// no pending? spak it directly 
	if (aice->state != TB_AICE_STATE_PENDING)
		return aicp->ptor->post(aicp->ptor, aice);

	// the host is addr? ok
	tb_aice_t addr = *aice;
	if (tb_ipv4_set(&addr.u.addr.addr, aice->u.addr.host))
	{
		addr.state = TB_AICE_STATE_OK;
		return aicp->ptor->post(aicp->ptor, &addr);
	}

	// post failed
	addr.state = TB_AICE_STATE_FAILED;
	return aicp->ptor->post(aicp->ptor, &addr);
}
