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
 * \file		waito.c
 *
 */

/* /////////////////////////////////////////////////////////
 * types
 */

// the waito reactor type
typedef struct __tb_aiop_reactor_waito_t
{
	// the reactor base
	tb_aiop_reactor_t 		base;

	// the objects hash
	tb_hash_t* 				hash;

	// the waito handles
	tb_vector_t* 			hdls;

}tb_aiop_reactor_waito_t;

/* /////////////////////////////////////////////////////////
 * implemention
 */
static tb_bool_t tb_aiop_reactor_waito_addo(tb_aiop_reactor_t* reactor, tb_handle_t handle, tb_size_t etype)
{
	tb_aiop_reactor_waito_t* rtor = (tb_aiop_reactor_waito_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->hdls && rtor->hash && reactor->aiop, TB_FALSE);

	// check
	tb_assert_and_check_return_val(tb_hash_size(rtor->hash) < MAXIMUM_WAIT_OBJECTS, TB_FALSE);

	// init obj
	tb_aioo_t o;
	tb_aioo_seto(&o, handle, reactor->aiop->type, etype);

	// add hdl
	tb_vector_insert_tail(rtor->hdls, handle);

	// add obj
	tb_hash_set(rtor->hash, handle, &o);
	
	// ok
	return TB_TRUE;
}
static tb_bool_t tb_aiop_reactor_waito_seto(tb_aiop_reactor_t* reactor, tb_handle_t handle, tb_size_t etype)
{
	tb_aiop_reactor_waito_t* rtor = (tb_aiop_reactor_waito_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->hdls && rtor->hash && reactor->aiop, TB_FALSE);

	// get obj
	tb_aioo_t* o = tb_hash_get(rtor->hash, handle);
	tb_assert_and_check_return_val(o, TB_FALSE);

	// set obj
	tb_aioo_seto(o, handle, reactor->aiop->type, etype);

	// ok
	return TB_TRUE;
}
static tb_bool_t tb_aiop_reactor_waito_delo(tb_aiop_reactor_t* reactor, tb_handle_t handle)
{
	tb_aiop_reactor_waito_t* rtor = (tb_aiop_reactor_waito_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->hdls && rtor->hash, TB_FALSE);

	// find hdl
	tb_size_t itor = tb_vector_itor_head(rtor->hdls);
	tb_size_t tail = tb_vector_itor_tail(rtor->hdls);
	for (; itor != tail; itor = tb_vector_itor_next(rtor->hdls, itor))
	{
		tb_handle_t hdl = (tb_handle_t)tb_vector_itor_at(rtor->hdls, itor);
		if (hdl == handle) break;
	}
	tb_assert_and_check_return_val(itor != tail, TB_FALSE);
	
	// del hdl
	tb_vector_remove(rtor->hdls, itor);

	// del obj
	tb_hash_del(rtor->hash, handle);
	
	// ok
	return TB_TRUE;
}
static tb_bool_t tb_aiop_reactor_waito_reto(tb_aiop_reactor_t* reactor, tb_aioo_t* objs, tb_size_t hdli, tb_handle_t handle)
{	
	tb_aiop_reactor_waito_t* rtor = (tb_aiop_reactor_waito_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->hash, TB_FALSE);

	// get obj
	tb_aioo_t* o = tb_hash_get(rtor->hash, handle);
	tb_assert_and_check_return_val(o, TB_FALSE);
	
	// add object
	objs[hdli] = *o;

	// ok
	return TB_TRUE;
}
static tb_long_t tb_aiop_reactor_waito_wait(tb_aiop_reactor_t* reactor, tb_aioo_t* objs, tb_size_t objm, tb_long_t timeout)
{	
	tb_aiop_reactor_waito_t* rtor = (tb_aiop_reactor_waito_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->hash && rtor->hdls, -1);

	// hdls
	tb_handle_t*	hdls = (tb_handle_t*)tb_vector_data(rtor->hdls);
	tb_size_t 		hdlm = tb_vector_size(rtor->hdls);
	tb_assert_and_check_return_val(hdls && hdlm, -1);

	// wait
	tb_long_t 		hdli = WaitForMultipleObjects((DWORD)hdlm, (HANDLE const*)hdls, FALSE, timeout >= 0? timeout : INFINITE);
	tb_assert_and_check_return_val(hdli != WAIT_FAILED, -1);

	// timeout?
	tb_check_return_val(hdli != WAIT_TIMEOUT, 0);

	// error?
	tb_check_return_val(hdli >= WAIT_OBJECT_0, -1);

	// has more event?
	tb_size_t hdln = 0;
	while (hdli < WAIT_OBJECT_0 + hdlm && hdln < objm)
	{
		// get object
		hdli -= WAIT_OBJECT_0;
		tb_aioo_t* o = tb_hash_get(rtor->hash, hdls[hdli]);
		tb_assert_and_check_return_val(o, -1);

		// add object
		objs[hdli] = *o;

		// next
		hdli++;
		hdln++;

		// end?
		tb_check_break(hdli < hdlm);

		// wait next
		hdls += hdli;
		hdlm -= hdli;
		hdli = WaitForMultipleObjects((DWORD)hdlm, (HANDLE const*)hdls, FALSE, 0);
		tb_assert_and_check_return_val(hdli != WAIT_FAILED, -1);

		// no events?
		tb_check_break(hdli != WAIT_TIMEOUT && hdli >= WAIT_OBJECT_0);
	}

	// ok
	return hdln;
}
static tb_void_t tb_aiop_reactor_waito_exit(tb_aiop_reactor_t* reactor)
{
	tb_aiop_reactor_waito_t* rtor = (tb_aiop_reactor_waito_t*)reactor;
	if (rtor)
	{
		// exit hdls
		if (rtor->hdls) tb_vector_exit(rtor->hdls);

		// exit hash
		if (rtor->hash) tb_hash_exit(rtor->hash);

		// free it
		tb_free(rtor);
	}
}

static tb_aiop_reactor_t* tb_aiop_reactor_waito_init(tb_aiop_t* aiop)
{
	// check
	tb_assert_and_check_return_val(aiop && aiop->maxn, TB_NULL);
	tb_assert_and_check_return_val(aiop->type == TB_AIOO_OTYPE_FILE, TB_NULL);
	tb_assert_static(sizeof(tb_handle_t) == sizeof(HANDLE));

	// alloc reactor
	tb_aiop_reactor_waito_t* rtor = tb_calloc(1, sizeof(tb_aiop_reactor_waito_t));
	tb_assert_and_check_return_val(rtor, TB_NULL);

	// init base
	rtor->base.aiop = aiop;
	rtor->base.exit = tb_aiop_reactor_waito_exit;
	rtor->base.addo = tb_aiop_reactor_waito_addo;
	rtor->base.seto = tb_aiop_reactor_waito_seto;
	rtor->base.delo = tb_aiop_reactor_waito_delo;
	rtor->base.wait = tb_aiop_reactor_waito_wait;

	// init hash
	rtor->hash = tb_hash_init(tb_align8(tb_int32_sqrt(aiop->maxn) + 1), tb_item_func_ptr(), tb_item_func_ifm(sizeof(tb_aioo_t), TB_NULL, TB_NULL));
	tb_assert_and_check_goto(rtor->hash, fail);

	// init hdls
	rtor->hdls = tb_vector_init(tb_align8((aiop->maxn >> 3) + 1), tb_item_func_ptr());
	tb_assert_and_check_goto(rtor->hdls, fail);

	// ok
	return (tb_aiop_reactor_t*)rtor;

fail:
	if (rtor) tb_aiop_reactor_waito_exit(rtor);
	return TB_NULL;
}

