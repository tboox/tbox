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
 * trace
 */
//#define TB_TRACE_IMPL_TAG 				"addr"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "addr.h"
#include "aico.h"
#include "aicp.h"
#include "pool.h"
#include "../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * impl
 */
static tb_aicp_addr_impl_t* tb_aicp_addr_impl_init(tb_aicp_t* aicp, tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aicp && aice, tb_null);

	// done
	tb_bool_t 				ok = tb_false;
	tb_aicp_addr_impl_t* 	impl = tb_null;
	do
	{
		// make impl
		impl = (tb_aicp_addr_impl_t*)tb_aicp_pool_malloc0(aicp, sizeof(tb_aicp_addr_impl_t));
		tb_assert_and_check_break(impl);
	
		// init aice
		impl->aice = *aice;

		// init list
		impl->itor = 1;
		impl->maxn = tb_dns_server_get(impl->list);
		tb_assert_and_check_break(impl->maxn && impl->maxn <= tb_arrayn(impl->list));

		// ok
		ok = tb_true;

	} while (0);

	// failed? exit it
	if (!ok)
	{
		if (impl) tb_aicp_pool_free(aicp, impl);
		impl = tb_null;
	}

	// ok?
	return impl;
}
static tb_void_t tb_aicp_addr_impl_exit(tb_aicp_t* aicp, tb_aicp_addr_impl_t* impl)
{
	// check
	tb_assert_and_check_return(aicp && aicp);

	// exit it
	if (impl) tb_aicp_pool_free(aicp, impl);
}

/* ///////////////////////////////////////////////////////////////////////
 * callback
 */
