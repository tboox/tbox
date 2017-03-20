/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// recv maxn
#define TB_DEMO_SOCK_RECV_MAXN          (1 << 16)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */
typedef struct __tb_demo_context_t
{
    // the sock aico
    tb_aico_ref_t       sock;

    // the file aico
    tb_aico_ref_t       file;

    // the size
    tb_hize_t           size;

    // the time
    tb_hong_t           time;

    // the base
    tb_hong_t           base;

    // the peak
    tb_size_t           peak;

    // the sped
    tb_size_t           sped;

    // the data
    tb_byte_t*          data;

}tb_demo_context_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_demo_sock_aico_clos(tb_aice_ref_t aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_CLOS, tb_false);

    // trace
    tb_trace_d("aico[%p]: clos: %s", aice->aico, tb_state_cstr(aice->state));

    // exit aico
    tb_aico_exit(aice->aico);

    // kill aicp
    tb_aicp_kill(tb_aico_aicp(aice->aico));

    // ok
    return tb_true;
}
static tb_bool_t tb_demo_file_aico_clos(tb_aice_ref_t aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_CLOS, tb_false);

    // the context
    tb_demo_context_t* context = (tb_demo_context_t*)aice->priv;
    tb_assert_and_check_return_val(context, tb_false);

    // trace
    tb_trace_d("aico[%p]: clos: %s", aice->aico, tb_state_cstr(aice->state));

    // exit aico
    tb_aico_exit(aice->aico);

    // exit sock aico
    if (context->sock) tb_aico_clos(context->sock, tb_demo_sock_aico_clos, tb_null);
    context->sock = tb_null;

    // ok
    return tb_true;
}
static tb_void_t tb_demo_context_exit(tb_demo_context_t* context)
{
    if (context)
    {
        // exit aico
        if (context->file) tb_aico_clos(context->file, tb_demo_file_aico_clos, context);
        context->file = tb_null;

        // exit data
        if (context->data) tb_free(context->data);
        context->data = tb_null;
    }
}
static tb_bool_t tb_demo_sock_recv_func(tb_aice_ref_t aice);
static tb_bool_t tb_demo_file_writ_func(tb_aice_ref_t aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_WRIT, tb_false);

    // the context
    tb_demo_context_t* context = (tb_demo_context_t*)aice->priv;
    tb_assert_and_check_return_val(context, tb_false);

    // ok?
    if (aice->state == TB_STATE_OK)
    {
        // trace
        tb_trace_d("writ[%p]: real: %lu, size: %lu", aice->aico, aice->u.writ.real, aice->u.writ.size);

        // continue?
        if (aice->u.writ.real < aice->u.writ.size)
        {
            // post writ to file
            if (!tb_aico_writ(aice->aico, aice->u.writ.seek + aice->u.writ.real, aice->u.writ.data + aice->u.writ.real, aice->u.writ.size - aice->u.writ.real, tb_demo_file_writ_func, context)) return tb_false;
        }
        // ok? 
        else
        {
            // post recv from server
            if (!tb_aico_recv(context->sock, context->data, TB_DEMO_SOCK_RECV_MAXN, tb_demo_sock_recv_func, context)) return tb_false;
        }
    }
    // closed or failed?
    else
    {
        // trace
        tb_trace_i("writ[%p]: %s", aice->aico, tb_state_cstr(aice->state));

        // exit context
        tb_demo_context_exit(context);
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_demo_sock_recv_func(tb_aice_ref_t aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_RECV, tb_false);

    // the context
    tb_demo_context_t* context = (tb_demo_context_t*)aice->priv;
    tb_assert_and_check_return_val(context, tb_false);

    // ok?
    if (aice->state == TB_STATE_OK)
    {
        // trace
        tb_trace_d("recv[%p]: real: %lu, size: %lu", aice->aico, aice->u.recv.real, aice->u.recv.size);

        // post writ to file
        if (!tb_aico_writ(context->file, context->size, aice->u.recv.data, aice->u.recv.real, tb_demo_file_writ_func, context)) return tb_false;

        // save size
        context->size += aice->u.recv.real;

        // compute speed
        context->peak += aice->u.recv.real;
        if (!context->time) 
        {
            context->time = tb_mclock();
            context->base = tb_mclock();
            context->sped = context->peak;
        }
        else if (tb_mclock() > context->time + 1000)
        {
            context->sped = context->peak;
            context->peak = 0;
            context->time = tb_mclock();

            // trace
            tb_trace_i("recv[%p]: size: %llu, sped: %lu KB/s", aice->aico, context->size, context->sped / 1000);
        }
    }
    // closed or failed?
    else
    {
        // trace
        tb_trace_i("recv[%p]: state: %s", aice->aico, tb_state_cstr(aice->state));

        // exit context
        tb_demo_context_exit(context);
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_demo_sock_conn_func(tb_aice_ref_t aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_CONN, tb_false);

    // the context
    tb_demo_context_t* context = (tb_demo_context_t*)aice->priv;
    tb_assert_and_check_return_val(context, tb_false);

    // connection ok?
    if (aice->state == TB_STATE_OK)
    {
        // trace
        tb_trace_i("conn[%p]: ok", aice->aico);

        // post recv from server
        if (!tb_aico_recv(aice->aico, context->data, TB_DEMO_SOCK_RECV_MAXN, tb_demo_sock_recv_func, context)) return tb_false;
    }
    // timeout or failed?
    else
    {
        // exit loop
        tb_trace_i("conn[%p]: state: %s", aice->aico, tb_state_cstr(aice->state));

        // exit context
        tb_demo_context_exit(context);
    }

    // ok
    return tb_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_asio_aicpc_main(tb_int_t argc, tb_char_t** argv)
{
    // check
    tb_assert_and_check_return_val(argv[1], 0);

    // init
    tb_aicp_ref_t       aicp = tb_null;
    tb_demo_context_t   context = {0};
    do
    {
        // init aicp
        aicp = tb_aicp_init(2);
        tb_assert_and_check_break(aicp);

        // init data
        context.data = tb_malloc_bytes(TB_DEMO_SOCK_RECV_MAXN);
        tb_assert_and_check_break(context.data);

        // init sock aico
        context.sock = tb_aico_init(aicp);
        tb_assert_and_check_break(context.sock);

        // init addr
        tb_ipaddr_t addr; 
        if (!tb_ipaddr_set(&addr, "127.0.0.1", 9090, TB_IPADDR_FAMILY_NONE)) break;

        // open sock aico
        if (!tb_aico_open_sock_from_type(context.sock, TB_SOCKET_TYPE_TCP, tb_ipaddr_family(&addr))) break;

        // init file aico
        context.file = tb_aico_init(aicp);
        tb_assert_and_check_break(context.file);

        // open file aico
        if (!tb_aico_open_file_from_path(context.file, argv[1], TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_BINARY | TB_FILE_MODE_TRUNC)) break;

        // init conn timeout
        tb_aico_timeout_set(context.sock, TB_AICO_TIMEOUT_CONN, 10000);

        // post conn
        tb_trace_i("conn: ..");
        if (!tb_aico_conn(context.sock, &addr, tb_demo_sock_conn_func, &context)) break;

        // loop aicp
        tb_aicp_loop(aicp);
        
        // trace
        if (tb_mclock() > context.base) tb_trace_i("size: %llu, sped: %llu KB/s", context.size, context.size / (tb_mclock() - context.base));

    } while (0);

    // trace
    tb_trace_i("end");

    // exit aicp
    if (aicp) tb_aicp_exit(aicp);

    return 0;
}
