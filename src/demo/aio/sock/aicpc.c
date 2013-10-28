/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * types
 */
typedef struct __tb_demo_context_t
{
	// the sock
	tb_handle_t 		sock;

	// the file
	tb_handle_t 		file;

	// the size
	tb_hize_t 			size;

	// the time
	tb_hong_t 			time;

	// the peak
	tb_size_t 			peak;

	// the sped
	tb_size_t 			sped;

}tb_demo_context_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_demo_sock_recv_func(tb_aicp_t* aicp, tb_aice_t const* aice);
static tb_bool_t tb_demo_file_writ_func(tb_aicp_t* aicp, tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aicp && aice && aice->code == TB_AICE_CODE_WRIT, tb_false);

	// the context
	tb_demo_context_t* context = (tb_demo_context_t*)aice->data;
	tb_assert_and_check_return_val(context, tb_false);

	// ok?
	if (aice->state == TB_AICE_STATE_OK)
	{
		// trace
		tb_print("writ[%p]: real: %lu, size: %lu", aice->handle, aice->u.writ.real, aice->u.writ.size);

		// continue?
		if (aice->u.writ.real < aice->u.writ.size)
		{
			// post writ to file
			if (!tb_aicp_writ(aicp, aice->handle, aice->u.writ.seek + aice->u.writ.real, aice->u.writ.data + aice->u.writ.real, aice->u.writ.size - aice->u.writ.real, tb_demo_file_writ_func, context)) return tb_false;
		}
		// ok? 
		else
		{
			// post recv from server
			if (!tb_aicp_recv(aicp, context->sock, 8192, tb_demo_sock_recv_func, context)) return tb_false;
		}
	}
	// closed or failed?
	else
	{
		if (aice->state == TB_AICE_STATE_CLOSED)
			tb_print("writ[%p]: closed", aice->handle);
		else tb_print("writ[%p]: state: %lu", aice->handle, aice->state);
		return tb_false;
	}

	// ok
	return tb_true;
}
static tb_bool_t tb_demo_sock_recv_func(tb_aicp_t* aicp, tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aicp && aice && aice->code == TB_AICE_CODE_RECV, tb_false);

	// the context
	tb_demo_context_t* context = (tb_demo_context_t*)aice->data;
	tb_assert_and_check_return_val(context, tb_false);

	// ok?
	if (aice->state == TB_AICE_STATE_OK)
	{
		// trace
		tb_print("recv[%p]: real: %lu, size: %lu, sped: %lu KB/s", aice->handle, aice->u.recv.real, aice->u.recv.size, context->sped / 1000);

		// has data?
		if (aice->u.recv.real)
		{
			// post writ to file
			if (!tb_aicp_writ(aicp, context->file, context->size, aice->u.recv.data, aice->u.recv.real, tb_demo_file_writ_func, context)) return tb_false;

			// save size
			context->size += aice->u.recv.real;

			// compute speed
			context->peak += aice->u.recv.real;
			if (!context->time) 
			{
				context->time = tb_mclock();
				context->sped = context->peak;
			}
			else if (tb_mclock() > context->time + 1000)
			{
				context->sped = context->peak;
				context->peak = 0;
				context->time = tb_mclock();
			}
		}
		// no data?
		else
		{	
			// post recv from server
			if (!tb_aicp_recv(aicp, context->sock, 8192, tb_demo_sock_recv_func, context)) return tb_false;
		}
	}
	// closed or failed?
	else
	{
		if (aice->state == TB_AICE_STATE_CLOSED)
			tb_print("writ[%p]: closed", aice->handle);
		else tb_print("writ[%p]: state: %lu", aice->handle, aice->state);
		return tb_false;
	}

	// ok
	return tb_true;
}
static tb_bool_t tb_demo_sock_conn_func(tb_aicp_t* aicp, tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aicp && aice && aice->code == TB_AICE_CODE_CONN, tb_false);

	// the context
	tb_demo_context_t* context = (tb_demo_context_t*)aice->data;
	tb_assert_and_check_return_val(context, tb_false);

	// connection ok?
	if (aice->state == TB_AICE_STATE_OK)
	{
		// trace
		tb_print("conn[%p]: ok", aice->handle);

		// post recv from server
		if (!tb_aicp_recv(aicp, aice->handle, 8192, tb_demo_sock_recv_func, context)) return tb_false;
	}
	// timeout?
	else if (aice->state == TB_AICE_STATE_TIMEOUT)
	{
		// exit loop
		tb_print("conn[%p]: timeout", aice->handle);
		return tb_false;
	}
	// failed?
	else
	{
		// exit loop
		tb_print("conn[%p]: state: %lu", aice->handle, aice->state);
		return tb_false;
	}

	// ok
	return tb_true;
}

/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// check
	tb_assert_and_check_return_val(argv[1], 0);

	// init tbox
	if (!tb_init(malloc(10 * 1024 * 1024), 10 * 1024 * 1024)) return 0;

	// init
	tb_handle_t 		aicp = tb_null;
	tb_demo_context_t 	context = {0};

	// init aicp
	aicp = tb_aicp_init(2);
	tb_assert_and_check_goto(aicp, end);

	// open sock
	context.sock = tb_socket_open(TB_SOCKET_TYPE_TCP);
	tb_assert_and_check_goto(context.sock, end);

	// init file
	context.file = tb_file_init(argv[1], TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_BINARY | TB_FILE_MODE_AICP);
	tb_assert_and_check_goto(context.file, end);

	// addo sock
	if (!tb_aicp_addo(aicp, context.sock, TB_AIOO_OTYPE_SOCK)) goto end;

	// addo file
	if (!tb_aicp_addo(aicp, context.file, TB_AIOO_OTYPE_FILE)) goto end;

	// post conn
	tb_print("conn: ..");
	if (!tb_aicp_conn(aicp, context.sock, "127.0.0.1", 9090, 15000, tb_demo_sock_conn_func, &context)) goto end;

	// loop aicp
	tb_aicp_loop(aicp, -1);
	
end:

	// trace
	tb_print("end");

	// exit aicp
	if (aicp) tb_aicp_exit(aicp);

	// exit sock
	if (context.sock) tb_socket_close(context.sock);

	// exit file
	if (context.file) tb_file_exit(context.file);

	// exit tbox
	tb_exit();
	return 0;
}
