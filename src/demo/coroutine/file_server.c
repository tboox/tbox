/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "file_server"
#define TB_TRACE_MODULE_DEBUG           (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// port
#define TB_DEMO_PORT    (9090)

// timeout
#define TB_DEMO_TIMEOUT  (-1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the file path
static tb_char_t    g_filepath[TB_PATH_MAXN];

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_demo_coroutine_client(tb_cpointer_t priv)
{
    // check
    tb_socket_ref_t sock = (tb_socket_ref_t)priv;
    tb_assert_and_check_return(sock);

    // trace
    tb_trace_d("[%p]: sending %s ..", sock, g_filepath);

    // init file
    tb_file_ref_t file = tb_file_init(g_filepath, TB_FILE_MODE_RO);
    tb_assert_and_check_return(file);

    // send data
    tb_hize_t send = 0;
    tb_hize_t size = tb_file_size(file);
    tb_long_t wait = 0;
    tb_hong_t time = tb_mclock();
    while (send < size)
    {
        // send it
        tb_hong_t real = tb_socket_sendf(sock, file, send, size - send);

        // trace
        tb_trace_d("[%p]: send: %lld, total: %llu", sock, real, send + (real > 0? real : 0));

        // has data?
        if (real > 0)
        {
            send += real;
            wait = 0;
        }
        // no data? wait it
        else if (!real && !wait)
        {
            // wait it
            wait = tb_socket_wait(sock, TB_SOCKET_EVENT_SEND, TB_DEMO_TIMEOUT);
            tb_assert_and_check_break(wait >= 0);
        }
        // failed or end?
        else break;
    }

    // trace
    tb_trace_i("[%p]: send: %lld bytes %lld ms", sock, send, tb_mclock() - time);

    // exit file
    tb_file_exit(file);

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
        tb_ipaddr_t addr;
        tb_ipaddr_set(&addr, tb_null, TB_DEMO_PORT, TB_IPADDR_FAMILY_IPV4);
        if (!tb_socket_bind(sock, &addr)) break;

        // listen socket
        if (!tb_socket_listen(sock, 1000)) break;

        // trace
        tb_trace_i("listening ..");

        // accept client sockets
        tb_size_t       count = 0;
        tb_socket_ref_t client = tb_null;
        while (1)
        {
            // accept and start client connection
            if ((client = tb_socket_accept(sock, tb_null)))
            {
                if (!tb_coroutine_start(tb_null, tb_demo_coroutine_client, client, 0)) break;
                count++;
            }
            else if (tb_socket_wait(sock, TB_SOCKET_EVENT_ACPT, -1) <= 0) break;
        }

        // trace
        tb_trace_i("listened %lu", count);

    } while (0);

    // exit socket
    if (sock) tb_socket_exit(sock);
    sock = tb_null;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_coroutine_file_server_main(tb_int_t argc, tb_char_t** argv)
{
    // check
    tb_assert_and_check_return_val(argc == 2 && argv[1], -1);

    // the file path
    tb_char_t const* filepath = argv[1];
    tb_assert_and_check_return_val(filepath, -1);

    // save the file path
    tb_strlcpy(g_filepath, filepath, sizeof(g_filepath));

    // init scheduler
    tb_co_scheduler_ref_t scheduler = tb_co_scheduler_init();
    if (scheduler)
    {
        // start listening
        tb_coroutine_start(scheduler, tb_demo_coroutine_listen, tb_null, 0);

        // run scheduler
        tb_co_scheduler_loop(scheduler, tb_true);

        // exit scheduler
        tb_co_scheduler_exit(scheduler);
    }
    return 0;
}
