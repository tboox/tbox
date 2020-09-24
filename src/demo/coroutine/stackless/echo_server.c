/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// port
#define TB_DEMO_PORT        (9090)

// timeout
#define TB_DEMO_TIMEOUT     (-1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the client type
typedef struct __tb_demo_lo_client_t
{
    // the client socket
    tb_socket_ref_t     sock;

    // the data buffer
    tb_char_t           data[64];

    // the data buffer size
    tb_size_t           size;

    // the read size
    tb_size_t           read;

    // the real size
    tb_long_t           real;

    // the wait state
    tb_long_t           wait;

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
        // read data
        client->size = sizeof(client->data) - 1;
        while (client->read < client->size)
        {
            // read it
            client->real = tb_socket_recv(client->sock, (tb_byte_t*)client->data + client->read, client->size - client->read);

            // has data?
            if (client->real > 0)
            {
                client->read += client->real;
                client->wait = 0;
            }
            // no data? wait it
            else if (!client->real && !client->wait)
            {
                // wait it
                tb_lo_coroutine_wait_sock(client->sock, TB_SOCKET_EVENT_RECV, TB_DEMO_TIMEOUT);

                // wait ok
                client->wait = tb_lo_coroutine_wait_result();
                tb_assert_and_check_break(client->wait >= 0);
            }
            // failed or end?
            else break;
        }

        // trace
        tb_trace_i("echo: %s", client->data);

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
tb_int_t tb_demo_lo_coroutine_echo_server_main(tb_int_t argc, tb_char_t** argv)
{
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
