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
 * @file        looker.c
 * @ingroup     network
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME        "dns_looker"
#define TB_TRACE_MODULE_DEBUG       (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "looker.h"
#include "cache.h"
#include "server.h"
#include "../../asio/asio.h"
#include "../../string/string.h"
#include "../../memory/memory.h"
#include "../../network/network.h"
#include "../../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the dns impl timeout
#define TB_DNS_LOOKER_TIMEOUT   (5000)

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

// the dns impl step enum
typedef enum __tb_dns_looker_step_e
{
    TB_DNS_LOOKER_STEP_NONE     = 0
,   TB_DNS_LOOKER_STEP_REQT     = 1
,   TB_DNS_LOOKER_STEP_RESP     = 2
,   TB_DNS_LOOKER_STEP_NEVT     = 4

}tb_dns_looker_step_e;

// the dns impl type
typedef struct __tb_dns_looker_impl_t
{
    // the name
    tb_static_string_t      name;

    // the request and response packet
    tb_static_buffer_t      rpkt;
    
    // the size for recv and send packet
    tb_size_t               size;

    // the iterator
    tb_size_t               itor;

    // the step
    tb_size_t               step;

    // the tryn
    tb_size_t               tryn;

    // the socket
    tb_socket_ref_t         sock;

    // the socket family
    tb_uint8_t              family;

    // the server list
    tb_ipaddr_t             list[2];

    // the server maxn
    tb_size_t               maxn;

    // the data
    tb_byte_t               data[TB_DNS_NAME_MAXN + TB_DNS_RPKT_MAXN];

}tb_dns_looker_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_long_t tb_dns_looker_reqt(tb_dns_looker_impl_t* impl)
{
    // check
    tb_check_return_val(!(impl->step & TB_DNS_LOOKER_STEP_REQT), 1);
    
    // format it first if the request is null
    if (!tb_static_buffer_size(&impl->rpkt))
    {
        // check size
        tb_assert_and_check_return_val(!impl->size, -1);

        // format query
        tb_static_stream_t  stream;
        tb_byte_t           rpkt[TB_DNS_RPKT_MAXN];
        tb_size_t           size = 0;
        tb_byte_t*          p = tb_null;
        tb_static_stream_init(&stream, rpkt, TB_DNS_RPKT_MAXN);

        // identification number
        tb_static_stream_writ_u16_be(&stream, TB_DNS_HEADER_MAGIC);

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
        tb_static_stream_writ_u16_be(&stream, 0x0100);
#else
        tb_static_stream_writ_u1(&stream, 0);          // this is a query
        tb_static_stream_writ_ubits32(&stream, 0, 4);  // this is a standard query
        tb_static_stream_writ_u1(&stream, 0);          // not authoritive answer
        tb_static_stream_writ_u1(&stream, 0);          // not truncated
        tb_static_stream_writ_u1(&stream, 1);          // recursion desired

        tb_static_stream_writ_u1(&stream, 0);          // recursion not available! hey we dont have it (lol)
        tb_static_stream_writ_u1(&stream, 0);
        tb_static_stream_writ_u1(&stream, 0);
        tb_static_stream_writ_u1(&stream, 0);
        tb_static_stream_writ_ubits32(&stream, 0, 4);
#endif

        /* we have only one question
         *
         * tb_uint16_t question;        // number of question entries
         * tb_uint16_t answer;          // number of answer entries
         * tb_uint16_t authority;       // number of authority entries
         * tb_uint16_t resource;        // number of resource entries
         *
         */
        tb_static_stream_writ_u16_be(&stream, 1); 
        tb_static_stream_writ_u16_be(&stream, 0);
        tb_static_stream_writ_u16_be(&stream, 0);
        tb_static_stream_writ_u16_be(&stream, 0);

        // set questions, see as tb_dns_question_t
        // name + question1 + question2 + ...
        tb_static_stream_writ_u8(&stream, '.');
        p = (tb_byte_t*)tb_static_stream_writ_cstr(&stream, tb_static_string_cstr(&impl->name));

        // only one question now.
        tb_static_stream_writ_u16_be(&stream, 1);      // we are requesting the ipv4 address
        tb_static_stream_writ_u16_be(&stream, 1);      // it's internet (lol)

        // encode dns name
        if (!p || !tb_dns_encode_name((tb_char_t*)p - 1)) return -1;

        // size
        size = tb_static_stream_offset(&stream);
        tb_assert_and_check_return_val(size, -1);

        // copy
        tb_static_buffer_memncpy(&impl->rpkt, rpkt, size);
    }

    // data && size
    tb_byte_t const*    data = tb_static_buffer_data(&impl->rpkt);
    tb_size_t           size = tb_static_buffer_size(&impl->rpkt);

    // check
    tb_assert_and_check_return_val(data && size && impl->size < size, -1);

    // try get addr from the dns list
    tb_ipaddr_ref_t addr = tb_null;
    if (impl->maxn && impl->itor && impl->itor <= impl->maxn)
        addr = &impl->list[impl->itor - 1];

    // check
    tb_assert_and_check_return_val(addr && !tb_ipaddr_is_empty(addr), -1);

    // family have been changed? reinit socket
    if (tb_ipaddr_family(addr) != impl->family)
    {
        // exit the previous socket
        if (impl->sock) tb_socket_exit(impl->sock);

        // init a new socket for the family
        impl->sock = tb_socket_init(TB_SOCKET_TYPE_UDP, tb_ipaddr_family(addr));
        tb_assert_and_check_return_val(impl->sock, -1);

        // update the new family
        impl->family = (tb_uint8_t)tb_ipaddr_family(addr);
    }

    // need wait if no data
    impl->step &= ~TB_DNS_LOOKER_STEP_NEVT;

    // trace
    tb_trace_d("request: try %{ipaddr}", addr);

    // send request
    while (impl->size < size)
    {
        // writ data
        tb_long_t writ = tb_socket_usend(impl->sock, addr, data + impl->size, size - impl->size);
        //tb_trace_d("writ: %d", writ);
        tb_assert_and_check_return_val(writ >= 0, -1);

        // no data? 
        if (!writ)
        {
            // abort?
            tb_check_return_val(!impl->size && !impl->tryn, -1);

            // tryn++
            impl->tryn++;

            // continue
            return 0;
        }
        else impl->tryn = 0;

        // update size
        impl->size += writ;
    }

    // finish it
    impl->step |= TB_DNS_LOOKER_STEP_REQT;
    impl->tryn = 0;

    // reset rpkt
    impl->size = 0;
    tb_static_buffer_clear(&impl->rpkt);

    // ok
    tb_trace_d("request: ok");
    return 1;
}
static tb_bool_t tb_dns_looker_resp_done(tb_dns_looker_impl_t* impl, tb_ipaddr_ref_t addr)
{
    // the rpkt and size
    tb_byte_t const*    rpkt = tb_static_buffer_data(&impl->rpkt);
    tb_size_t           size = tb_static_buffer_size(&impl->rpkt);
    tb_assert_and_check_return_val(rpkt && size >= TB_DNS_HEADER_SIZE, tb_false);

    // init stream
    tb_static_stream_t stream;
    tb_static_stream_init(&stream, (tb_byte_t*)rpkt, size);

    // init header
    tb_dns_header_t header;
    header.id           = tb_static_stream_read_u16_be(&stream); tb_static_stream_skip(&stream, 2);
    header.question     = tb_static_stream_read_u16_be(&stream);
    header.answer       = tb_static_stream_read_u16_be(&stream);
    header.authority    = tb_static_stream_read_u16_be(&stream);
    header.resource     = tb_static_stream_read_u16_be(&stream);

    // trace
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
    tb_static_stream_skip_cstr(&stream);
    tb_static_stream_skip(&stream, 4);
#else
    tb_char_t* name = tb_static_stream_read_cstr(&stream);
    //name = tb_dns_decode_name(name);
    tb_assert_and_check_return_val(name, tb_false);
    tb_static_stream_skip(&stream, 4);
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
        tb_char_t const* name = tb_dns_decode_name(&stream, answer.name); tb_used(name);
        tb_trace_d("response: name: %s", name);

        // decode resource
        answer.res.type     = tb_static_stream_read_u16_be(&stream);
        answer.res.class_   = tb_static_stream_read_u16_be(&stream);
        answer.res.ttl      = tb_static_stream_read_u32_be(&stream);
        answer.res.size     = tb_static_stream_read_u16_be(&stream);
        tb_trace_d("response: type: %d",    answer.res.type);
        tb_trace_d("response: class: %d",   answer.res.class_);
        tb_trace_d("response: ttl: %d",     answer.res.ttl);
        tb_trace_d("response: size: %d",    answer.res.size);

        // is ipv4?
        if (answer.res.type == 1)
        {
            // get ipv4
            tb_byte_t b1 = tb_static_stream_read_u8(&stream);
            tb_byte_t b2 = tb_static_stream_read_u8(&stream);
            tb_byte_t b3 = tb_static_stream_read_u8(&stream);
            tb_byte_t b4 = tb_static_stream_read_u8(&stream);

            // trace
            tb_trace_d("response: ipv4: %u.%u.%u.%u", b1, b2, b3, b4);

            // save the first ip
            if (!found) 
            {
                // save it
                if (addr)
                {
                    // init ipv4
                    tb_ipv4_t ipv4;
                    ipv4.u8[0] = b1;
                    ipv4.u8[1] = b2;
                    ipv4.u8[2] = b3;
                    ipv4.u8[3] = b4;

                    // save ipv4
                    tb_ipaddr_ipv4_set(addr, &ipv4);
                }

                // found it
                found = 1;

                // trace
                tb_trace_d("response: ");
                break;
            }
        }
        else
        {
            // decode rdata
            answer.rdata = (tb_byte_t*)tb_dns_decode_name(&stream, answer.name);

            // trace
            tb_trace_d("response: alias: %s", answer.rdata? (tb_char_t const*)answer.rdata : "");
        }

        // trace
        tb_trace_d("response: ");
    }

    // found it?
    tb_check_return_val(found, tb_false);

