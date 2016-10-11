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
static tb_void_t tb_demo_coroutine_pull(tb_cpointer_t priv)
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
        tb_ipaddr_set(&addr, "127.0.0.1", 9090, TB_IPADDR_FAMILY_IPV4);

        // trace
        tb_trace_i("connecting %{ipaddr} ..", &addr);

        // connect socket
        tb_long_t ok;
        while (!(ok = tb_socket_connect(sock, &addr))) 
        {
            // wait it
            if (tb_socket_wait(sock, TB_SOCKET_EVENT_CONN, TIMEOUT) <= 0) break;
        }

        // connect ok?
        tb_check_break(ok > 0);

        // trace
        tb_trace_i("recving %lu ..", (tb_size_t)priv);

        // recv data
        tb_byte_t   data[8192];
        tb_hize_t   recv = 0;
        tb_long_t   wait = 0;
        while (1)
        {
            // read it
            tb_long_t real = tb_socket_recv(sock, data, sizeof(data));

            // trace
            tb_trace_i("recv: %ld from: %p", real, sock);

            // has data?
            if (real > 0)
            {
                recv += real;
                wait = 0;
            }
            // no data? wait it
            else if (!real && !wait)
            {
                // wait it
                wait = tb_socket_wait(sock, TB_SOCKET_EVENT_RECV, TIMEOUT);
                tb_assert_and_check_break(wait >= 0);
            }
            // failed or end?
            else break;
        }

        // trace
        tb_trace_i("recv %llu bytes.", recv);

    } while (0);

    // exit socket
    if (sock) tb_socket_exit(sock);
    sock = tb_null;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_coroutine_file_client_main(tb_int_t argc, tb_char_t** argv)
{
    // check
    tb_assert_and_check_return_val(argc == 2 && argv[1], -1);

    // the coroutines count
    tb_size_t count = tb_atoi(argv[1]);

    // init scheduler
    tb_scheduler_ref_t scheduler = tb_scheduler_init();
    if (scheduler)
    {
        // start file
        tb_size_t i = 0; 
        for (i = 0; i < count; i++)
        {
            // start it
            tb_coroutine_start(scheduler, tb_demo_coroutine_pull, (tb_cpointer_t)i, 0);
        }

        // run scheduler
        tb_scheduler_loop(scheduler);

        // exit scheduler
        tb_scheduler_exit(scheduler);
    }

    // end
    return 0;
}
