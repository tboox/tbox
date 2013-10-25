/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdlib.h>

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
			tb_print("acpt[%p]: %p", aico->aioo.handle, aice->u.acpt.sock);

			// addo client
			tb_aico_t const* client = tb_aicp_addo(aicp, aice->u.acpt.sock, tb_aicb_work_func, tb_malloc(8192));
			tb_assert_and_check_return_val(client, tb_false);

			// post read for client
			if (!tb_aicp_read(aicp, client, client->aioo.odata, 8192)) return tb_false;

			// post acpt for server
//			if (!tb_aicp_acpt(aicp, aico)) return tb_false;
		}
		break;
	case TB_AICE_CODE_READ:
		{
			// trace
			tb_print("read[%p]: size: %ld, maxn: %lu, data: %s", aico->aioo.handle, aice->u.read.real, aice->u.read.size, aice->u.read.real? aice->u.read.data : tb_null);

			// post writ
			if (!tb_aicp_writ(aicp, aico, "hello", sizeof("hello"))) return tb_false;
	
			// patch read for closing
			if (!tb_aicp_read(aicp, aico, aico->aioo.odata, 8192)) return tb_false;
		}
		break;
	case TB_AICE_CODE_WRIT:
		{
			// trace
			tb_print("writ[%p]: size: %ld maxn: %lu, data: %s", aico->aioo.handle, aice->u.writ.real, aice->u.writ.size, aice->u.writ.real? aice->u.writ.data : tb_null);
		}
		break;
	case TB_AICE_CODE_CLOS:
		{
			// trace
			tb_print("clos[%p]: %ld", aico->aioo.handle, aice->u.clos.ok);

			// free data
			if (aico->aioo.odata) tb_free(aico->aioo.odata);
		}
		break;
	case TB_AICE_CODE_ERRO:
		{
			// trace
			tb_print("erro[%p]: %ld", aico->aioo.handle, aice->u.erro.code);
		}
		break;
	default:
		break;
	}

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

	// open sock
	sock = tb_socket_open(TB_SOCKET_TYPE_TCP);
	tb_assert_and_check_goto(sock, end);

	// bind port
	if (!tb_socket_bind(sock, tb_null, 9090)) goto end;

	// listen sock
	if (!tb_socket_listen(sock)) goto end;

	// init aicp
	aicp = tb_aicp_init(TB_AIOO_OTYPE_SOCK, 16);
	tb_assert_and_check_goto(aicp, end);

	// addo sock
	tb_aico_t const* aico = tb_aicp_addo(aicp, sock, tb_aicb_work_func, tb_null);
	tb_assert_and_check_goto(aico, end);

	// post acpt
	tb_print("acpt[%p]: ..", sock);
	if (!tb_aicp_acpt(aicp, aico)) goto end;

	// spak aicp
	while (tb_aicp_spak(aicp, -1) >= 0) ;

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