#if 0
    // decode authorities
    for (i = 0; i < header.authority; i++)
    {
        // decode answer
        tb_dns_answer_t answer;
        tb_trace_d("response: authority: %d", i);

        // decode dns name
        tb_char_t* name = tb_dns_decode_name(&stream, answer.name);
        tb_trace_d("response: name: %s", name? name : "");

        // decode resource
        answer.res.type =   tb_static_stream_read_u16_be(&stream);
        answer.res.class_ = tb_static_stream_read_u16_be(&stream);
        answer.res.ttl =    tb_static_stream_read_u32_be(&stream);
        answer.res.size =   tb_static_stream_read_u16_be(&stream);
        tb_trace_d("response: type: %d",    answer.res.type);
        tb_trace_d("response: class: %d",   answer.res.class_);
        tb_trace_d("response: ttl: %d",     answer.res.ttl);
        tb_trace_d("response: size: %d",    answer.res.size);

        // is ipv4?
        if (answer.res.type == 1)
        {
            tb_byte_t b1 = tb_static_stream_read_u8(&stream);
            tb_byte_t b2 = tb_static_stream_read_u8(&stream);
            tb_byte_t b3 = tb_static_stream_read_u8(&stream);
            tb_byte_t b4 = tb_static_stream_read_u8(&stream);
            tb_trace_d("response: ipv4: %u.%u.%u.%u", b1, b2, b3, b4);
        }
        else
        {
            // decode data
            answer.rdata = tb_dns_decode_name(&stream, answer.name);
            tb_trace_d("response: server: %s", answer.rdata? answer.rdata : "");
        }
        tb_trace_d("response: ");
    }

    for (i = 0; i < header.resource; i++)
    {
        // decode answer
        tb_dns_answer_t answer;
        tb_trace_d("response: resource: %d", i);

        // decode dns name
        tb_char_t* name = tb_dns_decode_name(&stream, answer.name);
        tb_trace_d("response: name: %s", name? name : "");

        // decode resource
        answer.res.type =   tb_static_stream_read_u16_be(&stream);
        answer.res.class_ = tb_static_stream_read_u16_be(&stream);
        answer.res.ttl =    tb_static_stream_read_u32_be(&stream);
        answer.res.size =   tb_static_stream_read_u16_be(&stream);
        tb_trace_d("response: type: %d",    answer.res.type);
        tb_trace_d("response: class: %d",   answer.res.class_);
        tb_trace_d("response: ttl: %d",     answer.res.ttl);
        tb_trace_d("response: size: %d",    answer.res.size);

        // is ipv4?
        if (answer.res.type == 1)
        {
            tb_byte_t b1 = tb_static_stream_read_u8(&stream);
            tb_byte_t b2 = tb_static_stream_read_u8(&stream);
            tb_byte_t b3 = tb_static_stream_read_u8(&stream);
            tb_byte_t b4 = tb_static_stream_read_u8(&stream);
            tb_trace_d("response: ipv4: %u.%u.%u.%u", b1, b2, b3, b4);
        }
        else
        {
            // decode data
            answer.rdata = tb_dns_decode_name(&stream, answer.name);
            tb_trace_d("response: alias: %s", answer.rdata? answer.rdata : "");
        }
        tb_trace_d("response: ");
    }
