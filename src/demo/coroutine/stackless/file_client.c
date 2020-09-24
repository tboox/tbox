/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "file_client"
#define TB_TRACE_MODULE_DEBUG           (0)

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

    // the address
    tb_ipaddr_t         addr;

    // the data buffer
    tb_byte_t           data[8192];

    // the recv size
    tb_hize_t           recv;

    // the real size
    tb_long_t           real;

    // the wait state
    tb_long_t           wait;

    // the time
    tb_hong_t           time;

    // connect ok?
    tb_long_t           ok;

}tb_demo_lo_client_t, *tb_demo_lo_client_ref_t;


/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_demo_lo_coroutine_pull(tb_lo_coroutine_ref_t coroutine, tb_cpointer_t priv)
{
    // check
    tb_demo_lo_client_ref_t client = (tb_demo_lo_client_ref_t)priv;
    tb_assert(client);

    // enter coroutine
    tb_lo_coroutine_enter(coroutine)
    {
        // done
        do
        {
            // init socket
            client->sock = tb_socket_init(TB_SOCKET_TYPE_TCP, TB_IPADDR_FAMILY_IPV4);
            tb_assert_and_check_break(client->sock);

            // init address
            tb_ipaddr_set(&client->addr, "127.0.0.1", TB_DEMO_PORT, TB_IPADDR_FAMILY_IPV4);

            // trace
            tb_trace_d("[%p]: connecting %{ipaddr} ..", client->sock, &client->addr);

            // connect socket
            while (!(client->ok = tb_socket_connect(client->sock, &client->addr)))
            {
                // wait it
                tb_lo_coroutine_wait_sock(client->sock, TB_SOCKET_EVENT_CONN, TB_DEMO_TIMEOUT);

                // wait failed?
                if (tb_lo_coroutine_wait_result() <= 0) break;
            }

            // connect ok?
            tb_check_break(client->ok > 0);

            // trace
            tb_trace_d("[%p]: recving ..", client->sock);

            // recv data
            client->time = tb_mclock();
            while (1)
            {
                // read it
                client->real = tb_socket_recv(client->sock, client->data, sizeof(client->data));

                // trace
                tb_trace_d("[%p]: recv: %ld", client->sock, client->real);

                // has data?
                if (client->real > 0)
                {
                    client->recv += client->real;
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
            tb_trace_i("[%p]: recv %llu bytes %lld ms", client->sock, client->recv, tb_mclock() - client->time);

        } while (0);

        // exit socket
        if (client->sock) tb_socket_exit(client->sock);
        client->sock = tb_null;
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_lo_coroutine_file_client_main(tb_int_t argc, tb_char_t** argv)
{
    // check
    tb_assert_and_check_return_val(argc == 2 && argv[1], -1);

    // the coroutines count
    tb_size_t count = tb_atoi(argv[1]);

    // init scheduler
    tb_lo_scheduler_ref_t scheduler = tb_lo_scheduler_init();
    if (scheduler)
    {
        // start file
        tb_size_t i = 0;
        for (i = 0; i < count; i++)
        {
            // start it
            tb_lo_coroutine_start(scheduler, tb_demo_lo_coroutine_pull, tb_lo_coroutine_pass(tb_demo_lo_client_t));
        }

        // run scheduler
        tb_lo_scheduler_loop(scheduler, tb_true);

        // exit scheduler
        tb_lo_scheduler_exit(scheduler);
    }

    // end
    return 0;
}
