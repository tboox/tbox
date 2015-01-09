/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_DEMO_FILE_READ_MAXN          (1 << 16)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */
typedef struct __tb_demo_context_t
{
    // the aioo
    tb_aioo_ref_t       aioo;

    // the sock
    tb_socket_ref_t     sock;

    // the file
    tb_file_ref_t       file;

    // the left
    tb_hize_t           left;

    // the send
    tb_size_t           send;

    // the real
    tb_size_t           real;

    // the data
    tb_byte_t*          data;

}tb_demo_context_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_demo_context_exit(tb_aiop_ref_t aiop, tb_demo_context_t* context)
{
    if (context)
    {
        // exit aioo
        if (context->aioo) tb_aiop_delo(aiop, context->aioo);
        context->aioo = tb_null;

        // exit sock
        if (context->sock) tb_socket_exit(context->sock);
        context->sock = tb_null;

        // exit file
        if (context->file) tb_file_exit(context->file);
        context->file = tb_null;

        // exit data
        if (context->data) tb_free(context->data);
        context->data = tb_null;

        // exit it
        tb_free(context);
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_asio_aiopd_main(tb_int_t argc, tb_char_t** argv)
{
    // check
    tb_assert_and_check_return_val(argv[1], 0);

    // done
    tb_socket_ref_t sock = tb_null;
    tb_aiop_ref_t   aiop = tb_null;
    do
    {
        // init addr
        tb_ipaddr_t addr;
        tb_ipaddr_set(&addr, tb_null, 9090, TB_IPADDR_FAMILY_IPV4);

        // init sock
        sock = tb_socket_init(TB_SOCKET_TYPE_TCP, tb_ipaddr_family(&addr));
        tb_assert_and_check_break(sock);

        // init aiop
        aiop = tb_aiop_init(16);
        tb_assert_and_check_break(aiop);

        // bind 
        if (!tb_socket_bind(sock, &addr)) break;

        // listen sock
        if (!tb_socket_listen(sock, 20)) break;

        // addo sock
        if (!tb_aiop_addo(aiop, sock, TB_AIOE_CODE_ACPT, tb_null)) break;

        // accept
        tb_aioe_t list[16];
        while (1)
        {
            // wait
            tb_long_t objn = tb_aiop_wait(aiop, list, 16, -1);
            tb_assert_and_check_break(objn >= 0);

            // walk list
            tb_size_t i = 0;
            for (i = 0; i < objn; i++)
            {
                // the aioo 
                tb_aioo_ref_t aioo = list[i].aioo;

                // check
                tb_assert_and_check_break(aioo && tb_aioo_sock(aioo));

                // acpt?
                if (list[i].code & TB_AIOE_CODE_ACPT)
                {
                    // done acpt
                    tb_bool_t           ok = tb_false;
                    tb_demo_context_t*  context = tb_null;
                    do
                    {
                        // make context
                        context = tb_malloc0_type(tb_demo_context_t);
                        tb_assert_and_check_break(context);

                        // init sock
                        context->sock = tb_socket_accept(tb_aioo_sock(aioo), tb_null);
                        tb_assert_and_check_break(context->sock);

                        // init file
                        context->file = tb_file_init(argv[1], TB_FILE_MODE_RO);
                        tb_assert_and_check_break(context->file);

                        // init data
                        context->data = tb_malloc_bytes(TB_DEMO_FILE_READ_MAXN);
                        tb_assert_and_check_break(context->data);

                        // addo sock
                        context->aioo = tb_aiop_addo(aiop, context->sock, TB_AIOE_CODE_SEND, context);
                        tb_assert_and_check_break(context->aioo);

                        // trace
                        tb_trace_i("acpt[%p]: ok", context->sock);

                        // init left
                        context->left = tb_file_size(context->file);

                        // done read
                        tb_long_t real = tb_file_read(context->file, context->data, tb_min((tb_size_t)context->left, TB_DEMO_FILE_READ_MAXN));
                        tb_assert_and_check_break(real > 0);

                        // save size
                        context->left -= real;

                        // trace
    //                  tb_trace_i("read[%p]: real: %ld", context->file, real);

                        // done send
                        context->send = real;
                        real = tb_socket_send(context->sock, context->data + context->real, context->send - context->real);
                        if (real >= 0)
                        {
                            // save real
                            context->real += real;

                            // trace
    //                      tb_trace_i("send[%p]: real: %ld", context->sock, real);
                        }
                        else
                        {
                            // trace
                            tb_trace_i("send[%p]: closed", context->sock);
                            break;
                        }

                        // ok
                        ok = tb_true;

                    } while (0);

                    // failed or closed?
                    if (!ok)
                    {
                        // exit context
                        tb_demo_context_exit(aiop, context);
                        break;
                    }
                }
                // writ?
                else if (list[i].code & TB_AIOE_CODE_SEND)
                {
                    // the context
                    tb_demo_context_t* context = (tb_demo_context_t*)list[i].priv;
                    tb_assert_and_check_break(context);

                    // continue to send it if not finished
                    if (context->real < context->send)
                    {
                        // done send
                        tb_long_t real = tb_socket_send(tb_aioo_sock(aioo), context->data + context->real, context->send - context->real);
                        if (real > 0)
                        {
                            // save real
                            context->real += real;

                            // trace
    //                      tb_trace_i("send[%p]: real: %ld", tb_aioo_sock(aioo), real);
                        }
                        else
                        {
                            // trace
                            tb_trace_i("send[%p]: closed", tb_aioo_sock(aioo));

                            // exit context
                            tb_demo_context_exit(aiop, context);
                            break;
                        }
                    }
                    // finished? read file
                    else if (context->left)
                    {
                        // init
                        context->real = 0;
                        context->send = 0;

                        // done read
                        tb_size_t tryn = 1;
                        tb_long_t real = 0;
                        while (!(real = tb_file_read(context->file, context->data, tb_min((tb_size_t)context->left, TB_DEMO_FILE_READ_MAXN))) && tryn--);
                        if (real > 0)
                        {
                            // save left
                            context->left -= real;

                            // trace
    //                      tb_trace_i("read[%p]: real: %ld", context->file, real);

                            // done send
                            context->send = real;
                            real = tb_socket_send(tb_aioo_sock(aioo), context->data, context->send);
                            if (real >= 0)
                            {
                                // save real
                                context->real += real;

                                // trace
    //                          tb_trace_i("send[%p]: real: %ld", tb_aioo_sock(aioo), real);
                            }
                            else
                            {
                                // trace
                                tb_trace_i("send[%p]: closed", tb_aioo_sock(aioo));

                                // exit context
                                tb_demo_context_exit(aiop, context);
                                break;
                            }
                        }
                        else
                        {
                            // trace
                            tb_trace_i("read[%p]: closed", tb_aioo_sock(aioo));

                            // exit context
                            tb_demo_context_exit(aiop, context);
                            break;
                        }
                    }
                    else 
                    {
                        // trace
                        tb_trace_i("read[%p]: closed", tb_aioo_sock(aioo));

                        // exit context
                        tb_demo_context_exit(aiop, context);
                        break;
                    }
                }
                // error?
                else 
                {
                    tb_trace_i("aioe[%p]: unknown code: %lu", tb_aioo_sock(aioo), list[i].code);
                    break;
                }
            }
        }
    } while (0);

    // trace
    tb_trace_i("end");

    // exit socket
    if (sock) tb_socket_exit(sock);

    // exit aiop
    if (aiop) tb_aiop_exit(aiop);

    // end
    return 0;
}
