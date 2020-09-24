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

    // the file
    tb_file_ref_t       file;

    // the file size
    tb_hize_t           size;

    // the file offset
    tb_hize_t           offset;

    // wait event
    tb_bool_t           wait;

}tb_demo_client_t, *tb_demo_client_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the file path
static tb_char_t    g_filepath[TB_PATH_MAXN];

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_demo_session_exit(tb_demo_client_ref_t client, tb_poller_ref_t poller)
{
    if (client)
    {
        // trace
        tb_trace_d("[%p]: send %llu", client->sock, client->offset);

        // remove socket from poller
        tb_poller_remove_sock(poller, client->sock);

        // exit file
        if (client->file) tb_file_exit(client->file);
        client->file = tb_null;

        // exit socket
        if (client->sock) tb_socket_exit(client->sock);
        client->sock = tb_null;

        // exit client
        tb_free(client);
    }
}
static tb_long_t tb_demo_session_send(tb_demo_client_ref_t client)
{
    tb_hize_t offset = client->offset;
    tb_hize_t size   = client->size;
    while (offset < size)
    {
        tb_hong_t real = tb_socket_sendf(client->sock, client->file, offset, size - offset);
        if (real > 0)
        {
            offset += real;
            client->wait = tb_false;
        }
        else if (!real && !client->wait)
        {
            client->wait = tb_true;
            break;
        }
        else return -1;
    }
    client->offset = offset;
    return offset < size? 0 : 1;
}
static tb_void_t tb_demo_session_start(tb_poller_ref_t poller, tb_socket_ref_t sock)
{
    // trace
    tb_trace_d("[%p]: sending %s ..", sock, g_filepath);

    // init file
    tb_file_ref_t file = tb_file_init(g_filepath, TB_FILE_MODE_RO);
    tb_assert_and_check_return(file);

    // init client
    tb_demo_client_ref_t client = tb_malloc0_type(tb_demo_client_t);
    if (client)
    {
        client->file   = file;
        client->sock   = sock;
        client->size   = tb_file_size(file);
        client->offset = 0;
        client->wait   = tb_false;

        // send file data to client
        if (!tb_demo_session_send(client))
        {
            tb_size_t events = TB_POLLER_EVENT_SEND;
            if (tb_poller_support(poller, TB_POLLER_EVENT_CLEAR))
                events |= TB_POLLER_EVENT_CLEAR;
            tb_poller_insert_sock(poller, sock, events, client);
        }
        else tb_demo_session_exit(client, poller);
    }
}
static tb_void_t tb_demo_poller_accept(tb_poller_ref_t poller, tb_socket_ref_t sock)
{
    tb_socket_ref_t client = tb_null;
    while ((client = tb_socket_accept(sock, tb_null)))
        tb_demo_session_start(poller, client);
}
static tb_void_t tb_demo_poller_event(tb_poller_ref_t poller, tb_poller_object_ref_t object, tb_long_t events, tb_cpointer_t priv)
{
    switch (events)
    {
    case TB_POLLER_EVENT_ACPT:
        tb_demo_poller_accept(poller, object->ref.sock);
        break;
    case TB_POLLER_EVENT_SEND:
        {
            tb_demo_client_ref_t client = (tb_demo_client_ref_t)priv;
            if (tb_demo_session_send(client))
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
tb_int_t tb_demo_platform_poller_server_main(tb_int_t argc, tb_char_t** argv)
{
    // check
    tb_assert_and_check_return_val(argc == 2 && argv[1], -1);

    // the file path
    tb_char_t const* filepath = argv[1];
    tb_assert_and_check_return_val(filepath, -1);

    // save the file path
    tb_strlcpy(g_filepath, filepath, sizeof(g_filepath));

    // start file server
    tb_socket_ref_t sock = tb_null;
    tb_poller_ref_t poller = tb_null;
    do
    {
        // init poller
        poller = tb_poller_init(tb_null);
        tb_assert_and_check_break(poller);

        // attach poller to the current thread
        tb_poller_attach(poller);

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

        // attempt to accept clients
        tb_demo_poller_accept(poller, sock);

        // start to wait accept events
        tb_size_t events = TB_POLLER_EVENT_ACPT;
        if (tb_poller_support(poller, TB_POLLER_EVENT_CLEAR))
            events |= TB_POLLER_EVENT_CLEAR;
        tb_poller_insert_sock(poller, sock, events, tb_null);

        // wait events
        while (tb_poller_wait(poller, tb_demo_poller_event, -1) >= 0) ;

    } while (0);

    // exit socket
    if (sock) tb_socket_exit(sock);
    sock = tb_null;

    // exit poller
    if (poller) tb_poller_exit(poller);
    poller = tb_null;
    return 0;
}
