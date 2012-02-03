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
 * If not, see <a href="dns://www.gnu.org/licenses/"> dns://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		dns.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "dns.h"
#include "ipv4.h"
#include "../aio/aio.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../string/string.h"
#include "../stream/stream.h"
#include "../memory/memory.h"
#include "../platform/platform.h"
#include "../container/container.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// the protocol header
#define TB_DNS_HEADER_SIZE 			(12)
#define TB_DNS_HEADER_MAGIC 		(0xbeef)

// the protocol port
#define TB_DNS_HOST_PORT 			(53)

// the host maximum size 
#define TB_DNS_HOST_MAXN 			(16)

// the name maximum size 
#define TB_DNS_NAME_MAXN 			(256)

// the rpkt maximum size 
#define TB_DNS_RPKT_MAXN 			(TB_DNS_HEADER_SIZE + TB_DNS_NAME_MAXN + 256)

// the timout default
#define TB_DNS_TIMEOUT 				(1000)

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the dns host type
typedef struct __tb_dns_host_t
{
	// the host
	tb_ipv4_t 		host;

	// the rate
	tb_long_t 		rate;

}tb_dns_host_t;

// the dns addr type
typedef struct __tb_dns_addr_t
{
	// the addr
	tb_ipv4_t 		addr;

	// the date
	tb_size_t 		date;

}tb_dns_addr_t;

// the dns list type
typedef struct __tb_dns_list_t
{
	// the list
	tb_slist_t* 	list;

	// the mutx
	tb_handle_t 	mutx;

	// the spool
	tb_spool_t* 	spool;

	// the cache
	tb_hash_t* 		cache;

}tb_dns_list_t;

// the dns step type
typedef enum __tb_dns_step_t
{
	TB_DNS_STEP_NONE 	= 0
,	TB_DNS_STEP_REQT 	= 1
,	TB_DNS_STEP_RESP 	= 2
,	TB_DNS_STEP_NEVT 	= 4

}tb_dns_step_t;

// the dns look type
typedef struct __tb_dns_look_t
{
	// the host
	tb_sstring_t 	host;

	// the name
	tb_sstring_t 	name;

	// the request & response packet
	tb_sbuffer_t 	rpkt;
	
	// the size for recv & send packet
	tb_size_t 		size;

	// the iterator
	tb_size_t 		itor;

	// the step
	tb_size_t 		step;

	// the wait
	tb_size_t 		wait;

	// try the next host?
	tb_bool_t 		next;

	// the socket
	tb_handle_t 	sock;

	// the data
	tb_byte_t 		data[TB_DNS_HOST_MAXN + TB_DNS_NAME_MAXN + TB_DNS_RPKT_MAXN];

}tb_dns_look_t;

// the dns header type
typedef struct __tb_dns_header_t
{
	tb_uint16_t 	id; 			// identification number

	tb_uint16_t 	qr     :1;		// query/response flag
	tb_uint16_t 	opcode :4;	    // purpose of message
	tb_uint16_t 	aa     :1;		// authoritive answer
	tb_uint16_t 	tc     :1;		// truncated message
	tb_uint16_t 	rd     :1;		// recursion desired

	tb_uint16_t 	ra     :1;		// recursion available
	tb_uint16_t 	z      :1;		// its z! reserved
	tb_uint16_t 	ad     :1;	    // authenticated data
	tb_uint16_t 	cd     :1;	    // checking disabled
	tb_uint16_t 	rcode  :4;	    // response code

	tb_uint16_t 	question;	    // number of question entries
	tb_uint16_t 	answer;			// number of answer entries
	tb_uint16_t 	authority;		// number of authority entries
	tb_uint16_t 	resource;		// number of resource entries

}tb_dns_header_t;

// the dns question type
typedef struct __tb_dns_question_t
{
	tb_uint16_t 	type;
	tb_uint16_t 	class;

}tb_dns_question_t;

// the dns resource type
typedef struct __tb_dns_resource_t
{
	tb_uint16_t 	type;
	tb_uint16_t 	class;
	tb_uint32_t 	ttl;
	tb_uint16_t 	size;

}tb_dns_resource_t;

// the dns answer type
typedef struct __tb_dns_answer_t
{
	tb_char_t 			name[TB_DNS_NAME_MAXN];
	tb_dns_resource_t 	res;
	tb_byte_t const* 	rdata;

}tb_dns_answer_t;

