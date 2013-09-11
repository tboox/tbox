/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * send
 */
static tb_size_t tb_test_sock_send(tb_handle_t sock, tb_byte_t* data, tb_size_t size)
{
	// send
	tb_size_t send = 0;
	while (send < size)
	{
		// try to send data
		tb_long_t n = tb_socket_send(sock, data + send, size - send);
		if (n > 0) send += n;
		else break;
	}
	
	// ok?
	return send;
}
/* ///////////////////////////////////////////////////////////////////////
 * recv
 */
static tb_size_t tb_test_sock_recv(tb_handle_t sock, tb_byte_t* data, tb_size_t size)
{
	// recv
	tb_size_t recv = 0;
	while (recv < size)
	{
		// try to recv data
		tb_long_t n = tb_socket_recv(sock, data + recv, size - recv);
		if (n > 0) recv += n;
		else break;
	}
	
	// ok?
	return recv;
}
/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// init tbox
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// open socket
	tb_handle_t sock = tb_socket_open(TB_SOCKET_TYPE_TCP);
	tb_assert_and_check_goto(sock, end);

	// init aipp
	tb_handle_t aipp = tb_aipp_init(TB_AIOO_OTYPE_SOCK, 16);
	tb_assert_and_check_goto(aipp, end);

	// bind 
	tb_print("[demo]: bind port: %u", tb_stou32(argv[1]));
	if (!tb_socket_bind(sock, tb_null, tb_stou32(argv[1]))) goto end;

	// listen sock
	if (!tb_socket_listen(sock)) goto end;

	// add aioo
	if (!tb_aipp_addo(aipp, sock, TB_AIOO_ETYPE_ACPT, tb_null)) goto end;

	// accept
	tb_aioo_t objs[16];
	while (1)
	{
		// waiting...
		tb_print("[demo]: listening...");
		tb_long_t objn = tb_aipp_wait(aipp, objs, 16, 10000);

		// error?
		if (objn < 0) 
		{
			tb_print("[demo]: listen failed");
			break ;
		}

		// timeout?
		if (!objn) 
		{
			tb_print("[demo]: listen timeout");
			continue ;
		}

		tb_size_t i = 0;
		for (i = 0; i < objn; i++)
		{
			tb_assert_and_check_break(objs[i].handle);
			if (objs[i].etype & TB_AIOO_ETYPE_ACPT)
			{
				tb_handle_t c = tb_socket_accept(sock);
				if (c)
				{
					tb_print("[demo]: accept ok");
					if (!tb_aipp_addo(aipp, c, TB_AIOO_ETYPE_READ, tb_null)) goto end;
				}
				else
				{
					tb_print("[demo]: connection closed");
					tb_aipp_delo(aipp, objs[i].handle);
				}
			}
			else if (objs[i].etype & TB_AIOO_ETYPE_READ)
			{
				tb_char_t data[4096] = {0};
				tb_size_t size = tb_test_sock_recv(objs[i].handle, data, 4096);

				if (size)
				{
					tb_print("[demo]: recv[%u]: %s", size, data);
					tb_test_sock_send(objs[i].handle, "ok", 3);
				}
				else
				{
					tb_print("[demo]: connection closed");
					tb_aipp_delo(aipp, objs[i].handle);
				}
			}
			else 
			{
				tb_print("[demo]: unknown aioo: %u", objs[i].etype);
			}
		}
	}

end:

	// close socket
	if (sock) tb_socket_close(sock);

	// exit aipp
	if (aipp) tb_aipp_exit(aipp);

	// exit
	tb_exit();
	return 0;
}
