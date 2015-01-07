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
 * @file        socket.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../file.h"
#include "../socket.h"
#include "../../utils/utils.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <signal.h>
#include <sys/uio.h>
#if defined(TB_CONFIG_OS_LINUX) || defined(TB_CONFIG_OS_ANDROID)
#   include <sys/sendfile.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_bool_t tb_socket_addr_save(tb_addr_ref_t addr, struct sockaddr_storage const* saddr)
{
    // check
    tb_assert_and_check_return_val(addr && saddr, tb_false);

    // clear address
    tb_addr_clear(addr);

    // done
    tb_bool_t ok = tb_false;
    switch (saddr->ss_family)
    {
    case AF_INET:
        {
            // the ipv4 addr
            struct sockaddr_in* addr4 = (struct sockaddr_in*)saddr;

            // save family
            tb_addr_family_set(addr, TB_ADDR_FAMILY_IPV4);

            // save ipv4
            addr->u.ipv4.u32 = (tb_uint32_t)addr4->sin_addr.s_addr;

            // save port
            tb_addr_port_set(addr, tb_bits_be_to_ne_u16(addr4->sin_port));

            // ok
            ok = tb_true;
        }
        break;
    case AF_INET6:
        {
            // the ipv6 addr
            struct sockaddr_in6* addr6 = (struct sockaddr_in6*)saddr;

            // check
            tb_assert_static(sizeof(addr->u.ipv6.u8) == sizeof(addr6->sin6_addr.s6_addr));
            tb_assert_static(tb_arrayn(addr->u.ipv6.u8) == tb_arrayn(addr6->sin6_addr.s6_addr));

            // save family
            tb_addr_family_set(addr, TB_ADDR_FAMILY_IPV6);

            // save ipv6
            tb_memcpy(addr->u.ipv6.u8, addr6->sin6_addr.s6_addr, sizeof(addr->u.ipv6.u8));

            // save port
            tb_addr_port_set(addr, tb_bits_be_to_ne_u16(addr6->sin6_port));

            // ok
            ok = tb_true;
        }
        break;
    default:
        tb_assert_abort(0);
        break;
    }
    
    // ok?
    return ok;
}
static tb_size_t tb_socket_addr_load(struct sockaddr_storage* saddr, tb_addr_ref_t addr)
{
    // check
    tb_assert_and_check_return_val(saddr && addr, 0);

    // check address
    tb_assert_abort(!tb_addr_is_empty(addr));

    // clear address
    tb_memset(saddr, 0, sizeof(struct sockaddr_storage));

    // done
    tb_size_t size = 0;
    switch (tb_addr_family(addr))
    {
    case TB_ADDR_FAMILY_IPV4:
        {
            // the ipv4 addr
            struct sockaddr_in* addr4 = (struct sockaddr_in*)saddr;

            // save family
            addr4->sin_family = AF_INET;

            // save ipv4
            addr4->sin_addr.s_addr = addr->u.ipv4.u32;

            // save port
            addr4->sin_port = tb_bits_ne_to_be_u16(tb_addr_port(addr));

            // save size
            size = sizeof(struct sockaddr_in);
        }
        break;
    case TB_ADDR_FAMILY_IPV6:
        {
            // the ipv6 addr
            struct sockaddr_in6* addr6 = (struct sockaddr_in6*)saddr;

            // check
            tb_assert_static(sizeof(addr->u.ipv6.u8) == sizeof(addr6->sin6_addr.s6_addr));
            tb_assert_static(tb_arrayn(addr->u.ipv6.u8) == tb_arrayn(addr6->sin6_addr.s6_addr));

            // save family
            addr6->sin6_family = AF_INET6;

            // save ipv6
            tb_memcpy(addr6->sin6_addr.s6_addr, addr->u.ipv6.u8, sizeof(addr6->sin6_addr.s6_addr));

            // save port
            addr6->sin6_port = tb_bits_ne_to_be_u16(tb_addr_port(addr));

            // save size
            size = sizeof(struct sockaddr_in6);
        }
        break;
    default:
        tb_assert_abort(0);
        break;
    }
    
    // ok?
    return size;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_socket_context_init()
{
    // ignore sigpipe
#if 1
    signal(SIGPIPE, SIG_IGN);
#else
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &sa, 0);
#endif

    // ok
    return tb_true;
}
tb_void_t tb_socket_context_exit()
{
}
tb_socket_ref_t tb_socket_init(tb_size_t type)
{
    // check
    tb_assert_and_check_return_val(type, tb_null);
    
    // init type & protocol
    tb_int_t t = 0;
    tb_int_t p = 0;
    switch (type)
    {
    case TB_SOCKET_TYPE_TCP:
        {
            t = SOCK_STREAM;
            p = IPPROTO_TCP;
        }
        break;
    case TB_SOCKET_TYPE_UDP:
        {
            t = SOCK_DGRAM;
            p = IPPROTO_UDP;
        }
        break;
    default:
        return tb_null;
    }

    // sock
    tb_int_t fd = socket(AF_INET, t, p);
    tb_assert_and_check_return_val(fd >= 0, tb_null);

    // non-block
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
 
#if 0
    int n = 0;
    int a = sizeof(n);
    getsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char *)&n, &a);
    tb_trace_i("n: %d", n);
