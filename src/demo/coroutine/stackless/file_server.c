/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "file_server"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// port
#define TB_DEMO_PORT    (9090)

// timeout
#define TB_DEMO_TIMEOUT  (-1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the client type
typedef struct __tb_demo_lo_client_t
{
    // the client socket
    tb_socket_ref_t     sock;

    // the file
    tb_file_ref_t       file;

    // the data buffer size
    tb_hize_t           size;

    // the send size
    tb_hize_t           send;

    // the real size
    tb_hong_t           real;

    // the wait state
    tb_long_t           wait;

    // the time
    tb_hong_t           time;

}tb_demo_lo_client_t, *tb_demo_lo_client_ref_t;

// the listen type
typedef struct __tb_demo_lo_listen_t
{
    // the listen socket
    tb_socket_ref_t     sock;

    // the address
    tb_ipaddr_t         addr;

    // the client socket
    tb_socket_ref_t     client;

}tb_demo_lo_listen_t, *tb_demo_lo_listen_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the file path
static tb_char_t    g_filepath[TB_PATH_MAXN];

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_demo_lo_coroutine_client(tb_lo_coroutine_ref_t coroutine, tb_cpointer_t priv)
{
    // check
    tb_demo_lo_client_ref_t client = (tb_demo_lo_client_ref_t)priv;
    tb_assert(client);

    // enter coroutine
    tb_lo_coroutine_enter(coroutine)
    {
        // trace
        tb_trace_d("[%p]: sending %s ..", client->sock, g_filepath);

        // init file
        client->file = tb_file_init(g_filepath, TB_FILE_MODE_RO);
        tb_assert(client->file);

        // send data
        client->size = tb_file_size(client->file);
        client->time = tb_mclock();
        while (client->send < client->size)
        {
            // send it
            client->real = tb_socket_sendf(client->sock, client->file, client->send, client->size - client->send);

            // trace
            tb_trace_d("[%p]: send: %ld", client->sock, client->real);

            // has data?
            if (client->real > 0)
            {
                client->send += client->real;
                client->wait = 0;
            }
            // no data? wait it
            else if (!client->real && !client->wait)
            {
                // wait it
                tb_lo_coroutine_wait_sock(client->sock, TB_SOCKET_EVENT_SEND, TB_DEMO_TIMEOUT);

                // wait ok
                client->wait = tb_lo_coroutine_wait_result();
                tb_assert_and_check_break(client->wait >= 0);
            }
            // failed or end?
            else break;
        }

        // trace
        tb_trace_i("[%p]: send: %lld bytes %lld ms", client->sock, client->send, tb_mclock() - client->time);

        // exit file
        tb_file_exit(client->file);

        // exit socket
        tb_socket_exit(client->sock);
    }
}
static tb_void_t tb_demo_lo_coroutine_listen(tb_lo_coroutine_ref_t coroutine, tb_cpointer_t priv)
{
    // check
    tb_demo_lo_listen_ref_t listen = (tb_demo_lo_listen_ref_t)priv;
    tb_assert(listen);

    // enter coroutine
    tb_lo_coroutine_enter(coroutine)
    {
        // done
        do
        {
            // init socket
            listen->sock = tb_socket_init(TB_SOCKET_TYPE_TCP, TB_IPADDR_FAMILY_IPV4);
            tb_assert_and_check_break(listen->sock);

            // bind socket
            tb_ipaddr_set(&listen->addr, tb_null, TB_DEMO_PORT, TB_IPADDR_FAMILY_IPV4);
            if (!tb_socket_bind(listen->sock, &listen->addr)) break;

            // listen socket
            if (!tb_socket_listen(listen->sock, 1000)) break;

            // trace
            tb_trace_i("listening ..");

            // loop
            while (1)
            {
                // accept client sockets
                if ((listen->client = tb_socket_accept(listen->sock, tb_null)))
                {
                    // start client connection
                    if (!tb_lo_coroutine_start(tb_lo_scheduler_self(), tb_demo_lo_coroutine_client, tb_lo_coroutine_pass1(tb_demo_lo_client_t, sock, listen->client))) break;
                }
                else
                {
                    // wait accept events
                    tb_lo_coroutine_wait_sock(listen->sock, TB_SOCKET_EVENT_ACPT, -1);
                    if (tb_lo_coroutine_wait_result() <= 0) break;
                }
            }

        } while (0);

        // exit socket
        if (listen->sock) tb_socket_exit(listen->sock);
        listen->sock = tb_null;
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_lo_coroutine_file_server_main(tb_int_t argc, tb_char_t** argv)
{
    // check
    tb_assert_and_check_return_val(argc == 2 && argv[1], -1);

    // the file path
    tb_char_t const* filepath = argv[1];
    tb_assert_and_check_return_val(filepath, -1);

    // save the file path
    tb_strlcpy(g_filepath, filepath, sizeof(g_filepath));

    // init scheduler
    tb_lo_scheduler_ref_t scheduler = tb_lo_scheduler_init();
    if (scheduler)
    {
        // start listening
        tb_lo_coroutine_start(scheduler, tb_demo_lo_coroutine_listen, tb_lo_coroutine_pass(tb_demo_lo_listen_t));

        // run scheduler
        tb_lo_scheduler_loop(scheduler, tb_true);

        // exit scheduler
        tb_lo_scheduler_exit(scheduler);
    }
    return 0;
}