/* ///////////////////////////////////////////////////////////////////////
 * globals
 */

// the dns list
static tb_dns_list_t* 	g_dns_list = TB_NULL;

/* ///////////////////////////////////////////////////////////////////////
 * implemention
 */

// size + data, e.g. .www.google.com => 3www6google3com
static tb_char_t const* tb_dns_encode_name(tb_char_t* name)
{
	tb_assert_and_check_return_val(name && name[0] == '.', TB_NULL);
	
	// encode
	tb_size_t 	n = 0;
	tb_char_t* 	b = name;
	tb_char_t* 	p = name + 1;
	while (*p)
	{
		if (*p == '.')
		{
			//*b = '0' + n;
			*b = n;
			n = 0;
			b = p;
		}
		else n++;
		p++;
	}
	//*b = '0' + n;
	*b = n;

	// ok
	return name;
}
static tb_char_t const* tb_dns_parse_name_impl(tb_char_t const* sb, tb_char_t const* se, tb_char_t const* ps, tb_char_t** pd)
{
	tb_char_t* p = ps;
	tb_char_t* q = *pd;
	while (p < se)
	{
		tb_byte_t c = *p++;
		if (!c) break;
		// is pointer? 11xxxxxx xxxxxxxx
		else if (c >= 0xc0)
		{
			tb_uint16_t pos = c;
			pos &= ~0xc0;
			pos <<= 8;
			pos |= *p++;
			tb_dns_parse_name_impl(sb, se, sb + pos, &q);
			break; 
		}
		// is ascii? 00xxxxxx
		else
		{
			while (c--) *q++ = *p++;
			*q++ = '.';
		}
	}
	*pd = q;
	return p;
}
static tb_char_t const* tb_dns_parse_name(tb_bstream_t* bst, tb_char_t* name)
{
	tb_char_t* q = name;
	tb_char_t* p = tb_dns_parse_name_impl(tb_bstream_beg(bst), tb_bstream_end(bst), tb_bstream_pos(bst), &q);
	if (p)
	{
		tb_assert(q - name < TB_DNS_NAME_MAXN);
		if (q > name && *(q - 1) == '.') *--q = '\0';
		tb_bstream_goto(bst, p);
		return name;
	}
	else return TB_NULL;
}
static tb_long_t tb_dns_host_rate(tb_char_t const* host)
{
	tb_assert_and_check_return_val(host, 0);

	// open socket
	tb_handle_t handle = tb_socket_open(TB_SOCKET_TYPE_UDP);
	tb_assert_and_check_return_val(handle, 0);

	// init 
	tb_long_t rate = -1;

	// format query
	tb_bstream_t 	bst;
	tb_byte_t 		rpkt[TB_DNS_RPKT_MAXN];
	tb_size_t 		size = 0;
	tb_char_t* 		p = TB_NULL;
	tb_bstream_attach(&bst, rpkt, TB_DNS_RPKT_MAXN);

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
	tb_bstream_set_u8(&bst, 0);

	// only one question now.
	tb_bstream_set_u16_be(&bst, 1); 		// we are requesting the ipv4 address
	tb_bstream_set_u16_be(&bst, 1); 		// it's internet (lol)

	// size
	size = tb_bstream_offset(&bst);
	tb_assert_and_check_goto(size, end);

	// init time
	tb_int64_t time = tb_mclock();

	// send request
	tb_long_t writ = 0;
	tb_bool_t wait = TB_FALSE;
	while (writ < size)
	{
		// writ data
		tb_long_t r = tb_socket_usend(handle, host, TB_DNS_HOST_PORT, rpkt + writ, size - writ);
		//tb_trace("[dns]: writ %d", r);
		tb_assert_and_check_goto(r >= 0, end);
		
		// no data?
		if (!r)
		{
			// abort?
			tb_check_goto(!wait, end);

			// wait
			tb_aioo_t o;
			tb_aioo_seto(&o, handle, TB_AIOO_OTYPE_SOCK, TB_AIOO_ETYPE_WRIT, TB_NULL);
			r = tb_aioo_wait(&o, TB_DNS_TIMEOUT);

			// fail or timeout?
			tb_check_goto(r > 0, end);

			// be waiting
			wait = TB_TRUE;
		}
		else
		{
			// no waiting
			wait = TB_FALSE;

			// update size
			writ += r;
		}
	}

	// reset wait
	wait = TB_FALSE;

	// only recv id, 2 bytes 
	tb_long_t read = 0;
	while (read < 2)
	{
		// read data
		tb_long_t r = tb_socket_urecv(handle, host, TB_DNS_HOST_PORT, rpkt + read, TB_DNS_RPKT_MAXN - read);
		//tb_trace("[dns]: read %d", r);
		tb_check_break(r >= 0);
		
		// no data?
		if (!r)
		{
			// end?
			tb_check_break(!wait);

			// wait
			tb_aioo_t o;
			tb_aioo_seto(&o, handle, TB_AIOO_OTYPE_SOCK, TB_AIOO_ETYPE_READ, TB_NULL);
			r = tb_aioo_wait(&o, TB_DNS_TIMEOUT);

			// fail or timeout?
			tb_check_break(r > 0);

			// be waiting
			wait = TB_TRUE;
		}
		else
		{
			// no waiting
			wait = TB_FALSE;

			// update size
			read += r;
		}
	}

	// check
	tb_check_goto(read >= 2, end);

	// check dns header
	tb_size_t id = tb_bits_get_u16_be(rpkt);
	tb_check_goto(id == TB_DNS_HEADER_MAGIC, end);

	// rate
	rate = (tb_long_t)(tb_mclock() - time);

	// ok
	tb_trace("[dns]: host: %s ok, rate: %u", host, rate);

end:
	// exit sock
	tb_socket_exit(handle);

	// ok
	return rate;
}
static tb_long_t tb_dns_look_reqt(tb_dns_look_t* look)
{
	tb_check_return_val(!(look->step & TB_DNS_STEP_REQT), 1);
	
	// format it first if the request is null
	if (!tb_sbuffer_size(&look->rpkt))
	{
		// check size
		tb_assert_and_check_return_val(!look->size, -1);

		// format query
		tb_bstream_t 	bst;
		tb_byte_t 		rpkt[TB_DNS_RPKT_MAXN];
		tb_size_t 		size = 0;
		tb_byte_t* 		p = TB_NULL;
		tb_bstream_attach(&bst, rpkt, TB_DNS_RPKT_MAXN);

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
		p = tb_bstream_set_string(&bst, tb_sstring_cstr(&look->name));

		// only one question now.
		tb_bstream_set_u16_be(&bst, 1); 		// we are requesting the ipv4 address
		tb_bstream_set_u16_be(&bst, 1); 		// it's internet (lol)

		// encode dns name
		if (!p || !tb_dns_encode_name(p - 1)) return -1;

		// size
		size = tb_bstream_offset(&bst);
		tb_assert_and_check_return_val(size, -1);

		// copy
		tb_sbuffer_memncpy(&look->rpkt, rpkt, size);
	}

	// data && size
	tb_byte_t const* 	data = tb_sbuffer_data(&look->rpkt);
	tb_size_t 			size = tb_sbuffer_size(&look->rpkt);

	// check
	tb_assert_and_check_return_val(data && size && look->size < size, -1);

	// try get host from the dns list
	if (!tb_sstring_size(&look->host))
	{
		if (g_dns_list && g_dns_list->mutx) 
		{
			// enter
			tb_mutex_enter(g_dns_list->mutx);

			// host
			tb_char_t 				ipv4[16];
			tb_dns_host_t const* 	item = (tb_dns_host_t const*)tb_slist_itor_const_at(g_dns_list->list, look->itor);
			if (item)
			{
				tb_char_t const* host = tb_ipv4_get(&item->host, ipv4, 16);
				if (host) tb_sstring_cstrcpy(&look->host, host);
			}

			// leave
			tb_mutex_leave(g_dns_list->mutx);
		}
	}

	// host
	tb_char_t const* host = tb_sstring_cstr(&look->host);

	// check
	tb_assert_and_check_return_val(host && tb_sstring_size(&look->host), -1);

	// need wait if no data
	look->step &= ~TB_DNS_STEP_NEVT;

	// send request
	tb_trace("[dns]: request: try %s", host);
	while (look->size < size)
	{
		// writ data
		tb_long_t writ = tb_socket_usend(look->sock, host, TB_DNS_HOST_PORT, data + look->size, size - look->size);
		//tb_trace("writ: %d", writ);
		tb_assert_and_check_return_val(writ >= 0, -1);

		// no data? 
		tb_check_return_val(writ, 0);

		// update size
		look->size += writ;
	}

	// finish it
	look->step |= TB_DNS_STEP_REQT;

	// reset rpkt
	look->size = 0;
	tb_sbuffer_clear(&look->rpkt);

	// ok
	tb_trace("[dns]: request: ok");
	return 1;
}
static tb_bool_t tb_dns_look_resp_done(tb_dns_look_t* look, tb_ipv4_t* ipv4)
{
	// rpkt && size
	tb_byte_t const* 	rpkt = tb_sbuffer_data(&look->rpkt);
	tb_size_t 			size = tb_sbuffer_size(&look->rpkt);

	// check
	tb_assert_and_check_return_val(rpkt && size >= TB_DNS_HEADER_SIZE, TB_FALSE);

	// parse dns header
	tb_bstream_t 	bst;
	tb_dns_header_t header;
	tb_bstream_attach(&bst, rpkt, size);
	header.id = tb_bstream_get_u16_be(&bst);
	tb_bstream_skip(&bst, 2);
	header.question 	= tb_bstream_get_u16_be(&bst);
	header.answer 		= tb_bstream_get_u16_be(&bst);
	header.authority 	= tb_bstream_get_u16_be(&bst);
	header.resource 	= tb_bstream_get_u16_be(&bst);
	tb_trace("[dns]: response: size: %u", 		size);
	tb_trace("[dns]: response: id: 0x%04x", 	header.id);
	tb_trace("[dns]: response: question: %d", 	header.question);
	tb_trace("[dns]: response: answer: %d", 	header.answer);
	tb_trace("[dns]: response: authority: %d", 	header.authority);
	tb_trace("[dns]: response: resource: %d", 	header.resource);
	tb_trace("[dns]: ");

	// check header
	tb_assert_and_check_return_val(header.id == TB_DNS_HEADER_MAGIC, TB_FALSE);

	// skip questions, only one question now.
	// name + question1 + question2 + ...
	tb_assert_and_check_return_val(header.question == 1, TB_FALSE);
#if 1
	tb_bstream_skip_string(&bst);
	tb_bstream_skip(&bst, 4);
#else
	tb_char_t* name = tb_bstream_get_string(&bst);
	//name = tb_dns_decode_name(name);
	tb_assert_and_check_return_val(name, TB_FALSE);
	tb_bstream_skip(&bst, 4);
	tb_trace("[dns]: response: name: %s", name);
#endif

	// parse answers
	tb_size_t i = 0;
	tb_size_t found = 0;
	for (i = 0; i < header.answer; i++)
	{
		// parse answer
		tb_dns_answer_t answer;
		tb_trace("[dns]: response: answer: %d", i);

		// parse dns name
		tb_char_t const* name = tb_dns_parse_name(&bst, answer.name);
		tb_trace("[dns]: response: name: %s", name? name : "");

		// parse resource
		answer.res.type 	= tb_bstream_get_u16_be(&bst);
		answer.res.class 	= tb_bstream_get_u16_be(&bst);
		answer.res.ttl 		= tb_bstream_get_u32_be(&bst);
		answer.res.size 	= tb_bstream_get_u16_be(&bst);
		tb_trace("[dns]: response: type: %d", 	answer.res.type);
		tb_trace("[dns]: response: class: %d", 	answer.res.class);
		tb_trace("[dns]: response: ttl: %d", 	answer.res.ttl);
		tb_trace("[dns]: response: size: %d", 	answer.res.size);

		// is ipv4?
		if (answer.res.type == 1)
		{
			tb_byte_t b1 = tb_bstream_get_u8(&bst);
			tb_byte_t b2 = tb_bstream_get_u8(&bst);
			tb_byte_t b3 = tb_bstream_get_u8(&bst);
			tb_byte_t b4 = tb_bstream_get_u8(&bst);
			tb_trace("[dns]: response: ipv4: %u.%u.%u.%u", b1, b2, b3, b4);

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
				tb_trace("[dns]: response: ");
				break;
			}
		}
		else
		{
			// parse rdata
			answer.rdata = tb_dns_parse_name(&bst, answer.name);
			tb_trace("[dns]: response: alias: %s", answer.rdata? answer.rdata : "");
		}
		tb_trace("[dns]: response: ");
	}

	// found it?
	tb_check_return_val(found, TB_FALSE);

