/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../demo.h"

static tb_void_t tb_demo_coroutine_client(tb_cpointer_t priv)
{
    // check
    tb_socket_ref_t sock = (tb_socket_ref_t)priv;
    tb_assert_and_check_return(sock);

    // loop
    tb_byte_t data[8192] = {0};
    tb_size_t size = sizeof("hello world\r\n") - 1;
    while (1)
    {
        // recv data
        if (tb_socket_brecv(sock, data, size))
        {
            // send data
            if (!tb_socket_bsend(sock, data, size))
            {
                // error
                tb_trace_e("send error!");
                break;
            }
        }
        else break;
    }

    // exit socket
    tb_socket_exit(sock);
}

static tb_void_t tb_demo_coroutine_listen(tb_cpointer_t priv)
{
    // done
    tb_socket_ref_t sock = tb_null;
    do
    {
        // init socket
        sock = tb_socket_init(TB_SOCKET_TYPE_TCP, TB_IPADDR_FAMILY_IPV4);
        tb_assert_and_check_break(sock);

        // bind socket
        if (!tb_socket_bind(sock, (tb_ipaddr_ref_t)priv)) break;

        // listen socket
        if (!tb_socket_listen(sock, 128)) break;

        // trace
        tb_trace_i("listening %{ipaddr}", priv);

        // wait accept events
        tb_socket_ref_t client = tb_null;
        while (1)
        {
            // accept and start client connection
            if ((client = tb_socket_accept(sock, tb_null)))
            {
                if (!tb_coroutine_start(tb_null, tb_demo_coroutine_client, client, 0)) break;
            }
            else if (tb_socket_wait(sock, TB_SOCKET_EVENT_ACPT, -1) <= 0) break;
        }

    } while (0);

    // exit socket
    if (sock) tb_socket_exit(sock);
    sock = tb_null;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_other_test_main(tb_int_t argc, tb_char_t** argv)
{
    // init listen address
    tb_ipaddr_t addr;
    tb_ipaddr_set(&addr, "127.0.0.1", 9001, TB_IPADDR_FAMILY_IPV4);

    // init scheduler
	tb_co_scheduler_ref_t scheduler = tb_co_scheduler_init();
    if (scheduler)
    {
        // start listener
	    tb_coroutine_start(scheduler, tb_demo_coroutine_listen, &addr, 0);

        // run scheduler
	    tb_co_scheduler_loop(scheduler, tb_true);

        // exit scheduler
	    tb_co_scheduler_exit(scheduler);
    }
    return 0;
}
