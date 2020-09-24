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
tb_int_t tb_demo_network_unix_echo_server_main(tb_int_t argc, tb_char_t** argv)
{
    // check
    tb_assert_and_check_return_val(argc == 2, -1);

    // get path
    tb_bool_t is_abstract = argv[1][0] == '@';
    tb_char_t* path = is_abstract? argv[1] + 1 : argv[1];

    // done
    tb_socket_ref_t sock = tb_null;
    do
    {
        // init socket
        sock = tb_socket_init(TB_SOCKET_TYPE_TCP, TB_IPADDR_FAMILY_UNIX);
        tb_assert_and_check_break(sock);

        if (!is_abstract)
        {
            // clear old socket
            tb_file_remove(path);
        }

        // bind socket
        tb_ipaddr_t addr;
        tb_ipaddr_unix_set_cstr(&addr, path, is_abstract);
        if (!tb_socket_bind(sock, &addr)) break;

        // listen socket
        if (!tb_socket_listen(sock, 1000)) break;

        // trace
        tb_trace_i("listening %{ipaddr} %s..", &addr, is_abstract? "(abstract)" : "");

        // accept client sockets
        tb_socket_ref_t client = tb_null;
        while (1)
        {
            // accept and start client connection
            if ((client = tb_socket_accept(sock, tb_null)))
            {
                // loop
                tb_byte_t data[8192] = {0};
                tb_size_t size = 13;
                while (1)
                {
                    // recv data
                    if (tb_socket_brecv(client, data, size))
                    {
                        // send data
                        if (!tb_socket_bsend(client, data, size))
                        {
                            // error
                            tb_trace_e("send error!");
                            break;
                        }
                    }
                    else break;
                }

                // trace
                tb_trace_d("echo: %s", data);

                // exit socket
                tb_socket_exit(client);
            }
            else if (tb_socket_wait(sock, TB_SOCKET_EVENT_ACPT, -1) <= 0) break;
        }
    } while(0);

    // exit socket
    if (sock) tb_socket_exit(sock);
    sock = tb_null;
    return 0;
}
