/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_demo_sock_dns_exit_func(tb_handle_t dns, tb_cpointer_t priv)
{
    // trace
    tb_trace_i("dns: exit");
}
static tb_void_t tb_demo_sock_dns_done_func(tb_handle_t dns, tb_char_t const* host, tb_ipv4_t const* addr, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return(dns);

    // the aicp
    tb_aicp_ref_t aicp = tb_aicp_dns_aicp(dns);
    tb_assert_and_check_return(aicp);

    // addr ok?
    if (addr)
    {
        // trace
        tb_trace_i("dns[%s]: %u.%u.%u.%u", host, addr->u8[0], addr->u8[1], addr->u8[2], addr->u8[3]);
    }
    // timeout or failed?
    else
    {
        // trace
        tb_trace_i("dns[%s]: failed", host);
    }

    // exit addr
    if (dns) tb_aicp_dns_exit(dns, tb_demo_sock_dns_exit_func, tb_null);

    // kill aicp
    tb_aicp_kill(aicp);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_asio_dns_main(tb_int_t argc, tb_char_t** argv)
{
    // check
    tb_assert_and_check_return_val(argv[1], 0);

    // init
    tb_aicp_ref_t          aicp = tb_null;
    tb_handle_t         dns = tb_null;

    // init aicp
    aicp = tb_aicp_init(2);
    tb_assert_and_check_goto(aicp, end);

    // init dns
    dns = tb_aicp_dns_init(aicp, -1);
    tb_assert_and_check_goto(dns, end);

    // sort server 
    tb_dns_server_sort();

    // init time
    tb_hong_t time = tb_mclock();

    // trace
    tb_trace_i("dns: %s: ..", argv[1]);

    // done dns
    tb_aicp_dns_done(dns, argv[1], tb_demo_sock_dns_done_func, tb_null);

    // loop aicp
    tb_aicp_loop(aicp);

    // exit time
    time = tb_mclock() - time;

    // trace
    tb_trace_i("dns: %s: time: %lld ms", argv[1], time);

end:

    // trace
    tb_trace_i("end");

    // exit aicp
    if (aicp) tb_aicp_exit(aicp);
    return 0;
}
