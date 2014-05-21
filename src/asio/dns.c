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
 * @author      ruki
 * @file        dns.c
 * @ingroup     asio
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "aicp_dns"
#define TB_TRACE_MODULE_DEBUG               (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "dns.h"
#include "aico.h"
#include "aicp.h"
#include "../network/network.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the aicp dns type
typedef struct __tb_aicp_dns_t
{
    // the done func
    tb_aicp_dns_done_func_t done;

    // the exit func
    tb_aicp_dns_exit_func_t exit;

    // the func private data
    tb_cpointer_t           priv;

    // the sock
    tb_handle_t             sock;

    // the aico
    tb_handle_t             aico;

    // the server indx
    tb_size_t               indx;

    // the server list
    tb_ipv4_t               list[3];

    // the server size
    tb_size_t               size;

    // the data
    tb_byte_t               data[TB_DNS_RPKT_MAXN];

    // the host
    tb_char_t               host[256];

}tb_aicp_dns_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_size_t tb_aicp_dns_reqt_init(tb_aicp_dns_t* dns)
{
    // check
    tb_assert_and_check_return_val(dns, 0);

    // init query data
    tb_static_stream_t sstream;
    tb_static_stream_init(&sstream, dns->data, TB_DNS_RPKT_MAXN);

    // identification number
    tb_static_stream_writ_u16_be(&sstream, TB_DNS_HEADER_MAGIC);

    /* 0x2104: 0 0000 001 0000 0000
     *
     * tb_uint16_t qr     :1;       // query/response flag
     * tb_uint16_t opcode :4;       // purpose of message
     * tb_uint16_t aa     :1;       // authoritive answer
     * tb_uint16_t tc     :1;       // truncated message
     * tb_uint16_t rd     :1;       // recursion desired

     * tb_uint16_t ra     :1;       // recursion available
     * tb_uint16_t z      :1;       // its z! reserved
     * tb_uint16_t ad     :1;       // authenticated data
     * tb_uint16_t cd     :1;       // checking disabled
     * tb_uint16_t rcode  :4;       // response code
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
    tb_static_stream_writ_u1(&sstream, 0);          // this is a query
    tb_static_stream_writ_ubits32(&sstream, 0, 4);  // this is a standard query
    tb_static_stream_writ_u1(&sstream, 0);          // not authoritive answer
    tb_static_stream_writ_u1(&sstream, 0);          // not truncated
    tb_static_stream_writ_u1(&sstream, 1);          // recursion desired

    tb_static_stream_writ_u1(&sstream, 0);          // recursion not available! hey we dont have it (lol)
    tb_static_stream_writ_u1(&sstream, 0);
    tb_static_stream_writ_u1(&sstream, 0);
    tb_static_stream_writ_u1(&sstream, 0);
    tb_static_stream_writ_ubits32(&sstream, 0, 4);
#endif

    /* we have only one question
     *
     * tb_uint16_t question;        // number of question entries
     * tb_uint16_t answer;          // number of answer entries
     * tb_uint16_t authority;       // number of authority entries
     * tb_uint16_t resource;        // number of resource entries
     *
     */
    tb_static_stream_writ_u16_be(&sstream, 1); 
    tb_static_stream_writ_u16_be(&sstream, 0);
    tb_static_stream_writ_u16_be(&sstream, 0);
    tb_static_stream_writ_u16_be(&sstream, 0);

    // set questions, see as tb_dns_question_t
    // name + question1 + question2 + ...
    tb_static_stream_writ_u8(&sstream, '.');
    tb_char_t* p = tb_static_stream_writ_cstr(&sstream, dns->host);

    // only one question now.
    tb_static_stream_writ_u16_be(&sstream, 1);      // we are requesting the ipv4 dnsess
    tb_static_stream_writ_u16_be(&sstream, 1);      // it's internet (lol)

    // encode dns name
    if (!p || !tb_dns_encode_name(p - 1)) return 0;

    // ok?
    return tb_static_stream_offset(&sstream);
}
static tb_bool_t tb_aicp_dns_resp_done(tb_aicp_dns_t* dns, tb_size_t size, tb_ipv4_t* ipv4)
{
    // check
    tb_assert_and_check_return_val(dns && ipv4, tb_false);

    // check
    tb_assert_and_check_return_val(size >= TB_DNS_HEADER_SIZE, tb_false);

    // decode dns header
    tb_static_stream_t  sstream;
    tb_dns_header_t header;
    tb_static_stream_init(&sstream, dns->data, size);
    header.id = tb_static_stream_read_u16_be(&sstream);
    tb_static_stream_skip(&sstream, 2);
    header.question     = tb_static_stream_read_u16_be(&sstream);
    header.answer       = tb_static_stream_read_u16_be(&sstream);
    header.authority    = tb_static_stream_read_u16_be(&sstream);
    header.resource     = tb_static_stream_read_u16_be(&sstream);
    tb_trace_d("response: size: %u",        size);
    tb_trace_d("response: id: 0x%04x",      header.id);
    tb_trace_d("response: question: %d",    header.question);
    tb_trace_d("response: answer: %d",      header.answer);
    tb_trace_d("response: authority: %d",   header.authority);
    tb_trace_d("response: resource: %d",    header.resource);
    tb_trace_d("");

    // check header
    tb_assert_and_check_return_val(header.id == TB_DNS_HEADER_MAGIC, tb_false);

    // skip questions, only one question now.
    // name + question1 + question2 + ...
    tb_assert_and_check_return_val(header.question == 1, tb_false);
#if 1
    tb_static_stream_skip_cstr(&sstream);
    tb_static_stream_skip(&sstream, 4);
#else
    tb_char_t* name = tb_static_stream_read_cstr(&sstream);
    //name = tb_dns_decode_name(name);
    tb_assert_and_check_return_val(name, tb_false);
    tb_static_stream_skip(&sstream, 4);
    tb_trace_d("response: name: %s", name);
#endif

    // decode answers
    tb_size_t i = 0;
    tb_size_t found = 0;
    for (i = 0; i < header.answer; i++)
    {
        // decode answer
        tb_dns_answer_t answer;
        tb_trace_d("response: answer: %d", i);

        // decode dns name
        tb_char_t const* name = tb_dns_decode_name(&sstream, answer.name); tb_used(name);
        tb_trace_d("response: name: %s", name);

        // decode resource
        answer.res.type     = tb_static_stream_read_u16_be(&sstream);
        answer.res.class_   = tb_static_stream_read_u16_be(&sstream);
        answer.res.ttl      = tb_static_stream_read_u32_be(&sstream);
        answer.res.size     = tb_static_stream_read_u16_be(&sstream);
        tb_trace_d("response: type: %d",    answer.res.type);
        tb_trace_d("response: class: %d",   answer.res.class_);
        tb_trace_d("response: ttl: %d",         answer.res.ttl);
        tb_trace_d("response: size: %d",    answer.res.size);

        // is ipv4?
        if (answer.res.type == 1)
        {
            tb_byte_t b1 = tb_static_stream_read_u8(&sstream);
            tb_byte_t b2 = tb_static_stream_read_u8(&sstream);
            tb_byte_t b3 = tb_static_stream_read_u8(&sstream);
            tb_byte_t b4 = tb_static_stream_read_u8(&sstream);
            tb_trace_d("response: ipv4: %u.%u.%u.%u", b1, b2, b3, b4);

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
                tb_trace_d("response: ");
                break;
            }
        }
        else
        {
            // decode rdata
            answer.rdata = (tb_byte_t const*)tb_dns_decode_name(&sstream, answer.name);
            tb_trace_d("response: alias: %s", answer.rdata? (tb_char_t const*)answer.rdata : "");
        }
        tb_trace_d("response: ");
    }

    // found it?
    tb_check_return_val(found, tb_false);

    // ok
    return tb_true;
}
static tb_bool_t tb_aicp_dns_reqt_func(tb_aice_t const* aice);
static tb_bool_t tb_aicp_dns_resp_func(tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_URECV, tb_false);

    // the aicp
    tb_aicp_t* aicp = (tb_aicp_t*)tb_aico_aicp(aice->aico);
    tb_assert_and_check_return_val(aicp, tb_false);
    
    // the dns
    tb_aicp_dns_t* dns = aice->priv; 
    tb_assert_and_check_return_val(dns, tb_false);

    // done
    tb_ipv4_t ipv4 = {0};
    if (aice->state == TB_STATE_OK)
    {
        // trace
        tb_trace_d("resp[%s]: server: %u.%u.%u.%u, real: %lu", dns->host, tb_ipv4_u8x4(aice->u.urecv.addr), aice->u.urecv.real);

        // check
        tb_assert_and_check_return_val(aice->u.urecv.real, tb_false);

        // done resp
        tb_aicp_dns_resp_done(dns, aice->u.urecv.real, &ipv4);
    }
    // timeout or failed?
    else
    {
        // trace
        tb_trace_d("resp[%s]: server: %u.%u.%u.%u, state: %s", dns->host, tb_ipv4_u8x4(aice->u.urecv.addr), tb_state_cstr(aice->state));
    }

    // ok or try to get ok from cache again if failed or timeout? 
    tb_bool_t from_cache = tb_false;
    if (ipv4.u32 || (from_cache = tb_dns_cache_get(dns->host, &ipv4))) 
    {
        // save to cache 
        if (!from_cache) tb_dns_cache_set(dns->host, &ipv4);
        
        // done func
        dns->done(dns, dns->host, &ipv4, dns->priv);
        return tb_true;
    }

    // try next server?
    tb_bool_t ok = tb_false;
    tb_ipv4_t const* server = &dns->list[dns->indx + 1];
    if (server->u32)
    {   
        // indx++
        dns->indx++;

        // init reqt
        tb_size_t size = tb_aicp_dns_reqt_init(dns);
        if (size)
        {
            // post reqt
            ok = tb_aico_usend(aice->aico, server, TB_DNS_HOST_PORT, dns->data, size, tb_aicp_dns_reqt_func, (tb_pointer_t)dns);
        }
    }

    // failed? done func
    if (!ok) dns->done(dns, dns->host, tb_null, dns->priv);

    // continue
    return tb_true;
}
static tb_bool_t tb_aicp_dns_reqt_func(tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_USEND, tb_false);

    // the aicp
    tb_aicp_t* aicp = (tb_aicp_t*)tb_aico_aicp(aice->aico);
    tb_assert_and_check_return_val(aicp, tb_false);
    
    // the dns
    tb_aicp_dns_t* dns = aice->priv; 
    tb_assert_and_check_return_val(dns && dns->done, tb_false);

    // done
    tb_bool_t ok = tb_false;
    if (aice->state == TB_STATE_OK)
    {
        // trace
        tb_trace_d("reqt[%s]: server: %u.%u.%u.%u, real: %lu", dns->host, tb_ipv4_u8x4(aice->u.usend.addr), aice->u.usend.real);

        // check
        tb_assert_and_check_return_val(aice->u.usend.real, tb_false);

        // the server 
        tb_ipv4_t const* server = &dns->list[dns->indx];
        tb_assert_and_check_return_val(server->u32, tb_false);

        // post resp
        ok = tb_aico_urecv(aice->aico, server, TB_DNS_HOST_PORT, dns->data, sizeof(dns->data), tb_aicp_dns_resp_func, (tb_pointer_t)dns);
    }
    // timeout or failed?
    else
    {
        // trace
        tb_trace_d("reqt[%s]: server: %u.%u.%u.%u, state: %s", dns->host, tb_ipv4_u8x4(aice->u.usend.addr), tb_state_cstr(aice->state));
            
        // the next server 
        tb_ipv4_t const* server = &dns->list[dns->indx + 1];
        if (server->u32)
        {   
            // indx++
            dns->indx++;

            // init reqt
            tb_size_t size = tb_aicp_dns_reqt_init(dns);
            if (size)
            {
                // post reqt
                ok = tb_aico_usend(aice->aico, server, TB_DNS_HOST_PORT, dns->data, size, tb_aicp_dns_reqt_func, (tb_pointer_t)dns);
            }
        }
    }

    // failed? done func
    if (!ok) dns->done(dns, dns->host, tb_null, dns->priv);

    // continue 
    return tb_true;
}
static tb_void_t tb_aicp_dns_exit_func(tb_handle_t aico, tb_cpointer_t priv)
{
    // check
    tb_aicp_dns_t* dns = (tb_aicp_dns_t*)priv;
    tb_assert_and_check_return(dns);

    // done exit
    if (dns->exit) dns->exit(dns, dns->priv);

    // exit sock
    if (dns->sock) tb_socket_clos(dns->sock);
    dns->sock = tb_null;

    // exit it
    tb_free(dns);

    // trace
    tb_trace_d("exit: ok");
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_handle_t tb_aicp_dns_init(tb_aicp_t* aicp, tb_long_t timeout, tb_aicp_dns_done_func_t done, tb_aicp_dns_exit_func_t exit, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(aicp && done, tb_null);

    // done
    tb_bool_t       ok = tb_false;
    tb_aicp_dns_t*  dns = tb_null;
    do
    {
        // make dns
        dns = (tb_aicp_dns_t*)tb_malloc0(sizeof(tb_aicp_dns_t));
        tb_assert_and_check_break(dns);

        // init sock
        dns->sock = tb_socket_open(TB_SOCKET_TYPE_UDP);
        tb_assert_and_check_break(dns->sock);

        // init aico
        dns->aico = tb_aico_init_sock(aicp, dns->sock, tb_aicp_dns_exit_func, dns);
        tb_assert_and_check_break(dns->aico);

        // init timeout
        tb_aico_timeout_set(dns->aico, TB_AICO_TIMEOUT_SEND, timeout);
        tb_aico_timeout_set(dns->aico, TB_AICO_TIMEOUT_RECV, timeout);

        // init dns
        dns->done = done;
        dns->exit = exit;
        dns->priv = priv;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (dns) tb_aicp_dns_exit(dns);
        dns = tb_null;
    }

    // ok?
    return dns;
}
tb_void_t tb_aicp_dns_kill(tb_handle_t handle)
{
    // check
    tb_aicp_dns_t* dns = (tb_aicp_dns_t*)handle;
    tb_assert_and_check_return(dns);

    // trace
    tb_trace_d("kill: ..");

    // kill sock
    if (dns->sock) tb_socket_kill(dns->sock, TB_SOCKET_KILL_RW);
}
tb_void_t tb_aicp_dns_exit(tb_handle_t handle)
{
    // check
    tb_aicp_dns_t* dns = (tb_aicp_dns_t*)handle;
    tb_assert_and_check_return(dns && dns->aico);

    // trace
    tb_trace_d("exit: ..");

    // exit aico
    tb_aico_exit(dns->aico);
}
tb_bool_t tb_aicp_dns_done(tb_handle_t handle, tb_char_t const* host)
{
    // check
    tb_aicp_dns_t* dns = (tb_aicp_dns_t*)handle;
    tb_assert_and_check_return_val(dns && dns->aico && dns->done && host && host[0], tb_false);
    
    // save host
    tb_strlcpy(dns->host, host, sizeof(dns->host) - 1);
 
    // only ipv4? ok
    tb_ipv4_t ipv4 = {0};
    if (tb_ipv4_set(&ipv4, dns->host))
    {
        dns->done(handle, dns->host, &ipv4, dns->priv);
        return tb_true;
    }

    // try to lookup it from cache first
    if (tb_dns_cache_get(dns->host, &ipv4))
    {
        dns->done(handle, dns->host, &ipv4, dns->priv);
        return tb_true;
    }

    // init server list
    if (!dns->size) dns->size = tb_dns_server_get(dns->list);
    tb_check_return_val(dns->size, tb_false);

    // get the server 
    tb_ipv4_t const* server = &dns->list[dns->indx = 0];
    tb_assert_and_check_return_val(server->u32, tb_false);

    // init reqt
    tb_size_t size = tb_aicp_dns_reqt_init(dns);
    tb_assert_and_check_return_val(size, tb_false);

    // post reqt
    return tb_aico_usend(dns->aico, server, TB_DNS_HOST_PORT, dns->data, size, tb_aicp_dns_reqt_func, (tb_pointer_t)dns);
}
tb_aicp_t* tb_aicp_dns_aicp(tb_handle_t handle)
{
    // check
    tb_aicp_dns_t* dns = (tb_aicp_dns_t*)handle;
    tb_assert_and_check_return_val(dns && dns->aico, tb_null);
    
    // the aicp
    return tb_aico_aicp(dns->aico);
}
