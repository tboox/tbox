/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"

/* ///////////////////////////////////////////////////////////////////////
 * aicb
 */
static tb_bool_t tb_aicb_work_func(tb_aicp_t* aicp, tb_aico_t const* aico, tb_aice_t const* aice)
{


	// ok
	return TB_TRUE;
}
static tb_bool_t tb_aicb_acpt_func(tb_aicp_t* aicp, tb_aico_t const* aico, tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aicp && aico && aice, TB_FALSE);
	tb_assert_and_check_return_val(aice->code == TB_AICE_CODE_ACPT, TB_FALSE);

	// trace
	tb_print("[demo]: acpt: ok: %s, data: %s", aice->ok, aico->aioo.odata);

	// ok
	return TB_TRUE;
}

/* ///////////////////////////////////////////////////////////////////////
 * work
 */
static tb_pointer_t tb_work_thread(tb_pointer_t cb_data)
{
	tb_size_t* data = (tb_size_t*)cb_data;
	tb_assert_and_check_goto(data, end);
	tb_print("[demo]: work: init");

	// aicp
	tb_handle_t aicp = (tb_handle_t)data[0];
	tb_assert_and_check_goto(aicp, end);

	// loop
	while (!tb_atomic_get(&data[1]))
	{
		// spak
		tb_long_t r = tb_aicp_spak(aicp);
		tb_assert_and_check_goto(r >= 0, end);
		
		// no aice?
		tb_check_continue(!r);

		// wait
		r = tb_aicp_wait(aicp, -1);
		tb_assert_and_check_goto(r >= 0, end);
	}

end:
	// exit work
	tb_print("[demo]: work: exit");
	tb_thread_return(TB_NULL);
	return TB_NULL;
}

/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// init
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// init
	tb_size_t 		data[2] = {0};
	tb_handle_t 	sock = TB_NULL;
	tb_handle_t 	work = TB_NULL;
	tb_handle_t 	aicp = TB_NULL;
	tb_handle_t 	aico = TB_NULL;
	tb_aice_t 		aice = {0};
	tb_aioo_t 		aioo = {0};

	// open sock
	sock = tb_socket_open(TB_SOCKET_TYPE_TCP);
	tb_assert_and_check_goto(sock, end);

	// bind port
	tb_print("[demo]: bind: %u", tb_stou32(argv[1]));
	if (!tb_socket_bind(sock, tb_stou32(argv[1]))) goto end;

	// init aioo
	tb_aioo_seto(&aioo, sock, TB_AIOO_OTYPE_SOCK, TB_AIOO_ETYPE_ACPT, TB_NULL);

	// init aicp
	aicp = tb_aicp_init(TB_AIOO_OTYPE_SOCK, 16);
	tb_assert_and_check_goto(aicp, end);

	// init aico
	aico = tb_aicp_addo(aicp, sock, tb_aicb_acpt_func, "acpt");
	tb_assert_and_check_goto(aico, end);

	// init work
	data[0] = (tb_size_t)aicp;
	data[1] = (tb_size_t)0;
	work 	= tb_thread_init(TB_NULL, tb_work_thread, data, 0);
	tb_assert_and_check_goto(work, end);

	// listen...
	while (1)
	{
		// wait
		tb_print("[demo]: aicp: wait");
		tb_long_t r = tb_aioo_wait(&aioo, -1);
		tb_assert_and_check_goto(r > 0, end);

		// acpt
		tb_print("[demo]: acpt: post");
		tb_aicp_acpt(aicp, aico);
	}

end:

	// kill work
	if (work) 
	{
		tb_atomic_set(&data[1], 1);
		if (!tb_thread_wait(work, 5000))
			tb_thread_kill(work);
		tb_thread_exit(work);
	}

	// close sock
	if (sock) tb_socket_close(sock);

	// exit aicp
	if (aicp) tb_aicp_exit(aicp);

	// exit
	tb_exit();
	return 0;
}
