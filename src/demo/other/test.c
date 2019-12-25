/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../demo.h"

#define BUF_SIZE 32767
#define N 100000

#define WORKAROUND 0

static tb_int_t server(tb_cpointer_t priv) {
    tb_socket_ref_t sock = (tb_socket_ref_t) priv;
    tb_byte_t buf[BUF_SIZE];
    tb_size_t count = 0;

        tb_trace_i("server: ..");
    while (1) {
        count++;
        tb_check_break(tb_socket_brecv(sock, buf, sizeof buf));
        tb_usleep(50000);
        tb_assert(tb_socket_bsend(sock, buf, sizeof buf));
        tb_trace_i(" ===  %.4llu (%llu bytes)", count, count * BUF_SIZE);
    }
    return 0;
}

static tb_void_t c_send(tb_cpointer_t priv) {
    tb_socket_ref_t sock = (tb_socket_ref_t) priv;
    //tb_file_ref_t rand   = tb_file_init("/dev/urandom", TB_FILE_MODE_RO);

        tb_trace_i("c_send: ..");
    for (tb_size_t i = 0; i < N; i++) {
        tb_byte_t buf[BUF_SIZE];
      //  tb_file_read(rand, buf, sizeof buf);

#if !WORKAROUND
        tb_assert(tb_socket_bsend(sock, buf, sizeof buf));
#else
        tb_long_t ret;
        while (1) {
            ret = tb_socket_send(sock, buf, sizeof buf);
            tb_trace_i("%llu", ret);
            if (ret != sizeof buf) {
                tb_trace_i("[CLI] retry");
                tb_coroutine_sleep(100);
                // tb_coroutine_waitio(sock, TB_SOCKET_EVENT_SEND, -1); NOT WORKING!
                continue;
            }
            break;
        }
#endif
        tb_trace_i("[-->] %.4llu", i);
    }

 //   tb_file_exit(rand);
}

static tb_void_t c_recv(tb_cpointer_t priv) {
        tb_trace_i("c_recv: ..");
    tb_socket_ref_t sock = (tb_socket_ref_t) priv;
    tb_byte_t buf[BUF_SIZE];

    for (tb_size_t count = 0;; count++) {
        tb_check_break(tb_socket_brecv(sock, buf, sizeof buf));
        tb_trace_i("[<--] %.4llu", count);
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_other_test_main(tb_int_t argc, tb_char_t** argv)
{
    tb_co_scheduler_ref_t sche = tb_co_scheduler_init();

    tb_socket_ref_t pair[2];
    tb_assert(tb_socket_pair(TB_SOCKET_TYPE_TCP, pair));

    tb_thread_ref_t server_thread = tb_thread_init("server", server, pair[1], 0);

    tb_coroutine_start(sche, c_send, pair[0], 0);
    tb_coroutine_start(sche, c_recv, pair[0], 0);

    tb_co_scheduler_loop(sche, tb_true);

    tb_socket_kill(pair[0], TB_SOCKET_KILL_RW);
    tb_socket_kill(pair[1], TB_SOCKET_KILL_RW);
    tb_socket_exit(pair[0]);
    tb_socket_exit(pair[1]);

    tb_thread_wait(server_thread, 0, tb_null);
    tb_thread_exit(server_thread);

    return 0;
}
