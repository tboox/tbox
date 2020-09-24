/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define BUFSIZE     (32767)
#define COUNT       (100000)

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_int_t tb_demo_thread_server(tb_cpointer_t priv)
{
    tb_socket_ref_t sock = (tb_socket_ref_t) priv;
    tb_byte_t data[BUFSIZE];
    tb_size_t count = 0;
    while (1)
    {
        count++;
        if (!tb_socket_brecv(sock, data, sizeof(data))) break;
        tb_usleep(50000);
        if (!tb_socket_bsend(sock, data, sizeof(data))) break;
        tb_trace_i(" ===  %.4llu (%llu bytes)", count, count * BUFSIZE);
    }
    return 0;
}

static tb_void_t tb_demo_coroutine_send(tb_cpointer_t priv)
{
    tb_byte_t data[BUFSIZE];
    tb_socket_ref_t sock = (tb_socket_ref_t) priv;
    for (tb_size_t i = 0; i < COUNT; i++)
    {
        if (!tb_socket_bsend(sock, data, sizeof(data))) break;
        tb_trace_i("[-->] %.4llu", i);
    }
}

static tb_void_t tb_demo_coroutine_recv(tb_cpointer_t priv)
{
    tb_byte_t data[BUFSIZE];
    tb_socket_ref_t sock = (tb_socket_ref_t) priv;
    for (tb_size_t count = 0;; count++)
    {
        if (!tb_socket_brecv(sock, data, sizeof(data))) break;
        tb_trace_i("[<--] %.4llu", count);
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_coroutine_thread_main(tb_int_t argc, tb_char_t** argv)
{
    tb_co_scheduler_ref_t scheduler = tb_co_scheduler_init();
    if (scheduler)
    {
        tb_socket_ref_t pair[2];
        if (tb_socket_pair(TB_SOCKET_TYPE_SOCK_STREAM, pair))
        {
            tb_thread_ref_t thread = tb_thread_init(tb_null, tb_demo_thread_server, pair[1], 0);
            if (thread)
            {
                // start coroutines
                tb_coroutine_start(scheduler, tb_demo_coroutine_send, pair[0], 0);
                tb_coroutine_start(scheduler, tb_demo_coroutine_recv, pair[0], 0);

                // do loop
                tb_co_scheduler_loop(scheduler, tb_true);

                // wait and exit thread
                tb_thread_wait(thread, 0, tb_null);
                tb_thread_exit(thread);
            }
            tb_socket_exit(pair[0]);
            tb_socket_exit(pair[1]);
        }
        tb_co_scheduler_exit(scheduler);
    }

    return 0;
}
