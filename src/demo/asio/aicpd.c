/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// read maxn
#define TB_DEMO_FILE_READ_MAXN          (1 << 16)

// mode
#define TB_DEMO_MODE_SENDF

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */
typedef struct __tb_demo_context_t
{
    // the file
    tb_file_ref_t       file;

    // the aico
    tb_aico_ref_t       aico[2];

    // the size
    tb_hize_t           size;

#ifndef TB_DEMO_MODE_SENDF
    // the data
    tb_byte_t*          data;
#endif

}tb_demo_context_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_demo_aico_clos(tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_CLOS, tb_false);

    // trace
    tb_trace_d("aico[%p]: clos: %s", aice->aico, tb_state_cstr(aice->state));

    // exit aico
    tb_aico_exit(aice->aico);

    // ok
    return tb_true;
}
#ifdef TB_DEMO_MODE_SENDF
static tb_void_t tb_demo_context_exit(tb_demo_context_t* context)
{
    if (context)
    {
        // clos aico
        if (context->aico[0]) tb_aico_clos(context->aico[0], tb_demo_aico_clos, tb_null);
        context->aico[0] = tb_null;

        // exit
        tb_free(context);
    }
}
#else
static tb_void_t tb_demo_context_exit(tb_demo_context_t* context)
{
    if (context)
    {
        // exit aico
        if (context->aico[0]) tb_aico_clos(context->aico[0], tb_demo_aico_clos, tb_null);
        context->aico[0] = tb_null;

        // exit aico
        if (context->aico[1]) tb_aico_clos(context->aico[1], tb_demo_aico_clos, tb_null);
        context->aico[1] = tb_null;

        // exit data
        if (context->data) tb_free(context->data);
        context->data = tb_null;

        // exit
        tb_free(context);
    }
}
static tb_bool_t tb_demo_sock_send_func(tb_aice_t const* aice);
static tb_bool_t tb_demo_file_read_func(tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_READ, tb_false);

    // the context
    tb_demo_context_t* context = (tb_demo_context_t*)aice->priv;
    tb_assert_and_check_return_val(context, tb_false);

    // ok?
    if (aice->state == TB_STATE_OK)
    {
        // trace
        tb_trace_d("read[%p]: real: %lu, size: %lu, seek: %llu", aice->aico, aice->u.read.real, aice->u.read.size, aice->u.read.seek);
            
        // post send to client
        if (!tb_aico_send(context->aico[0], aice->u.read.data, aice->u.read.real, tb_demo_sock_send_func, context)) return tb_false;
    }
    // closed or failed?
    else
    {
        tb_trace_i("read[%p]: state: %s", aice->aico, tb_state_cstr(aice->state));
        tb_demo_context_exit(context);
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_demo_sock_send_func(tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_SEND, tb_false);

    // the context
    tb_demo_context_t* context = (tb_demo_context_t*)aice->priv;
    tb_assert_and_check_return_val(context, tb_false);

    // ok?
    if (aice->state == TB_STATE_OK)
    {
        // trace
        tb_trace_d("send[%p]: real: %lu, size: %lu", aice->aico, aice->u.send.real, aice->u.send.size);

        // save size
        context->size += aice->u.send.real;

        // continue?
        if (aice->u.send.real < aice->u.send.size)
        {
            // post send to client
            if (!tb_aico_send(aice->aico, aice->u.send.data + aice->u.send.real, aice->u.send.size - aice->u.send.real, tb_demo_sock_send_func, context)) return tb_false;
        }
        // ok? 
        else 
        {
            // post read from file
            if (!tb_aico_read(context->aico[1], context->size, context->data, TB_DEMO_FILE_READ_MAXN, tb_demo_file_read_func, context)) return tb_false;
        }
    }
    // closed or failed?
    else
    {
        tb_trace_i("send[%p]: state: %s", aice->aico, tb_state_cstr(aice->state));
        tb_demo_context_exit(context);
    }

    // ok
    return tb_true;
}
#endif
#ifdef TB_DEMO_MODE_SENDF
static tb_bool_t tb_demo_sock_sendf_func(tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_SENDF, tb_false);

    // the context
    tb_demo_context_t* context = (tb_demo_context_t*)aice->priv;
    tb_assert_and_check_return_val(context, tb_false);

    // ok?
    if (aice->state == TB_STATE_OK)
    {
        // trace
        tb_trace_d("sendf[%p]: real: %lu, size: %llu", aice->aico, aice->u.sendf.real, aice->u.sendf.size);

        // save size
        context->size += aice->u.sendf.real;

        // continue to send it?
        if (aice->u.sendf.real < aice->u.sendf.size)
        {
            // post sendf from file
            if (!tb_aico_sendf(aice->aico, context->file, context->size, aice->u.sendf.size - aice->u.sendf.real, tb_demo_sock_sendf_func, context)) return tb_false;
        }
        else 
        {
            tb_trace_i("sendf[%p]: finished", aice->aico);
            tb_demo_context_exit(context);
        }
    }
    // closed or failed?
    else
    {
        tb_trace_i("sendf[%p]: state: %s", aice->aico, tb_state_cstr(aice->state));
        tb_demo_context_exit(context);
    }

    // ok
    return tb_true;
}
#endif
static tb_bool_t tb_demo_sock_acpt_func(tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_ACPT, tb_false);

    // the file path
    tb_char_t const* path = (tb_char_t const*)aice->priv;
    tb_assert_and_check_return_val(path, tb_false);

    // the aicp
    tb_aicp_ref_t aicp = tb_aico_aicp(aice->aico);
    tb_assert_and_check_return_val(aicp, tb_false);

    // acpt ok?
    if (aice->state == TB_STATE_OK)
    {
        // trace
        tb_trace_i("acpt[%p]: %p", aice->aico, aice->u.acpt.aico);

        // done
        tb_bool_t           ok = tb_false;
        tb_demo_context_t*  context = tb_null;
        do
        {
            // make context
            context = tb_malloc0_type(tb_demo_context_t);
            tb_assert_and_check_break(context);

#ifdef TB_DEMO_MODE_SENDF
            // init file
            context->file = tb_file_init(path, TB_FILE_MODE_RO | TB_FILE_MODE_ASIO);
            tb_assert_and_check_break(context->file);

            // init sock aico
            context->aico[0] = aice->u.acpt.aico;
            tb_assert_and_check_break(context->aico[0]);

            // post sendf from file
            if (!tb_aico_sendf(context->aico[0], context->file, 0ULL, tb_file_size(context->file), tb_demo_sock_sendf_func, context)) break;
#else
            // init data
            context->data = tb_malloc_bytes(TB_DEMO_FILE_READ_MAXN);
            tb_assert_and_check_break(context->file && context->data);

            // init sock aico
            context->aico[0] = aice->u.acpt.aico;
            tb_assert_and_check_break(context->aico[0]);

            // init file aico
            context->aico[1] = tb_aico_init(aicp);
            tb_assert_and_check_break(context->aico[1]);

            // open file aico
            if (!tb_aico_open_file_from_path(context->aico[1], path, TB_FILE_MODE_RO)) break;

            // post read from file
            if (!tb_aico_read(context->aico[1], context->size, context->data, TB_DEMO_FILE_READ_MAXN, tb_demo_file_read_func, context)) break;
#endif
            // ok
            ok = tb_true;

        } while (0);

        // failed?
        if (!ok)
        {
            // exit context
            if (context) tb_demo_context_exit(context);
        }
    }
    // failed?
    else
    {
        // exit loop
        tb_trace_i("acpt[%p]: state: %s", aice->aico, tb_state_cstr(aice->state));

        // clos aico
        if (aice->aico) tb_aico_clos(aice->aico, tb_demo_aico_clos, tb_null);
    }

    // ok
    return tb_true;
}
#if 0
static tb_bool_t tb_demo_task_func(tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_RUNTASK, tb_false);

    // ok?
    if (aice->state == TB_STATE_OK)
    {
        // trace
        tb_trace_i("task[%p]: now: %lld", aice->aico, tb_cache_time_mclock());

        // run task
        if (!tb_aico_task_run(aice->aico, 1001, tb_demo_task_func, aice->priv)) return tb_false;
    }
    // failed?
    else
    {
        // trace
        tb_trace_i("task[%p]: state: %s", aice->aico, tb_state_cstr(aice->state));

        // clos aico
        if (aice->aico) tb_aico_clos(aice->aico, tb_demo_aico_clos, tb_null);
    }

    // ok
    return tb_true;
}
#endif
static tb_pointer_t tb_demo_loop(tb_cpointer_t priv)
{
    // aicp
    tb_aicp_ref_t   aicp = (tb_aicp_ref_t)priv;
    tb_size_t       self = tb_thread_self();

    // trace
    tb_trace_i("[loop: %lu]: init", self);

    // loop aicp
    if (aicp) tb_aicp_loop(aicp);
    
    // trace
    tb_trace_i("[loop: %lu]: exit", self);

    // exit
    tb_thread_return(tb_null);
    return tb_null;
}
/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_asio_aicpd_main(tb_int_t argc, tb_char_t** argv)
{
    // check
    tb_assert_and_check_return_val(argv[1], 0);

    // init
    tb_aicp_ref_t       aicp = tb_null;
    tb_aico_ref_t       aico = tb_null;
//    tb_aico_ref_t       task = tb_null;
    tb_thread_ref_t     loop[16] = {tb_null};
    do
    {
        // init aicp
        aicp = tb_aicp_init(16);
        tb_assert_and_check_break(aicp);

        // init sock aico
        aico = tb_aico_init(aicp);
        tb_assert_and_check_break(aico);

        // open sock aico
        if (!tb_aico_open_sock_from_type(aico, TB_SOCKET_TYPE_TCP)) break;

        // bind port
        if (!tb_socket_bind(tb_aico_sock(aico), tb_null, 9090)) break;

        // listen sock
        if (!tb_socket_listen(tb_aico_sock(aico), 20)) break;

#if 0
        // init task aico
        task = tb_aico_init(aicp);
        tb_assert_and_check_break(task);

        // open task aico
        if (!tb_aico_open_task(task, tb_false)) break;

        // run task
        if (!tb_aico_task_run(task, 0, tb_demo_task_func, tb_null)) break;
        if (!tb_aico_task_run(aico, 0, tb_demo_task_func, tb_null)) break;
#endif

        // post acpt
        if (!tb_aico_acpt(aico, tb_demo_sock_acpt_func, argv[1])) break;

        // done loop
        loop[0] = tb_thread_init(tb_null, tb_demo_loop, aicp, 0);
        loop[1] = tb_thread_init(tb_null, tb_demo_loop, aicp, 0);
        loop[2] = tb_thread_init(tb_null, tb_demo_loop, aicp, 0);
        loop[3] = tb_thread_init(tb_null, tb_demo_loop, aicp, 0);

        // wait exit
        getchar();

    } while (0);

    // trace
    tb_trace_i("end");
 
#if 1
    if (aicp)
    {
        // kill all
        tb_aicp_kill_all(aicp);
   
        // wait all
        tb_aicp_wait_all(aicp, -1);

        // kill aicp
        tb_aicp_kill(aicp);
    }

    // wait exit
    {
        // exit loop
        tb_thread_ref_t* l = loop;
        for (; *l; l++)
        {
            tb_thread_wait(*l, -1);
            tb_thread_exit(*l);
        }
    }
#endif

    // exit aicp
    if (aicp) tb_aicp_exit(aicp);
    return 0;
}