static tb_size_t tb_aicp_addr_impl_reqt_init(tb_aicp_addr_impl_t* impl, tb_char_t const* host)
{
	// check
	tb_assert_and_check_return_val(impl && host, 0);

	// init query data
	tb_bstream_t bst;
	tb_bstream_init(&bst, impl->data, TB_DNS_RPKT_MAXN);

	// identification number
	tb_bstream_set_u16_be(&bst, TB_DNS_HEADER_MAGIC);

	/* 0x2104: 0 0000 001 0000 0000
	 *
	 * tb_uint16_t qr     :1;		// query/response flag
	 * tb_uint16_t opcode :4;	    // purpose of message
	 * tb_uint16_t aa     :1;		// authoritive answer
	 * tb_uint16_t tc     :1;		// truncated message
	 * tb_uint16_t rd     :1;		// recursion desired

	 * tb_uint16_t ra     :1;		// recursion available
	 * tb_uint16_t z      :1;		// its z! reserved
	 * tb_uint16_t ad     :1;	    // authenticated data
	 * tb_uint16_t cd     :1;	    // checking disabled
	 * tb_uint16_t rcode  :4;	    // response code
	 *
	 * this is a query 
	 * this is a standard query 
	 * not authoritive answer 
	 * not truncated 
	 * recursion desired
	 *
	 * recursion not available! hey we dont have it (lol)
	 *
	 */
#if 1
	tb_bstream_set_u16_be(&bst, 0x0100);
#else
	tb_bstream_set_u1(&bst, 0); 			// this is a query
	tb_bstream_set_ubits32(&bst, 0, 4); 	// this is a standard query
	tb_bstream_set_u1(&bst, 0); 			// not authoritive answer
	tb_bstream_set_u1(&bst, 0); 			// not truncated
	tb_bstream_set_u1(&bst, 1); 			// recursion desired

	tb_bstream_set_u1(&bst, 0); 			// recursion not available! hey we dont have it (lol)
	tb_bstream_set_u1(&bst, 0);
	tb_bstream_set_u1(&bst, 0);
	tb_bstream_set_u1(&bst, 0);
	tb_bstream_set_ubits32(&bst, 0, 4);
#endif

	/* we have only one question
	 *
	 * tb_uint16_t question;	    // number of question entries
	 * tb_uint16_t answer;			// number of answer entries
	 * tb_uint16_t authority;		// number of authority entries
	 * tb_uint16_t resource;		// number of resource entries
	 *
	 */
	tb_bstream_set_u16_be(&bst, 1); 
	tb_bstream_set_u16_be(&bst, 0);
	tb_bstream_set_u16_be(&bst, 0);
	tb_bstream_set_u16_be(&bst, 0);

	// set questions, see as tb_dns_question_t
	// name + question1 + question2 + ...
	tb_bstream_set_u8(&bst, '.');
	tb_byte_t* p = tb_bstream_set_string(&bst, host);

	// only one question now.
	tb_bstream_set_u16_be(&bst, 1); 		// we are requesting the ipv4 address
	tb_bstream_set_u16_be(&bst, 1); 		// it's internet (lol)

	// encode dns name
	if (!p || !tb_dns_encode_name(p - 1)) return 0;

	// ok?
	return tb_bstream_offset(&bst);
}
static tb_bool_t tb_aicp_addr_impl_resp_done(tb_aicp_addr_impl_t* impl, tb_size_t size, tb_ipv4_t* addr)
{
	// check
	tb_assert_and_check_return_val(impl && addr, tb_false);

	// check
	tb_assert_and_check_return_val(size >= TB_DNS_HEADER_SIZE, tb_false);

	// decode dns header
	tb_bstream_t 	bst;
	tb_dns_header_t header;
	tb_bstream_init(&bst, impl->data, size);
	header.id = tb_bstream_get_u16_be(&bst);
	tb_bstream_skip(&bst, 2);
	header.question 	= tb_bstream_get_u16_be(&bst);
	header.answer 		= tb_bstream_get_u16_be(&bst);
	header.authority 	= tb_bstream_get_u16_be(&bst);
	header.resource 	= tb_bstream_get_u16_be(&bst);
	tb_trace_impl("response: size: %u", 		size);
	tb_trace_impl("response: id: 0x%04x", 		header.id);
	tb_trace_impl("response: question: %d", 	header.question);
	tb_trace_impl("response: answer: %d", 		header.answer);
	tb_trace_impl("response: authority: %d", 	header.authority);
	tb_trace_impl("response: resource: %d", 	header.resource);
	tb_trace_impl("");

	// check header
	tb_assert_and_check_return_val(header.id == TB_DNS_HEADER_MAGIC, tb_false);

	// skip questions, only one question now.
	// name + question1 + question2 + ...
	tb_assert_and_check_return_val(header.question == 1, tb_false);
#if 1
	tb_bstream_skip_string(&bst);
	tb_bstream_skip(&bst, 4);
#else
	tb_char_t* name = tb_bstream_get_string(&bst);
	//name = tb_dns_decode_name(name);
	tb_assert_and_check_return_val(name, tb_false);
	tb_bstream_skip(&bst, 4);
	tb_trace_impl("response: name: %s", name);
#endif

	// decode answers
	tb_size_t i = 0;
	tb_size_t found = 0;
	for (i = 0; i < header.answer; i++)
	{
		// decode answer
		tb_dns_answer_t answer;
		tb_trace_impl("response: answer: %d", i);

		// decode dns name
		tb_char_t const* name = tb_dns_decode_name(&bst, answer.name); tb_used(name);
		tb_trace_impl("response: name: %s", name);

		// decode resource
		answer.res.type 	= tb_bstream_get_u16_be(&bst);
		answer.res.class 	= tb_bstream_get_u16_be(&bst);
		answer.res.ttl 		= tb_bstream_get_u32_be(&bst);
		answer.res.size 	= tb_bstream_get_u16_be(&bst);
		tb_trace_impl("response: type: %d", 	answer.res.type);
		tb_trace_impl("response: class: %d", 	answer.res.class);
		tb_trace_impl("response: ttl: %d", 		answer.res.ttl);
		tb_trace_impl("response: size: %d", 	answer.res.size);

		// is ipv4?
		if (answer.res.type == 1)
		{
			tb_byte_t b1 = tb_bstream_get_u8(&bst);
			tb_byte_t b2 = tb_bstream_get_u8(&bst);
			tb_byte_t b3 = tb_bstream_get_u8(&bst);
			tb_byte_t b4 = tb_bstream_get_u8(&bst);
			tb_trace_impl("response: ipv4: %u.%u.%u.%u", b1, b2, b3, b4);

			// save the first ip
			if (!found) 
			{
				// save it
				if (addr)
				{
					addr->u8[0] = b1;
					addr->u8[1] = b2;
					addr->u8[2] = b3;
					addr->u8[3] = b4;
				}

				// found it
				found = 1;
				tb_trace_impl("response: ");
				break;
			}
		}
		else
		{
			// decode rdata
			answer.rdata = tb_dns_decode_name(&bst, answer.name);
			tb_trace_impl("response: alias: %s", answer.rdata? answer.rdata : "");
		}
		tb_trace_impl("response: ");
	}

	// found it?
	tb_check_return_val(found, tb_false);

#if 0
	// decode authorities
	for (i = 0; i < header.authority; i++)
	{
		// decode answer
		tb_dns_answer_t answer;
		tb_trace_impl("response: authority: %d", i);

		// decode dns name
		tb_char_t* name = tb_dns_decode_name(&bst, answer.name);
		tb_trace_impl("response: name: %s", name? name : "");

		// decode resource
		answer.res.type = 	tb_bstream_get_u16_be(&bst);
		answer.res.class = 	tb_bstream_get_u16_be(&bst);
		answer.res.ttl = 	tb_bstream_get_u32_be(&bst);
		answer.res.size = 	tb_bstream_get_u16_be(&bst);
		tb_trace_impl("response: type: %d", 	answer.res.type);
		tb_trace_impl("response: class: %d", 	answer.res.class);
		tb_trace_impl("response: ttl: %d", 		answer.res.ttl);
		tb_trace_impl("response: size: %d", 	answer.res.size);

		// is ipv4?
		if (answer.res.type == 1)
		{
			tb_byte_t b1 = tb_bstream_get_u8(&bst);
			tb_byte_t b2 = tb_bstream_get_u8(&bst);
			tb_byte_t b3 = tb_bstream_get_u8(&bst);
			tb_byte_t b4 = tb_bstream_get_u8(&bst);
			tb_trace_impl("response: ipv4: %u.%u.%u.%u", b1, b2, b3, b4);
		}
		else
		{
			// decode data
			answer.rdata = tb_dns_decode_name(&bst, answer.name);
			tb_trace_impl("response: server: %s", answer.rdata? answer.rdata : "");
		}
		tb_trace_impl("response: ");
	}

	for (i = 0; i < header.resource; i++)
	{
		// decode answer
		tb_dns_answer_t answer;
		tb_trace_impl("response: resource: %d", i);

		// decode dns name
		tb_char_t* name = tb_dns_decode_name(&bst, answer.name);
		tb_trace_impl("response: name: %s", name? name : "");

		// decode resource
		answer.res.type = 	tb_bstream_get_u16_be(&bst);
		answer.res.class = 	tb_bstream_get_u16_be(&bst);
		answer.res.ttl = 	tb_bstream_get_u32_be(&bst);
		answer.res.size = 	tb_bstream_get_u16_be(&bst);
		tb_trace_impl("response: type: %d", 	answer.res.type);
		tb_trace_impl("response: class: %d", 	answer.res.class);
		tb_trace_impl("response: ttl: %d", 		answer.res.ttl);
		tb_trace_impl("response: size: %d", 	answer.res.size);

		// is ipv4?
		if (answer.res.type == 1)
		{
			tb_byte_t b1 = tb_bstream_get_u8(&bst);
			tb_byte_t b2 = tb_bstream_get_u8(&bst);
			tb_byte_t b3 = tb_bstream_get_u8(&bst);
			tb_byte_t b4 = tb_bstream_get_u8(&bst);
			tb_trace_impl("response: ipv4: %u.%u.%u.%u", b1, b2, b3, b4);
		}
		else
		{
			// decode data
			answer.rdata = tb_dns_decode_name(&bst, answer.name);
			tb_trace_impl("response: alias: %s", answer.rdata? answer.rdata : "");
		}
		tb_trace_impl("response: ");
	}
#endif

	// ok
	return tb_true;
}
static tb_bool_t tb_aicp_addr_impl_reqt_func(tb_aice_t const* aice);
static tb_bool_t tb_aicp_addr_impl_resp_func(tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_URECV, tb_false);

	// the aicp
	tb_aicp_t* aicp = (tb_aicp_t*)tb_aico_aicp(aice->aico);
	tb_assert_and_check_return_val(aicp, tb_false);
	
	// the impl
	tb_aicp_addr_impl_t* impl = aice->data; 
	tb_assert_and_check_return_val(impl, tb_false);

	// addr ok?
	tb_bool_t next = tb_false;
	if (aice->state == TB_AICE_STATE_OK)
	{
		// trace
		tb_trace_impl("resp[%s]: server: %u.%u.%u.%u, real: %lu", impl->aice.u.addr.host, tb_ipv4_u8x4(aice->u.urecv.addr), aice->u.urecv.real);

		// check
		tb_assert_and_check_return_val(aice->u.urecv.real, tb_false);

		// done resp: ok?
		if (tb_aicp_addr_impl_resp_done(impl, aice->u.urecv.real, &impl->aice.u.addr.addr))
		{
			// save state
			impl->aice.state = TB_AICE_STATE_OK;
		}
		else
		{
			// save state
			impl->aice.state = TB_AICE_STATE_FAILED;

			// try next server
			next = tb_true;
		}
	}
	// timeout or failed?
	else
	{
		// trace
		tb_trace_impl("resp[%s]: server: %u.%u.%u.%u, state: %s", impl->aice.u.addr.host, tb_ipv4_u8x4(aice->u.urecv.addr), tb_aice_state_cstr(aice));

		// save state
		impl->aice.state = aice->state;

		// try next server
		next = tb_true;
	}

	// try next server?
	if (next)
	{
		// reset next
		next = tb_false;

		// get the next server addr
		tb_ipv4_t const* addr = tb_null;
		if (impl->itor < impl->maxn) 
		{
			impl->itor++;
			addr = &impl->list[impl->itor - 1];
		}

		// has next?
		if (addr)
		{	
			// init reqt
			tb_size_t size = tb_aicp_addr_impl_reqt_init(impl, impl->aice.u.addr.host);
			if (size)
			{
				// post reqt
				if (tb_aico_usend(aice->aico, addr, TB_DNS_HOST_PORT, impl->data, size, tb_aicp_addr_impl_reqt_func, (tb_pointer_t)impl))
				{
					// continue? not free it now.
					impl = tb_null;

					// next ok
					next = tb_true;
				}
			}
		}
	}

	// post it if not next
	if (!next) tb_aicp_post(aicp, &impl->aice);

	// exit impl
	if (impl) tb_aicp_addr_impl_exit(aicp, impl);

	// ok
	return tb_true;
}

