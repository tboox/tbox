/* ///////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"

/* ///////////////////////////////////////////////////////////////////
 * connect
 */
static tb_bool_t tb_test_sock_connect(tb_handle_t s, tb_eobject_t* o, tb_char_t const* ip, tb_size_t port)
{
	// add event
	if (!tb_eobject_adde(o, TB_ETYPE_CONN)) return TB_FALSE;

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
			tb_print("waiting...");
			tb_long_t etype = tb_eobject_wait(o, 10000);

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
			tb_assert_and_check_break(etype & TB_ETYPE_CONN);
		}
		// error
		else 
		{
			tb_print("connect error.");
			return TB_FALSE;
		}
	}

	// del event
	tb_eobject_dele(o, TB_ETYPE_CONN);

	// ok
	tb_print("connect ok.");
	return TB_TRUE;
}

/* ///////////////////////////////////////////////////////////////////
 * send
 */
static tb_bool_t tb_test_sock_send(tb_handle_t s, tb_eobject_t* o, tb_byte_t* data, tb_size_t size)
{
	// add event
	if (!tb_eobject_adde(o, TB_ETYPE_WRIT)) return TB_FALSE;

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
			tb_print("waiting...");
			tb_long_t etype = tb_eobject_wait(o, 10000);

			// error?
			if (etype < 0)
			{
				tb_print("send failed");
				return TB_FALSE;
			}

			// timeout?
			if (!etype)
			{
				tb_print("send timeout");
				return TB_FALSE;
			}

			// has send?
			tb_assert_and_check_break(etype & TB_ETYPE_WRIT);

			// be waiting
			wait = TB_TRUE;
		}
		else break;
	}
	
	// del event
	tb_eobject_dele(o, TB_ETYPE_CONN);

	// ok?
	return send == size? TB_TRUE : TB_FALSE;
}
/* ///////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// open socket
	tb_handle_t s = tb_socket_open(TB_SOCKET_TYPE_TCP);
	tb_assert_and_check_goto(s, end);

	// init eobject
	tb_eobject_t o;
	if (!tb_eobject_init(&o, TB_EOTYPE_SOCK, TB_ETYPE_NULL, s)) goto end;

	// connect
	if (!tb_test_sock_connect(s, &o, argv[1], tb_stou32(argv[2]))) goto end;

	// send
//	if (!tb_test_sock_send(s, &o, "hello world", 12)) goto end;

end:

	// exit eobject
	if (s) tb_eobject_exit(&o);

	// close socket
	if (s) tb_socket_close(s);

	// exit
	tb_exit();
	return 0;
}