#endif

    // ok
    return tb_true;
}
static tb_long_t tb_dns_looker_resp(tb_dns_looker_impl_t* impl, tb_ipaddr_ref_t addr)
{
    // check
    tb_check_return_val(!(impl->step & TB_DNS_LOOKER_STEP_RESP), 1);

    // need wait if no data
    impl->step &= ~TB_DNS_LOOKER_STEP_NEVT;

    // recv response data
    tb_byte_t rpkt[4096];
    while (1)
    {
        // read data
        tb_long_t read = tb_socket_urecv(impl->sock, tb_null, rpkt, 4096);
        //tb_trace_d("read %d", read);
        tb_assert_and_check_return_val(read >= 0, -1);

        // no data? 
        if (!read)
        {
            // end? read x, read 0
            tb_check_break(!tb_static_buffer_size(&impl->rpkt));
    
            // abort? read 0, read 0
            tb_check_return_val(!impl->tryn, -1);
            
            // tryn++
            impl->tryn++;

            // continue 
            return 0;
        }
        else impl->tryn = 0;

        // copy data
        tb_static_buffer_memncat(&impl->rpkt, rpkt, read);
    }

    // done
    if (!tb_dns_looker_resp_done(impl, addr)) return -1;

    // check
    tb_assert_and_check_return_val(tb_static_string_size(&impl->name) && !tb_ipaddr_ip_is_empty(addr), -1);

    // save address to cache
    tb_dns_cache_set(tb_static_string_cstr(&impl->name), addr);

    // finish it
    impl->step |= TB_DNS_LOOKER_STEP_RESP;
    impl->tryn = 0;

    // reset rpkt
    impl->size = 0;
    tb_static_buffer_clear(&impl->rpkt);

    // ok
    tb_trace_d("response: ok");
    return 1;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_dns_looker_ref_t tb_dns_looker_init(tb_char_t const* name)
{
    // check
    tb_assert_and_check_return_val(name, tb_null);

    // must be not address
    tb_assert(!tb_ipaddr_ip_cstr_set(tb_null, name, TB_IPADDR_FAMILY_NONE));

    // done
    tb_bool_t               ok = tb_false;
    tb_dns_looker_impl_t*   impl = tb_null;
    do
    {
        // make impl
        impl = tb_malloc0_type(tb_dns_looker_impl_t);
        tb_assert_and_check_return_val(impl, tb_null);

        // dump server
//      tb_dns_server_dump();

        // get the dns server list
        impl->maxn = tb_dns_server_get(impl->list);
        tb_check_break(impl->maxn && impl->maxn <= tb_arrayn(impl->list));

        // init name
        if (!tb_static_string_init(&impl->name, (tb_char_t*)impl->data, TB_DNS_NAME_MAXN)) break;
        tb_static_string_cstrcpy(&impl->name, name);

        // init rpkt
        if (!tb_static_buffer_init(&impl->rpkt, impl->data + TB_DNS_NAME_MAXN, TB_DNS_RPKT_MAXN)) break;

        // init family
        impl->family = TB_IPADDR_FAMILY_IPV4;

        // init sock
        impl->sock = tb_socket_init(TB_SOCKET_TYPE_UDP, impl->family);
        tb_assert_and_check_break(impl->sock);

        // init itor
        impl->itor = 1;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (impl) tb_dns_looker_exit((tb_dns_looker_ref_t)impl);
        impl = tb_null;
    }

    // ok?
    return (tb_dns_looker_ref_t)impl;
}
tb_long_t tb_dns_looker_spak(tb_dns_looker_ref_t looker, tb_ipaddr_ref_t addr)
{
    // check
    tb_dns_looker_impl_t* impl = (tb_dns_looker_impl_t*)looker;
    tb_assert_and_check_return_val(impl && addr, -1);

    // init 
    tb_long_t r = -1;
    do
    {
        // request
        r = tb_dns_looker_reqt(impl);
        tb_check_break(r > 0);
            
        // response
        r = tb_dns_looker_resp(impl, addr);
        tb_check_break(r > 0);

    } while (0);

    // failed?
    if (r < 0)
    {
        // next
        if (impl->itor + 1 <= impl->maxn) impl->itor++;
        else impl->itor = 0;

        // has next?
        if (impl->itor)
        {
            // reset step, no event now, need not wait
            impl->step = TB_DNS_LOOKER_STEP_NONE | TB_DNS_LOOKER_STEP_NEVT;

            // reset rpkt
            impl->size = 0;
            tb_static_buffer_clear(&impl->rpkt);

            // continue 
            r = 0;
        }
    }

    // ok?
    return r;
}
tb_long_t tb_dns_looker_wait(tb_dns_looker_ref_t looker, tb_long_t timeout)
{
    // check
    tb_dns_looker_impl_t* impl = (tb_dns_looker_impl_t*)looker;
    tb_assert_and_check_return_val(impl && impl->sock, -1);

    // has asio event?
    tb_size_t e = TB_AIOE_CODE_NONE;
    if (!(impl->step & TB_DNS_LOOKER_STEP_NEVT))
    {
        if (!(impl->step & TB_DNS_LOOKER_STEP_REQT)) e = TB_AIOE_CODE_SEND;
        else if (!(impl->step & TB_DNS_LOOKER_STEP_RESP)) e = TB_AIOE_CODE_RECV;
    }

    // need wait?
    tb_long_t r = 0;
    if (e)
    {
        // wait
        r = tb_aioo_wait(impl->sock, e, timeout);

        // fail or timeout?
        tb_check_return_val(r > 0, r);
    }

    // ok?
    return r;
}
tb_void_t tb_dns_looker_exit(tb_dns_looker_ref_t looker)
{
    // the impl
    tb_dns_looker_impl_t* impl = (tb_dns_looker_impl_t*)looker;
    if (impl)
    {
        // exit sock
        if (impl->sock) tb_socket_exit(impl->sock);
        impl->sock = tb_null;

        // exit it
        tb_free(impl);
    }
}
tb_bool_t tb_dns_looker_done(tb_char_t const* name, tb_ipaddr_ref_t addr)
{
    // check
    tb_assert_and_check_return_val(name && addr, tb_false);

    // try to lookup it from cache first
    if (tb_dns_cache_get(name, addr)) return tb_true;

    // init looker
    tb_dns_looker_ref_t looker = tb_dns_looker_init(name);
    tb_check_return_val(looker, tb_false);

    // spak
    tb_long_t r = -1;
    while (!(r = tb_dns_looker_spak(looker, addr)))
    {
        // wait
        r = tb_dns_looker_wait(looker, TB_DNS_LOOKER_TIMEOUT);
        tb_assert_and_check_break(r >= 0);
    }

    // exit
    tb_dns_looker_exit(looker);

    // ok
    return r > 0? tb_true : tb_false;
}