#if 0
	// parse authorities
	for (i = 0; i < header.authority; i++)
	{
		// parse answer
		tb_dns_answer_t answer;
		tb_trace("[dns]: response: authority: %d", i);

		// parse dns name
		tb_char_t* name = tb_dns_parse_name(&bst, answer.name);
		tb_trace("[dns]: response: name: %s", name? name : "");

		// parse resource
		answer.res.type = 	tb_bstream_get_u16_be(&bst);
		answer.res.class = 	tb_bstream_get_u16_be(&bst);
		answer.res.ttl = 	tb_bstream_get_u32_be(&bst);
		answer.res.size = 	tb_bstream_get_u16_be(&bst);
		tb_trace("[dns]: response: type: %d", 	answer.res.type);
		tb_trace("[dns]: response: class: %d", 	answer.res.class);
		tb_trace("[dns]: response: ttl: %d", 	answer.res.ttl);
		tb_trace("[dns]: response: size: %d", 	answer.res.size);

		// is ipv4?
		if (answer.res.type == 1)
		{
			tb_byte_t b1 = tb_bstream_get_u8(&bst);
			tb_byte_t b2 = tb_bstream_get_u8(&bst);
			tb_byte_t b3 = tb_bstream_get_u8(&bst);
			tb_byte_t b4 = tb_bstream_get_u8(&bst);
			tb_trace("[dns]: response: ipv4: %u.%u.%u.%u", b1, b2, b3, b4);
		}
		else
		{
			// parse data
			answer.rdata = tb_dns_parse_name(&bst, answer.name);
			tb_trace("[dns]: response: server: %s", answer.rdata? answer.rdata : "");
		}
		tb_trace("[dns]: response: ");
	}

	for (i = 0; i < header.resource; i++)
	{
		// parse answer
		tb_dns_answer_t answer;
		tb_trace("[dns]: response: resource: %d", i);

		// parse dns name
		tb_char_t* name = tb_dns_parse_name(&bst, answer.name);
		tb_trace("[dns]: response: name: %s", name? name : "");

		// parse resource
		answer.res.type = 	tb_bstream_get_u16_be(&bst);
		answer.res.class = 	tb_bstream_get_u16_be(&bst);
		answer.res.ttl = 	tb_bstream_get_u32_be(&bst);
		answer.res.size = 	tb_bstream_get_u16_be(&bst);
		tb_trace("[dns]: response: type: %d", 	answer.res.type);
		tb_trace("[dns]: response: class: %d", 	answer.res.class);
		tb_trace("[dns]: response: ttl: %d", 	answer.res.ttl);
		tb_trace("[dns]: response: size: %d", 	answer.res.size);

		// is ipv4?
		if (answer.res.type == 1)
		{
			tb_byte_t b1 = tb_bstream_get_u8(&bst);
			tb_byte_t b2 = tb_bstream_get_u8(&bst);
			tb_byte_t b3 = tb_bstream_get_u8(&bst);
			tb_byte_t b4 = tb_bstream_get_u8(&bst);
			tb_trace("[dns]: response: ipv4: %u.%u.%u.%u", b1, b2, b3, b4);
		}
		else
		{
			// parse data
			answer.rdata = tb_dns_parse_name(&bst, answer.name);
			tb_trace("[dns]: response: alias: %s", answer.rdata? answer.rdata : "");
		}
		tb_trace("[dns]: response: ");
	}
