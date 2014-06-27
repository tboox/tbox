/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

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
    // the sock
    tb_socket_ref_t     sock;

    // the file
    tb_file_ref_t       file;

    // the aico
    tb_aico_ref_t       aico[2];

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
static tb_void_t tb_demo_file_exit_func(tb_aico_ref_t aico, tb_cpointer_t priv);
static tb_void_t tb_demo_sock_exit_func(tb_aico_ref_t aico, tb_cpointer_t priv);
static tb_void_t tb_demo_context_exit(tb_demo_context_t* context)
{
    if (context)
    {
        // exit aico
        if (context->aico[0]) tb_aico_exit(context->aico[0], tb_demo_sock_exit_func, tb_null);
        context->aico[0] = tb_null;

        // exit aico
        if (context->aico[1]) tb_aico_exit(context->aico[1], tb_demo_file_exit_func, tb_null);
        context->aico[1] = tb_null;

        // exit data
        if (context->data) tb_free(context->data);
        context->data = tb_null;
    }
}
static tb_void_t tb_demo_file_exit_func(tb_aico_ref_t aico, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return(aico);

    // trace
    tb_trace_i("aico: file: exit: %p", tb_aico_file(aico));

    // exit it
    tb_file_exit(tb_aico_file(aico));

    // kill aicp
    tb_aicp_kill((tb_aicp_ref_t)tb_aico_aicp(aico));
}
static tb_void_t tb_demo_sock_exit_func(tb_aico_ref_t aico, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return(aico);

    // trace
    tb_trace_i("aico: sock: exit: %p", tb_aico_sock(aico));

    // exit it
    tb_socket_clos(tb_aico_sock(aico));

    // exit file
    if (priv) tb_aico_exit((tb_aico_ref_t)priv, tb_demo_file_exit_func, tb_null);
}
static tb_bool_t tb_demo_sock_recv_func(tb_aice_t const* aice);
static tb_bool_t tb_demo_file_writ_func(tb_aice_t const* aice)
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
            if (!tb_aico_recv(context->aico[0], context->data, TB_DEMO_SOCK_RECV_MAXN, tb_demo_sock_recv_func, context)) return tb_false;
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
static tb_bool_t tb_demo_sock_recv_func(tb_aice_t const* aice)
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
        if (!tb_aico_writ(context->aico[1], context->size, aice->u.recv.data, aice->u.recv.real, tb_demo_file_writ_func, context)) return tb_false;

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
static tb_bool_t tb_demo_sock_conn_func(tb_aice_t const* aice)
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

    // init aicp
    aicp = tb_aicp_init(2);
    tb_assert_and_check_goto(aicp, end);

    // init sock
    context.sock = tb_socket_open(TB_SOCKET_TYPE_TCP);
    tb_assert_and_check_goto(context.sock, end);

    // init file
    context.file = tb_file_init(argv[1], TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_BINARY | TB_FILE_MODE_TRUNC | TB_FILE_MODE_ASIO);
    tb_assert_and_check_goto(context.file, end);

    // init data
    context.data = tb_malloc_bytes(TB_DEMO_SOCK_RECV_MAXN);
    tb_assert_and_check_goto(context.data, end);

    // addo sock
    context.aico[0] = tb_aico_init_sock(aicp, context.sock);
    tb_assert_and_check_goto(context.aico[0], end);

    // addo file
    context.aico[1] = tb_aico_init_file(aicp, context.file);
    tb_assert_and_check_goto(context.aico[1], end);

    // init conn timeout
    tb_aico_timeout_set(context.aico[0], TB_AICO_TIMEOUT_CONN, 10000);

    // post conn
    tb_trace_i("conn: ..");
    tb_ipv4_t addr; tb_ipv4_set(&addr, "127.0.0.1");
    if (!tb_aico_conn(context.aico[0], &addr, 9090, tb_demo_sock_conn_func, &context)) goto end;

    // loop aicp
    tb_aicp_loop(aicp);
        
    // trace
    if (tb_mclock() > context.base) tb_trace_i("recv[%p]: size: %llu, sped: %llu KB/s", context.sock, context.size, context.size / (tb_mclock() - context.base));

end:

    // trace
    tb_trace_i("end");

    // exit aicp
    if (aicp) tb_aicp_exit(aicp);

    return 0;
}
