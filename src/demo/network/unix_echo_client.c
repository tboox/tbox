/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// timeout
#define TB_DEMO_TIMEOUT     (-1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_network_unix_echo_client_main(tb_int_t argc, tb_char_t** argv)
{
    // check
    tb_assert_and_check_return_val(argc == 2, -1);
    // done
    tb_socket_ref_t sock = tb_null;
    do
    {
        // init socket
        sock = tb_socket_init(TB_SOCKET_TYPE_TCP, TB_IPADDR_FAMILY_UNIX);
        tb_assert_and_check_break(sock);

        tb_bool_t is_abstract = argv[1][0] == '@';
        tb_char_t* path = is_abstract? argv[1] + 1 : argv[1];

        // init address
        tb_ipaddr_t addr;
        tb_ipaddr_unix_set_cstr(&addr, path, is_abstract);

        // trace
        tb_trace_i("connecting(%p): %{ipaddr} %s..", sock, &addr, is_abstract? "(abstract)" : "");

        // connect socket
        tb_long_t ok;
        while (!(ok = tb_socket_connect(sock, &addr)))
        {
            // wait it
            if (tb_socket_wait(sock, TB_SOCKET_EVENT_CONN, TB_DEMO_TIMEOUT) <= 0) break;
        }

        // connect ok?
        tb_check_break(ok > 0);

        // loop
        tb_byte_t data[8192] = {0};
        tb_long_t count = 10000;
        while (count--)
        {
            // send data
            if (tb_socket_bsend(sock, (tb_byte_t const*)"hello world..", 13))
            {
                // recv data
                if (!tb_socket_brecv(sock, data, 13))
                {
                    // error
                    tb_trace_e("send error!");
                    break;
                }
            }
            else break;
        }

        // trace
        tb_trace_d("send(%p): %s %s", sock, data, count <= 0? "ok" : "failed");

    } while (0);

    // exit socket
    if (sock) tb_socket_exit(sock);
    sock = tb_null;
    return 0;
}