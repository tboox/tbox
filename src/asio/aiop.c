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
 * @file		aiop.c
 * @ingroup 	asio
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "aiop.h"
#include "aioo.h"
#include "../math/math.h"

/* ///////////////////////////////////////////////////////////////////////
 * declaration
 */
tb_aiop_reactor_t* tb_aiop_reactor_init(tb_aiop_t* aiop);

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_aiop_t* tb_aiop_init(tb_size_t maxn)
{
	// check
	tb_assert_and_check_return_val(maxn, tb_null);

	// alloc aiop
	tb_aiop_t* aiop = tb_malloc0(sizeof(tb_aiop_t));
	tb_assert_and_check_return_val(aiop, tb_null);

	// init aiop
	aiop->maxn = maxn;

	// init reactor
	aiop->rtor = tb_aiop_reactor_init(aiop);
	tb_assert_and_check_goto(aiop->rtor, fail);

	// ok
	return aiop;

fail:
	if (aiop) tb_aiop_exit(aiop);
	return tb_null;
}

tb_void_t tb_aiop_exit(tb_aiop_t* aiop)
{
	// check
	tb_assert_and_check_return(aiop);

	// exit reactor
	if (aiop->rtor && aiop->rtor->exit)
		aiop->rtor->exit(aiop->rtor);

	// free aiop
	tb_free(aiop);
}
tb_void_t tb_aiop_cler(tb_aiop_t* aiop)
{
	// check
	tb_assert_and_check_return(aiop);

	// clear reactor
	if (aiop->rtor && aiop->rtor->cler)
		aiop->rtor->cler(aiop->rtor);
}
tb_size_t tb_aiop_flag(tb_aiop_t* aiop)
{
	// check
	tb_assert_and_check_return_val(aiop && aiop->rtor && aiop->rtor->flag, TB_AIOP_FLAG_NONE);

	// flag
	return aiop->rtor->flag(aiop->rtor);
}
tb_bool_t tb_aiop_addo(tb_aiop_t* aiop, tb_handle_t handle)
{
	// check
	tb_assert_and_check_return_val(aiop && aiop->rtor && aiop->rtor->addo && handle, tb_false);

	// addo
	return aiop->rtor->addo(aiop->rtor, handle);
}
tb_bool_t tb_aiop_delo(tb_aiop_t* aiop, tb_handle_t handle)
{
	// check
	tb_assert_and_check_return_val(aiop && aiop->rtor && aiop->rtor->delo && handle, tb_false);

	// delo
	return aiop->rtor->delo(aiop->rtor, handle);
}
tb_bool_t tb_aiop_post(tb_aiop_t* aiop, tb_aioe_t const* list, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(aiop && aiop->rtor && aiop->rtor->post && list && size, tb_false);

	// post
	return aiop->rtor->post(aiop->rtor, list, size);
}
tb_bool_t tb_aiop_sete(tb_aiop_t* aiop, tb_handle_t handle, tb_size_t code, tb_pointer_t data)
{
	// check
	tb_assert_and_check_return(aiop && handle && code, tb_false);

	// init aioe
	tb_aioe_t aioe;
	aioe.code = code;
	aioe.flag = TB_AIOE_FLAG_SETE;
	aioe.data = data;
	aioe.handle = handle;

	// post aioe
	return tb_aiop_post(aiop, &aioe, 1);
}
tb_bool_t tb_aiop_adde(tb_aiop_t* aiop, tb_handle_t handle, tb_size_t code, tb_pointer_t data)
{
	// check
	tb_assert_and_check_return(aiop && handle && code, tb_false);

	// init aioe
	tb_aioe_t aioe;
	aioe.code = code;
	aioe.flag = TB_AIOE_FLAG_ADDE;
	aioe.data = data;
	aioe.handle = handle;

	// post aioe
	return tb_aiop_post(aiop, &aioe, 1);
}
tb_bool_t tb_aiop_dele(tb_aiop_t* aiop, tb_handle_t handle, tb_size_t code, tb_pointer_t data)
{
	// check
	tb_assert_and_check_return(aiop && handle && code, tb_false);

	// init aioe
	tb_aioe_t aioe;
	aioe.code = code;
	aioe.flag = TB_AIOE_FLAG_DELE;
	aioe.data = data;
	aioe.handle = handle;

	// post aioe
	return tb_aiop_post(aiop, &aioe, 1);
}

tb_long_t tb_aiop_wait(tb_aiop_t* aiop, tb_aioe_t* list, tb_size_t maxn, tb_long_t timeout)
{	
	// check
	tb_assert_and_check_return_val(aiop && aiop->rtor && aiop->rtor->wait && list, -1);

	// wait 
	return aiop->rtor->wait(aiop->rtor, list, maxn, timeout);
}

