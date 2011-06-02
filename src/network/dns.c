/*!The Tiny Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		dns.c
 *
 */

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "dns.h"
#include "../math/math.h"
#include "../stream/stream.h"
#include "../string/string.h"
#include "../utils/utils.h"
#include "../platform/platform.h"


/* ////////////////////////////////////////////////////////////////////////
 * macros
 */
#if 0
# 	define TB_DNS_DBG 				TB_DBG
#else
# 	define TB_DNS_DBG
#endif

//#define TB_DNS_SERVER_TCP
#define TB_DNS_SERVER_PORT 			(53)
#define TB_DNS_SERVER_MAX 			(16)
#define TB_DNS_HEADER_SIZE 			(12)
#define TB_DNS_HEADER_ID 			(0xbeef) 	// or other...
#define TB_DNS_NAME_MAX 			(1024)

/* /////////////////////////////////////////////////////////
 * types
 */

// the dns header type
typedef struct __tb_dns_header_t
{
	tb_uint16_t id; 			// identification number

	tb_uint16_t qr     :1;		// query/response flag
	tb_uint16_t opcode :4;	    // purpose of message
	tb_uint16_t aa     :1;		// authoritive answer
	tb_uint16_t tc     :1;		// truncated message
	tb_uint16_t rd     :1;		// recursion desired

	tb_uint16_t ra     :1;		// recursion available
	tb_uint16_t z      :1;		// its z! reserved
	tb_uint16_t ad     :1;	    // authenticated data
	tb_uint16_t cd     :1;	    // checking disabled
	tb_uint16_t rcode  :4;	    // response code

	tb_uint16_t question;	    // number of question entries
	tb_uint16_t answer;			// number of answer entries
	tb_uint16_t authority;		// number of authority entries
	tb_uint16_t resource;		// number of resource entries

}tb_dns_header_t;

// the dns question type
typedef struct __tb_dns_question_t
{
	tb_uint16_t 		type;
	tb_uint16_t 		class;

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
	tb_char_t 			name[TB_DNS_NAME_MAX];
	tb_dns_resource_t 	res;
	tb_byte_t const* 	rdata;

}tb_dns_answer_t;
/* ////////////////////////////////////////////////////////////////////////
 * globals 
 */

static tb_int_t 	g_dns_init = 0;
static tb_char_t 	g_dns_servers[TB_DNS_SERVER_MAX][16] = {0};

/* ////////////////////////////////////////////////////////////////////////
 * details
 */

