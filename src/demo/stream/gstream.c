/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * callback
 */
static tb_bool_t tb_gstream_demo_hfunc(tb_handle_t http, tb_char_t const* line)
{
	tb_print("[demo]: response: %s", line);
	return tb_true;
}
static tb_handle_t tb_gstream_demo_sfunc_init(tb_handle_t gst)
{
	tb_print("[demo]: ssl: init: %p", gst);
	tb_handle_t sock = tb_null;
	if (gst && tb_gstream_type(gst) == TB_GSTREAM_TYPE_SOCK) 
		tb_gstream_ctrl(gst, TB_SSTREAM_CTRL_GET_HANDLE, &sock);
	return sock;
}
static tb_void_t tb_gstream_demo_sfunc_exit(tb_handle_t ssl)
{
	tb_print("[demo]: ssl: exit");
}
static tb_long_t tb_gstream_demo_sfunc_read(tb_handle_t ssl, tb_byte_t* data, tb_size_t size)
{
	tb_print("[demo]: ssl: read: %lu", size);
	return ssl? tb_socket_recv(ssl, data, size) : -1;
}
static tb_long_t tb_gstream_demo_sfunc_writ(tb_handle_t ssl, tb_byte_t const* data, tb_size_t size)
{
	tb_print("[demo]: ssl: writ: %lu", size);
	return ssl? tb_socket_send(ssl, data, size) : -1;
}

/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// init tbox
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// init stream
	tb_gstream_t* ist = tb_gstream_init_from_url(argv[1]);
	tb_gstream_t* ost = tb_gstream_init_from_url(argv[2]);
	tb_assert_and_check_return_val(ist && ost, 0);
	
	// init sfunc
	tb_gstream_sfunc_t sfunc = 
	{
		tb_gstream_demo_sfunc_init
	,	tb_gstream_demo_sfunc_exit
	,	tb_gstream_demo_sfunc_read
	,	tb_gstream_demo_sfunc_writ
	};

	// ctrl
	if (tb_gstream_type(ist) == TB_GSTREAM_TYPE_HTTP) 
	{
#if 0
		// init option
		tb_http_option_t* option = tb_null;
		tb_gstream_ctrl(ist, TB_HSTREAM_CTRL_GET_OPTION, &option);
		if (option)
		{
			// init hfunc
			option->hfunc = tb_gstream_demo_hfunc;

			// init method
			option->method = TB_HTTP_METHOD_POST;

			// init post
			option->post = tb_strlen(argv[3]);
		}

		// init sfunc
		tb_gstream_ctrl(ist, TB_GSTREAM_CTRL_SET_SFUNC, &sfunc);
#endif
	}
	if (tb_gstream_type(ost) == TB_GSTREAM_TYPE_FILE) 
		tb_gstream_ctrl(ost, TB_FSTREAM_CTRL_SET_MODE, TB_FILE_MODE_WO | TB_FILE_MODE_CREAT | TB_FILE_MODE_TRUNC);

	// open stream
	tb_hong_t itime = tb_mclock();
	if (!tb_gstream_bopen(ist)) goto end;
	itime = tb_mclock() - itime;

	tb_hong_t otime = tb_mclock();
	if (!tb_gstream_bopen(ost)) goto end;
	otime = tb_mclock() - otime;

	// writ post
//	tb_gstream_bwrit(ist, argv[3], tb_strlen(argv[3]));
//	tb_gstream_bfwrit(ist, tb_null, 0);

	// read data
	tb_byte_t 		data[TB_GSTREAM_BLOCK_MAXN];
	tb_hize_t 		read = 0;
	tb_hize_t 		left = tb_gstream_left(ist);
	tb_hong_t 		base = tb_mclock();
	tb_hong_t 		basc = tb_mclock();
	do
	{
		// read data
		tb_long_t n = tb_gstream_aread(ist, data, TB_GSTREAM_BLOCK_MAXN);
//		tb_trace("read: %d, offset: %llu, left: %llu, size: %llu", n, tb_gstream_offset(ist), tb_gstream_left(ist), tb_gstream_size(ist));
		if (n > 0)
		{
			// writ data
			if (!tb_gstream_bwrit(ost, data, n)) break;

			// update read
			read += n;
		}
		else if (!n) 
		{
			// wait
			tb_long_t e = tb_gstream_wait(ist, TB_AIOO_ETYPE_READ, tb_gstream_timeout(ist));
			tb_assert_and_check_break(e >= 0);

			// timeout?
			tb_check_break(e);

			// has read?
			tb_assert_and_check_break(e & TB_AIOO_ETYPE_READ);
		}
		else break;

		// is end?
		if (left && read >= left) break;

		// print info
		if (tb_mclock() - basc > 5000) 
		{
			tb_print("[gst]: load: %llu bytes, speed: %llu bytes / s", tb_gstream_offset(ist), (tb_gstream_offset(ist) * 1000) / (tb_mclock() - base));
			basc = tb_mclock();
		}

	} while(1);

end:

	// trace
	tb_print("[gst]: load: %llu bytes, size: %llu bytes, time: %llu ms, state: %s", read, tb_gstream_size(ist), tb_mclock() - base, tb_gstream_state_cstr(ist));

	// exit stream
	tb_gstream_exit(ist);
	tb_gstream_exit(ost);

	// exit tbox
	tb_exit();
	return 0;
}
