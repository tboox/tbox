/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_demo_sock_addr_func(tb_handle_t haddr, tb_ipv4_t const* addr, tb_pointer_t data)
{
	// check
	tb_assert_and_check_return_val(haddr, tb_false);

	// addr ok?
	if (addr)
	{
		// trace
		tb_print("addr[%s]: %u.%u.%u.%u", tb_aicp_addr_host(haddr), addr->u8[0], addr->u8[1], addr->u8[2], addr->u8[3]);
	}
	// timeout or failed?
	else
	{
		// trace
		tb_print("addr[%s]: failed");
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
	tb_handle_t 		addr = tb_null;

	// init aicp
	aicp = tb_aicp_init(2);
	tb_assert_and_check_goto(aicp, end);

	// init addr
	addr = tb_aicp_addr_init(aicp, argv[1], tb_demo_sock_addr_func, tb_null);
	tb_assert_and_check_goto(addr, end);

	// sort server 
	tb_dns_server_sort();

	// init time
	tb_hong_t time = tb_mclock();

	// trace
	tb_print("addr: %s: ..", argv[1]);

	// done addr
	tb_aicp_addr_done(addr);

	// loop aicp
	tb_aicp_loop(aicp);

	// exit time
	time = tb_mclock() - time;

	// trace
	tb_print("addr: %s: ok, time: %lld ms", argv[1], time);

end:

	// trace
	tb_print("end");

	// exit addr
	if (addr) tb_aicp_addr_exit(addr);

	// exit aicp
	if (aicp) tb_aicp_exit(aicp);

	// exit tbox
	tb_exit();
	return 0;
}
