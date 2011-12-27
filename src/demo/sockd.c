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

	// init epool
	tb_handle_t ep = tb_epool_init(16);
	tb_assert_and_check_goto(ep, end);

	// bind 
	tb_print("bind port: %u", tb_stou32(argv[1]));
	if (!tb_socket_bind(s, tb_stou32(argv[1]))) goto end;

	// add event
	if (!tb_epool_addo(ep, s, TB_EOTYPE_SOCK, TB_ETYPE_ACPT)) goto end;

	// accept
	while (1)
	{
		// waiting...
		tb_print("listening...");
		tb_eobject_t* objs = TB_NULL;
		tb_long_t objn = tb_epool_wait(ep, &objs, 10000);

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
			if (objs[i].etype & TB_ETYPE_ACPT)
			{
				tb_handle_t c = tb_socket_accept(s);
				if (c)
				{
					tb_print("accept ok");
					if (!tb_epool_addo(ep, c, TB_EOTYPE_SOCK, TB_ETYPE_READ)) goto end;
				}
				else
				{
					tb_print("accept failed");
				}
			}
			else if (objs[i].etype & TB_ETYPE_READ)
			{
				tb_assert_and_check_break(objs[i].handle);
			}
		}
	}

end:

	// close socket
	if (s) tb_socket_close(s);

	// exit epool
	if (ep) tb_epool_exit(ep);

	// exit
	tb_exit();
	return 0;
}
