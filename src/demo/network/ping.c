/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_ICMP_ECHOREPLY  (0)
#define TB_ICMP_ECHOREQ    (8)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */
#include "tbox/prefix/packed.h"

// the ip header type - rfc791
typedef struct __tb_ip_header_t
{
    // version and ihl
    tb_uint8_t              vihl;

    // type of service
    tb_uint8_t              tos;

    // total length of the packet
    tb_uint16_t             total_len;

    // unique identifier
    tb_uint16_t             id;

    // flags and fragment offset
    tb_uint16_t             frag_and_flags;

    // time to live
    tb_uint8_t              ttl;

    // the protocol, tcp, udp, ..
    tb_uint8_t              proto;

    // the checksum
    tb_uint16_t             checksum;

    // the source ipaddr
    tb_uint32_t             source_ip;

    // the dest ipaddr
    tb_uint32_t             dest_ip;

} __tb_packed__ tb_ip_header_t;

// the icmp header type - rfc792
typedef struct __tb_icmp_header_t
{
    // icmp type
    tb_uint8_t              type;

    // type sub code
    tb_uint8_t              code;

    // checksum
    tb_uint16_t             checksum;

    // identifier
    tb_uint16_t             id;

    // sequence numner
    tb_uint16_t             seq;

    // data
    tb_byte_t               data[1];

} __tb_packed__ tb_icmp_header_t;

// the icmp echo request type
typedef struct __tb_icmp_echo_request_t
{
    // the icmp header
    tb_icmp_header_t        icmp;

    // the request time
    tb_uint64_t             time;

} __tb_packed__ tb_icmp_echo_request_t;

// the icmp echo reply type
typedef struct __tb_icmp_echo_reply_t
{
    // the ip header
    tb_ip_header_t          ip;

    // the echo request
    tb_icmp_echo_request_t  request;

} __tb_packed__ tb_icmp_echo_reply_t;

#include "tbox/prefix/packed.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_uint16_t tb_calculate_checksum(tb_byte_t const* data, tb_long_t size)
{
    // calculate sum
    tb_uint32_t         sum = 0;
    tb_long_t           nleft = size;
    tb_byte_t const*    p = data;
    while (nleft > 1)
    {
        sum += tb_bits_get_u16_le(p);
        nleft -= 2;
        p += 2;
    }

    // calculate answer
    tb_uint16_t answer = 0;
    if (nleft == 1)
    {
        *(tb_byte_t *)(&answer) = *p;
        sum += answer;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;
    return answer;
}
static tb_bool_t tb_ping_send(tb_socket_ref_t sock, tb_ipaddr_ref_t addr, tb_uint16_t seq)
{
    // init echo
    tb_icmp_echo_request_t echo;
    tb_uint64_t time            = tb_mclock();
    echo.icmp.type              = TB_ICMP_ECHOREQ;
    echo.icmp.code              = 0;
    tb_bits_set_u16_le((tb_byte_t*)&echo.icmp.checksum, 0);
    tb_bits_set_u16_le((tb_byte_t*)&echo.icmp.id, 0xbeaf);
    tb_bits_set_u16_le((tb_byte_t*)&echo.icmp.seq, seq);
    tb_bits_set_u64_le((tb_byte_t*)&echo.time, time);
    tb_uint16_t checksum        = tb_calculate_checksum((tb_byte_t const*)&echo, sizeof(echo));
    tb_bits_set_u16_le((tb_byte_t*)&echo.icmp.checksum, checksum);

    // send echo
    tb_long_t send = 0;
    tb_long_t size = sizeof(echo);
    tb_bool_t wait = tb_false;
    tb_byte_t const* data = (tb_byte_t const*)&echo;
    while (send < size)
    {
        tb_long_t real = tb_socket_usend(sock, addr, data + send, size - send);
        if (real > 0)
        {
            send += real;
            wait = tb_false;
        }
        else if (!real && !wait)
        {
            wait = tb_true;
            real = tb_socket_wait(sock, TB_SOCKET_EVENT_SEND, -1);
            tb_assert_and_check_break(real > 0);
        }
        else break;
    }

    // ok?
    return send == size;
}
static tb_bool_t tb_ping_recv(tb_socket_ref_t sock, tb_uint16_t seq)
{
    // recv echo
    tb_icmp_echo_reply_t echo;
    tb_long_t recv = 0;
    tb_long_t size = sizeof(echo);
    tb_bool_t wait = tb_false;
    tb_byte_t* data = (tb_byte_t*)&echo;
    while (recv < size)
    {
        tb_long_t real = tb_socket_urecv(sock, tb_null, data + recv, size - recv);
        if (real > 0)
        {
            recv += real;
            wait = tb_false;
        }
        else if (!real && !wait)
        {
            wait = tb_true;
            real = tb_socket_wait(sock, TB_SOCKET_EVENT_RECV, -1);
            tb_assert_and_check_break(real > 0);
        }
        else break;
    }
    tb_assert_and_check_return_val(recv == size, tb_false);

    // check protocol (icmp)
    tb_assert_and_check_return_val(echo.ip.proto == 1, tb_false);

    // check icmp type
    tb_assert_and_check_return_val(echo.request.icmp.type == TB_ICMP_ECHOREPLY, tb_false);

    // check id
    tb_assert_and_check_return_val(tb_bits_get_u16_le((tb_byte_t const*)&echo.request.icmp.id) == 0xbeaf, tb_false);

    // get source ip address
    tb_ipv4_t source_ip;
    source_ip.u32 = tb_bits_get_u32_le((tb_byte_t const*)&echo.ip.source_ip);

    // trace
    tb_uint64_t time = tb_bits_get_u64_le((tb_byte_t const*)&echo.request.time);
    tb_trace_i("%ld bytes from %{ipv4}: icmp_seq=%d ttl=%d time=%ld ms", size, &source_ip, seq, echo.ip.ttl, tb_mclock() - (tb_hong_t)time);

    // ok
    return tb_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_network_ping_main(tb_int_t argc, tb_char_t** argv)
{
    // check
    tb_assert_and_check_return_val(argc == 2 && argv[1], -1);

    // init addr
    tb_ipaddr_t addr;
    if (!tb_dns_looker_done(argv[1], &addr)) return -1;

    // init socket
    tb_socket_ref_t sock = tb_socket_init(TB_SOCKET_TYPE_ICMP, TB_IPADDR_FAMILY_IPV4);
    if (sock)
    {
        // trace
        tb_trace_i("PING %s (%{ipv4}): %d data bytes", argv[1], tb_ipaddr_ipv4(&addr), sizeof(tb_icmp_echo_request_t));

        // send ping
        tb_uint16_t i = 0;
        tb_uint16_t n = 10;
        while (i < n && tb_ping_send(sock, &addr, i))
        {
            // recv ping
            if (!tb_ping_recv(sock, i)) break;
            i++;

            // wait some time
            tb_sleep(1);
        }

        // exit socket
        tb_socket_exit(sock);
    }

    return 0;
}