#endif

	// ok
	return TB_TRUE;
}
static tb_long_t tb_dns_look_resp(tb_dns_look_t* look, tb_ipv4_t* ipv4)
{
	tb_check_return_val(!(look->step & TB_DNS_STEP_RESP), 1);
	
	// try get host from the dns list
	if (!tb_sstring_size(&look->host))
	{
		if (g_dns_list && g_dns_list->mutx) 
		{
			// enter
			tb_mutex_enter(g_dns_list->mutx);

			// host
			tb_char_t 				addr[16];
			tb_dns_host_t const* 	item = (tb_dns_host_t const*)tb_slist_itor_const_at(g_dns_list->list, look->itor);
			if (item)
			{
				tb_char_t const* host = tb_ipv4_get(&item->host, addr, 16);
				if (host) tb_sstring_cstrcpy(&look->host, host);
			}

			// leave
			tb_mutex_leave(g_dns_list->mutx);
		}
	}

	// host
	tb_char_t const* host = tb_sstring_cstr(&look->host);

	// check
	tb_assert_and_check_return_val(host && tb_sstring_size(&look->host), -1);

	// need wait if no data
	look->step &= ~TB_DNS_STEP_NEVT;

	// recv response data
	tb_byte_t rpkt[4096];
	while (1)
	{
		// read data
		tb_long_t read = tb_socket_urecv(look->sock, host, TB_DNS_HOST_PORT, rpkt, 4096);
		tb_trace("[dns]: read %d", read);
		tb_assert_and_check_return_val(read >= 0, -1);

		// no data? 
		if (!read)
		{
			// no wait? wait it
			if (look->wait != TB_AIOO_ETYPE_READ) return 0;
			// end?
			else break;
		}
		// clear wait
		else look->wait = 0;

		// copy data
		tb_sbuffer_memncat(&look->rpkt, rpkt, read);
	}

	// done
	if (!tb_dns_look_resp_done(look, ipv4)) return -1;

	// finish it
	look->step |= TB_DNS_STEP_RESP;

	// reset rpkt
	look->size = 0;
	tb_sbuffer_clear(&look->rpkt);

	// ok
	tb_trace("[dns]: response: ok");
	return 1;
}
/* ///////////////////////////////////////////////////////////////////////
 * list
 */
