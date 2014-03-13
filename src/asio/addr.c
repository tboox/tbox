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
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author		ruki
 * @file		addr.c
 * @ingroup 	asio
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_IMPL_TAG 				"addr"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "addr.h"
#include "aico.h"
#include "aicp.h"
#include "../network/network.h"
#include "../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the aicp addr type
typedef struct __tb_aicp_addr_t
{
	// the func
	tb_aicp_addr_func_t 	func;

	// the func data
	tb_pointer_t 			priv;

	// the sock
	tb_handle_t 			sock;

	// the aico
	tb_handle_t 			aico;

	// the server indx
	tb_size_t 				indx;

	// the server list
	tb_ipv4_t 				list[3];

	// the server size
	tb_size_t 				size;

	// the data
	tb_byte_t 				data[TB_DNS_RPKT_MAXN];

	// the host
	tb_char_t 				host[256];

}tb_aicp_addr_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_size_t tb_aicp_addr_reqt_init(tb_aicp_addr_t* addr)
{
	// check
	tb_assert_and_check_return_val(addr, 0);

	// init query data
	tb_bstream_t bst;
	tb_bstream_init(&bst, addr->data, TB_DNS_RPKT_MAXN);

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
	tb_char_t* p = tb_bstream_set_string(&bst, addr->host);

	// only one question now.
	tb_bstream_set_u16_be(&bst, 1); 		// we are requesting the ipv4 address
	tb_bstream_set_u16_be(&bst, 1); 		// it's internet (lol)

	// encode dns name
	if (!p || !tb_dns_encode_name(p - 1)) return 0;

	// ok?
	return tb_bstream_offset(&bst);
}
static tb_bool_t tb_aicp_addr_resp_done(tb_aicp_addr_t* addr, tb_size_t size, tb_ipv4_t* ipv4)
{
	// check
	tb_assert_and_check_return_val(addr && ipv4, tb_false);

	// check
	tb_assert_and_check_return_val(size >= TB_DNS_HEADER_SIZE, tb_false);

	// decode dns header
	tb_bstream_t 	bst;
	tb_dns_header_t header;
	tb_bstream_init(&bst, addr->data, size);
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
		answer.res.class_ 	= tb_bstream_get_u16_be(&bst);
		answer.res.ttl 		= tb_bstream_get_u32_be(&bst);
		answer.res.size 	= tb_bstream_get_u16_be(&bst);
		tb_trace_impl("response: type: %d", 	answer.res.type);
		tb_trace_impl("response: class: %d", 	answer.res.class_);
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
				if (ipv4)
				{
					ipv4->u8[0] = b1;
					ipv4->u8[1] = b2;
					ipv4->u8[2] = b3;
					ipv4->u8[3] = b4;
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
			answer.rdata = (tb_byte_t const*)tb_dns_decode_name(&bst, answer.name);
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
		answer.res.class_ = 	tb_bstream_get_u16_be(&bst);
		answer.res.ttl = 	tb_bstream_get_u32_be(&bst);
		answer.res.size = 	tb_bstream_get_u16_be(&bst);
		tb_trace_impl("response: type: %d", 	answer.res.type);
		tb_trace_impl("response: class: %d", 	answer.res.class_);
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
		answer.res.class_ = 	tb_bstream_get_u16_be(&bst);
		answer.res.ttl = 	tb_bstream_get_u32_be(&bst);
		answer.res.size = 	tb_bstream_get_u16_be(&bst);
		tb_trace_impl("response: type: %d", 	answer.res.type);
		tb_trace_impl("response: class: %d", 	answer.res.class_);
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
static tb_bool_t tb_aicp_addr_reqt_func(tb_aice_t const* aice);
static tb_bool_t tb_aicp_addr_resp_func(tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_URECV, tb_false);

	// the aicp
	tb_aicp_t* aicp = (tb_aicp_t*)tb_aico_aicp(aice->aico);
	tb_assert_and_check_return_val(aicp, tb_false);
	
	// the addr
	tb_aicp_addr_t* addr = aice->priv; 
	tb_assert_and_check_return_val(addr, tb_false);

	// done
	tb_ipv4_t ipv4 = {0};
	if (aice->state == TB_AICE_STATE_OK)
	{
		// trace
		tb_trace_impl("resp[%s]: server: %u.%u.%u.%u, real: %lu", addr->host, tb_ipv4_u8x4(aice->u.urecv.addr), aice->u.urecv.real);

		// check
		tb_assert_and_check_return_val(aice->u.urecv.real, tb_false);

		// done resp
		tb_aicp_addr_resp_done(addr, aice->u.urecv.real, &ipv4);
	}
	// timeout or failed?
	else
	{
		// trace
		tb_trace_impl("resp[%s]: server: %u.%u.%u.%u, state: %s", addr->host, tb_ipv4_u8x4(aice->u.urecv.addr), tb_aice_state_cstr(aice));
	}

	// ok?
	if (ipv4.u32) 
	{
		// save to cache
		tb_dns_cache_set(addr->host, &ipv4);
		
		// done func
		addr->func(addr, addr->host, &ipv4, addr->priv);
		return tb_true;
	}

	// try next server?
	tb_bool_t ok = tb_false;
	tb_ipv4_t const* server = &addr->list[addr->indx + 1];
	if (server->u32)
	{	
		// indx++
		addr->indx++;

		// init reqt
		tb_size_t size = tb_aicp_addr_reqt_init(addr);
		if (size)
		{
			// post reqt
			ok = tb_aico_usend(aice->aico, server, TB_DNS_HOST_PORT, addr->data, size, tb_aicp_addr_reqt_func, (tb_pointer_t)addr);
		}
	}

	// failed? done func
	if (!ok) addr->func(addr, addr->host, tb_null, addr->priv);

	// continue
	return tb_true;
}
static tb_bool_t tb_aicp_addr_reqt_func(tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_USEND, tb_false);

	// the aicp
	tb_aicp_t* aicp = (tb_aicp_t*)tb_aico_aicp(aice->aico);
	tb_assert_and_check_return_val(aicp, tb_false);
	
	// the addr
	tb_aicp_addr_t* addr = aice->priv; 
	tb_assert_and_check_return_val(addr && addr->func, tb_false);

	// done
	tb_bool_t ok = tb_false;
	if (aice->state == TB_AICE_STATE_OK)
	{
		// trace
		tb_trace_impl("reqt[%s]: server: %u.%u.%u.%u, real: %lu", addr->host, tb_ipv4_u8x4(aice->u.usend.addr), aice->u.usend.real);

		// check
		tb_assert_and_check_return_val(aice->u.usend.real, tb_false);

		// the server 
		tb_ipv4_t const* server = &addr->list[addr->indx];
		tb_assert_and_check_return_val(server->u32, tb_false);

		// post resp
		ok = tb_aico_urecv(aice->aico, server, TB_DNS_HOST_PORT, addr->data, sizeof(addr->data), tb_aicp_addr_resp_func, (tb_pointer_t)addr);
	}
	// timeout or failed?
	else
	{
		// trace
		tb_trace_impl("reqt[%s]: server: %u.%u.%u.%u, state: %s", addr->host, tb_ipv4_u8x4(aice->u.usend.addr), tb_aice_state_cstr(aice));
			
		// the next server 
		tb_ipv4_t const* server = &addr->list[addr->indx + 1];
		if (server->u32)
		{	
			// indx++
			addr->indx++;

			// init reqt
			tb_size_t size = tb_aicp_addr_reqt_init(addr);
			if (size)
			{
				// post reqt
				ok = tb_aico_usend(aice->aico, server, TB_DNS_HOST_PORT, addr->data, size, tb_aicp_addr_reqt_func, (tb_pointer_t)addr);
			}
		}
	}

	// failed? done func
	if (!ok) addr->func(addr, addr->host, tb_null, addr->priv);

	// continue 
	return tb_true;
}
/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_handle_t tb_aicp_addr_init(tb_aicp_t* aicp, tb_long_t timeout, tb_aicp_addr_func_t func, tb_pointer_t data)
{
	// check
	tb_assert_and_check_return_val(aicp && func, tb_null);

	// done
	tb_bool_t 		ok = tb_false;
	tb_aicp_addr_t* addr = tb_null;
	tb_handle_t 	sock = tb_null;
	tb_handle_t 	aico = tb_null;
	do
	{
		// init sock
		sock = tb_socket_open(TB_SOCKET_TYPE_UDP);
		tb_assert_and_check_break(sock);

		// init aico
		aico = tb_aico_init_sock(aicp, sock);
		tb_assert_and_check_break(aico);

		// init timeout
		tb_aico_timeout_set(aico, TB_AICO_TIMEOUT_SEND, timeout);
		tb_aico_timeout_set(aico, TB_AICO_TIMEOUT_RECV, timeout);

		// make addr
		addr = (tb_aicp_addr_t*)tb_aico_pool_malloc0(aico, sizeof(tb_aicp_addr_t));
		tb_assert_and_check_break(addr);

		// init addr
		addr->sock = sock;
		addr->aico = aico;
		addr->func = func;
		addr->priv = data;
		sock = tb_null;
		aico = tb_null;

		// ok
		ok = tb_true;

	} while (0);

	// failed?
	if (!ok)
	{
		// exit it
		if (addr) tb_aicp_addr_exit(addr, tb_false);
		addr = tb_null;

		// exit aico
		if (aico) tb_aico_exit(aico, tb_false);
		aico = tb_null;

		// exit sock
		if (sock) tb_socket_clos(sock);
		sock = tb_null;
	}

	// ok?
	return addr;
}
tb_void_t tb_aicp_addr_kill(tb_handle_t handle)
{
	// check
	tb_aicp_addr_t* addr = (tb_aicp_addr_t*)handle;
	tb_assert_and_check_return(addr);

	// trace
	tb_trace_impl("kill: ..");

	// kill sock
	if (addr->sock) tb_socket_kill(addr->sock, TB_SOCKET_KILL_RW);
}
tb_void_t tb_aicp_addr_exit(tb_handle_t handle, tb_bool_t bcalling)
{
	// check
	tb_aicp_addr_t* addr = (tb_aicp_addr_t*)handle;
	tb_assert_and_check_return(addr);

	// trace
	tb_trace_impl("exit: ..");

	// the aico
	tb_handle_t aico = addr->aico;

	// the sock
	tb_handle_t sock = addr->sock;

	// wait pending before freeing addr
	if (aico) tb_aico_wait(aico, bcalling);

	// exit it
	if (aico) tb_aico_pool_free(aico, addr);

	// exit aico
	if (aico) tb_aico_exit(aico, bcalling);

	// exit sock
	if (sock) tb_socket_clos(sock);

	// trace
	tb_trace_impl("exit: ok");
}
tb_bool_t tb_aicp_addr_done(tb_handle_t handle, tb_char_t const* host)
{
	// check
	tb_aicp_addr_t* addr = (tb_aicp_addr_t*)handle;
	tb_assert_and_check_return_val(addr && addr->aico && addr->func && host && host[0], tb_false);
	
	// save host
	tb_strlcpy(addr->host, host, sizeof(addr->host) - 1);
 
	// only ipv4? ok
	tb_ipv4_t ipv4 = {0};
	if (tb_ipv4_set(&ipv4, addr->host))
	{
		addr->func(handle, addr->host, &ipv4, addr->priv);
		return tb_true;
	}

	// try to lookup it from cache first
	if (tb_dns_cache_get(addr->host, &ipv4))
	{
		addr->func(handle, addr->host, &ipv4, addr->priv);
		return tb_true;
	}

	// init server list
	if (!addr->size) addr->size = tb_dns_server_get(addr->list);
	tb_check_return_val(addr->size, tb_false);

	// get the server 
	tb_ipv4_t const* server = &addr->list[addr->indx = 0];
	tb_assert_and_check_return_val(server->u32, tb_false);

	// init reqt
	tb_size_t size = tb_aicp_addr_reqt_init(addr);
	tb_assert_and_check_return_val(size, tb_false);

	// post reqt
	return tb_aico_usend(addr->aico, server, TB_DNS_HOST_PORT, addr->data, size, tb_aicp_addr_reqt_func, (tb_pointer_t)addr);
}
tb_aicp_t* tb_aicp_addr_aicp(tb_handle_t handle)
{
	// check
	tb_aicp_addr_t* addr = (tb_aicp_addr_t*)handle;
	tb_assert_and_check_return_val(addr && addr->aico, tb_null);
	
	// the aicp
	return tb_aico_aicp(addr->aico);
}
