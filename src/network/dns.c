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
 * \file		dns.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "dns.h"
#include "ipv4.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../string/string.h"
#include "../stream/stream.h"
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

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the dns item type
typedef struct __tb_dns_item_t
{
	// the host
	tb_ipv4_t 		host;

	// the rate
	tb_size_t 		rate;

}tb_dns_item_t;

// the dns list type
typedef struct __tb_dns_list_t
{
	// the list
	tb_vector_t* 	list;

	// the mutx
	tb_handle_t 	mutx;

}tb_dns_list_t;

// the dns step type
typedef enum __tb_dns_step_t
{
	TB_DNS_STEP_NONE 	= 0
,	TB_DNS_STEP_REQT 	= 1
,	TB_DNS_STEP_RESP 	= 2

}tb_dns_step_t;

// the dns name type
typedef struct __tb_dns_name_t
{
	// the host
	tb_sstring_t 	host;

	// the name
	tb_sstring_t 	name;

	// the request & response packet
	tb_sstring_t 	rpkt;
	
	// the size for recv & send packet
	tb_size_t 		size;

	// the iterator
	tb_size_t 		itor;

	// the step
	tb_size_t 		step;

	// the socket
	tb_handle_t 	sock;

	// the data
	tb_byte_t 		data[TB_DNS_HOST_MAXN + TB_DNS_NAME_MAXN + TB_DNS_RPKT_MAXN];

}tb_dns_name_t;

