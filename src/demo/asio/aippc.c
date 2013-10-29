/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_DEMO_SOCK_RECV_MAXN 			(65536)

/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// check
	tb_assert_and_check_return_val(argv[1], 0);

	// init tbox
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// init sock
	tb_handle_t sock = tb_socket_open(TB_SOCKET_TYPE_TCP);
	tb_assert_and_check_goto(sock, end);

	// init file
	tb_handle_t file = tb_file_init(argv[1], TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_BINARY);
	tb_assert_and_check_goto(file, end);

	// init data
	tb_byte_t* data = tb_malloc(TB_DEMO_SOCK_RECV_MAXN);
	tb_assert_and_check_goto(data, end);

	// done conn
	tb_long_t conn = -1;
	while (!(conn = tb_socket_connect(sock, "127.0.0.1", 9090)))
	{
		// wait
		tb_aioo_t aioo;
		tb_aioo_seto(&aioo, sock, TB_AIOO_OTYPE_SOCK, TB_AIOO_ETYPE_CONN, tb_null);
		conn = tb_aioo_wait(&aioo, 20000);
		tb_check_break(conn > 0);
	}

	// ok?
	if (conn > 0) tb_print("conn[%p]: ok", sock);
	// timeout?
	else if (!conn) tb_print("conn[%p]: timeout", sock);
	// failed?
	else tb_print("conn[%p]: failed", sock);

	// done sock
	tb_size_t peak = 0;
	tb_size_t sped = 0;
	tb_hong_t time = 0;
	tb_hize_t size = 0;
	tb_bool_t wait = tb_false;
	while (1)
	{
		// recv data
		tb_long_t real = tb_socket_recv(sock, data, TB_DEMO_SOCK_RECV_MAXN);
		if (real > 0)
		{
			// save size
			size += real;

			// trace
//			tb_print("recv[%p]: real: %ld", sock, real);

			// compute speed
			peak += real;
			if (!time) 
			{
				time = tb_mclock();
				sped = peak;
			}
			else if (tb_mclock() > time + 1000)
			{
				sped = peak;
				peak = 0;
				time = tb_mclock();
	
				// trace
				tb_print("recv[%p]: size: %llu, sped: %lu KB/s", sock, size, sped / 1000);
			}

			// init wait
			wait = tb_false;

			// done file
			tb_size_t need = real;
			tb_size_t writ = 0;
			while (writ < need)
			{
				// writ data
				real = tb_file_writ(file, data + writ, need - writ);
				if (real > 0)
				{
					// trace
//					tb_print("writ[%p]: real: %ld", file, real);

					// save writ
					writ += real;
					wait = tb_false;
				}
				else if (!real && !wait)
				{
					// wait
					tb_aioo_t aioo;
					tb_aioo_seto(&aioo, file, TB_AIOO_OTYPE_FILE, TB_AIOO_ETYPE_WRIT, tb_null);
					if (tb_aioo_wait(&aioo, -1) <= 0) 
					{
						tb_print("writ[%p]: wait: failed", file);
						break;
					}
					wait = tb_true;
				}
				else
				{
					tb_print("writ[%p]: failed", file);
				}
			}

			// init wait
			wait = tb_false;
		}
		else if (!real && !wait)
		{
			// wait
			tb_aioo_t aioo;
			tb_aioo_seto(&aioo, sock, TB_AIOO_OTYPE_SOCK, TB_AIOO_ETYPE_READ, tb_null);
			if (tb_aioo_wait(&aioo, -1) <= 0) 
			{
				tb_print("recv[%p]: wait: failed", sock);
				break;
			}
			wait = tb_true;
		}
		else if (!real)
		{
			tb_print("recv[%p]: closed", sock);
			break;
		}
		else 
		{
			tb_print("recv[%p]: failed", sock);
			break;
		}
	}

end:

	// trace
	tb_print("end");

	// exit sock
	if (sock) tb_socket_close(sock);

	// exit file
	if (file) tb_file_exit(file);

	// exit data
	if (data) tb_free(data);

	// exit
	tb_exit();
	return 0;
}
