/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the client type
typedef struct __tb_demo_client_t
{
    // the pipe file
    tb_pipe_file_ref_t  pipe;

    // the read size
    tb_hize_t           size;

    // the read data
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
        // remove pipe from poller
        tb_poller_remove_pipe(poller, client->pipe);

        // exit pipe file
        if (client->pipe) tb_pipe_file_exit(client->pipe);
        client->pipe = tb_null;

        // exit client
        tb_free(client);
    }
}
static tb_long_t tb_demo_session_read(tb_demo_client_ref_t client)
{
    while (1)
    {
        tb_long_t real = tb_pipe_file_read(client->pipe, client->data, sizeof(client->data));
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
    tb_trace_d("[%p]: read %llu", client->pipe, client->size);
    return 0;
}
static tb_int_t tb_demo_session_writ(tb_cpointer_t priv)
{
    tb_pipe_file_ref_t pipe = (tb_pipe_file_ref_t)priv;
    tb_byte_t data[8192 * 16];
    tb_size_t count = 100;
    tb_hize_t total = 0;
    while (count--)
    {
        total += sizeof(data);
        tb_trace_d("[%p]: write %llu", pipe, total);
        if (!tb_pipe_file_bwrit(pipe, data, sizeof(data))) break;
        tb_usleep(50000);
    }
    tb_used(&total);
    return 0;
}
static tb_void_t tb_demo_session_start(tb_poller_ref_t poller, tb_pipe_file_ref_t pipe)
{
    // trace
    tb_trace_d("[%p]: recving ..", pipe);

    // init client
    tb_demo_client_ref_t client = tb_malloc0_type(tb_demo_client_t);
    if (client)
    {
        client->pipe = pipe;
        client->wait = tb_false;
        if (!tb_demo_session_read(client))
        {
            tb_size_t events = TB_POLLER_EVENT_RECV;
            if (tb_poller_support(poller, TB_POLLER_EVENT_CLEAR))
                events |= TB_POLLER_EVENT_CLEAR;
            tb_poller_insert_pipe(poller, pipe, events, client);
        }
        else tb_demo_session_exit(client, poller);
    }
}
static tb_void_t tb_demo_poller_open(tb_poller_ref_t poller)
{
    tb_pipe_file_ref_t pair[2];
    if (tb_pipe_file_init_pair(pair, tb_null, 4096))
    {
        tb_demo_session_start(poller, pair[0]);
        tb_thread_init(tb_null, tb_demo_session_writ, pair[1], 0);
    }
}
static tb_void_t tb_demo_poller_event(tb_poller_ref_t poller, tb_poller_object_ref_t object, tb_long_t events, tb_cpointer_t priv)
{
    switch (events)
    {
    case TB_POLLER_EVENT_RECV | TB_POLLER_EVENT_EOF:
    case TB_POLLER_EVENT_RECV:
        {
            tb_demo_client_ref_t client = (tb_demo_client_ref_t)priv;
            if (tb_demo_session_read(client) || (events & TB_POLLER_EVENT_EOF))
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
tb_int_t tb_demo_platform_poller_pipe_main(tb_int_t argc, tb_char_t** argv)
{
    // start file client
    tb_poller_ref_t poller = tb_null;
    do
    {
        // init poller
        poller = tb_poller_init(tb_null);
        tb_assert_and_check_break(poller);

        // attach poller to the current thread
        tb_poller_attach(poller);

        // attempt to open clients
        tb_demo_poller_open(poller);

        // wait events
        while (tb_poller_wait(poller, tb_demo_poller_event, -1) >= 0) ;

    } while (0);

    // exit poller
    if (poller) tb_poller_exit(poller);
    poller = tb_null;
    return 0;
}
