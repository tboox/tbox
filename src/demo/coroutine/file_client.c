/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "file_client"
#define TB_TRACE_MODULE_DEBUG           (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// port
#define TB_DEMO_PORT        (9090)

// timeout
#define TB_DEMO_TIMEOUT     (-1)

// eanble conn/recv coroutine
#define TB_DEMO_CONN_RECV   (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#if TB_DEMO_CONN_RECV
static tb_void_t tb_demo_coroutine_recv(tb_cpointer_t priv)
{
    // check
    tb_socket_ref_t sock = (tb_socket_ref_t)priv;
    tb_assert_and_check_return(sock);

    // trace
    tb_trace_d("[%p]: recving ..", sock);

    // recv data
    tb_byte_t data[8192];
    tb_hize_t recv = 0;
    tb_long_t wait = 0;
    tb_hong_t time = tb_mclock();
    while (1)
    {
        // read it
        tb_long_t real = tb_socket_recv(sock, data, sizeof(data));

        // trace
        tb_trace_d("[%p]: recv: %ld, total: %lu", sock, real, recv + (real > 0? real : 0));

        // has data?
        if (real > 0)
        {
            recv += real;
            wait = 0;
        }
        // no data? wait it
        else if (!real && !wait)
        {
            // wait it
            wait = tb_socket_wait(sock, TB_SOCKET_EVENT_RECV, TB_DEMO_TIMEOUT);
            tb_assert_and_check_break(wait >= 0);
        }
        // failed or end?
        else break;
    }

    // trace
    tb_trace_i("[%p]: recv %llu bytes %lld ms", sock, recv, tb_mclock() - time);

    // exit socket
    if (sock) tb_socket_exit(sock);
    sock = tb_null;
}
static tb_void_t tb_demo_coroutine_pull(tb_cpointer_t priv)
{
    tb_long_t ok = -1;
    tb_socket_ref_t sock = tb_null;
    do
    {
        // init socket
        sock = tb_socket_init(TB_SOCKET_TYPE_TCP, TB_IPADDR_FAMILY_IPV4);
        tb_assert_and_check_break(sock);

        // init address
        tb_ipaddr_t addr;
        tb_ipaddr_set(&addr, "127.0.0.1", TB_DEMO_PORT, TB_IPADDR_FAMILY_IPV4);

        // trace
        tb_trace_d("[%p]: connecting %{ipaddr} ..", sock, &addr);

        // connect socket
        while (!(ok = tb_socket_connect(sock, &addr)))
        {
            // wait it
            if (tb_socket_wait(sock, TB_SOCKET_EVENT_CONN, TB_DEMO_TIMEOUT) <= 0) break;
        }

        // connect ok?
        tb_check_break(ok > 0);

        // start recv coroutine
        tb_coroutine_start(tb_co_scheduler_self(), tb_demo_coroutine_recv, sock, 0);

    } while (0);

    // failed?
    if (ok <= 0)
    {
        if (sock) tb_socket_exit(sock);
        sock = tb_null;
    }
}
#else
static tb_void_t tb_demo_coroutine_pull(tb_cpointer_t priv)
{
    // done
    tb_socket_ref_t sock = tb_null;
    do
    {
        // init socket
        sock = tb_socket_init(TB_SOCKET_TYPE_TCP, TB_IPADDR_FAMILY_IPV4);
        tb_assert_and_check_break(sock);

        // init address
        tb_ipaddr_t addr;
        tb_ipaddr_set(&addr, "127.0.0.1", TB_DEMO_PORT, TB_IPADDR_FAMILY_IPV4);

        // trace
        tb_trace_d("[%p]: connecting %{ipaddr} ..", sock, &addr);

        // connect socket
        tb_long_t ok;
        while (!(ok = tb_socket_connect(sock, &addr)))
        {
            // wait it
            if (tb_socket_wait(sock, TB_SOCKET_EVENT_CONN, TB_DEMO_TIMEOUT) <= 0) break;
        }

        // connect ok?
        tb_check_break(ok > 0);

        // trace
        tb_trace_d("[%p]: recving ..", sock);

        // recv data
        tb_byte_t data[8192];
        tb_hize_t recv = 0;
        tb_long_t wait = 0;
        tb_hong_t time = tb_mclock();
        while (1)
        {
            // read it
            tb_long_t real = tb_socket_recv(sock, data, sizeof(data));

            // trace
            tb_trace_d("[%p]: recv: %ld, total: %lu", sock, real, recv + (real > 0? real : 0));

            // has data?
            if (real > 0)
            {
                recv += real;
                wait = 0;
            }
            // no data? wait it
            else if (!real && !wait)
            {
                // wait it
                wait = tb_socket_wait(sock, TB_SOCKET_EVENT_RECV, TB_DEMO_TIMEOUT);
                tb_assert_and_check_break(wait >= 0);
            }
            // failed or end?
            else break;
        }

        // trace
        tb_trace_i("[%p]: recv %llu bytes %lld ms", sock, recv, tb_mclock() - time);

    } while (0);

    // exit socket
    if (sock) tb_socket_exit(sock);
    sock = tb_null;
}
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_coroutine_file_client_main(tb_int_t argc, tb_char_t** argv)
{
    // check
    tb_assert_and_check_return_val(argc == 2 && argv[1], -1);

    // the coroutines count
    tb_size_t count = tb_atoi(argv[1]);

    // init scheduler
    tb_co_scheduler_ref_t scheduler = tb_co_scheduler_init();
    if (scheduler)
    {
        // start file
        tb_size_t i = 0;
        for (i = 0; i < count; i++)
        {
            // start it
            tb_coroutine_start(scheduler, tb_demo_coroutine_pull, tb_null, 0);
        }

        // run scheduler
        tb_co_scheduler_loop(scheduler, tb_true);

        // exit scheduler
        tb_co_scheduler_exit(scheduler);
    }

    // end
    return 0;
}
