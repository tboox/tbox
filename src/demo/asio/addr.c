/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_demo_sock_addr_func(tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_ADDR, tb_false);

	// addr ok?
	if (aice->state == TB_AICE_STATE_OK)
	{
		// trace
		tb_print("addr[%p]: host: %s, addr: %u.%u.%u.%u", aice->aico, aice->u.addr.host, aice->u.addr.addr.u8[0], aice->u.addr.addr.u8[1], aice->u.addr.addr.u8[2], aice->u.addr.addr.u8[3]);
	}
	// timeout or failed?
	else
	{
		// trace
		tb_print("addr[%p]: host: %s, state: %s", aice->aico, aice->u.addr.host, tb_aice_state_cstr(aice));
	}

	// end
	return tb_false;
}

/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// check
	tb_assert_and_check_return_val(argv[1], 0);

	// init tbox
	if (!tb_init(malloc(10 * 1024 * 1024), 10 * 1024 * 1024)) return 0;

	// init
	tb_handle_t 		aicp = tb_null;
	tb_handle_t 		sock = tb_null;
	tb_handle_t 		aico = tb_null;

	// init aicp
	aicp = tb_aicp_init(2);
	tb_assert_and_check_goto(aicp, end);

	// init sock
	sock = tb_socket_open(TB_SOCKET_TYPE_TCP);
	tb_assert_and_check_goto(sock, end);

	// addo sock
	aico = tb_aico_init_sock(aicp, sock);
	tb_assert_and_check_goto(aico, end);

	// init time
	tb_hong_t time = tb_mclock();

	// trace
	tb_print("addr: %s: ..", argv[1]);

	// post addr
	if (!tb_aico_addr(aico, argv[1], tb_demo_sock_addr_func, tb_null)) goto end;

	// loop aicp
	tb_aicp_loop(aicp);

	// exit time
	time = tb_mclock() - time;

	// trace
	tb_print("addr: %s: ok, time: %lld ms", argv[1], time);

end:

	// trace
	tb_print("end");

	// exit aicp
	if (aicp) tb_aicp_exit(aicp);

	// exit sock
	if (sock) tb_socket_close(sock);

	// exit tbox
	tb_exit();
	return 0;
}
