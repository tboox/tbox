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
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * @author		ruki
 * @file		dns.c
 * @ingroup 	network
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
//#define TB_TRACE_IMPL_TAG 		"dns"

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

// the list maximum size 
#define TB_DNS_LIST_MAXN 			(2)

// the protocol port
#define TB_DNS_HOST_PORT 			(53)

// the host maximum size 
#define TB_DNS_HOST_MAXN 			(16)

// the name maximum size 
#define TB_DNS_NAME_MAXN 			(256)

// the cache maximum size
#ifdef TB_CONFIG_MEMORY_MODE_SMALL
# 	define TB_DNS_CACHE_MAXN 		(64)
#else
# 	define TB_DNS_CACHE_MAXN 		(256)
#endif

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
	// the ipv4
	tb_ipv4_t 		ipv4;

	// the time
	tb_size_t 		time;

}tb_dns_addr_t;

// the dns list type
typedef struct __tb_dns_list_t
{
	// the list
	tb_dns_host_t 	list[TB_DNS_LIST_MAXN];
	tb_size_t 		size;

	// the mutx
	tb_handle_t 	mutx;

	// the spool
	tb_handle_t		spool;

	// the cache
	tb_hash_t* 		cache;

	// the times
	tb_hize_t 		times;

	// the expired
	tb_size_t 		expired;

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

	// the tryn
	tb_size_t 		tryn;

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
static tb_dns_list_t* 	g_dns_list = tb_null;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */

// size + data, e.g. .www.google.com => 3www6google3com
static tb_char_t const* tb_dns_encode_name(tb_char_t* name)
{
	tb_assert_and_check_return_val(name && name[0] == '.', tb_null);
	
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
	else return tb_null;
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
	tb_bstream_set_u8(&bst, 3);
	tb_bstream_set_u8(&bst, 'w');
	tb_bstream_set_u8(&bst, 'w');
	tb_bstream_set_u8(&bst, 'w');
	tb_bstream_set_u8(&bst, 5);
	tb_bstream_set_u8(&bst, 't');
	tb_bstream_set_u8(&bst, 'b');
	tb_bstream_set_u8(&bst, 'o');
	tb_bstream_set_u8(&bst, 'o');
	tb_bstream_set_u8(&bst, 'x');
	tb_bstream_set_u8(&bst, 3);
	tb_bstream_set_u8(&bst, 'c');
	tb_bstream_set_u8(&bst, 'o');
	tb_bstream_set_u8(&bst, 'm');
	tb_bstream_set_u8(&bst, '\0');

	// only one question now.
	tb_bstream_set_u16_be(&bst, 1); 		// we are requesting the ipv4 address
	tb_bstream_set_u16_be(&bst, 1); 		// it's internet (lol)

	// size
	size = tb_bstream_offset(&bst);
	tb_assert_and_check_goto(size, end);

	// init time
	tb_hong_t time = tb_mclock();

	// se/nd request
	tb_long_t writ = 0;
	while (writ < size)
	{
		// writ data
		tb_long_t r = tb_socket_usend(handle, host, TB_DNS_HOST_PORT, rpkt + writ, size - writ);
		//tb_trace_impl("writ %d", r);
		tb_assert_and_check_goto(r >= 0, end);
		
		// no data?
		if (!r)
		{
			// abort?
			tb_check_goto(!writ, end);

			// wait
			tb_aioo_t o;
			tb_aioo_seto(&o, handle, TB_AIOO_OTYPE_SOCK, TB_AIOO_ETYPE_WRIT, tb_null);
			r = tb_aioo_wait(&o, TB_DNS_TIMEOUT);

			// fail or timeout?
			tb_check_goto(r > 0, end);
		}
		else writ += r;
	}

	// only recv id & answer, 8 bytes 
	tb_long_t read = 0;
	while (read < 8)
	{
		// read data
		tb_long_t r = tb_socket_urecv(handle, host, TB_DNS_HOST_PORT, rpkt + read, TB_DNS_RPKT_MAXN - read);
		//tb_trace_impl("read %d", r);
		tb_check_break(r >= 0);
		
		// no data?
		if (!r)
		{
			// end?
			tb_check_break(!read);

			// wait
			tb_aioo_t o;
			tb_aioo_seto(&o, handle, TB_AIOO_OTYPE_SOCK, TB_AIOO_ETYPE_READ, tb_null);
			r = tb_aioo_wait(&o, TB_DNS_TIMEOUT);
			//tb_trace_impl("wait %d", r);

			// fail or timeout?
			tb_check_break(r > 0);
		}
		else read += r;
	}

	// check
	tb_check_goto(read >= 8, end);

	// check protocol
	tb_size_t id = tb_bits_get_u16_be(rpkt);
	tb_check_goto(id == TB_DNS_HEADER_MAGIC, end);

	// check answer
	tb_size_t answer = tb_bits_get_u16_be(rpkt + 6);
	tb_check_goto(answer > 0, end);

	// rate
	rate = (tb_long_t)(tb_mclock() - time);

	// ok
	tb_trace("[dns]: host: %s ok, rate: %u", host, rate);

end:
	// exit sock
	tb_socket_close(handle);

	// ok
	return rate;
}
static tb_bool_t tb_dns_look_clr4(tb_hash_t* cache, tb_hash_item_t* item, tb_bool_t* bdel, tb_pointer_t data)
{
	tb_assert_and_check_return_val(cache && bdel && data, tb_false);

	// the expired time
	tb_dns_list_t* list = (tb_dns_list_t*)data;
	if (item)
	{
		// address
		tb_dns_addr_t const* addr = item->data;
		tb_assert_and_check_return_val(addr, tb_false);

		// is expired?
		if (addr->time < list->expired)
		{
			// remove it
			*bdel = tb_true;

			// trace
			tb_trace_impl("cache: clr %s => %u.%u.%u.%u, time: %u, size: %u"
				, (tb_char_t const*)item->name
				, addr->ipv4.u8[0]
				, addr->ipv4.u8[1]
				, addr->ipv4.u8[2]
				, addr->ipv4.u8[3]
				, addr->time
				, tb_hash_size(cache));

			// update times
			tb_assert(list->times >= addr->time);
			list->times -= addr->time;
		}
	}

	// ok
	return tb_true;
}
static tb_bool_t tb_dns_look_stat(tb_hash_t* cache, tb_hash_item_t* item, tb_bool_t* bdel, tb_pointer_t data)
{
	tb_assert_and_check_return_val(cache && bdel && data, tb_false);

	tb_size_t* stat = (tb_size_t*)data;
	if (item && item->data)
	{
		// min & max
		tb_size_t time = ((tb_dns_addr_t const*)item->data)->time;
		if (!stat[0] || time < stat[0]) stat[0] = time;
		if (!stat[1] || time > stat[1]) stat[1] = time;
	}

	// ok
	return tb_true;
}
static tb_void_t tb_dns_look_add4(tb_char_t const* name, tb_ipv4_t const* ipv4)
{
	tb_assert_and_check_return(name && ipv4 && ipv4->u32);

	// trace
	tb_trace_impl("add4: %s => %u.%u.%u.%u", name, ipv4->u8[0], ipv4->u8[1], ipv4->u8[2], ipv4->u8[3]);

	// init
	tb_dns_addr_t addr;
	addr.ipv4 = *ipv4;
	addr.time = (tb_size_t)(tb_mclock() / 1000);

	// has list?
	if (g_dns_list && g_dns_list->mutx) 
	{
		// enter
		tb_mutex_enter(g_dns_list->mutx);

		// has list?
		if (g_dns_list)
		{
			// cache
			tb_hash_t* cache = g_dns_list->cache;
			if (cache)
			{
				// remove the expired items if full
				if (tb_hash_size(cache) >= TB_DNS_CACHE_MAXN) 
				{
					// which is better? lol
					// ...
#if 1
					// the expired time
					g_dns_list->expired = ((tb_size_t)(g_dns_list->times / tb_hash_size(cache)) + 1);
#else

					// stat
					tb_size_t stat[2] = {0};
					tb_hash_walk(cache, tb_dns_look_stat, stat);

					// the expired time
					g_dns_list->expired = tb_max(stat[0] + 1, ((stat[0] + stat[1]) >> 2));
#endif

					tb_assert(g_dns_list->expired);
					tb_trace_impl("cache: expired: %u", g_dns_list->expired);

					// remove the expired times
					tb_hash_walk(cache, tb_dns_look_clr4, g_dns_list);
				}

				// check
				tb_assert(tb_hash_size(cache) < TB_DNS_CACHE_MAXN);

				// add it
				if (tb_hash_size(cache) < TB_DNS_CACHE_MAXN) 
				{
					// set
					tb_hash_set(cache, name, &addr);

					// update times
					g_dns_list->times += addr.time;

					// trace
					tb_trace_impl("cache: add %s => %u.%u.%u.%u, time: %u, size: %u"
						, name
						, addr.ipv4.u8[0]
						, addr.ipv4.u8[1]
						, addr.ipv4.u8[2]
						, addr.ipv4.u8[3]
						, addr.time
						, tb_hash_size(cache));
				}
			}

			// leave
			if (g_dns_list->mutx) tb_mutex_leave(g_dns_list->mutx);
		}
	}
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
			if (look->itor)
			{
				tb_char_t 				ipv4[16];
				tb_dns_host_t const* 	item = &g_dns_list->list[look->itor - 1];
				tb_char_t const* 		host = tb_ipv4_get(&item->host, ipv4, 16);
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
	tb_trace_impl("request: try %s", host);
	while (look->size < size)
	{
		// writ data
		tb_long_t writ = tb_socket_usend(look->sock, host, TB_DNS_HOST_PORT, data + look->size, size - look->size);
		//tb_trace("writ: %d", writ);
		tb_assert_and_check_return_val(writ >= 0, -1);

		// no data? 
		if (!writ)
		{
			// abort?
			tb_check_return_val(!look->size && !look->tryn, -1);

			// tryn++
			look->tryn++;

			// continue
			return 0;
		}
		else look->tryn = 0;

		// update size
		look->size += writ;
	}

	// finish it
	look->step |= TB_DNS_STEP_REQT;
	look->tryn = 0;

	// reset rpkt
	look->size = 0;
	tb_sbuffer_clear(&look->rpkt);

	// ok
	tb_trace_impl("request: ok");
	return 1;
}
static tb_bool_t tb_dns_look_resp_done(tb_dns_look_t* look, tb_ipv4_t* ipv4)
{
	// rpkt && size
	tb_byte_t const* 	rpkt = tb_sbuffer_data(&look->rpkt);
	tb_size_t 			size = tb_sbuffer_size(&look->rpkt);

	// check
	tb_assert_and_check_return_val(rpkt && size >= TB_DNS_HEADER_SIZE, tb_false);

	// parse dns header
	tb_bstream_t 	bst;
	tb_dns_header_t header;
	tb_bstream_init(&bst, rpkt, size);
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

	// parse answers
	tb_size_t i = 0;
	tb_size_t found = 0;
	for (i = 0; i < header.answer; i++)
	{
		// parse answer
		tb_dns_answer_t answer;
		tb_trace_impl("response: answer: %d", i);

		// parse dns name
		tb_char_t const* name = tb_dns_parse_name(&bst, answer.name); tb_used(name);
		tb_trace_impl("response: name: %s", name);

		// parse resource
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
			// parse rdata
			answer.rdata = tb_dns_parse_name(&bst, answer.name);
			tb_trace_impl("response: alias: %s", answer.rdata? answer.rdata : "");
		}
		tb_trace_impl("response: ");
	}

	// found it?
	tb_check_return_val(found, tb_false);

#if 0
	// parse authorities
	for (i = 0; i < header.authority; i++)
	{
		// parse answer
		tb_dns_answer_t answer;
		tb_trace_impl("response: authority: %d", i);

		// parse dns name
		tb_char_t* name = tb_dns_parse_name(&bst, answer.name);
		tb_trace_impl("response: name: %s", name? name : "");

		// parse resource
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
			// parse data
			answer.rdata = tb_dns_parse_name(&bst, answer.name);
			tb_trace_impl("response: server: %s", answer.rdata? answer.rdata : "");
		}
		tb_trace_impl("response: ");
	}

	for (i = 0; i < header.resource; i++)
	{
		// parse answer
		tb_dns_answer_t answer;
		tb_trace_impl("response: resource: %d", i);

		// parse dns name
		tb_char_t* name = tb_dns_parse_name(&bst, answer.name);
		tb_trace_impl("response: name: %s", name? name : "");

		// parse resource
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
			// parse data
			answer.rdata = tb_dns_parse_name(&bst, answer.name);
			tb_trace_impl("response: alias: %s", answer.rdata? answer.rdata : "");
		}
		tb_trace_impl("response: ");
	}
