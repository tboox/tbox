/* ///////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"

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
	if (!tb_eobject_init(&o, TB_EOTYPE_SOCK, TB_ETYPE_CONN, s)) goto end;

	// connect...
	while (1)
	{
		// connecting
		tb_print("connecting %s:%u", argv[1], tb_stou32(argv[2]));
		tb_long_t r = tb_socket_connect(s, argv[1], tb_stou32(argv[2]));

		// ok
		if (r > 0) break;
		// continue ?
		else if (!r)
		{
			// waiting...
			tb_print("waiting...");
			tb_long_t etype = tb_eobject_wait(&o, 10000);

			// error?
			if (etype < 0)
			{
				tb_print("connect failed");
				goto end;
			}

			// timeout?
			if (!etype)
			{
				tb_print("connect timeout");
				goto end;
			}

			// has connect?
			tb_assert_and_check_break(etype & TB_ETYPE_CONN);
		}
		// error
		else 
		{
			tb_print("connect error.");
			goto end;
		}
	}
	tb_print("connect ok.");


end:

	// exit eobject
	if (s) tb_eobject_exit(&o);

	// close socket
	if (s) tb_socket_close(s);

	// exit
	tb_exit();
	return 0;
}
