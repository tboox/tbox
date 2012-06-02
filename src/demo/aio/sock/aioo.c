/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * connect
 */
static tb_bool_t tb_test_sock_connect(tb_handle_t s, tb_aioo_t* o, tb_char_t const* ip, tb_size_t port)
{
	// add event
	if (!tb_aioo_adde(o, TB_AIOO_ETYPE_CONN)) return TB_FALSE;

	// connect...
	while (1)
	{
		// connecting
		tb_print("connecting %s:%u", ip, port);
		tb_long_t r = tb_socket_connect(s, ip, port);

		// ok
		if (r > 0) break;
		// continue ?
		else if (!r)
		{
			// waiting...
			tb_print("connect waiting...");
			tb_long_t etype = tb_aioo_wait(o, 10000);

			// error?
			if (etype < 0)
			{
				tb_print("connect failed");
				return TB_FALSE;
			}

			// timeout?
			if (!etype)
			{
				tb_print("connect timeout");
				return TB_FALSE;
			}

			// has connect?
			tb_assert_and_check_break(etype & TB_AIOO_ETYPE_CONN);
		}
		// error
		else 
		{
			tb_print("connect error.");
			return TB_FALSE;
		}
	}

	// del event
	tb_aioo_dele(o, TB_AIOO_ETYPE_CONN);

	// ok
	tb_print("connect ok.");
	return TB_TRUE;
}

/* ///////////////////////////////////////////////////////////////////////
 * send
 */
static tb_bool_t tb_test_sock_send(tb_handle_t s, tb_aioo_t* o, tb_byte_t* data, tb_size_t size)
{
	// add event
	if (!tb_aioo_adde(o, TB_AIOO_ETYPE_WRIT)) return TB_FALSE;

	// send
	tb_size_t send = 0;
	tb_bool_t wait = TB_FALSE;
	while (send < size)
	{
		// try to send data
		tb_long_t n = tb_socket_send(s, data + send, size - send);
		if (n > 0)
		{
			// update send
			send += n;

			// no waiting
			wait = TB_FALSE;
		}
		else if (!n && !wait)
		{
			// waiting...
			tb_print("send waiting...");
			tb_long_t etype = tb_aioo_wait(o, 10000);

			// error?
			if (etype < 0)
			{
				tb_print("send failed");
				break ; 
			}

			// timeout?
			if (!etype)
			{
				tb_print("send timeout");
				break ; 
			}

			// has send?
			tb_assert_and_check_break(etype & TB_AIOO_ETYPE_WRIT);

			// be waiting
			wait = TB_TRUE;
		}
		else break;
	}
	
	// del event
	tb_aioo_dele(o, TB_AIOO_ETYPE_WRIT);

	// ok?
	return send == size? TB_TRUE : TB_FALSE;
}

/* ///////////////////////////////////////////////////////////////////////
 * recv
 */
static tb_size_t tb_test_sock_recv(tb_handle_t s, tb_aioo_t* o, tb_byte_t* data, tb_size_t size)
{
	// add event
	if (!tb_aioo_adde(o, TB_AIOO_ETYPE_READ)) return 0;

	// recv
	tb_size_t recv = 0;
	tb_bool_t wait = TB_FALSE;
	while (recv < size)
	{
		// try to recv data
		tb_long_t n = tb_socket_recv(s, data + recv, size - recv);
		if (n > 0)
		{
			// update recv
			recv += n;

			// no waiting
			wait = TB_FALSE;
		}
		else if (!n && !wait)
		{
			// waiting...
			tb_print("recv waiting...");
			tb_long_t etype = tb_aioo_wait(o, 1000);

			// error?
			if (etype < 0)
			{
				tb_print("recv failed");
				break ; 
			}

			// timeout?
			if (!etype)
			{
				tb_print("recv timeout");
				break ; 
			}

			// has recv?
			tb_assert_and_check_break(etype & TB_AIOO_ETYPE_READ);

			// be waiting
			wait = TB_TRUE;
		}
		else break;
	}
	
	// del event
	tb_aioo_dele(o, TB_AIOO_ETYPE_READ);

	// ok?
	return recv;
}
/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// open socket
	tb_handle_t s = tb_socket_open(TB_SOCKET_TYPE_TCP);
	tb_assert_and_check_goto(s, end);

	// init aioo
	tb_aioo_t o;
	tb_aioo_seto(&o, s, TB_AIOO_OTYPE_SOCK, TB_AIOO_ETYPE_NULL, TB_NULL);

	// connect
	if (!tb_test_sock_connect(s, &o, argv[1], tb_stou32(argv[2]))) goto end;

	// send
	if (!tb_test_sock_send(s, &o, "hello world", 12)) goto end;
	tb_print("send: %s", "hello world");

	// recv
	tb_char_t data[4096] = {0};
	tb_size_t size = tb_test_sock_recv(s, &o, data, 4096);
	tb_print("recv[%u]: %s", size, data);

end:

	// close socket
	if (s) tb_socket_close(s);

	// exit
	tb_exit();
	return 0;
}
