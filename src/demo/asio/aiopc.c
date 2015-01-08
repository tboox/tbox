/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_DEMO_SOCK_RECV_MAXN          (1 << 16)

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_asio_aiopc_main(tb_int_t argc, tb_char_t** argv)
{
    // check
    tb_assert_and_check_return_val(argv[1], 0);

    // done
    tb_socket_ref_t sock = tb_null;
    tb_file_ref_t   file = tb_null;
    tb_byte_t*      data = tb_null;
    do
    {
        // init addr
        tb_addr_t addr;
        if (!tb_addr_set(&addr, "127.0.0.1", 9090, TB_ADDR_FAMILY_NONE)) break;

        // init sock
        sock = tb_socket_init(TB_SOCKET_TYPE_TCP, tb_addr_family(&addr));
        tb_assert_and_check_break(sock);

        // init file
        file = tb_file_init(argv[1], TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_BINARY | TB_FILE_MODE_TRUNC);
        tb_assert_and_check_break(file);

        // init data
        data = tb_malloc_bytes(TB_DEMO_SOCK_RECV_MAXN);
        tb_assert_and_check_break(data);

        // done conn
        tb_long_t conn = -1;
        while (!(conn = tb_socket_connect(sock, &addr)))
        {
            // wait
            conn = tb_aioo_wait(sock, TB_AIOE_CODE_CONN, 20000);
            tb_check_break(conn > 0);
        }

        // ok?
        if (conn > 0) tb_trace_i("conn[%p]: ok", sock);
        // timeout?
        else if (!conn) tb_trace_i("conn[%p]: timeout", sock);
        // failed?
        else tb_trace_i("conn[%p]: failed", sock);
        tb_check_break(conn > 0);

        // done sock
        tb_size_t peak = 0;
        tb_size_t sped = 0;
        tb_hong_t time = 0;
        tb_hong_t base = tb_mclock();
        tb_hize_t size = 0;
        tb_bool_t wait = tb_false;
        while (1)
        {
            // recv data
            tb_long_t real = tb_socket_recv(sock, data, TB_DEMO_SOCK_RECV_MAXN);
            if (real > 0)
            {
                // save size
                size += real;

                // trace
    //          tb_trace_i("recv[%p]: real: %ld", sock, real);

                // compute speed
                peak += real;
                if (!time) 
                {
                    time = tb_mclock();
                    sped = peak;
                }
                else if (tb_mclock() > time + 1000)
                {
                    sped = peak;
                    peak = 0;
                    time = tb_mclock();
        
                    // trace
                    tb_trace_i("recv[%p]: size: %llu, sped: %lu KB/s", sock, size, sped / 1000);
                }

                // init wait
                wait = tb_false;

                // done file
                tb_size_t need = real;
                tb_size_t writ = 0;
                while (writ < need)
                {
                    // writ data
                    real = tb_file_writ(file, data + writ, need - writ);
                    if (real > 0)
                    {
                        // trace
    //                  tb_trace_i("writ[%p]: real: %ld", file, real);

                        // save writ
                        writ += real;
                        wait = tb_false;
                    }
                    else if (!real && !wait) wait = tb_true;
                    else
                    {
                        tb_trace_i("writ[%p]: failed", file);
                    }
                }

                // init wait
                wait = tb_false;
            }
            else if (!real && !wait)
            {
                // wait
                if (tb_aioo_wait(sock, TB_AIOE_CODE_RECV, -1) <= 0) 
                {
                    tb_trace_i("recv[%p]: wait: failed", sock);
                    break;
                }
                wait = tb_true;
            }
            else
            {
                tb_trace_i("recv[%p]: closed", sock);
                break;
            }
        }
        
        // trace
        if (sock && tb_mclock() > base) tb_trace_i("recv[%p]: size: %llu, sped: %llu KB/s", sock, size, size / (tb_mclock() - base));

    } while (0);

    // trace
    tb_trace_i("end");

    // exit sock
    if (sock) tb_socket_exit(sock);

    // exit file
    if (file) tb_file_exit(file);

    // exit data
    if (data) tb_free(data);
    return 0;
}
