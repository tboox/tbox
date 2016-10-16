/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */ 

// port
#define TB_DEMO_PORT        (9090)

// timeout
#define TB_DEMO_TIMEOUT     (-1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */ 
static tb_void_t tb_demo_coroutine_echo(tb_cpointer_t priv)
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
        tb_ipaddr_set(&addr, "127.0.0.1", TB_DEMO_PORT, TB_IPADDR_FAMILY_IPV4);

        // trace
        tb_trace_i("connecting %{ipaddr} ..", &addr);

        // connect socket
        tb_long_t ok;
        while (!(ok = tb_socket_connect(sock, &addr))) 
        {
            // wait it
            if (tb_socket_wait(sock, TB_SOCKET_EVENT_CONN, TB_DEMO_TIMEOUT) <= 0) break;
        }

        // connect ok?
        tb_check_break(ok > 0);

        // trace
        tb_trace_i("sending %lu ..", (tb_size_t)priv);

        // make data
        tb_char_t data[64] = {0};
        tb_snprintf(data, sizeof(data), "hello: %lu", (tb_size_t)priv);

        // send data
        tb_size_t   send = 0;
        tb_size_t   size = tb_strlen(data) + 1;
        tb_long_t   wait = 0;
        while (send < size)
        {
            // read it
            tb_long_t real = tb_socket_send(sock, (tb_byte_t*)data + send, size - send);

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

        // ok?
        tb_assert_and_check_break(send == size);

        // trace
        tb_trace_i("send ok!");

    } while (0);

    // exit socket
    if (sock) tb_socket_exit(sock);
    sock = tb_null;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_coroutine_echo_client_main(tb_int_t argc, tb_char_t** argv)
{
    // check
    tb_assert_and_check_return_val(argc == 2 && argv[1], -1);

    // the coroutines count
    tb_size_t count = tb_atoi(argv[1]);

    // init scheduler
    tb_co_scheduler_ref_t scheduler = tb_co_scheduler_init();
    if (scheduler)
    {
        // start echo
        tb_size_t i = 0; 
        for (i = 0; i < count; i++)
        {
            // start it
            tb_coroutine_start(scheduler, tb_demo_coroutine_echo, (tb_cpointer_t)i, 0);
        }

        // run scheduler
        tb_co_scheduler_loop(scheduler);

        // exit scheduler
        tb_co_scheduler_exit(scheduler);
    }

    // end
    return 0;
}
