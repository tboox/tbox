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
	if (!tb_eobject_init(&o, TB_EOTYPE_SOCK, TB_ETYPE_ACPT, s)) goto end;

	// bind 
	tb_print("bind port: %u", tb_stou32(argv[1]));
	if (!tb_socket_bind(s, tb_stou32(argv[1]))) goto end;

	// accept
	while (1)
	{
		// try accepting
		tb_print("accepting...");
		tb_handle_t c = tb_socket_accept(s);

		// ok?
		if (c)
		{
			tb_print("accept ok.");
		}
		else
		{
			// waiting...
			tb_print("waiting...");
			tb_long_t etype = tb_eobject_wait(&o, 10000);

			// error?
			if (etype < 0) 
			{
				tb_print("accept failed");
				continue ;
			}

			// timeout?
			if (!etype) 
			{
				tb_print("accept timeout");
				continue ;
			}

			// has accept?
			tb_assert_and_check_break(etype & TB_ETYPE_ACPT);
		}
	}

end:

	// exit eobject
	if (s) tb_eobject_exit(&o);

	// close socket
	if (s) tb_socket_close(s);

	// exit
	tb_exit();
	return 0;
}