#endif

    // trace
    tb_trace_d("open: %p", tb_fd2sock(fd));

    // ok
    return tb_fd2sock(fd);
}
tb_socket_ref_t tb_socket_init2(tb_size_t type, tb_size_t family)
{
    // check
    tb_assert_and_check_return_val(type, tb_null);
    
    // done
    tb_socket_ref_t sock = tb_null;
    do
    {
        // init socket type and protocol
        tb_int_t t = 0;
        tb_int_t p = 0;
        if (type == TB_SOCKET_TYPE_TCP)
        {
            t = SOCK_STREAM;
            p = IPPROTO_TCP;
        }
        else if(type == TB_SOCKET_TYPE_UDP)
        {
            t = SOCK_DGRAM;
            p = IPPROTO_UDP;
        }
        else break;

        // init socket family
        tb_int_t f = (family == TB_ADDR_FAMILY_IPV6)? AF_INET6 : AF_INET;

        // sock
        tb_int_t fd = socket(f, t, p);
        tb_assert_and_check_break(fd >= 0);

        // set non-block mode
        fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);

        // save socket
        sock = tb_fd2sock(fd);

    } while (0);

    // trace
    tb_trace_d("init: type: %lu, family: %lu, sock: %p", type, family, sock);

    // ok?
    return sock;
}
tb_bool_t tb_socket_pair(tb_size_t type, tb_socket_ref_t pair[2])
{
    // check
    tb_assert_and_check_return_val(type && pair, tb_false);

    // init type
    tb_int_t t = 0;
    switch (type)
    {
    case TB_SOCKET_TYPE_TCP:
        t = SOCK_STREAM;
        break;
    case TB_SOCKET_TYPE_UDP:
        t = SOCK_DGRAM;
        break;
    default:
        return tb_false;
    }

    // make pair
    tb_int_t fd[2] = {0};
    if (socketpair(AF_LOCAL, t, 0, fd) == -1) return tb_false;

    // non-block
    fcntl(fd[0], F_SETFL, fcntl(fd[0], F_GETFL) | O_NONBLOCK);
    fcntl(fd[1], F_SETFL, fcntl(fd[1], F_GETFL) | O_NONBLOCK);

    // save pair
    pair[0] = tb_fd2sock(fd[0]);
    pair[1] = tb_fd2sock(fd[1]);

    // trace
    tb_trace_d("pair: %p %p", pair[0], pair[1]);

    // ok
    return tb_true;
}
tb_void_t tb_socket_block(tb_socket_ref_t sock, tb_bool_t block)
{
    // check
    tb_assert_and_check_return(sock);

    // block it?
    if (block) fcntl(tb_sock2fd(sock), F_SETFL, fcntl(tb_sock2fd(sock), F_GETFL) & ~O_NONBLOCK);
    else fcntl(tb_sock2fd(sock), F_SETFL, fcntl(tb_sock2fd(sock), F_GETFL) | O_NONBLOCK);
}
tb_bool_t tb_socket_ctrl(tb_socket_ref_t sock, tb_size_t ctrl, ...)
{
    // check
    tb_assert_and_check_return_val(sock, tb_false);

    // init args
    tb_va_list_t args;
    tb_va_start(args, ctrl);

    // done
    tb_int_t    fd = tb_sock2fd(sock);
    tb_bool_t   ok = tb_false;
    switch (ctrl)
    {
    case TB_SOCKET_CTRL_SET_BLOCK:
        {
            // set block
            tb_bool_t is_block = (tb_bool_t)tb_va_arg(args, tb_bool_t);

            // block it?
            if (is_block) fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) & ~O_NONBLOCK);
            else fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);

            // ok 
            ok = tb_true;
        }
        break;
    case TB_SOCKET_CTRL_GET_BLOCK:
        {
            // the pis_block
            tb_bool_t* pis_block = (tb_bool_t*)tb_va_arg(args, tb_bool_t*);
            tb_assert_and_check_return_val(pis_block, tb_false);

            // is block?
            *pis_block = (fcntl(fd, F_GETFL) & O_NONBLOCK)? tb_false : tb_true;

            // ok
            ok = tb_true;
        }
        break;
    case TB_SOCKET_CTRL_SET_RECV_BUFF_SIZE:
        {
            // the buff_size
            tb_size_t buff_size = (tb_size_t)tb_va_arg(args, tb_size_t);

            // set the recv buffer size
            tb_int_t real = (tb_int_t)buff_size;
            if (!setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (tb_char_t*)&real, sizeof(real)))
            {
                // ok
                ok = tb_true;
            }
        }
        break;
    case TB_SOCKET_CTRL_GET_RECV_BUFF_SIZE:
        {
            // the pbuff_size
            tb_size_t* pbuff_size = (tb_size_t*)tb_va_arg(args, tb_size_t*);
            tb_assert_and_check_return_val(pbuff_size, tb_false);

            // get the recv buffer size
            tb_int_t    real = 0;
            socklen_t   size = sizeof(real);
            if (!getsockopt(fd, SOL_SOCKET, SO_RCVBUF, (tb_char_t*)&real, &size))
            {
                // save it
                *pbuff_size = real;
            
                // ok
                ok = tb_true;
            }
            else *pbuff_size = 0;
        }
        break;
    case TB_SOCKET_CTRL_SET_SEND_BUFF_SIZE:
        {
            // the buff_size
            tb_size_t buff_size = (tb_size_t)tb_va_arg(args, tb_size_t);

            // set the send buffer size
            tb_int_t real = (tb_int_t)buff_size;
            if (!setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (tb_char_t*)&real, sizeof(real)))
            {
                // ok
                ok = tb_true;
            }
        }
        break;
    case TB_SOCKET_CTRL_GET_SEND_BUFF_SIZE:
        {
            // the pbuff_size
            tb_size_t* pbuff_size = (tb_size_t*)tb_va_arg(args, tb_size_t*);
            tb_assert_and_check_return_val(pbuff_size, tb_false);

            // get the send buffer size
            tb_int_t    real = 0;
            socklen_t   size = sizeof(real);
            if (!getsockopt(fd, SOL_SOCKET, SO_SNDBUF, (tb_char_t*)&real, &size))
            {
                // save it
                *pbuff_size = real;
            
                // ok
                ok = tb_true;
            }
            else *pbuff_size = 0;
        }
        break;
    default:
        {
            // trace
            tb_trace_e("unknown socket ctrl: %lu", ctrl);
        }
        break;
    }
    
    // exit args
    tb_va_end(args);

    // ok?
    return ok;
}
tb_long_t tb_socket_connect(tb_socket_ref_t sock, tb_addr_ref_t addr)
{
    // check
    tb_assert_and_check_return_val(sock && addr, -1);

    // load addr
    tb_size_t               n = 0;
	struct sockaddr_storage d = {0};
    if (!(n = tb_socket_addr_load(&d, addr))) return -1;

    // connect
    tb_long_t r = connect(tb_sock2fd(sock), (struct sockaddr *)&d, n);

    // ok?
    if (!r || errno == EISCONN) return 1;

    // continue?
    if (errno == EINTR || errno == EINPROGRESS || errno == EAGAIN) return 0;

    // error
    return -1;
}
tb_size_t tb_socket_bind(tb_socket_ref_t sock, tb_ipv4_ref_t addr, tb_uint16_t port)
{
    // check
    tb_assert_and_check_return_val(sock, 0);

    // init
    struct sockaddr_in d = {0};
    d.sin_family = AF_INET;
    d.sin_port = tb_bits_ne_to_be_u16(port);
    d.sin_addr.s_addr = (addr && addr->u32)? addr->u32 : INADDR_ANY;

    // reuse addr
#ifdef SO_REUSEADDR
    //if (addr && addr->u32)
    {
        tb_int_t reuseaddr = 1;
        if (setsockopt(tb_sock2fd(sock), SOL_SOCKET, SO_REUSEADDR, (tb_int_t *)&reuseaddr, sizeof(reuseaddr)) < 0) 
            tb_trace_e("reuseaddr: failed");
    }
#endif

    // reuse port
#ifdef SO_REUSEPORT
    if (port)
    {
        tb_int_t reuseport = 1;
        if (setsockopt(tb_sock2fd(sock), SOL_SOCKET, SO_REUSEPORT, (tb_int_t *)&reuseport, sizeof(reuseport)) < 0) 
            tb_trace_e("reuseport: %lu failed", port);
    }
#endif

    // bind 
    if (bind(tb_sock2fd(sock), (struct sockaddr *)&d, sizeof(d)) < 0) return 0;
    
    // bind one random port? get the bound port
    if (!port)
    {
        tb_int_t n = sizeof(d);
        if (getsockname(tb_sock2fd(sock), (struct sockaddr *)&d, (socklen_t *)&n) == -1) return 0;
        port = tb_bits_be_to_ne_u16(d.sin_port);
    }

    // ok?
    return port;
}
tb_bool_t tb_socket_listen(tb_socket_ref_t sock, tb_size_t backlog)
{
    // check
    tb_assert_and_check_return_val(sock, tb_false);

    // listen
    return (listen(tb_sock2fd(sock), backlog) < 0)? tb_false : tb_true;
}
tb_socket_ref_t tb_socket_accept(tb_socket_ref_t sock, tb_ipv4_ref_t addr, tb_uint16_t* port)
{
    // check
    tb_assert_and_check_return_val(sock, tb_null);

    // done  
    struct sockaddr_in  d = {0};
    socklen_t           n = sizeof(struct sockaddr_in);
    tb_long_t           fd = accept(tb_sock2fd(sock), (struct sockaddr *)&d, &n);

    // no client?
    tb_check_return_val(fd > 0, tb_null);

    // non-block
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);

    // save address
    if (addr) tb_ipv4_cstr_set(addr, inet_ntoa(d.sin_addr));

    // save port
    if (port) *port = tb_bits_be_to_ne_u16(d.sin_port);
        
    // ok
    return tb_fd2sock(fd);
}
tb_bool_t tb_socket_local(tb_socket_ref_t sock, tb_ipv4_ref_t addr, tb_uint16_t* port)
{
    // check
    tb_assert_and_check_return_val(sock, tb_false);

    // get local address
    struct sockaddr_in  d = {0};
    tb_int_t            n = sizeof(d);
    if (getsockname(tb_sock2fd(sock), (struct sockaddr *)&d, (socklen_t *)&n) == -1) return tb_false;

    // save address
    if (addr) tb_ipv4_cstr_set(addr, inet_ntoa(d.sin_addr));

    // save port
    if (port) *port = tb_bits_be_to_ne_u16(d.sin_port);

    // ok
    return tb_true;
}
tb_bool_t tb_socket_kill(tb_socket_ref_t sock, tb_size_t mode)
{
    // check
    tb_assert_and_check_return_val(sock, tb_false);

    // init how
    tb_int_t how = SHUT_RDWR;
    switch (mode)
    {
    case TB_SOCKET_KILL_RO:
        how = SHUT_RD;
        break;
    case TB_SOCKET_KILL_WO:
        how = SHUT_WR;
        break;
    case TB_SOCKET_KILL_RW:
        how = SHUT_RDWR;
        break;
    default:
        break;
    }

    // kill it
    tb_bool_t ok = !shutdown(tb_sock2fd(sock), how)? tb_true : tb_false;
 
    // failed?
    if (!ok)
    {
        // trace
        tb_trace_e("kill: %p failed, errno: %d", sock, errno);
    }

    // ok?
    return ok;
}
tb_bool_t tb_socket_exit(tb_socket_ref_t sock)
{
    // check
    tb_assert_and_check_return_val(sock, tb_false);

    // trace
    tb_trace_d("clos: %p", sock);

    // close it
    tb_bool_t ok = !close(tb_sock2fd(sock))? tb_true : tb_false;
    
    // failed?
    if (!ok)
    {
        // trace
        tb_trace_e("clos: %p failed, errno: %d", sock, errno);
    }

    // ok?
    return ok;
}
tb_long_t tb_socket_recv(tb_socket_ref_t sock, tb_byte_t* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(sock && data, -1);
    tb_check_return_val(size, 0);

    // recv
    tb_long_t real = recv(tb_sock2fd(sock), data, (tb_int_t)size, 0);

    // ok?
    if (real >= 0) return real;

    // continue?
    if (errno == EINTR || errno == EAGAIN) return 0;

    // error
    return -1;
}
tb_long_t tb_socket_send(tb_socket_ref_t sock, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(sock && data, -1);
    tb_check_return_val(size, 0);

    // send
    tb_long_t real = send(tb_sock2fd(sock), data, (tb_int_t)size, 0);

    // ok?
    if (real >= 0) return real;

    // continue?
    if (errno == EINTR || errno == EAGAIN) return 0;

    // error
    return -1;
}
tb_long_t tb_socket_recvv(tb_socket_ref_t sock, tb_iovec_t const* list, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(sock && list && size, -1);

    // check iovec
    tb_assert_static(sizeof(tb_iovec_t) == sizeof(struct iovec));
    tb_assert_return_val(tb_memberof_eq(tb_iovec_t, data, struct iovec, iov_base), -1);
    tb_assert_return_val(tb_memberof_eq(tb_iovec_t, size, struct iovec, iov_len), -1);

    // read it
    tb_long_t real = readv(tb_sock2fd(sock), (struct iovec const*)list, size);

    // ok?
    if (real >= 0) return real;

    // continue?
    if (errno == EINTR || errno == EAGAIN) return 0;

    // error
    return -1;
}
tb_long_t tb_socket_sendv(tb_socket_ref_t sock, tb_iovec_t const* list, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(sock && list && size, -1);

    // check iovec
    tb_assert_static(sizeof(tb_iovec_t) == sizeof(struct iovec));
    tb_assert_return_val(tb_memberof_eq(tb_iovec_t, data, struct iovec, iov_base), -1);
    tb_assert_return_val(tb_memberof_eq(tb_iovec_t, size, struct iovec, iov_len), -1);

    // writ it
    tb_long_t real = writev(tb_sock2fd(sock), (struct iovec const*)list, size);

    // ok?
    if (real >= 0) return real;

    // continue?
    if (errno == EINTR || errno == EAGAIN) return 0;

    // error
    return -1;
}
tb_hong_t tb_socket_sendf(tb_socket_ref_t sock, tb_file_ref_t file, tb_hize_t offset, tb_hize_t size)
{
    // check
    tb_assert_and_check_return_val(sock && file && size, -1);

#if defined(TB_CONFIG_OS_LINUX) || defined(TB_CONFIG_OS_ANDROID)

    // send it
    off_t       seek = offset;
    tb_hong_t   real = sendfile(tb_sock2fd(sock), tb_sock2fd(file), &seek, (size_t)size);

    // ok?
    if (real >= 0) return real;

    // continue?
    if (errno == EINTR || errno == EAGAIN) return 0;

    // error
    return -1;

#elif defined(TB_CONFIG_OS_MAC) || defined(TB_CONFIG_OS_IOS)

    // send it
    off_t real = (off_t)size;
    if (!sendfile(tb_sock2fd(file), tb_sock2fd(sock), (off_t)offset, &real, tb_null, 0)) return (tb_hong_t)real;

    // continue?
    if (errno == EINTR || errno == EAGAIN) return (tb_hong_t)real;

    // error
    return -1;
#else

    // read data
    tb_byte_t data[8192];
    tb_long_t read = tb_file_pread(file, data, sizeof(data), offset);
    tb_check_return_val(read > 0, read);

    // send data
    tb_size_t writ = 0;
    while (writ < read)
    {
        tb_long_t real = tb_socket_send(sock, data + writ, read - writ);
        if (real > 0) writ += real;
        else break;
    }

    // ok?
    return writ == read? writ : -1;
#endif
}
tb_long_t tb_socket_urecv(tb_socket_ref_t sock, tb_addr_ref_t addr, tb_byte_t* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(sock && data, -1);
    tb_check_return_val(size, 0);

    // recv
	struct sockaddr_storage d = {0};
    socklen_t               n = sizeof(d);
    tb_long_t               r = recvfrom(tb_sock2fd(sock), data, (tb_int_t)size, 0, (struct sockaddr*)&d, &n);

    // ok?
    if (r >= 0) 
    {
        // save address
        if (addr) tb_socket_addr_save(addr, &d);

        // ok
        return r;
    }

    // continue?
    if (errno == EINTR || errno == EAGAIN) return 0;

    // error
    return -1;
}
tb_long_t tb_socket_usend(tb_socket_ref_t sock, tb_addr_ref_t addr, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(sock && addr && data, -1);

    // no size?
    tb_check_return_val(size, 0);

    // load addr
    tb_size_t               n = 0;
	struct sockaddr_storage d = {0};
    if (!(n = tb_socket_addr_load(&d, addr))) return -1;

    // send
    tb_long_t r = sendto(tb_sock2fd(sock), data, (tb_int_t)size, 0, (struct sockaddr*)&d, n);

    // ok?
    if (r >= 0) return r;

    // continue?
    if (errno == EINTR || errno == EAGAIN) return 0;

    // error
    return -1;
}
tb_long_t tb_socket_urecvv(tb_socket_ref_t sock, tb_ipv4_ref_t addr, tb_uint16_t* port, tb_iovec_t const* list, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(sock && list && size, -1);

    // init msg
    struct msghdr       msg = {0};
    struct sockaddr_in  d = {0};
    msg.msg_name        = (tb_pointer_t)&d;
    msg.msg_namelen     = sizeof(d);
    msg.msg_iov         = (struct iovec*)list;
    msg.msg_iovlen      = (size_t)size;
    msg.msg_control     = tb_null;
    msg.msg_controllen  = 0;
    msg.msg_flags       = 0;

    // recv
    tb_long_t   r = recvmsg(tb_sock2fd(sock), &msg, 0);

    // ok?
    if (r >= 0)
    {
        // save address
        if (addr) tb_ipv4_cstr_set(addr, inet_ntoa(d.sin_addr));

        // save port
        if (port) *port = tb_bits_be_to_ne_u16(d.sin_port);

        // ok
        return r;
    }

    // continue?
    if (errno == EINTR || errno == EAGAIN) return 0;

    // error
    return -1;
}
tb_long_t tb_socket_usendv(tb_socket_ref_t sock, tb_ipv4_ref_t addr, tb_uint16_t port, tb_iovec_t const* list, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(sock && addr && port && list && size, -1);

    // init
    struct sockaddr_in d = {0};
    d.sin_family = AF_INET;
    d.sin_port = tb_bits_ne_to_be_u16(port);
    d.sin_addr.s_addr = addr->u32;

    // init msg
    struct msghdr msg = {0};
    msg.msg_name        = (tb_pointer_t)&d;
    msg.msg_namelen     = sizeof(d);
    msg.msg_iov         = (struct iovec*)list;
    msg.msg_iovlen      = (size_t)size;
    msg.msg_control     = tb_null;
    msg.msg_controllen  = 0;
    msg.msg_flags       = 0;

    // recv
    tb_long_t   r = sendmsg(tb_sock2fd(sock), &msg, 0);

    // ok?
    if (r >= 0) return r;

    // continue?
    if (errno == EINTR || errno == EAGAIN) return 0;

    // error
    return -1;
}
