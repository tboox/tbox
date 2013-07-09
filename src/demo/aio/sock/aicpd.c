/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_WORK_MAXN 		(3)

/* ///////////////////////////////////////////////////////////////////////
 * aicb
 */
static tb_bool_t tb_aicb_work_func(tb_aicp_t* aicp, tb_aico_t const* aico, tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aicp && aico && aice, tb_false);

	switch (aice->code)
	{
	case TB_AICE_CODE_ACPT:
		tb_print("acpt: sock: %p", aice->u.acpt.sock);
		if (!tb_aicp_acpt(aicp, aico)) return tb_false;
		if (!tb_aicp_read(aicp, aico, tb_malloc(1024), 1024)) return tb_false;
		break;
	case TB_AICE_CODE_READ:
		tb_print("read: size: %ld maxn: %lu, data: %s", aice->u.read.size, aice->u.read.maxn, aice->u.read.data);
		if (!tb_aicp_writ(aicp, aico, "ok", 3)) return tb_false;
		break;
	case TB_AICE_CODE_WRIT:
		tb_print("writ: size: %ld maxn: %lu, data: %s", aice->u.read.size, aice->u.writ.maxn, aice->u.writ.data);
		break;
	default:
		break;
	}

	// ok
	return tb_true;
}

/* ///////////////////////////////////////////////////////////////////////
 * work
 */
static tb_pointer_t tb_work_thread(tb_pointer_t cb_data)
{
	tb_size_t* data = (tb_size_t*)cb_data;
	tb_assert_and_check_goto(data, end);

	// id
	tb_size_t id = data[1];
	tb_print("[demo]: work[%d]: init", id);

	// aicp
	tb_handle_t aicp = (tb_handle_t)data[0];
	tb_assert_and_check_goto(aicp, end);

	// done
	while (tb_aicp_done(aicp)) tb_msleep(200);

end:
	// exit work
	tb_print("[demo]: work[%d]: exit", id);
	tb_thread_return(tb_null);
	return tb_null;
}

/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// init
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// init
	tb_size_t 		i = 0;
	tb_size_t 		data[2] = {0};
	tb_handle_t 	work[2] = {tb_null};
	tb_handle_t 	sock = tb_null;
	tb_handle_t 	aicp = tb_null;

	// open sock
	sock = tb_socket_open(TB_SOCKET_TYPE_TCP);
	tb_assert_and_check_goto(sock, end);

	// bind port
	tb_print("[demo]: bind: %u", tb_stou32(argv[1]));
	if (!tb_socket_bind(sock, tb_stou32(argv[1]))) goto end;

	// init aicp
	aicp = tb_aicp_init(TB_AIOO_OTYPE_SOCK, 16);
	tb_assert_and_check_goto(aicp, end);

	// init work
	data[0] = (tb_size_t)aicp;
	for (i = 0; i < TB_WORK_MAXN; i++)
	{
		data[1] 	= (tb_size_t)i;
		work[i] 	= tb_thread_init(tb_null, tb_work_thread, data, 256 * 1024);
		tb_assert_and_check_goto(work[i], end);
		tb_msleep(500);
	}

	// addo sock
	tb_aico_t const* aico = tb_aicp_addo(aicp, sock, tb_aicb_work_func, "work");
	tb_assert_and_check_goto(aico, end);

	// post acpt
	if (!tb_aicp_acpt(aicp, aico)) goto end;

	// spak
	while (tb_aicp_spak(aicp)) ;

end:

	// kill work
	tb_aicp_kill(aicp);
	for (i = 0; i < TB_WORK_MAXN; i++)
	{
		if (work[i]) 
		{
			if (!tb_thread_wait(work[i], 5000))
				tb_thread_kill(work[i]);
			tb_thread_exit(work[i]);
		}
	}

	// close sock
	if (sock) tb_socket_close(sock);

	// exit aicp
	if (aicp) tb_aicp_exit(aicp);

	// exit
	tb_exit();
	return 0;
}
