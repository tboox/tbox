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
 * @naio		naio.c
 * @ingroup 	platform
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
#undef TB_TRACE_IMPL_TAG
#define TB_TRACE_IMPL_TAG 				"aicp_naio"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_handle_t tb_aicp_naio_init(tb_aicp_proactor_aiop_t* ptor)
{
	return tb_null;
}
static tb_void_t tb_aicp_naio_exit(tb_handle_t naio)
{
}
static tb_bool_t tb_aicp_naio_addo(tb_handle_t naio, tb_aico_t* aico)
{
	return tb_false;
}
static tb_bool_t tb_aicp_naio_delo(tb_handle_t naio, tb_aico_t* aico)
{
	return tb_false;
}
static tb_bool_t tb_aicp_naio_post(tb_handle_t naio, tb_aice_t const* list, tb_size_t size)
{
	return tb_false;
}
static tb_long_t tb_aicp_naio_spak(tb_handle_t naio, tb_aice_t* aice)
{
	return -1;
}
static tb_void_t tb_aicp_naio_kill(tb_handle_t naio)
{
}