#endif

	// ok
	return tb_true;
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
			if (look->itor)
			{
				tb_char_t 				addr[16];
				tb_dns_host_t const* 	item = (tb_dns_host_t const*)&g_dns_list->list[look->itor - 1];
				tb_char_t const* 		host = tb_ipv4_get(&item->host, addr, 16);
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
		//tb_trace_impl("read %d", read);
		tb_assert_and_check_return_val(read >= 0, -1);

		// no data? 
		if (!read)
		{
			// end? read x, read 0
			tb_check_break(!tb_sbuffer_size(&look->rpkt));
	
			// abort? read 0, read 0
			tb_check_return_val(!look->tryn, -1);
			
			// tryn++
			look->tryn++;

			// continue 
			return 0;
		}
		else look->tryn = 0;

		// copy data
		tb_sbuffer_memncat(&look->rpkt, rpkt, read);
	}

	// done
	if (!tb_dns_look_resp_done(look, ipv4)) return -1;

	// check
	tb_assert_and_check_return_val(tb_sstring_size(&look->name) && ipv4->u32, -1);

	// add ipv4 to cache
	tb_dns_look_add4(tb_sstring_cstr(&look->name), ipv4);

	// finish it
	look->step |= TB_DNS_STEP_RESP;
	look->tryn = 0;

	// reset rpkt
	look->size = 0;
	tb_sbuffer_clear(&look->rpkt);

	// ok
	tb_trace_impl("response: ok");
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
		g_dns_list = tb_malloc0(sizeof(tb_dns_list_t));
		tb_assert_and_check_return_val(g_dns_list, tb_false);

		// init mutx
		g_dns_list->mutx = tb_mutex_init(tb_null);
		tb_assert_and_check_goto(g_dns_list->mutx, fail);
			
		// init spool
		g_dns_list->spool = tb_spool_init(TB_SPOOL_GROW_DEFAULT, 0);
		tb_assert_and_check_goto(g_dns_list->spool, fail);
	
		// init cache
		g_dns_list->cache = tb_hash_init(tb_align8(tb_int32_sqrt(TB_DNS_CACHE_MAXN) + 1), tb_item_func_str(tb_false, g_dns_list->spool), tb_item_func_ifm(sizeof(tb_dns_addr_t), tb_null, tb_null));
		tb_assert_and_check_goto(g_dns_list->cache, fail);
	}

	// add the hosts
	tb_dns_list_adds("8.8.4.4");
	tb_dns_list_adds("8.8.8.8");

	// init local
	tb_dns_local_init();

	// ok
	return tb_true;

