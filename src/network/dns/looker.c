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
 * @file		looker.c
 * @ingroup 	network
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
//#define TB_TRACE_IMPL_TAG 		"looker"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "looker.h"
#include "cache.h"
#include "server.h"
#include "../../asio/asio.h"
#include "../../string/string.h"
#include "../../memory/memory.h"
#include "../../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */

// the dns looker step enum
typedef enum __tb_dns_looker_step_e
{
	TB_DNS_LOOKER_STEP_NONE 	= 0
,	TB_DNS_LOOKER_STEP_REQT 	= 1
,	TB_DNS_LOOKER_STEP_RESP 	= 2
,	TB_DNS_LOOKER_STEP_NEVT 	= 4

}tb_dns_looker_step_e;

// the dns looker type
typedef struct __tb_dns_looker_t
{
	// the name
	tb_sstring_t 		name;

	// the request & response packet
	tb_sbuffer_t 		rpkt;
	
	// the size for recv & send packet
	tb_size_t 			size;

	// the iterator
	tb_size_t 			itor;

	// the step
	tb_size_t 			step;

	// the tryn
	tb_size_t 			tryn;

	// the socket
	tb_handle_t 		sock;

	// the server list
	tb_ipv4_t 			list[2];

	// the server maxn
	tb_size_t 			maxn;

	// the data
	tb_byte_t 			data[TB_DNS_NAME_MAXN + TB_DNS_RPKT_MAXN];

}tb_dns_looker_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_long_t tb_dns_looker_reqt(tb_dns_looker_t* looker)
{
	// check
	tb_check_return_val(!(looker->step & TB_DNS_LOOKER_STEP_REQT), 1);
	
	// format it first if the request is null
	if (!tb_sbuffer_size(&looker->rpkt))
	{
		// check size
		tb_assert_and_check_return_val(!looker->size, -1);

		// format query
		tb_bstream_t 	bst;
		tb_byte_t 		rpkt[TB_DNS_RPKT_MAXN];
		tb_size_t 		size = 0;
		tb_byte_t* 		p = tb_null;
		tb_bstream_init(&bst, rpkt, TB_DNS_RPKT_MAXN);

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
		p = tb_bstream_set_string(&bst, tb_sstring_cstr(&looker->name));

		// only one question now.
		tb_bstream_set_u16_be(&bst, 1); 		// we are requesting the ipv4 address
		tb_bstream_set_u16_be(&bst, 1); 		// it's internet (lol)

		// encode dns name
		if (!p || !tb_dns_encode_name(p - 1)) return -1;

		// size
		size = tb_bstream_offset(&bst);
		tb_assert_and_check_return_val(size, -1);

		// copy
		tb_sbuffer_memncpy(&looker->rpkt, rpkt, size);
	}

	// data && size
	tb_byte_t const* 	data = tb_sbuffer_data(&looker->rpkt);
	tb_size_t 			size = tb_sbuffer_size(&looker->rpkt);

	// check
	tb_assert_and_check_return_val(data && size && looker->size < size, -1);

	// try get addr from the dns list
	tb_ipv4_t const* addr = tb_null;
	if (looker->maxn && looker->itor && looker->itor <= looker->maxn)
		addr = &looker->list[looker->itor - 1];

	// check
	tb_assert_and_check_return_val(addr && addr->u32, -1);

	// need wait if no data
	looker->step &= ~TB_DNS_LOOKER_STEP_NEVT;

	// send request
	tb_trace_impl("request: try %u.%u.%u.%u", addr->u8[0], addr->u8[1], addr->u8[2], addr->u8[3]);
	while (looker->size < size)
	{
		// writ data
		tb_long_t writ = tb_socket_usend(looker->sock, addr, TB_DNS_HOST_PORT, data + looker->size, size - looker->size);
		//tb_trace("writ: %d", writ);
		tb_assert_and_check_return_val(writ >= 0, -1);

		// no data? 
		if (!writ)
		{
			// abort?
			tb_check_return_val(!looker->size && !looker->tryn, -1);

			// tryn++
			looker->tryn++;

			// continue
			return 0;
		}
		else looker->tryn = 0;

		// update size
		looker->size += writ;
	}

	// finish it
	looker->step |= TB_DNS_LOOKER_STEP_REQT;
	looker->tryn = 0;

	// reset rpkt
	looker->size = 0;
	tb_sbuffer_clear(&looker->rpkt);

	// ok
	tb_trace_impl("request: ok");
	return 1;
}
static tb_bool_t tb_dns_looker_resp_done(tb_dns_looker_t* looker, tb_ipv4_t* ipv4)
{
	// rpkt && size
	tb_byte_t const* 	rpkt = tb_sbuffer_data(&looker->rpkt);
	tb_size_t 			size = tb_sbuffer_size(&looker->rpkt);

	// check
	tb_assert_and_check_return_val(rpkt && size >= TB_DNS_HEADER_SIZE, tb_false);

	// decode dns header
	tb_bstream_t 	bst;
	tb_dns_header_t header;
	tb_bstream_init(&bst, (tb_byte_t*)rpkt, size);
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
static tb_long_t tb_dns_looker_resp(tb_dns_looker_t* looker, tb_ipv4_t* ipv4)
{
	// check
	tb_check_return_val(!(looker->step & TB_DNS_LOOKER_STEP_RESP), 1);

	// try get addr from the dns list
	tb_ipv4_t const* addr = tb_null;
	if (looker->maxn && looker->itor && looker->itor <= looker->maxn)
		addr = &looker->list[looker->itor - 1];

	// check
	tb_assert_and_check_return_val(addr && addr->u32, -1);

	// need wait if no data
	looker->step &= ~TB_DNS_LOOKER_STEP_NEVT;

	// recv response data
	tb_byte_t rpkt[4096];
	while (1)
	{
		// read data
		tb_long_t read = tb_socket_urecv(looker->sock, addr, TB_DNS_HOST_PORT, rpkt, 4096);
		//tb_trace_impl("read %d", read);
		tb_assert_and_check_return_val(read >= 0, -1);

		// no data? 
		if (!read)
		{
			// end? read x, read 0
			tb_check_break(!tb_sbuffer_size(&looker->rpkt));
	
			// abort? read 0, read 0
			tb_check_return_val(!looker->tryn, -1);
			
			// tryn++
			looker->tryn++;

			// continue 
			return 0;
		}
		else looker->tryn = 0;

		// copy data
		tb_sbuffer_memncat(&looker->rpkt, rpkt, read);
	}

	// done
	if (!tb_dns_looker_resp_done(looker, ipv4)) return -1;

	// check
	tb_assert_and_check_return_val(tb_sstring_size(&looker->name) && ipv4->u32, -1);

	// add to cache
	tb_dns_cache_set(tb_sstring_cstr(&looker->name), ipv4);

	// finish it
	looker->step |= TB_DNS_LOOKER_STEP_RESP;
	looker->tryn = 0;

	// reset rpkt
	looker->size = 0;
	tb_sbuffer_clear(&looker->rpkt);

	// ok
	tb_trace_impl("response: ok");
	return 1;
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_handle_t tb_dns_looker_init(tb_char_t const* name)
{
	// check
	tb_assert_and_check_return_val(name, tb_null);

	// must be not ipv4
	tb_assert_return_val(!tb_ipv4_set(tb_null, name), tb_null);

	// make looker
	tb_dns_looker_t* looker = tb_malloc0(sizeof(tb_dns_looker_t));
	tb_assert_and_check_return_val(looker, tb_null);

	// dump server
//	tb_dns_server_dump();

	// get the dns server list
	looker->maxn = tb_dns_server_get(looker->list);
	tb_assert_and_check_goto(looker->maxn && looker->maxn <= tb_arrayn(looker->list), fail);

	// init name
	if (!tb_sstring_init(&looker->name, looker->data, TB_DNS_NAME_MAXN)) goto fail;
	tb_sstring_cstrcpy(&looker->name, name);

	// init rpkt
	if (!tb_sbuffer_init(&looker->rpkt, looker->data + TB_DNS_NAME_MAXN, TB_DNS_RPKT_MAXN)) goto fail;

	// init sock
	looker->sock = tb_socket_open(TB_SOCKET_TYPE_UDP);
	tb_assert_and_check_goto(looker->sock, fail);

	// init itor
	looker->itor = 1;

	// ok
	return (tb_handle_t)looker;

fail:
	if (looker) tb_dns_looker_exit(looker);
	return tb_null;
}
tb_long_t tb_dns_looker_spak(tb_handle_t handle, tb_ipv4_t* addr)
{
	// check
	tb_dns_looker_t* looker = (tb_dns_looker_t*)handle;
	tb_assert_and_check_return_val(looker && addr, -1);

	// init 
	tb_long_t r = -1;
	
	// request
	r = tb_dns_looker_reqt(looker);
	tb_check_goto(r >= 0, fail);
	tb_check_return_val(r > 0, r);
		
	// response
	r = tb_dns_looker_resp(looker, addr);
	tb_check_goto(r >= 0, fail);
	tb_check_return_val(r > 0, r);

	// ok
	return r;

fail:
	
	// next
	if (looker->itor + 1 <= looker->maxn) looker->itor++;
	else looker->itor = 0;

	// has next?
	if (looker->itor)
	{
		// reset step, no event now, need not wait
		looker->step = TB_DNS_LOOKER_STEP_NONE | TB_DNS_LOOKER_STEP_NEVT;

		// reset rpkt
		looker->size = 0;
		tb_sbuffer_clear(&looker->rpkt);

		// continue 
		return 0;
	}

	return -1;
}
tb_long_t tb_dns_looker_wait(tb_handle_t handle, tb_long_t timeout)
{
	// check
	tb_dns_looker_t* looker = (tb_dns_looker_t*)handle;
	tb_assert_and_check_return_val(looker && looker->sock, -1);

	// has asio event?
	tb_size_t e = TB_AIOE_CODE_NONE;
	if (!(looker->step & TB_DNS_LOOKER_STEP_NEVT))
	{
		if (!(looker->step & TB_DNS_LOOKER_STEP_REQT)) e = TB_AIOE_CODE_SEND;
		else if (!(looker->step & TB_DNS_LOOKER_STEP_RESP)) e = TB_AIOE_CODE_RECV;
	}

	// need wait?
	tb_long_t r = 0;
	if (e)
	{
		// wait
		r = tb_aioo_wait(looker->sock, e, timeout);

		// fail or timeout?
		tb_check_return_val(r > 0, r);
	}

	// ok?
	return r;
}
tb_void_t tb_dns_looker_exit(tb_handle_t handle)
{
	// the looker
	tb_dns_looker_t* looker = (tb_dns_looker_t*)handle;
	if (looker)
	{
		// exit sock
		if (looker->sock) tb_socket_clos(looker->sock);
		looker->sock = tb_null;

		// exit it
		tb_free(looker);
	}
}
tb_bool_t tb_dns_looker_done(tb_char_t const* name, tb_ipv4_t* addr)
{
	// check
	tb_assert_and_check_return_val(name && addr, tb_false);

	// try to lookup it from cache first
	if (tb_dns_cache_get(name, addr)) return tb_true;

	// init looker
	tb_handle_t looker = tb_dns_looker_init(name);
	tb_assert_and_check_return_val(looker, tb_false);

	// spak
	tb_long_t r = -1;
	while (!(r = tb_dns_looker_spak(looker, addr)))
	{
		// wait
		r = tb_dns_looker_wait(looker, TB_DNS_TIMEOUT);
		tb_assert_and_check_goto(r >= 0, end);
	}

end:

	// exit
	tb_dns_looker_exit(looker);

	// ok
	return r > 0? tb_true : tb_false;
}

