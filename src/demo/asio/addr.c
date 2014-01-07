/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_demo_sock_addr_func(tb_handle_t haddr, tb_ipv4_t const* addr, tb_pointer_t data)
{
	// check
	tb_assert_and_check_return(haddr);

	// the aicp
	tb_aicp_t* aicp = tb_aicp_addr_aicp(haddr);
	tb_assert_and_check_return(aicp);

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
		tb_print("addr[%s]: failed", tb_aicp_addr_host(haddr));
	}

	// exit addr
	if (haddr) tb_aicp_addr_exit(haddr);

	// kill aicp
	tb_aicp_kill(aicp);
}

/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_asio_addr_main(tb_int_t argc, tb_char_t** argv)
{
	// check
	tb_assert_and_check_return_val(argv[1], 0);

	// init
	tb_handle_t 		aicp = tb_null;
	tb_handle_t 		addr = tb_null;

	// init aicp
	aicp = tb_aicp_init(2);
	tb_assert_and_check_goto(aicp, end);

	// init addr
	addr = tb_aicp_addr_init(aicp, tb_demo_sock_addr_func, tb_null);
	tb_assert_and_check_goto(addr, end);

	// sort server 
	tb_dns_server_sort();

	// init time
	tb_hong_t time = tb_mclock();

	// trace
	tb_print("addr: %s: ..", argv[1]);

	// done addr
	tb_aicp_addr_done(addr, argv[1]);

	// loop aicp
	tb_aicp_loop(aicp);

	// exit time
	time = tb_mclock() - time;

	// trace
	tb_print("addr: %s: time: %lld ms", argv[1], time);

end:

	// trace
	tb_print("end");

	// exit aicp
	if (aicp) tb_aicp_exit(aicp);
	return 0;
}
