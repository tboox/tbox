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
	tb_size_t 		port = argv[1]? tb_stou32(argv[1]) : 9090;

	// open sock
	sock = tb_socket_open(TB_SOCKET_TYPE_TCP);
	tb_assert_and_check_goto(sock, end);

	// init aicp
	aicp = tb_aicp_init(TB_AIOO_OTYPE_SOCK, 1);
	tb_assert_and_check_goto(aicp, end);

	// init aico
	aico = tb_aicp_addo(aicp, sock, tb_aicb_work_func, "work");
	tb_assert_and_check_goto(aico, end);

	// post conn
	tb_print("conn: %lu: ..", port);
	if (!tb_aicp_conn(aicp, aico, "127.0.0.1", port)) goto end;

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
