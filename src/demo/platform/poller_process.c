/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define COUNT   (50)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the client type
typedef struct __tb_demo_client_t
{
    // the pipe file
    tb_pipe_file_ref_t  pipe[2];

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
        tb_poller_remove_pipe(poller, client->pipe[0]);

        // exit pipe file
        if (client->pipe[0]) tb_pipe_file_exit(client->pipe[0]);
        if (client->pipe[1]) tb_pipe_file_exit(client->pipe[1]);
        client->pipe[0] = tb_null;
        client->pipe[1] = tb_null;

        // exit client
        tb_free(client);
    }
}
static tb_long_t tb_demo_session_read(tb_demo_client_ref_t client)
{
    // closed?
    tb_check_return_val(client && client->pipe[0], -1);

    while (1)
    {
        tb_long_t real = tb_pipe_file_read(client->pipe[0], client->data, sizeof(client->data));
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
    tb_trace_d("[%p]: read %llu", client->pipe[0], client->size);
    return 0;
}
static tb_demo_client_ref_t tb_demo_session_start(tb_poller_ref_t poller, tb_pipe_file_ref_t pipe[2])
{
    // trace
    tb_trace_d("[%p]: recving ..", pipe);

    // init client
    tb_demo_client_ref_t client = tb_malloc0_type(tb_demo_client_t);
    if (client)
    {
        client->pipe[0] = pipe[0];
        client->pipe[1] = pipe[1];
        client->wait    = tb_false;
        if (!tb_demo_session_read(client))
        {
            tb_size_t events = TB_POLLER_EVENT_RECV;
            if (tb_poller_support(poller, TB_POLLER_EVENT_CLEAR))
                events |= TB_POLLER_EVENT_CLEAR;
            tb_poller_insert_pipe(poller, pipe[0], events, client);
            return client;
        }
        else tb_demo_session_exit(client, poller);
    }
    return tb_null;
}
static tb_void_t tb_demo_poller_event(tb_poller_ref_t poller, tb_poller_object_ref_t object, tb_long_t events, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return(poller && object);

    // process?
    tb_demo_client_ref_t client = (tb_demo_client_ref_t)priv;
    if (object->type == TB_POLLER_OBJECT_PROC)
    {
        // update count
        static tb_size_t count = 0;
        count++;

        // trace
        tb_trace_i("process(%p): exited, status: %ld, count: %lu", object->ref.proc, events, count);

        // exit process
        tb_process_exit(object->ref.proc);

        // finished?
        if (count == COUNT)
            tb_poller_kill(poller);
    }
    else
    {
        switch (events)
        {
        case TB_POLLER_EVENT_RECV | TB_POLLER_EVENT_EOF:
        case TB_POLLER_EVENT_RECV:
            tb_demo_session_read(client);
            break;
        default:
            break;
        }
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_platform_poller_process_main(tb_int_t argc, tb_char_t** argv)
{
    // check
    tb_assert_and_check_return_val(argc >= 2, -1);

    // start file client
    tb_poller_ref_t poller = tb_null;
    do
    {
        // init poller
        poller = tb_poller_init(tb_null);
        tb_assert_and_check_break(poller);

        // attach poller to the current thread
        tb_poller_attach(poller);

        // start processes
        tb_size_t count = COUNT;
        tb_demo_client_ref_t clients[COUNT];
        while (count--)
        {
            tb_pipe_file_ref_t pipe[2] = {0};
            if (tb_pipe_file_init_pair(pipe, tb_null, 4096))
            {
                tb_process_attr_t attr = {0};
                attr.out.pipe = pipe[1];
                attr.outtype = TB_PROCESS_REDIRECT_TYPE_PIPE;
                tb_process_ref_t process = tb_process_init(argv[1], (tb_char_t const**)(argv + 1), &attr);
                if (process)
                {
                    clients[count] = tb_demo_session_start(poller, pipe);
                    if (clients[count])
                    {
                        tb_poller_object_t object;
                        object.type = TB_POLLER_OBJECT_PROC;
                        object.ref.proc = process;
                        tb_poller_insert(poller, &object, 0, clients[count]);
                    }
                }
            }
        }

        // wait events
        while (tb_poller_wait(poller, tb_demo_poller_event, -1) >= 0) ;

        // exit clients
        count = COUNT;
        while (count--)
        {
            if (clients[count]) tb_demo_session_exit(clients[count], poller);
            clients[count] = 0;
        }

        // end
        tb_trace_i("finished");

    } while (0);

    // exit poller
    if (poller) tb_poller_exit(poller);
    poller = tb_null;
    return 0;
}