tb_bool_t tb_dns_list_init()
{
	// no list?
	if (!g_dns_list)
	{
		// alloc list
		g_dns_list = tb_calloc(1, sizeof(tb_dns_list_t));
		tb_assert_and_check_return_val(g_dns_list, TB_FALSE);

		// init mutx
		g_dns_list->mutx = tb_mutex_init(TB_NULL);
		tb_assert_and_check_goto(g_dns_list->mutx, fail);
			
		// init list
		g_dns_list->list = tb_slist_init(8, tb_item_func_ifm(sizeof(tb_dns_host_t), TB_NULL, TB_NULL));
		tb_assert_and_check_goto(g_dns_list->list, fail);

		// init spool
		g_dns_list->spool = tb_spool_init(TB_SPOOL_SIZE_DEFAULT);
		tb_assert_and_check_goto(g_dns_list->spool, fail);
	
		// init cache
		g_dns_list->cache = tb_hash_init(TB_HASH_SIZE_DEFAULT, tb_item_func_str(TB_FALSE, g_dns_list->spool), tb_item_func_ifm(sizeof(tb_dns_addr_t), TB_NULL, TB_NULL));
		tb_assert_and_check_goto(g_dns_list->cache, fail);
	}

	// add the hosts
	tb_dns_list_adds("8.8.4.4");
	tb_dns_list_adds("8.8.8.8");

	// init local
	tb_dns_local_init();

	// ok
	return TB_TRUE;

fail:
	tb_dns_list_exit();
	return TB_FALSE;
}
tb_void_t tb_dns_list_adds(tb_char_t const* host)
{
	tb_assert_and_check_return(host);

	// init item
	tb_dns_host_t item;
	if (tb_ipv4_set(&item.host, host))
	{
		// the rate
		item.rate = tb_dns_host_rate(host);
		tb_check_goto(item.rate >= 0, fail);

		// has list?
		if (g_dns_list && g_dns_list->mutx) 
		{
			// enter
			tb_mutex_enter(g_dns_list->mutx);

			// has list?
			if (g_dns_list && g_dns_list->list)
			{
				// list
				tb_slist_t* list = g_dns_list->list;

				// find item
				tb_size_t prev = 0;
				tb_size_t itor = tb_slist_itor_head(list);
				tb_size_t tail = tb_slist_itor_tail(list);
				for (; itor != tail; prev = itor, itor = tb_slist_itor_next(list, itor))
				{
					tb_dns_host_t const* it = tb_slist_itor_const_at(list, itor);
					if (it && item.rate < it->rate) break;
				}

				// insert item by sort
				tb_slist_insert_next(g_dns_list->list, prev, &item);

				// leave
				if (g_dns_list->mutx) tb_mutex_leave(g_dns_list->mutx);
			}
		}

		// ok
		return ;
	}

fail:
	// trace
	tb_trace("[dns]: host: %s invalid", host);
}
tb_void_t tb_dns_list_dels(tb_char_t const* host)
{
	tb_assert_and_check_return(g_dns_list && host);

	// enter
	if (g_dns_list->mutx) tb_mutex_enter(g_dns_list->mutx);

	// list
	tb_slist_t* list = g_dns_list->list;
	tb_assert_and_check_goto(list, end);

	// ipv4
	tb_uint32_t ipv4 = tb_ipv4_set(TB_NULL, host);

	// find it
	tb_size_t itor = tb_slist_itor_head(list);
	tb_size_t tail = tb_slist_itor_tail(list);
	for (; itor != tail; itor = tb_slist_itor_next(list, itor))
	{
		tb_dns_host_t const* item = tb_slist_itor_const_at(list, itor);
		if (item && item->host.u32 == ipv4) break;
	}

	// remove it
	if (itor != tail) tb_slist_remove(list, itor);

end:
	// leave
	if (g_dns_list->mutx) tb_mutex_leave(g_dns_list->mutx);
}
tb_void_t tb_dns_list_exit()
{
	// exit local
	tb_dns_local_exit();

	// exit list
	tb_handle_t mutx = TB_NULL;
	if (g_dns_list)
	{
		// enter
		if (g_dns_list->mutx) tb_mutex_enter(g_dns_list->mutx);

		// exists?
		if (g_dns_list)
		{
			// save mutx
			mutx = g_dns_list->mutx;
			g_dns_list->mutx = TB_NULL;

			// exit cache
			if (g_dns_list->cache) tb_hash_exit(g_dns_list->cache);
			g_dns_list->cache = TB_NULL;

			// exit spool
			if (g_dns_list->spool) tb_spool_exit(g_dns_list->spool);
			g_dns_list->spool = TB_NULL;

			// free list
			if (g_dns_list->list) tb_slist_exit(g_dns_list->list);
			g_dns_list->list = TB_NULL;

			// free it
			tb_free(g_dns_list);
			g_dns_list = TB_NULL;

			// free mutx
			if (mutx) 
			{
				// leave
				tb_mutex_leave(mutx);

				// exit mutx
				tb_mutex_exit(mutx);
			}
		}
	}
}
#ifdef TB_DEBUG
tb_void_t tb_dns_list_dump()
{	
	tb_assert_and_check_return(g_dns_list);

	// enter
	if (g_dns_list->mutx) tb_mutex_enter(g_dns_list->mutx);

	// list
	tb_slist_t* list = g_dns_list->list;
	tb_assert_and_check_return(list);
	
	// find it
	tb_print("============================================================");
	tb_print("[dns]: list: %u items", tb_slist_size(list));
	tb_size_t itor = tb_slist_itor_head(list);
	tb_size_t tail = tb_slist_itor_tail(list);
	for (; itor != tail; itor = tb_slist_itor_next(list, itor))
	{
		tb_dns_host_t const* item = tb_slist_itor_const_at(list, itor);
		if (item) 
		{
			tb_print("[dns]: host: %u.%u.%u.%u, rate: %u"
				, item->host.u8[0]
				, item->host.u8[1]
				, item->host.u8[2]
				, item->host.u8[3]
				, item->rate);
		}
	}

	// leave
	if (g_dns_list->mutx) tb_mutex_leave(g_dns_list->mutx);
}
#endif

