/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_WORK_MAXN 		(3)

/* ///////////////////////////////////////////////////////////////////////
 * aicb
 */
static tb_bool_t tb_aicb_work_func(tb_aicp_t* aicp, tb_aico_t const* aico, tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aicp && aico && aice, tb_false);

	// done
	switch (aice->code)
	{
	case TB_AICE_CODE_ACPT:
		{
			// trace
			tb_print("acpt[%p]: ok", aice->u.acpt.sock);

			// addo client
			tb_aico_t const* client = tb_aicp_addo(aicp, aice->u.acpt.sock, tb_aicb_work_func, "client");
			tb_assert_and_check_return_val(client, tb_false);

			// post read for client
			if (!tb_aicp_read(aicp, client, tb_malloc(1024), 1024)) return tb_false;

			// post acpt for server
			if (!tb_aicp_acpt(aicp, aico)) return tb_false;
		}
		break;
	case TB_AICE_CODE_READ:
		{
			// trace
			tb_print("read[%p]: size: %ld, maxn: %lu, data: %s", aico->aioo.handle, aice->u.read.size, aice->u.read.maxn, aice->u.read.data);

			// exit data
			if (aice->u.read.data) tb_free(aice->u.read.data);

			// post writ
			if (!tb_aicp_writ(aicp, aico, "ok", 3)) return tb_false;
		}
		break;
	case TB_AICE_CODE_WRIT:
		{
			// trace
			tb_print("writ[%p]: size: %ld maxn: %lu, data: %s", aico->aioo.handle, aice->u.read.size, aice->u.writ.maxn, aice->u.writ.data);
		}
		break;
	default:
		break;
	}

	// FIXME: done close

	// ok
	return tb_true;
}

/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// init tbox
	if (!tb_init(malloc(10 * 1024 * 1024), 10 * 1024 * 1024)) return 0;

	// init
	tb_handle_t 	sock = tb_null;
	tb_handle_t 	aicp = tb_null;
	tb_size_t 		port = argv[1]? tb_stou32(argv[1]) : 9090;

	// open sock
	sock = tb_socket_open(TB_SOCKET_TYPE_TCP);
	tb_assert_and_check_goto(sock, end);

	// bind port
	if (!tb_socket_bind(sock, tb_null, port)) goto end;

	// listen sock
	if (!tb_socket_listen(sock)) goto end;
	tb_print("listen: %lu", port);

	// init aicp
	aicp = tb_aicp_init(TB_AIOO_OTYPE_SOCK, 16);
	tb_assert_and_check_goto(aicp, end);

	// addo sock
	tb_aico_t const* aico = tb_aicp_addo(aicp, sock, tb_aicb_work_func, "server");
	tb_assert_and_check_goto(aico, end);

	// post acpt
	tb_print("acpt[%p]: ..", sock);
	if (!tb_aicp_acpt(aicp, aico)) goto end;

	// spak aicp
	while (tb_aicp_spak(aicp)) ;

end:

	// trace
	tb_print("end");

	// close sock
	if (sock) tb_socket_close(sock);

	// exit aicp
	if (aicp) tb_aicp_exit(aicp);

	// exit tbox
	tb_exit();
	return 0;
}
