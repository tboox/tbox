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

	// the aico sock
	tb_aico_t const* 	aico_sock;

	// the aico file
	tb_aico_t const* 	aico_file;

	// the data sock
	tb_byte_t 			data_sock[8192];

	// the data file
	tb_byte_t 			data_file[8192];

}tb_demo_context_t;

/* ///////////////////////////////////////////////////////////////////////
 * worker
 */
static tb_bool_t tb_demo_sock_work_func(tb_aicp_t* aicp, tb_aico_t const* aico, tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aicp && aico && aice, tb_false);

	// the context
	tb_demo_context_t* context = (tb_demo_context_t*)aico->aioo.odata;
	tb_assert_and_check_return_val(context, tb_false);

	// done
	switch (aice->code)
	{
	case TB_AICE_CODE_CONN:
		{
			// trace
			tb_print("conn[%p]: %ld", aico->aioo.handle, aice->u.conn.ok);

			// ok?
			if (aice->u.conn.ok > 0)
			{
				// post read from server
				if (!tb_aicp_read(aicp, aico, context->data_sock, sizeof(context->data_sock))) return tb_false;
			}
		}
		break;
	case TB_AICE_CODE_READ:
		{
			// trace
			tb_print("read[%p]: real: %ld, size: %lu", aico->aioo.handle, aice->u.read.real, aice->u.read.size);

			// post writ to file, FIXME: queued multi-data, multi-sync
			if (!tb_aicp_writ(aicp, context->aico_file, aice->u.read.data, aice->u.read.real)) return tb_false;

			// post read from server
			if (!tb_aicp_read(aicp, aico, context->data_sock, sizeof(context->data_sock))) return tb_false;
		}
		break;
	case TB_AICE_CODE_CLOS:
		{
			// trace
			tb_print("clos[%p]: %ld", aico->aioo.handle, aice->u.clos.ok);

			// post sync to file
//			if (!tb_aicp_sync(aicp, context->aico_file)) return tb_false;

			// post clos to file
			if (!tb_aicp_clos(aicp, context->aico_file)) return tb_false;

			// exit spak
			return tb_false;
		}
		break;
	case TB_AICE_CODE_ERRO:
		{
			// trace
			tb_print("erro[%p]: %ld", aico->aioo.handle, aice->u.erro.code);

			// exit spak
			return tb_false;
		}
		break;
	default:
		tb_assert_and_check_return_val(0, tb_false);
		break;
	}

	// ok
	return tb_true;
}
static tb_bool_t tb_demo_file_work_func(tb_aicp_t* aicp, tb_aico_t const* aico, tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aicp && aico && aice, tb_false);

	// the context
	tb_demo_context_t* context = (tb_demo_context_t*)aico->aioo.odata;
	tb_assert_and_check_return_val(context, tb_false);

	// done
	switch (aice->code)
	{
	case TB_AICE_CODE_WRIT:
		{
			// trace
			tb_print("writ[%p]: real: %ld, size: %lu", aico->aioo.handle, aice->u.writ.real, aice->u.writ.size);
	
		}
		break;
	case TB_AICE_CODE_CLOS:
		{
			// trace
			tb_print("clos[%p]: %ld", aico->aioo.handle, aice->u.clos.ok);

			// exit spak
			return tb_false;
		}
		break;
	case TB_AICE_CODE_ERRO:
		{
			// trace
			tb_print("erro[%p]: %ld", aico->aioo.handle, aice->u.erro.code);

			// exit spak
			return tb_false;
		}
		break;
	default:
		break;
	}

	// ok
	return tb_true;
}
/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// init tbox
	if (!tb_init(malloc(10 * 1024 * 1024), 10 * 1024 * 1024)) return 0;

	// init
	tb_handle_t 		aicp = tb_null;
	tb_demo_context_t 	context = {0};

	// init aicp
	aicp = tb_aicp_init(TB_AIOO_OTYPE_SOCK, 1);
	tb_assert_and_check_goto(aicp, end);

	// open sock
	context.sock = tb_socket_open(TB_SOCKET_TYPE_TCP);
	tb_assert_and_check_goto(context.sock, end);

	// init file
	context.file = tb_file_init(argv[1], TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_BINARY | TB_FILE_MODE_AICP);
	tb_assert_and_check_goto(context.file, end);

	// init aico sock
	context.aico_sock = tb_aicp_addo(aicp, context.sock, tb_demo_sock_work_func, &context);
	tb_assert_and_check_goto(context.aico_sock, end);

	// init aico file
	context.aico_file = tb_aicp_addo(aicp, context.file, tb_demo_file_work_func, &context);
	tb_assert_and_check_goto(context.aico_file, end);

	// post conn
	tb_print("conn: ..");
	if (!tb_aicp_conn(aicp, context.aico_sock, "127.0.0.1", 9090)) goto end;

	// spak aicp
	while (tb_aicp_spak(aicp, 1000) >= 0) ;
	
end:

	// trace
	tb_print("end");

	// exit aicp
	if (aicp) tb_aicp_exit(aicp);

	// exit tbox
	tb_exit();
	return 0;
}