// size + data, e.g. .www.google.com => 3www6google3com
static tb_char_t const* tb_dns_encode_name(tb_char_t* name)
{
	TB_ASSERT(name && name[0] == '.');
	if (!name || name[0] != '.') return TB_NULL;
	
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
	return name;
}
static tb_char_t const* tb_dns_parse_name_impl(tb_char_t const* sb, tb_char_t const* se, tb_char_t const* ps, tb_char_t** pd)
{
	tb_char_t* p = ps;
	tb_char_t* q = *pd;
	while (p < se)
	{
		tb_byte_t c = *p++;
		//TB_DNS_DBG("%x", c);
		if (!c) break;
		// is pointer? 11xxxxxx xxxxxxxx
		else if (c >= 0xc0)
		{
			tb_uint16_t pos = c;
			pos &= ~0xc0;
			pos <<= 8;
			pos |= *p++;
			//TB_DNS_DBG("skip to: %d", pos);
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
		if (q > name && *(q - 1) == '.') *--q = '\0';
		tb_bstream_goto(bst, p);
		return name;
	}
	else return TB_NULL;
}
#if 0 
static tb_int_t tb_dns_send(tb_handle_t hsocket, tb_char_t const* server, tb_byte_t* data, tb_size_t size)
{
	tb_int_t send = 0;
	tb_int_t time = (tb_int_t)tb_mclock();
	while(send < size)
	{
		tb_int_t ret = tb_socket_sendto(hsocket, server, TB_DNS_SERVER_PORT, data + send, size - send);
		//TB_DBG("ret: %d", ret);
		if (ret < 0) break;
		else if (!ret) 
		{
			// > 10s?
			tb_int_t timeout = ((tb_int_t)tb_mclock()) - time;
			if (timeout > 10000 || timeout < 0) break;
		}
		else
		{
			send += ret;
			time = (tb_int_t)tb_mclock();
		}
	}
	return send;
}
static tb_int_t tb_dns_recv(tb_handle_t hsocket, tb_char_t const* server, tb_byte_t* data, tb_size_t size)
{
	tb_int_t recv = 0;
	tb_int_t time = (tb_int_t)tb_mclock();
	while(recv < size)
	{
		tb_int_t ret = tb_socket_recvfrom(hsocket, server, TB_DNS_SERVER_PORT, data + recv, size - recv);
		//TB_DBG("ret: %d", ret);
		if (ret < 0) break;
		else if (!ret) 
		{
			// > 10s?
			tb_size_t timeout = ((tb_size_t)tb_mclock()) - time;
			if (timeout > 10000 || timeout < 0) break;
		}
		else
		{
			recv += ret;
			time = (tb_size_t)tb_mclock();
		}
	}
	return recv;
}
#endif
/* ////////////////////////////////////////////////////////////////////////
 * interfaces
 */

void tb_dns_server_add(tb_char_t const* ip)
{
	// ensure init
	if (!g_dns_init)
	{
		tb_int_t i = 0;
		for (i = 0; i < TB_DNS_SERVER_MAX; i++) g_dns_servers[i][0] = '\0';
		g_dns_init = 1;
	}

	// add it
	tb_int_t i = 0;
	for (i = 0; i < TB_DNS_SERVER_MAX; i++)
	{
		if (!g_dns_servers[i][0]) 
		{
			tb_char_t* s = tb_cstring_ncopy(g_dns_servers[i], ip, 16);
			if (s) s[15] = '\0';
			break;
		}
	}
}
void tb_dns_server_del(tb_char_t const* ip)
{
	TB_ASSERT(g_dns_init);
	if (!g_dns_init) return ;

	tb_int_t i = 0;
	for (i = 0; i < TB_DNS_SERVER_MAX; i++)
	{
		if (g_dns_servers[i][0] && !tb_cstring_compare(g_dns_servers[i], ip)) 
			g_dns_servers[i][0] = '\0';
	}
}
void tb_dns_server_dump()
{
	TB_ASSERT(g_dns_init);
	if (!g_dns_init) return ;

	tb_int_t i = 0;
	for (i = 0; i < TB_DNS_SERVER_MAX; i++)
	{
		if (g_dns_servers[i][0]) tb_printf("dns server: %s\n", g_dns_servers[i]);
	}
}

tb_char_t const* tb_dns_lookup_server(tb_char_t const* server, tb_char_t const* host, tb_char_t* ip)
{
	TB_DNS_DBG("lookup host: %s from %s", host, server);

	// connect dns server
#ifdef TB_DNS_SERVER_TCP
	tb_handle_t hserver = tb_socket_client_open(server, TB_DNS_SERVER_PORT, TB_SOCKET_TYPE_TCP, TB_TRUE);
#else
	tb_handle_t hserver = tb_socket_client_open(TB_NULL, 0, TB_SOCKET_TYPE_UDP, TB_TRUE);
#endif
	if (!hserver) goto fail;
	TB_DNS_DBG("connect ok.");
		
	// bstream
	tb_byte_t data[8192];
	tb_bstream_t bst;

	// set dns header, see as tb_dns_header_t
	tb_bstream_attach(&bst, data, 8192);
	tb_bstream_set_u16_be(&bst, TB_DNS_HEADER_ID); 	// identification number
	tb_bstream_set_u1(&bst, 0); 			// this is a query
	tb_bstream_set_ubits(&bst, 0, 4); 		// this is a standard query
	tb_bstream_set_u1(&bst, 0); 			// not authoritive answer
	tb_bstream_set_u1(&bst, 0); 			// not truncated
	tb_bstream_set_u1(&bst, 1); 			// recursion desired

	tb_bstream_set_u1(&bst, 0); 			// recursion not available! hey we dont have it (lol)
	tb_bstream_set_u1(&bst, 0);
	tb_bstream_set_u1(&bst, 0);
	tb_bstream_set_u1(&bst, 0);
	tb_bstream_set_ubits(&bst, 0, 4);

	tb_bstream_set_u16_be(&bst, 1); 		// we have only one question
	tb_bstream_set_u16_be(&bst, 0);
	tb_bstream_set_u16_be(&bst, 0);
	tb_bstream_set_u16_be(&bst, 0);

	// set questions, see as tb_dns_question_t
	// name + question1 + question2 + ...
	tb_bstream_set_u8(&bst, '.');
	tb_char_t* name = tb_bstream_set_string(&bst, host);

	// only one question now.
	tb_bstream_set_u16_be(&bst, 1); 		// we are requesting the ipv4 address
	tb_bstream_set_u16_be(&bst, 1); 		// it's internet (lol)

	// encode dns name
	if (!name || !tb_dns_encode_name(name - 1)) goto fail;

	// send query
	tb_int_t size = tb_bstream_offset(&bst);
#if 0
	{
		tb_int_t i = 0;
		for (i = 0; i < size; i++)
		{
			TB_DBG("%02x", data[i]);
		}
	}
#endif

#ifdef TB_DNS_SERVER_TCP
	if (size != tb_socket_send(hserver, data, size)) goto fail;
#else
	if (size != tb_socket_sendto(hserver, server, TB_DNS_SERVER_PORT, data, size)) goto fail;
	//if (size != tb_dns_send(hserver, server, data, size)) goto fail;
#endif
	TB_DNS_DBG("send query ok.");

	// recv dns header
#ifdef TB_DNS_SERVER_TCP
	size = tb_socket_recv(hserver, data, 8192);
#else
	size = tb_socket_recvfrom(hserver, server, TB_DNS_SERVER_PORT, data, 8192);
	//size = tb_dns_recv(hserver, server, data, 8192);
#endif
	if (size <= 0) goto fail;
	TB_DNS_DBG("recv response ok.");

	// parse dns header
	tb_dns_header_t header;
	tb_bstream_attach(&bst, data, size);
	header.id = tb_bstream_get_u16_be(&bst);
	tb_bstream_skip(&bst, 2);
	header.question 	= tb_bstream_get_u16_be(&bst);
	header.answer 		= tb_bstream_get_u16_be(&bst);
	header.authority 	= tb_bstream_get_u16_be(&bst);
	header.resource 	= tb_bstream_get_u16_be(&bst);
	TB_DNS_DBG("response: %d", size);
	TB_DNS_DBG("id: 0x%04x", 	header.id);
	TB_DNS_DBG("question: %d", 	header.question);
	TB_DNS_DBG("answer: %d", 	header.answer);
	TB_DNS_DBG("authority: %d", header.authority);
	TB_DNS_DBG("resource: %d", 	header.resource);
	TB_DNS_DBG("");
	if (header.id != TB_DNS_HEADER_ID) goto fail;

	// skip questions, only one question now.
	// name + question1 + question2 + ...
	TB_ASSERT(header.question == 1);
#if 1
	tb_bstream_skip_string(&bst);
	tb_bstream_skip(&bst, 4);
#else
	name = tb_bstream_get_string(&bst);
	name = tb_dns_decode_name(name);
	if (!name) goto fail;
	tb_bstream_skip(&bst, 4);
	TB_DNS_DBG("recv dns name: %s", name);
#endif

	// parse answers
	tb_int_t i = 0;
	tb_int_t found = 0;
	for (i = 0; i < header.answer; i++)
	{
		// parse answer
		tb_dns_answer_t answer;
		TB_DNS_DBG("answer: %d", i);

		// parse dns name
		name = tb_dns_parse_name(&bst, answer.name);
		TB_DNS_DBG("name: %s", name? name : "");

		// parse resource
		answer.res.type = tb_bstream_get_u16_be(&bst);
		answer.res.class = tb_bstream_get_u16_be(&bst);
		answer.res.ttl = tb_bstream_get_u32_be(&bst);
		answer.res.size = tb_bstream_get_u16_be(&bst);
		TB_DNS_DBG("type: %d", answer.res.type);
		TB_DNS_DBG("class: %d", answer.res.class);
		TB_DNS_DBG("ttl: %d", answer.res.ttl);
		TB_DNS_DBG("size: %d", answer.res.size);

		// is ipv4?
		if (answer.res.type == 1)
		{
			tb_byte_t b1 = tb_bstream_get_u8(&bst);
			tb_byte_t b2 = tb_bstream_get_u8(&bst);
			tb_byte_t b3 = tb_bstream_get_u8(&bst);
			tb_byte_t b4 = tb_bstream_get_u8(&bst);
			TB_DNS_DBG("ipv4: %d.%d.%d.%d", b1, b2, b3, b4);

			// save the first ip
			if (!found) 
			{
				snprintf(ip, 16, "%d.%d.%d.%d", b1, b2, b3, b4);
				found = 1;
				TB_DNS_DBG("");
				break;
			}
		}
		else
		{
			// parse rdata
			answer.rdata = tb_dns_parse_name(&bst, answer.name);
			TB_DNS_DBG("alias: %s", answer.rdata? answer.rdata : "");
		}
		TB_DNS_DBG("");
	}

	// is found?
	if (found) goto ok;
	else goto fail;

#if 0
	// parse authorities
	for (i = 0; i < header.authority; i++)
	{
		// parse answer
		tb_dns_answer_t answer;
		TB_DNS_DBG("authority: %d", i);

		// parse dns name
		name = tb_dns_parse_name(&bst, answer.name);
		TB_DNS_DBG("name: %s", name? name : "");

		// parse resource
		answer.res.type = tb_bstream_get_u16_be(&bst);
		answer.res.class = tb_bstream_get_u16_be(&bst);
		answer.res.ttl = tb_bstream_get_u32_be(&bst);
		answer.res.size = tb_bstream_get_u16_be(&bst);
		TB_DNS_DBG("type: %d", answer.res.type);
		TB_DNS_DBG("class: %d", answer.res.class);
		TB_DNS_DBG("ttl: %d", answer.res.ttl);
		TB_DNS_DBG("size: %d", answer.res.size);

		// is ipv4?
		if (answer.res.type == 1)
		{
			tb_byte_t b1 = tb_bstream_get_u8(&bst);
			tb_byte_t b2 = tb_bstream_get_u8(&bst);
			tb_byte_t b3 = tb_bstream_get_u8(&bst);
			tb_byte_t b4 = tb_bstream_get_u8(&bst);
			TB_DNS_DBG("ipv4: %d.%d.%d.%d", b1, b2, b3, b4);
		}
		else
		{
			// parse data
			answer.rdata = tb_dns_parse_name(&bst, answer.name);
			TB_DNS_DBG("server: %s", answer.rdata? answer.rdata : "");
		}
		TB_DNS_DBG("");
	}

	for (i = 0; i < header.resource; i++)
	{
		// parse answer
		tb_dns_answer_t answer;
		TB_DNS_DBG("resource: %d", i);

		// parse dns name
		name = tb_dns_parse_name(&bst, answer.name);
		TB_DNS_DBG("name: %s", name? name : "");

		// parse resource
		answer.res.type = tb_bstream_get_u16_be(&bst);
		answer.res.class = tb_bstream_get_u16_be(&bst);
		answer.res.ttl = tb_bstream_get_u32_be(&bst);
		answer.res.size = tb_bstream_get_u16_be(&bst);
		TB_DNS_DBG("type: %d", answer.res.type);
		TB_DNS_DBG("class: %d", answer.res.class);
		TB_DNS_DBG("ttl: %d", answer.res.ttl);
		TB_DNS_DBG("size: %d", answer.res.size);

		// is ipv4?
		if (answer.res.type == 1)
		{
			tb_byte_t b1 = tb_bstream_get_u8(&bst);
			tb_byte_t b2 = tb_bstream_get_u8(&bst);
			tb_byte_t b3 = tb_bstream_get_u8(&bst);
			tb_byte_t b4 = tb_bstream_get_u8(&bst);
			TB_DNS_DBG("ipv4: %d.%d.%d.%d", b1, b2, b3, b4);
		}
		else
		{
			// parse data
			answer.rdata = tb_dns_parse_name(&bst, answer.name);
			TB_DNS_DBG("alias: %s", answer.rdata? answer.rdata : "");
		}
		TB_DNS_DBG("");
	}
#endif

	if (!found) goto fail;

ok:
	// close server
	tb_socket_close(hserver);
	return ip;
fail:
	TB_DNS_DBG("lookup failed.");
	if (hserver) tb_socket_close(hserver);
	return TB_NULL;
}
tb_char_t const* tb_dns_lookup(tb_char_t const* host, tb_char_t* ip)
{
	TB_ASSERT(g_dns_init);
	if (!g_dns_init) goto fail;

	// check
	TB_ASSERT(host && ip);
	if (!host || !ip || !*host) goto fail;

	// lookup servers
	tb_int_t i = 0;
	for (i = 0; i < TB_DNS_SERVER_MAX; i++)
	{
		if (g_dns_servers[i][0]) 
		{
			if (tb_dns_lookup_server(g_dns_servers[i], host, ip)) break;
		}
	}

	// no found?
	if (i == TB_DNS_SERVER_MAX) goto fail;

	// ok
	ip[15] = '\0';
	return ip;

fail:
	return TB_NULL;
}
