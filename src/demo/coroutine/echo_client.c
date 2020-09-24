/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// port
#define TB_DEMO_PORT        (9001)

// timeout
#define TB_DEMO_TIMEOUT     (-1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_demo_coroutine_echo(tb_cpointer_t priv)
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
        tb_trace_i("connecting(%p): %{ipaddr} ..", sock, &addr);

        // connect socket
        tb_long_t ok;
        while (!(ok = tb_socket_connect(sock, &addr)))
        {
            // wait it
            if (tb_socket_wait(sock, TB_SOCKET_EVENT_CONN, TB_DEMO_TIMEOUT) <= 0) break;
        }

        // connect ok?
        tb_check_break(ok > 0);

        // loop
        tb_byte_t data[8192] = {0};
        tb_long_t count = (tb_long_t)priv;
        while (count--)
        {
            // send data
            if (tb_socket_bsend(sock, (tb_byte_t const*)"hello world..", 13))
            {
                // recv data
                if (!tb_socket_brecv(sock, data, 13))
                {
                    // error
                    tb_trace_e("send error!");
                    break;
                }
            }
            else break;
        }

        // trace
        tb_trace_d("send(%p): %s %s", sock, data, count <= 0? "ok" : "failed");

    } while (0);

    // exit socket
    if (sock) tb_socket_exit(sock);
    sock = tb_null;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_coroutine_echo_client_main(tb_int_t argc, tb_char_t** argv)
{
    // check
    tb_assert_and_check_return_val(argc >= 1, -1);

    // the coroutines count
    tb_size_t count = argc > 1? tb_atoi(argv[1]) : 100;

    // the request count
    tb_size_t reqt_count = argc > 2? tb_atoi(argv[2]) : 10000;

    // init scheduler
    tb_co_scheduler_ref_t scheduler = tb_co_scheduler_init();
    if (scheduler)
    {
        // start echo
        tb_size_t i = 0;
        for (i = 0; i < count; i++)
            tb_coroutine_start(scheduler, tb_demo_coroutine_echo, (tb_cpointer_t)reqt_count, 0);

        // start time
        tb_hong_t startime = tb_mclock();

        // run scheduler
        tb_co_scheduler_loop(scheduler, tb_true);

        // compute the spent time
        tb_hong_t spent = tb_mclock() - startime;

        // trace
        tb_trace_i("echo: clients: %lu, request: %lu, spent: %lld ms", count, reqt_count, spent);

        // exit scheduler
        tb_co_scheduler_exit(scheduler);
    }

    // end
    return 0;
}
