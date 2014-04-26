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
 * @file		server.c
 * @ingroup 	network
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME 		"dns_server"
#define TB_TRACE_MODULE_DEBUG 		(0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "server.h"
#include "../../utils/utils.h"
#include "../../stream/stream.h"
#include "../../platform/platform.h"
#include "../../container/container.h"
#include "../../algorithm/algorithm.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the dns server test timeout
#define TB_DNS_SERVER_TEST_TIMEOUT 	(500)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the dns server type
typedef struct __tb_dns_server_t
{
	// the rate
	tb_size_t 				rate;

	// the addr
	tb_ipv4_t 				addr;

}tb_dns_server_t;

// the dns server list type
typedef struct __tb_dns_server_list_t
{
	// is sorted?
	tb_bool_t 				sort;

	// the server list
	tb_vector_t* 			list;

}tb_dns_server_list_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the lock
static tb_spinlock_t 		g_lock = TB_SPINLOCK_INIT;

// the server list
static tb_dns_server_list_t g_list = {0};

/* //////////////////////////////////////////////////////////////////////////////////////
 * server
 */
static tb_long_t tb_dns_server_comp(tb_item_func_t* func, tb_cpointer_t litem, tb_cpointer_t ritem)
{
	// check
	tb_assert_return_val(litem && ritem, -1);

	// the rate
	tb_size_t lrate = ((tb_dns_server_t const*)litem)->rate;
	tb_size_t rrate = ((tb_dns_server_t const*)ritem)->rate;

	// comp
	return (lrate > rrate? 1 : (lrate < rrate? -1 : 0));
}
static tb_long_t tb_dns_server_test(tb_ipv4_t const* addr)
{
	// check
	tb_assert_and_check_return_val(addr && addr->u32, -1);

	// init sock
	tb_handle_t sock = tb_socket_open(TB_SOCKET_TYPE_UDP);
	tb_assert_and_check_return_val(sock, -1);

	// init rate
	tb_long_t rate = -1;
 
	// format query
	tb_static_stream_t 	sstream;
	tb_byte_t 			rpkt[TB_DNS_RPKT_MAXN];
	tb_size_t 			size = 0;
	tb_static_stream_init(&sstream, rpkt, TB_DNS_RPKT_MAXN);

	// identification number
	tb_static_stream_writ_u16_be(&sstream, TB_DNS_HEADER_MAGIC);

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
	tb_static_stream_writ_u16_be(&sstream, 0x0100);
#else
	tb_static_stream_writ_u1(&sstream, 0); 			// this is a query
	tb_static_stream_writ_ubits32(&sstream, 0, 4); 	// this is a standard query
	tb_static_stream_writ_u1(&sstream, 0); 			// not authoritive answer
	tb_static_stream_writ_u1(&sstream, 0); 			// not truncated
	tb_static_stream_writ_u1(&sstream, 1); 			// recursion desired

	tb_static_stream_writ_u1(&sstream, 0); 			// recursion not available! hey we dont have it (lol)
	tb_static_stream_writ_u1(&sstream, 0);
	tb_static_stream_writ_u1(&sstream, 0);
	tb_static_stream_writ_u1(&sstream, 0);
	tb_static_stream_writ_ubits32(&sstream, 0, 4);
#endif

	/* we have only one question
	 *
	 * tb_uint16_t question;	    // number of question entries
	 * tb_uint16_t answer;			// number of answer entries
	 * tb_uint16_t authority;		// number of authority entries
	 * tb_uint16_t resource;		// number of resource entries
	 *
	 */
	tb_static_stream_writ_u16_be(&sstream, 1); 
	tb_static_stream_writ_u16_be(&sstream, 0);
	tb_static_stream_writ_u16_be(&sstream, 0);
	tb_static_stream_writ_u16_be(&sstream, 0);

	// set questions, see as tb_dns_question_t
	// name + question1 + question2 + ...
	tb_static_stream_writ_u8(&sstream, 3);
	tb_static_stream_writ_u8(&sstream, 'w');
	tb_static_stream_writ_u8(&sstream, 'w');
	tb_static_stream_writ_u8(&sstream, 'w');
	tb_static_stream_writ_u8(&sstream, 5);
	tb_static_stream_writ_u8(&sstream, 't');
	tb_static_stream_writ_u8(&sstream, 'b');
	tb_static_stream_writ_u8(&sstream, 'o');
	tb_static_stream_writ_u8(&sstream, 'o');
	tb_static_stream_writ_u8(&sstream, 'x');
	tb_static_stream_writ_u8(&sstream, 3);
	tb_static_stream_writ_u8(&sstream, 'c');
	tb_static_stream_writ_u8(&sstream, 'o');
	tb_static_stream_writ_u8(&sstream, 'm');
	tb_static_stream_writ_u8(&sstream, '\0');

	// only one question now.
	tb_static_stream_writ_u16_be(&sstream, 1); 		// we are requesting the ipv4 address
	tb_static_stream_writ_u16_be(&sstream, 1); 		// it's internet (lol)

	// size
	size = tb_static_stream_offset(&sstream);
	tb_assert_and_check_goto(size, end);

	// init time
	tb_hong_t time = tb_ctime_spak();

	// se/nd request
	tb_long_t writ = 0;
	while (writ < size)
	{
		// writ data
		tb_long_t r = tb_socket_usend(sock, addr, TB_DNS_HOST_PORT, rpkt + writ, size - writ);
		tb_trace_d("writ %d", r);
		tb_check_goto(r >= 0, end);
		
		// no data?
		if (!r)
		{
			// abort?
			tb_check_goto(!writ, end);
 
			// wait
			r = tb_aioo_wait(sock, TB_AIOE_CODE_SEND, TB_DNS_SERVER_TEST_TIMEOUT);

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
		tb_long_t r = tb_socket_urecv(sock, addr, TB_DNS_HOST_PORT, rpkt + read, TB_DNS_RPKT_MAXN - read);
		tb_trace_d("read %d", r);
		tb_check_break(r >= 0);
		
		// no data?
		if (!r)
		{
			// end?
			tb_check_break(!read);

			// wait
			r = tb_aioo_wait(sock, TB_AIOE_CODE_RECV, TB_DNS_SERVER_TEST_TIMEOUT);
			tb_trace_d("wait %d", r);

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
	rate = (tb_long_t)(tb_ctime_spak() - time);

	// ok
	tb_trace_d("addr: %u.%u.%u.%u ok, rate: %u", addr->u8[0], addr->u8[1], addr->u8[2], addr->u8[3], rate);

end:
	// exit sock
	tb_socket_clos(sock);

	// ok
	return rate;
}
static tb_bool_t tb_dns_server_rate(tb_vector_t* vector, tb_pointer_t item, tb_bool_t* bdel, tb_pointer_t priv)
{
	// check
	tb_assert_and_check_return_val(vector && bdel, tb_false);

	// the server
	tb_dns_server_t* server = (tb_dns_server_t*)item;
	if (server && !server->rate)
	{
		// done
		tb_bool_t ok = tb_false;
		do
		{
			// the server rate
			tb_long_t rate = tb_dns_server_test(&server->addr);
			tb_check_break(rate >= 0);

			// save the server rate
			server->rate = rate;

			// ok
			ok = tb_true;

		} while (0);

		// failed? remove it
		if (!ok) *bdel = tb_true;
	}

	// ok
	return tb_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_bool_t tb_dns_server_init()
{
	// enter
	tb_spinlock_enter(&g_lock);

	// done
	tb_bool_t ok = tb_false;
	do
	{
		// init list
		if (!g_list.list) 
		{
			g_list.list = tb_vector_init(8, tb_item_func_mem(sizeof(tb_dns_server_t), tb_null, tb_null));
			g_list.sort = tb_false;
		}
		tb_assert_and_check_break(g_list.list);

		// ok
		ok = tb_true;

	} while (0);

	// leave
	tb_spinlock_leave(&g_lock);

	// failed? exit it
	if (!ok) tb_dns_server_exit();

	// ok?
	return ok;
}
tb_void_t tb_dns_server_exit()
{
	// enter
	tb_spinlock_enter(&g_lock);

	// exit list
	if (g_list.list) tb_vector_exit(g_list.list);
	g_list.list = tb_null;

	// exit sort
	g_list.sort = tb_false;

	// leave
	tb_spinlock_leave(&g_lock);
}
tb_void_t tb_dns_server_dump()
{
	// enter
	tb_spinlock_enter(&g_lock);
	
	// dump list
	if (g_list.list) 
	{
		// trace
		tb_trace_i("============================================================");
		tb_trace_i("list: %u servers", tb_vector_size(g_list.list));

		// walk
		tb_size_t i = 0;
		tb_size_t n = tb_vector_size(g_list.list);
		for (; i < n; i++)
		{
			tb_dns_server_t const* server = (tb_dns_server_t const*)tb_iterator_item(g_list.list, i);
			if (server)
			{
				// trace
				tb_trace_i("server: %u.%u.%u.%u, rate: %u", 	server->addr.u8[0]
															, 	server->addr.u8[1]
															, 	server->addr.u8[2]
															, 	server->addr.u8[3]
															, 	server->rate);
			}
		}
	}

	// leave
	tb_spinlock_leave(&g_lock);
}
tb_void_t tb_dns_server_sort()
{
	// enter
	tb_spinlock_enter(&g_lock);

	// done
	tb_vector_t* list = tb_null;
	do
	{
		// check
		tb_assert_and_check_break(g_list.list);

		// need sort?
		tb_check_break(!g_list.sort);

		// init func
		tb_item_func_t func = tb_item_func_mem(sizeof(tb_dns_server_t), tb_null, tb_null);
		func.comp = tb_dns_server_comp;

		// init list
		list = tb_vector_init(8, func);
		tb_assert_and_check_break(list);
		
		// copy list
		tb_vector_copy(list, g_list.list);

	} while (0);

	/* sort ok, only done once sort
	 * using the unsorted server list at the other thread if the sort have been not finished
	 */
	g_list.sort = tb_true;

	// leave
	tb_spinlock_leave(&g_lock);

	// need sort?
	tb_check_return(list);

	// rate list and remove no-rate servers
	tb_vector_walk(list, tb_dns_server_rate, tb_null);

	// sort list
	tb_sort_all(list, tb_null);
	
	// enter
	tb_spinlock_enter(&g_lock);

	// save the sorted server list
	if (tb_vector_size(list)) tb_vector_copy(g_list.list, list);
	else
	{
		// no faster server? using the previous server list
		tb_trace_w("no faster server");
	}

	// leave
	tb_spinlock_leave(&g_lock);

	// exit list
	tb_vector_exit(list);
}
tb_size_t tb_dns_server_get(tb_ipv4_t addr[2])
{ 
	// check
	tb_assert_and_check_return_val(addr, 0);

	// sort first
	tb_dns_server_sort();
		
	// enter
	tb_spinlock_enter(&g_lock);

	// done
	tb_size_t ok = 0;
	do
	{
		// check
		tb_assert_and_check_break(g_list.list && g_list.sort);

		// walk
		tb_size_t i = 0;
		tb_size_t n = tb_min(tb_vector_size(g_list.list), 2);
		tb_assert_and_check_break(n <= 2);
		for (; i < n; i++)
		{
			tb_dns_server_t const* server = (tb_dns_server_t const*)tb_iterator_item(g_list.list, i);
			if (server) addr[ok++] = server->addr;
		}

	} while (0);

	// leave
	tb_spinlock_leave(&g_lock);

	// trace
	if (!ok) tb_trace_e("no server!");

	// ok?
	return ok;
}
tb_void_t tb_dns_server_add(tb_char_t const* addr)
{
	// check
	tb_assert_and_check_return(addr);

	// init first
	tb_dns_server_init();

	// enter
	tb_spinlock_enter(&g_lock);

	// done
	do
	{
		// check
		tb_assert_and_check_break(g_list.list);

		// init server
		tb_dns_server_t server = {0};
		if (!tb_ipv4_set(&server.addr, addr)) break;

		// add server
		tb_vector_insert_tail(g_list.list, &server);

		// need sort it again
		g_list.sort = tb_false;

	} while (0);

	// leave
	tb_spinlock_leave(&g_lock);
}