fail:
	tb_dns_list_exit();
	return tb_false;
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
			if (g_dns_list)
			{
				// list
				tb_dns_host_t* list = g_dns_list->list;

				// find item
				tb_size_t p = 0;
				tb_size_t i = 0;
				tb_size_t n = g_dns_list->size;
				for (; i < n; p = i + 1, i++) if (item.rate < list[i].rate) break;

				// insert item by sort
				if (p < TB_DNS_LIST_MAXN) 
				{
					// move items
					tb_size_t m = tb_min(n, TB_DNS_LIST_MAXN - 1) - p;
					if (m) tb_memmov(list + p + 1, list + p, m * sizeof(tb_dns_host_t));

					// insert it
					list[p] = item;

					// update size
					if (n < TB_DNS_LIST_MAXN) g_dns_list->size++;
				}

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
	tb_assert_and_check_return(host);

	// has list?
	if (g_dns_list && g_dns_list->mutx) 
	{
		// enter
		tb_mutex_enter(g_dns_list->mutx);

		// has list?
		if (g_dns_list)
		{
			// list
			tb_dns_host_t* list = g_dns_list->list;

			// ipv4
			tb_uint32_t ipv4 = tb_ipv4_set(tb_null, host);

			// find it
			tb_size_t i = 0;
			tb_size_t n = g_dns_list->size;
			for (; i < n; i++) if (ipv4 == list[i].host.u32) break;

			// remove it
			if (i < n) 
			{
				// move items
				if (i + 1 < n) tb_memmov(list + i, list + i + 1, (n - i - 1) * sizeof(tb_dns_host_t));

				// update size
				tb_assert(g_dns_list->size);
				g_dns_list->size--;
			}

			// leave
			if (g_dns_list->mutx) tb_mutex_leave(g_dns_list->mutx);
		}
	}
}
tb_void_t tb_dns_list_exit()
{
	// exit local
	tb_dns_local_exit();

	// exit list
	tb_handle_t mutx = tb_null;
	if (g_dns_list)
	{
		// enter
		if (g_dns_list->mutx) tb_mutex_enter(g_dns_list->mutx);

		// exists?
		if (g_dns_list)
		{
			// save mutx
			mutx = g_dns_list->mutx;
			g_dns_list->mutx = tb_null;

			// exit cache
			if (g_dns_list->cache) tb_hash_exit(g_dns_list->cache);
			g_dns_list->cache = tb_null;

			// exit spool
			if (g_dns_list->spool) tb_spool_exit(g_dns_list->spool);
			g_dns_list->spool = tb_null;

			// free it
			tb_free(g_dns_list);
			g_dns_list = tb_null;

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
	tb_dns_host_t* list = g_dns_list->list;
	
	// find it
	tb_print("============================================================");
	tb_print("[dns]: list: %u items", g_dns_list->size);
	tb_size_t i = 0;
	tb_size_t n = g_dns_list->size;
	for (; i < n; i++)
	{
		tb_dns_host_t const* item = &list[i];
		tb_print("[dns]: host: %u.%u.%u.%u, rate: %u"
			, item->host.u8[0]
			, item->host.u8[1]
			, item->host.u8[2]
			, item->host.u8[3]
			, item->rate);
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
	tb_assert_and_check_return_val(name && ipv4, tb_false);

	// trace
	tb_trace_impl("try4: %s", name);

	// is ipv4?
	tb_check_return_val(!tb_ipv4_set(ipv4, name), tb_true);

	// clear ipv4
	tb_ipv4_clr(ipv4);

	// has list?
	if (g_dns_list && g_dns_list->mutx) 
	{
		// enter
		tb_mutex_enter(g_dns_list->mutx);

		// has list?
		if (g_dns_list)
		{
			// cache
			tb_hash_t* cache = g_dns_list->cache;
			if (cache)
			{
				// exists?
				tb_dns_addr_t* addr = tb_hash_get(cache, name);
				if (addr)
				{
					// trace
					tb_trace_impl("cache: get %s => %u.%u.%u.%u, time: %u => %u, size: %u"
						, name
						, addr->ipv4.u8[0]
						, addr->ipv4.u8[1]
						, addr->ipv4.u8[2]
						, addr->ipv4.u8[3]
						, addr->time
						, (tb_size_t)(tb_mclock() / 1000)
						, tb_hash_size(cache));

					// update time
					tb_assert(g_dns_list->times >= addr->time);
					g_dns_list->times -= addr->time;
					addr->time = (tb_size_t)(tb_mclock() / 1000);
					g_dns_list->times += addr->time;

					// ok
					*ipv4 = addr->ipv4;
				}
			}

			// leave
			if (g_dns_list->mutx) tb_mutex_leave(g_dns_list->mutx);
		}
	}

	// trace
	tb_trace_impl("try4: %s", ipv4->u32? "ok" : "failed");

	// ok?
	return ipv4->u32? tb_true : tb_false;
}
tb_handle_t tb_dns_look_init(tb_char_t const* name)
{
	tb_assert_and_check_return_val(name, tb_null);

	// must be not ipv4
	tb_assert_return_val(!tb_ipv4_set(tb_null, name), tb_null);

	// alloc
	tb_dns_look_t* look = tb_malloc0(sizeof(tb_dns_look_t));
	tb_assert_and_check_return_val(look, tb_null);

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
		if (g_dns_list->size) look->itor = 1;

		// leave
		tb_mutex_leave(g_dns_list->mutx);
	}
	tb_assert_and_check_goto(look->itor, fail);

	// ok
	return (tb_handle_t)look;

fail:
	if (look) tb_dns_look_exit(look);
	return tb_null;
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
		if (look->itor + 1 <= g_dns_list->size) look->itor++;
		else look->itor = 0;

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

	// need wait?
	tb_long_t r = 0;
	if (e)
	{
		// wait
		tb_aioo_seto(&o, look->sock, TB_AIOO_OTYPE_SOCK, e, tb_null);
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
	tb_assert_and_check_return_val(name && ipv4, tb_false);

	// init
	tb_handle_t handle = tb_dns_look_init(name);
	tb_assert_and_check_return_val(handle, tb_false);

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
	return r > 0? tb_true : tb_false;
}

