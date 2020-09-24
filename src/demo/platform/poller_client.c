/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// port
#define TB_DEMO_PORT        (9090)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the client type
typedef struct __tb_demo_client_t
{
    // the socket
    tb_socket_ref_t     sock;

    // the recv size
    tb_hize_t           size;

    // the recv data
    tb_byte_t           data[8192];

    // wait event
    tb_bool_t           wait;

}tb_demo_client_t, *tb_demo_client_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_demo_session_exit(tb_demo_client_ref_t client, tb_poller_ref_t poller)
{
    if (client)
    {
        // trace
        tb_trace_d("[%p]: recv %llu", client->sock, client->size);

        // remove socket from poller
        tb_poller_remove_sock(poller, client->sock);

        // exit socket
        if (client->sock) tb_socket_exit(client->sock);
        client->sock = tb_null;

        // exit client
        tb_free(client);
    }
}
static tb_long_t tb_demo_session_recv(tb_demo_client_ref_t client)
{
    while (1)
    {
        tb_long_t real = tb_socket_recv(client->sock, client->data, sizeof(client->data));
        if (real > 0)
        {
            client->size += real;
            client->wait = tb_false;
        }
        else if (!real && !client->wait)
        {
            client->wait = tb_true;
            break;
        }
        else return -1;
    }
    return 0;
}
static tb_void_t tb_demo_session_start(tb_poller_ref_t poller, tb_socket_ref_t sock)
{
    // trace
    tb_trace_d("[%p]: recving ..", sock);

    // init client
    tb_demo_client_ref_t client = tb_malloc0_type(tb_demo_client_t);
    if (client)
    {
        client->sock = sock;
        client->wait = tb_false;
        if (!tb_demo_session_recv(client))
        {
            tb_size_t events = TB_POLLER_EVENT_RECV;
            if (tb_poller_support(poller, TB_POLLER_EVENT_CLEAR))
                events |= TB_POLLER_EVENT_CLEAR;
            tb_poller_insert_sock(poller, sock, events, client);
        }
        else tb_demo_session_exit(client, poller);
    }
}
static tb_void_t tb_demo_poller_connect(tb_poller_ref_t poller, tb_ipaddr_ref_t addr)
{
    tb_socket_ref_t sock = tb_socket_init(TB_SOCKET_TYPE_TCP, TB_IPADDR_FAMILY_IPV4);
    if (sock)
    {
        tb_long_t ok = tb_socket_connect(sock, addr);
        if (ok > 0) tb_demo_session_start(poller, sock);
        else if (!ok) tb_poller_insert_sock(poller, sock, TB_POLLER_EVENT_CONN | TB_POLLER_EVENT_ONESHOT, tb_null);
        else tb_socket_exit(sock);
    }
}
static tb_void_t tb_demo_poller_event(tb_poller_ref_t poller, tb_poller_object_ref_t object, tb_long_t events, tb_cpointer_t priv)
{
    switch (events)
    {
    case TB_POLLER_EVENT_CONN:
        tb_demo_session_start(poller, object->ref.sock);
        break;
    case TB_POLLER_EVENT_RECV | TB_POLLER_EVENT_EOF:
    case TB_POLLER_EVENT_RECV:
        {
            tb_demo_client_ref_t client = (tb_demo_client_ref_t)priv;
            if (tb_demo_session_recv(client) || (events & TB_POLLER_EVENT_EOF))
                tb_demo_session_exit(client, poller);
        }
        break;
    default:
        break;
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_platform_poller_client_main(tb_int_t argc, tb_char_t** argv)
{
    // check
    tb_assert_and_check_return_val(argc == 2 && argv[1], -1);

    // the client count
    tb_size_t count = tb_atoi(argv[1]);

    // start file client
    tb_poller_ref_t poller = tb_null;
    do
    {
        // init poller
        poller = tb_poller_init(tb_null);
        tb_assert_and_check_break(poller);

        // attach poller to the current thread
        tb_poller_attach(poller);

        // init address
        tb_ipaddr_t addr;
        tb_ipaddr_set(&addr, "127.0.0.1", TB_DEMO_PORT, TB_IPADDR_FAMILY_IPV4);

        // attempt to connect clients
        while (count--) tb_demo_poller_connect(poller, &addr);

        // wait events
        while (tb_poller_wait(poller, tb_demo_poller_event, -1) >= 0) ;

    } while (0);

    // exit poller
    if (poller) tb_poller_exit(poller);
    poller = tb_null;
    return 0;
}