#if 0
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
#endif

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
static tb_handle_t tb_dns_host_post(tb_char_t const* host, tb_char_t const* name)
{
	tb_assert_and_check_return_val(name, TB_NULL);

	// open socket
	tb_handle_t handle = tb_socket_open(TB_SOCKET_TYPE_UDP);
	tb_assert_and_check_return_val(handle, TB_NULL);

	// format query
	tb_bstream_t 	bst;
	tb_byte_t 		rpkt[TB_DNS_RPKT_MAXN];
	tb_size_t 		size = 0;
	tb_char_t* 		p = TB_NULL;
	tb_bstream_attach(&bst, rpkt, TB_DNS_RPKT_MAXN);

	// identification number
	tb_bstream_set_u16_be(&bst, TB_DNS_HEADER_MAGIC);

	/* 0x2104: 0 0100 001 0000 0100
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
	tb_bstream_set_u16_be(&bst, 0x2104);

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
	p = tb_bstream_set_string(&bst, name);

	// only one question now.
	tb_bstream_set_u16_be(&bst, 1); 		// we are requesting the ipv4 address
	tb_bstream_set_u16_be(&bst, 1); 		// it's internet (lol)

	// encode dns name
	if (!p || !tb_dns_encode_name(p - 1)) goto fail;

	// size
	size = tb_bstream_offset(&bst);
	tb_assert_and_check_goto(size, fail);

	// send query
	tb_long_t r = tb_socket_usend(handle, host, TB_DNS_HOST_PORT, rpkt, size);
	tb_print("%d => %d", size, r);

	// ok
	return handle;

fail:
	if (handle) tb_socket_exit(handle);
	return TB_NULL;
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
		g_dns_list->list = tb_vector_init(8, tb_item_func_ifm(sizeof(tb_dns_item_t), TB_NULL, TB_NULL));
		tb_assert_and_check_goto(g_dns_list->list, fail);
	}

	// init local
	tb_dns_local_init();

	// add the hosts
	tb_dns_list_adds("8.8.8.8");
	tb_dns_list_adds("8.8.4.4");

	// ok
	return TB_TRUE;

fail:
	tb_dns_list_exit();
	return TB_FALSE;
}
tb_void_t tb_dns_list_adds(tb_char_t const* host)
{
	tb_assert_and_check_return(g_dns_list && host);

	// init item
	tb_dns_item_t item;
	if (tb_ipv4_set(&item.host, host))
	{
		item.rate = 0;

		// enter
		if (g_dns_list->mutx) tb_mutex_enter(g_dns_list->mutx);

		// add
		tb_vector_insert_tail(g_dns_list->list, &item);

		// leave
		if (g_dns_list->mutx) tb_mutex_leave(g_dns_list->mutx);

		// ok
		return ;
	}

	// trace
	tb_trace("[dns]: host: add %s failed", host);
}
tb_void_t tb_dns_list_dels(tb_char_t const* host)
{
	tb_assert_and_check_return(g_dns_list && host);

	// enter
	if (g_dns_list->mutx) tb_mutex_enter(g_dns_list->mutx);

	// list
	tb_vector_t* list = g_dns_list->list;
	tb_assert_and_check_goto(list, end);

	// ipv4
	tb_uint32_t ipv4 = tb_ipv4_set(TB_NULL, host);

	// find it
	tb_size_t itor = tb_vector_itor_head(list);
	tb_size_t tail = tb_vector_itor_tail(list);
	for (; itor != tail; itor = tb_vector_itor_next(list, itor))
	{
		tb_dns_item_t const* item = tb_vector_itor_const_at(list, itor);
		if (item && item->host.u32 == ipv4) break;
	}

	// remove it
	if (itor != tail) tb_vector_remove(list, itor);

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

			// free list
			if (g_dns_list->list) tb_vector_exit(g_dns_list->list);
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
	tb_vector_t* list = g_dns_list->list;
	tb_assert_and_check_return(list);
	
	// find it
	tb_print("============================================================");
	tb_print("[dns]: list: %u items", tb_vector_size(list));
	tb_size_t itor = tb_vector_itor_head(list);
	tb_size_t tail = tb_vector_itor_tail(list);
	for (; itor != tail; itor = tb_vector_itor_next(list, itor))
	{
		tb_dns_item_t const* item = tb_vector_itor_const_at(list, itor);
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
tb_handle_t tb_dns_name_init(tb_char_t const* name)
{
	tb_assert_and_check_return_val(name, TB_NULL);

	// alloc
	tb_dns_name_t* h = tb_calloc(1, sizeof(tb_dns_name_t));
	tb_assert_and_check_return_val(h, TB_NULL);

	// init host
	if (!tb_sstring_init(&h->host, h->data, TB_DNS_HOST_MAXN)) goto fail;

	// init name
	if (!tb_sstring_init(&h->name, h->data + TB_DNS_HOST_MAXN, TB_DNS_NAME_MAXN)) goto fail;
	tb_sstring_cstrcpy(&h->name, name);

	// init rpkt
	if (!tb_sstring_init(&h->host, h->data + TB_DNS_HOST_MAXN + TB_DNS_NAME_MAXN, TB_DNS_RPKT_MAXN)) goto fail;

	// init sock
	h->sock = tb_socket_open(TB_SOCKET_TYPE_UDP);
	tb_assert_and_check_goto(h->sock, fail);

	// ok
	return (tb_handle_t)h;

fail:
	if (h) tb_dns_name_exit(h);
	return TB_NULL;
}
tb_long_t tb_dns_name_spak(tb_handle_t handle, tb_char_t* data, tb_size_t maxn)
{
	tb_trace_noimpl();
	return 0;
}
tb_long_t tb_dns_name_wait(tb_handle_t handle, tb_size_t timeout)
{
	tb_trace_noimpl();
	return 0;
}
tb_void_t tb_dns_name_exit(tb_handle_t handle)
{
	tb_dns_name_t* h = (tb_dns_name_t*)handle;
	if (h)
	{
		// close sock
		if (h->sock) tb_socket_close(h->sock);

		// free it
		tb_free(h);
	}
}
tb_char_t const* tb_dns_name_done(tb_char_t const* name, tb_char_t* data, tb_size_t maxn)
{
	tb_assert_and_check_return_val(name && data && maxn > 15, TB_NULL);

	// init
	tb_handle_t handle = tb_dns_name_init(name);
	tb_assert_and_check_return_val(handle, TB_NULL);

	// spak
	tb_long_t r = -1;
	while (!(r = tb_dns_name_spak(handle, data, maxn)))
	{
		// wait
		r = tb_dns_name_wait(handle, 1000);
		tb_assert_and_check_goto(r >= 0, end);
	}

end:

	// exit
	tb_dns_name_exit(handle);

	// ok
	return r > 0? data : TB_NULL;
}