/* ///////////////////////////////////////////////////////////////////////
 * name
 */
tb_bool_t tb_dns_look_try4(tb_char_t const* name, tb_ipv4_t* ipv4)
{
	tb_assert_and_check_return_val(name && ipv4, TB_FALSE);

	// is ipv4?
	tb_check_return_val(!tb_ipv4_set(ipv4, name), TB_TRUE);

	// fail
	return TB_FALSE;
}
tb_handle_t tb_dns_look_init(tb_char_t const* name)
{
	tb_assert_and_check_return_val(name, TB_NULL);

	// must be not ipv4
	tb_assert_return_val(!tb_ipv4_set(TB_NULL, name), TB_NULL);

	// alloc
	tb_dns_look_t* look = tb_calloc(1, sizeof(tb_dns_look_t));
	tb_assert_and_check_return_val(look, TB_NULL);

	// init host
	if (!tb_sstring_init(&look->host, look->data, TB_DNS_HOST_MAXN)) goto fail;

	// init name
	if (!tb_sstring_init(&look->name, look->data + TB_DNS_HOST_MAXN, TB_DNS_NAME_MAXN)) goto fail;
	tb_sstring_cstrcpy(&look->name, name);

	// init rpkt
	if (!tb_sbuffer_init(&look->rpkt, look->data + TB_DNS_HOST_MAXN + TB_DNS_NAME_MAXN, TB_DNS_RPKT_MAXN)) goto fail;

	// init sock
	look->sock = tb_socket_open(TB_SOCKET_TYPE_UDP);
	tb_assert_and_check_goto(look->sock, fail);

	// init itor
	if (g_dns_list && g_dns_list->mutx) 
	{
		// enter
		tb_mutex_enter(g_dns_list->mutx);

		// itor
		if (g_dns_list && g_dns_list->list) look->itor = tb_slist_itor_head(g_dns_list->list);

		// leave
		tb_mutex_leave(g_dns_list->mutx);
	}
	tb_assert_and_check_goto(look->itor, fail);

	// ok
	return (tb_handle_t)look;

fail:
	if (look) tb_dns_look_exit(look);
	return TB_NULL;
}
tb_long_t tb_dns_look_spak(tb_handle_t handle, tb_ipv4_t* ipv4)
{
	tb_dns_look_t* look = (tb_dns_look_t*)handle;
	tb_assert_and_check_return_val(look && ipv4, -1);

	// init 
	tb_long_t r = -1;
	
	// request
	r = tb_dns_look_reqt(look);
	tb_check_goto(r >= 0, fail);
	tb_check_return_val(r > 0, r);
		
	// response
	r = tb_dns_look_resp(look, ipv4);
	tb_check_goto(r >= 0, fail);
	tb_check_return_val(r > 0, r);

	// ok
	return r;

fail:
	
	// next
	if (g_dns_list && g_dns_list->mutx) 
	{
		// enter
		tb_mutex_enter(g_dns_list->mutx);

		// next
		if (g_dns_list && g_dns_list->list) look->itor = tb_slist_itor_next(g_dns_list->list, look->itor);

		// leave
		tb_mutex_leave(g_dns_list->mutx);
	}

	// try next host
	if (look->itor)
	{
		// reset step, no event now, need not wait
		look->step = TB_DNS_STEP_NONE | TB_DNS_STEP_NEVT;

		// reset host
		tb_sstring_clear(&look->host);

		// reset rpkt
		look->size = 0;
		tb_sbuffer_clear(&look->rpkt);

		// reset wait
		look->wait = 0;

		// continue 
		return 0;
	}

	return -1;
}
tb_long_t tb_dns_look_wait(tb_handle_t handle, tb_long_t timeout)
{
	tb_dns_look_t* look = (tb_dns_look_t*)handle;
	tb_assert_and_check_return_val(look && look->sock, -1);

	// has aio event?
	tb_aioo_t o;
	tb_size_t e = TB_AIOO_ETYPE_NULL;
	if (!(look->step & TB_DNS_STEP_NEVT))
	{
		if (!(look->step & TB_DNS_STEP_REQT)) e = TB_AIOO_ETYPE_WRIT;
		else if (!(look->step & TB_DNS_STEP_RESP)) e = TB_AIOO_ETYPE_READ;
	}

	// save the event
	look->wait = e;

	// need wait?
	tb_long_t r = 0;
	if (e)
	{
		// wait
		tb_aioo_seto(&o, look->sock, TB_AIOO_OTYPE_SOCK, e, TB_NULL);
		r = tb_aioo_wait(&o, timeout);

		// fail or timeout?
		tb_check_return_val(r > 0, r);
	}

	// ok?
	return r;
}
tb_void_t tb_dns_look_exit(tb_handle_t handle)
{
	tb_dns_look_t* look = (tb_dns_look_t*)handle;
	if (look)
	{
		// close sock
		if (look->sock) tb_socket_close(look->sock);

		// free it
		tb_free(look);
	}
}
tb_bool_t tb_dns_look_done(tb_char_t const* name, tb_ipv4_t* ipv4)
{
	tb_assert_and_check_return_val(name && ipv4, TB_FALSE);

	// init
	tb_handle_t handle = tb_dns_look_init(name);
	tb_assert_and_check_return_val(handle, TB_FALSE);

	// spak
	tb_long_t r = -1;
	while (!(r = tb_dns_look_spak(handle, ipv4)))
	{
		// wait
		r = tb_dns_look_wait(handle, TB_DNS_TIMEOUT);
		tb_assert_and_check_goto(r >= 0, end);
	}

end:

	// exit
	tb_dns_look_exit(handle);

	// ok
	return r > 0? TB_TRUE : TB_FALSE;
}

