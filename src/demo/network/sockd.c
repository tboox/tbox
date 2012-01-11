/* ///////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"

/* ///////////////////////////////////////////////////////////////////
 * send
 */
static tb_size_t tb_test_sock_send(tb_handle_t s, tb_byte_t* data, tb_size_t size)
{
	// send
	tb_size_t send = 0;
	while (send < size)
	{
		// try to send data
		tb_long_t n = tb_socket_send(s, data + send, size - send);
		if (n > 0) send += n;
		else break;
	}
	
	// ok?
	return send;
}
/* ///////////////////////////////////////////////////////////////////
 * recv
 */
static tb_size_t tb_test_sock_recv(tb_handle_t s, tb_byte_t* data, tb_size_t size)
{
	// recv
	tb_size_t recv = 0;
	while (recv < size)
	{
		// try to recv data
		tb_long_t n = tb_socket_recv(s, data + recv, size - recv);
		if (n > 0) recv += n;
		else break;
	}
	
	// ok?
	return recv;
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

	// init eiop
	tb_handle_t ep = tb_eiop_init(TB_EIO_OTYPE_SOCK, 16);
	tb_assert_and_check_goto(ep, end);

	// bind 
	tb_print("bind port: %u", tb_stou32(argv[1]));
	if (!tb_socket_bind(s, tb_stou32(argv[1]))) goto end;

	// add event
	if (!tb_eiop_addo(ep, s, TB_EIO_ETYPE_ACPT)) goto end;

	// accept
	while (1)
	{
		// waiting...
		tb_print("listening...");
		tb_long_t 		objn = tb_eiop_wait(ep, 10000);
		tb_eio_t* 	objs = tb_eiop_objs(ep);

		// error?
		if (objn < 0) 
		{
			tb_print("listen failed");
			break ;
		}

		// timeout?
		if (!objn) 
		{
			tb_print("listen timeout");
			continue ;
		}

		// ok?
		tb_assert_and_check_break(objs);
		tb_size_t i = 0;
		for (i = 0; i < objn; i++)
		{
			if (objs[i].etype & TB_EIO_ETYPE_ACPT)
			{
				tb_handle_t c = tb_socket_accept(s);
				if (c)
				{
					tb_print("accept ok");
					if (!tb_eiop_addo(ep, c, TB_EIO_ETYPE_READ)) goto end;
				}
				else
				{
					tb_print("connection closed");
					tb_eiop_delo(ep, objs[i].handle);
				}
			}
			else if (objs[i].etype & TB_EIO_ETYPE_READ)
			{
				tb_assert_and_check_break(objs[i].handle);

				tb_char_t data[4096] = {0};
				tb_size_t size = tb_test_sock_recv(objs[i].handle, data, 4096);

				if (size)
				{
					tb_print("recv[%u]: %s", size, data);
					tb_test_sock_send(objs[i].handle, "ok", 3);
				}
				else
				{
					tb_print("connection closed");
					tb_eiop_delo(ep, objs[i].handle);
				}
			}
			else 
			{
				tb_print("unknown event: %u", objs[i].etype);
			}
		}
	}

end:

	// close socket
	if (s) tb_socket_close(s);

	// exit eiop
	if (ep) tb_eiop_exit(ep);

	// exit
	tb_exit();
	return 0;
}