static tb_bool_t tb_aicp_addr_impl_reqt_func(tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_USEND, tb_false);

	// the aicp
	tb_aicp_t* aicp = (tb_aicp_t*)tb_aico_aicp(aice->aico);
	tb_assert_and_check_return_val(aicp, tb_false);
	
	// the impl
	tb_aicp_addr_impl_t* impl = aice->data; 
	tb_assert_and_check_return_val(impl, tb_false);

	// addr ok?
	if (aice->state == TB_AICE_STATE_OK)
	{
		// trace
		tb_trace_impl("reqt[%s]: server: %u.%u.%u.%u, real: %lu", impl->aice.u.addr.host, tb_ipv4_u8x4(aice->u.usend.addr), aice->u.usend.real);

		// check
		tb_assert_and_check_return_val(aice->u.usend.real, tb_false);

		// try get addr from the dns list
		tb_ipv4_t const* addr = tb_null;
		if (impl->maxn && impl->itor && impl->itor <= impl->maxn)
			addr = &impl->list[impl->itor - 1];
		tb_assert_and_check_return_val(addr, tb_false);

		// post resp
		tb_aico_urecv(aice->aico, addr, TB_DNS_HOST_PORT, impl->data, sizeof(impl->data), tb_aicp_addr_impl_resp_func, (tb_pointer_t)impl);
	}
	// timeout or failed?
	else
	{
		// trace
		tb_trace_impl("reqt[%s]: server: %u.%u.%u.%u, state: %s", impl->aice.u.addr.host, tb_ipv4_u8x4(aice->u.usend.addr), tb_aice_state_cstr(aice));
			
		// save state
		impl->aice.state = aice->state;

		// get the next server addr
		tb_bool_t next = tb_false;
		tb_ipv4_t const* addr = tb_null;
		if (impl->itor < impl->maxn) 
		{
			impl->itor++;
			addr = &impl->list[impl->itor - 1];
		}

		// has next?
		if (addr)
		{	
			// init reqt
			tb_size_t size = tb_aicp_addr_impl_reqt_init(impl, impl->aice.u.addr.host);
			if (size)
			{
				// post reqt
				if (tb_aico_usend(aice->aico, addr, TB_DNS_HOST_PORT, impl->data, size, tb_aicp_addr_impl_reqt_func, (tb_pointer_t)impl))
				{
					// continue? not free it now.
					impl = tb_null;

					// next ok
					next = tb_true;
				}
			}
		}

		// post it if not next
		if (!next) tb_aicp_post(aicp, &impl->aice);

		// exit impl
		if (impl) tb_aicp_addr_impl_exit(aicp, impl);
	}

	// ok
	return tb_true;
}

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_aicp_post_addr_impl(tb_aicp_t* aicp, tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aicp && aice->aico && aice->code == TB_AICE_CODE_ADDR, tb_false);
	tb_assert_and_check_return_val(aice->state == TB_AICE_STATE_PENDING && aice->u.addr.host, tb_false);
	
	// the host is addr? ok
	tb_aice_t addr = *aice;
	if (tb_ipv4_set(&addr.u.addr.addr, aice->u.addr.host))
	{
		addr.state = TB_AICE_STATE_OK;
		return tb_aicp_post(aicp, &addr);
	}

	// done
	tb_aicp_addr_impl_t* impl = tb_null;
	do
	{
		// make impl
		impl = tb_aicp_addr_impl_init(aicp, aice);
		tb_assert_and_check_break(impl);

		// try get addr from the dns list
		tb_ipv4_t const* addr = tb_null;
		if (impl->maxn && impl->itor && impl->itor <= impl->maxn)
			addr = &impl->list[impl->itor - 1];

		// check
		tb_assert_and_check_break(addr && addr->u32);

		// init reqt
 		tb_size_t size = tb_aicp_addr_impl_reqt_init(impl, aice->u.addr.host);
		tb_assert_and_check_break(size);

		// post reqt
		return tb_aico_usend(aice->aico, addr, TB_DNS_HOST_PORT, impl->data, size, tb_aicp_addr_impl_reqt_func, (tb_pointer_t)impl);
	
	} while (0);

	// exit impl
	if (impl) tb_aicp_addr_impl_exit(aicp, impl);
	impl = tb_null;

	// post failed
	addr.state = TB_AICE_STATE_FAILED;
	return tb_aicp_post(aicp, &addr);
}
