/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */ 

// timeout
#define TIMEOUT     (-1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */ 
static tb_void_t tb_demo_coroutine_client(tb_cpointer_t priv)
{
    // check
    tb_value_ref_t tuple = (tb_value_ref_t)priv;
    tb_assert_and_check_return(tuple);

    // the socket
    tb_socket_ref_t sock = (tb_socket_ref_t)tuple[1].cptr;
    tb_assert_and_check_return(sock);

    // the filepath
    tb_char_t const* filepath = tuple[0].cstr;
    tb_assert_and_check_return(filepath);

    // init file
    tb_file_ref_t file = tb_file_init(filepath, TB_FILE_MODE_RO | TB_FILE_MODE_BINARY);
    tb_assert_and_check_return(file);

    // send data
    tb_hize_t send = 0;
    tb_hize_t size = tb_file_size(file);
    tb_long_t wait = 0;
    while (send < size)
    {
        // send it
        tb_long_t real = tb_socket_sendf(sock, file, send, size - send);

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
            wait = tb_socket_wait(sock, TB_SOCKET_EVENT_SEND, TIMEOUT);
            tb_assert_and_check_break(wait >= 0);
        }
        // failed or end?
        else break;
    }

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
        tb_ipaddr_set(&addr, tb_null, 9090, TB_IPADDR_FAMILY_IPV4);
        if (!tb_socket_bind(sock, &addr)) break;

        // listen socket
        if (!tb_socket_listen(sock, 20)) break;

        // trace
        tb_trace_i("listening ..");

        // wait accept events
        tb_value_t tuple[2];
        tuple[0].cptr = priv;
        while (tb_socket_wait(sock, TB_SOCKET_EVENT_ACPT, -1) > 0)
        {
            // accept client sockets
            tb_socket_ref_t client = tb_null;
            while ((client = tb_socket_accept(sock, tb_null)))
            {
                // start client connection
                tuple[1].cptr = (tb_cpointer_t)client;
                if (!tb_coroutine_start(tb_null, tb_demo_coroutine_client, tuple, 0)) break;
            }
        }

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

    // init scheduler
    tb_scheduler_ref_t scheduler = tb_scheduler_init();
    if (scheduler)
    {
        // start listening
        tb_coroutine_start(scheduler, tb_demo_coroutine_listen, filepath, 0);

        // run scheduler
        tb_scheduler_loop(scheduler);

        // exit scheduler
        tb_scheduler_exit(scheduler);
    }
    return 0;
}
