/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// read maxn
#define TB_DEMO_FILE_READ_MAXN 			(1 << 16)

// mode
#define TB_DEMO_MODE_SENDF

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */
typedef struct __tb_demo_context_t
{
	// the sock
	tb_handle_t 		sock;

	// the file
	tb_handle_t 		file;

	// the aico
	tb_handle_t 		aico[2];

	// the size
	tb_hize_t 			size;

#ifndef TB_DEMO_MODE_SENDF
	// the data
	tb_byte_t* 			data;
#endif

}tb_demo_context_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#ifdef TB_DEMO_MODE_SENDF
static tb_void_t tb_demo_context_exit(tb_demo_context_t* context)
{
	if (context)
	{
		// exit file
		if (context->file) tb_file_exit(context->file);
		context->file = tb_null;

		// exit sock
		if (context->sock) tb_socket_clos(context->sock);
		context->sock = tb_null;

		// exit aico
		if (context->aico[0]) tb_aico_exit(context->aico[0], tb_true);
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
		// exit file
		if (context->file) tb_file_exit(context->file);
		context->file = tb_null;

		// exit sock
		if (context->sock) tb_socket_clos(context->sock);
		context->sock = tb_null;

		// exit aico
		if (context->aico[0]) tb_aico_exit(context->aico[0], tb_true);
		context->aico[0] = tb_null;

		if (context->aico[1]) tb_aico_exit(context->aico[1], tb_true);
		context->aico[1] = tb_null;
		
		// exit data
		if (context->data) tb_free(context->data);
		context->data = tb_null;

		// exit
		tb_free(context);
	}
}
static tb_bool_t tb_demo_file_read_func(tb_aice_t const* aice);
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
//		tb_trace_i("send[%p]: real: %lu, size: %lu", aice->aico, aice->u.send.real, aice->u.send.size);

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
//		tb_trace_i("read[%p]: real: %lu, size: %lu, seek: %llu", aice->aico, aice->u.read.real, aice->u.read.size, aice->u.read.seek);
			
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
//		tb_trace_i("sendf[%p]: real: %lu, size: %lu", aice->aico, aice->u.sendf.real, aice->u.sendf.size);

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
	tb_handle_t aicp = tb_aico_aicp(aice->aico);
	tb_assert_and_check_return_val(aicp, tb_false);

	// acpt ok?
	if (aice->state == TB_STATE_OK)
	{
		// trace
		tb_trace_i("acpt[%p]: %p", aice->aico, aice->u.acpt.sock);

		// done
		tb_bool_t 			ok = tb_false;
		tb_demo_context_t* 	context = tb_null;
		do
		{
			// check
			tb_assert_and_check_break(aice->u.acpt.sock);

			// make context
			context = tb_malloc0(sizeof(tb_demo_context_t));
			tb_assert_and_check_break(context);

#ifdef TB_DEMO_MODE_SENDF
			// init context
			context->sock = aice->u.acpt.sock;
			context->file = tb_file_init(path, TB_FILE_MODE_RO | TB_FILE_MODE_AICP);
			tb_assert_and_check_break(context->file);

			// addo sock
			context->aico[0] = tb_aico_init_sock(aicp, context->sock);
			tb_assert_and_check_break(context->aico[0]);

			// post sendf from file
			if (!tb_aico_sendf(context->aico[0], context->file, 0ULL, tb_file_size(context->file), tb_demo_sock_sendf_func, context)) break;
#else
			// init context
			context->sock = aice->u.acpt.sock;
			context->file = tb_file_init(path, TB_FILE_MODE_RO | TB_FILE_MODE_AICP);
			context->data = tb_malloc(TB_DEMO_FILE_READ_MAXN);
			tb_assert_and_check_break(context->file && context->data);

			// addo sock
			context->aico[0] = tb_aico_init_sock(aicp, context->sock);
			tb_assert_and_check_break(context->aico[0]);

			// addo file
			context->aico[1] = tb_aico_init_file(aicp, context->file);
			tb_assert_and_check_break(context->aico[1]);

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

			// exit loop
			tb_aicp_kill(tb_aico_aicp(aice->aico));
		}
		else
		{
			// continue it
			if (!tb_aico_acpt(aice->aico, tb_demo_sock_acpt_func, (tb_pointer_t)path)) return tb_false;
		}
	}
	// timeout?
	else if (aice->state == TB_STATE_TIMEOUT)
	{
		// trace
		tb_trace_i("acpt[%p]: timeout", aice->aico);
	
		// continue it
		if (!tb_aico_acpt(aice->aico, tb_demo_sock_acpt_func, (tb_pointer_t)path)) return tb_false;
	}
	// failed?
	else
	{
		// exit loop
		tb_trace_i("acpt[%p]: state: %s", aice->aico, tb_state_cstr(aice->state));
		tb_aicp_kill(tb_aico_aicp(aice->aico));
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
		tb_trace_i("task[%p]: now: %lld", aice->aico, tb_ctime_time());

		// run task
		if (!tb_aico_task_run(aice->aico, 1001, tb_demo_task_func, aice->priv)) return tb_false;
	}
	// failed?
	else
	{
		// trace
		tb_trace_i("task[%p]: state: %s", aice->aico, tb_state_cstr(aice->state));
		tb_aicp_kill(tb_aico_aicp(aice->aico));
	}

	// ok
	return tb_true;
}
#endif
static tb_pointer_t tb_demo_loop_thread(tb_pointer_t data)
{
	// aicp
	tb_handle_t aicp = data;
	tb_size_t 	self = tb_thread_self();

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
	tb_handle_t 		sock = tb_null;
	tb_handle_t 		aicp = tb_null;
	tb_handle_t 		aico = tb_null;
	tb_handle_t 		task = tb_null;
	tb_handle_t 		loop[16] = {0};

	// open sock
	sock = tb_socket_open(TB_SOCKET_TYPE_TCP);
	tb_assert_and_check_goto(sock, end);

	// bind port
	if (!tb_socket_bind(sock, tb_null, 9090)) goto end;

	// listen sock
	if (!tb_socket_listen(sock)) goto end;

	// init aicp
	aicp = tb_aicp_init(16);
	tb_assert_and_check_goto(aicp, end);

	// addo sock
	aico = tb_aico_init_sock(aicp, sock);
	tb_assert_and_check_goto(aico, end);

	// addo task
	task = tb_aico_init_task(aicp, tb_false);
	tb_assert_and_check_goto(task, end);

	// run task
//	if (!tb_aico_task_run(task, 0, tb_demo_task_func, tb_null)) goto end;
//	if (!tb_aico_task_run(aico, 0, tb_demo_task_func, tb_null)) goto end;

	// init acpt timeout
	tb_aico_timeout_set(aico, TB_AICO_TIMEOUT_ACPT, 10000);

	// post acpt
	if (!tb_aico_acpt(aico, tb_demo_sock_acpt_func, argv[1])) goto end;
//	if (!tb_aico_acpt_after(aico, 5000, tb_demo_sock_acpt_func, argv[1])) goto end;

	// done loop
	loop[0] = tb_thread_init(tb_null, tb_demo_loop_thread, aicp, 0);
	loop[1] = tb_thread_init(tb_null, tb_demo_loop_thread, aicp, 0);
	loop[2] = tb_thread_init(tb_null, tb_demo_loop_thread, aicp, 0);
	loop[3] = tb_thread_init(tb_null, tb_demo_loop_thread, aicp, 0);

	// wait exit
	getchar();

end:

	// trace
	tb_trace_i("end");

	// kill aicp
	if (aicp) tb_aicp_kill(aicp);
	
	// wait exit
	{
		// exit loop
		tb_handle_t* l = loop;
		for (; *l; l++)
		{
			tb_thread_wait(*l, -1);
			tb_thread_exit(*l);
		}
	}

	// exit aico
	if (aico) tb_aico_exit(aico, tb_false);

	// exit task
	if (task) tb_aico_exit(task, tb_false);

	// exit aicp
	if (aicp) tb_aicp_exit(aicp);

	// exit sock
	if (sock) tb_socket_clos(sock);

	return 0;
}
