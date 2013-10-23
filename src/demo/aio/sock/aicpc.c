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
	case TB_AICE_CODE_CONN:
		{
			// trace
			tb_print("conn[%p]: %ld", aico->aioo.handle, aice->u.conn.ok);

			// ok?
			if (aice->u.conn.ok > 0)
			{
				// post writ to server
				if (!tb_aicp_writ(aicp, aico, "hello", sizeof("hello"))) return tb_false;
			}
		}
		break;
	case TB_AICE_CODE_READ:
		{
			// trace
			tb_print("read[%p]: size: %ld, maxn: %lu, data: %s", aico->aioo.handle, aice->u.read.size, aice->u.read.maxn, aice->u.read.size? aice->u.read.data : tb_null);

			// clos it
			if (!tb_aicp_clos(aicp, aico)) return tb_false;
		}
		break;
	case TB_AICE_CODE_WRIT:
		{
			// trace
			tb_print("writ[%p]: size: %ld maxn: %lu, data: %s", aico->aioo.handle, aice->u.writ.size, aice->u.writ.maxn, aice->u.writ.size? aice->u.writ.data : tb_null);
	
			// post read from server
			if (!tb_aicp_read(aicp, aico, aico->aioo.odata, 8192)) return tb_false;
		}
		break;
	case TB_AICE_CODE_CLOS:
		{
			// trace
			tb_print("clos[%p]: %ld", aico->aioo.handle, aice->u.conn.ok);

			// exit spak
			return tb_false;
		}
		break;
	case TB_AICE_CODE_ERRO:
		{
			// trace
			tb_print("erro[%p]: %ld", aico->aioo.handle, aice->u.erro.code);

			// exit spak
			return tb_false;
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
	tb_handle_t 	aico = tb_null;
	tb_byte_t 		data[8192];

	// open sock
	sock = tb_socket_open(TB_SOCKET_TYPE_TCP);
	tb_assert_and_check_goto(sock, end);

	// init aicp
	aicp = tb_aicp_init(TB_AIOO_OTYPE_SOCK, 1);
	tb_assert_and_check_goto(aicp, end);

	// init aico
	aico = tb_aicp_addo(aicp, sock, tb_aicb_work_func, data);
	tb_assert_and_check_goto(aico, end);

	// post conn
	tb_print("conn: ..");
	if (!tb_aicp_conn(aicp, aico, "127.0.0.1", 9090)) goto end;

	// spak aicp
	while (tb_aicp_spak(aicp, 1000) >= 0) ;
	
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
