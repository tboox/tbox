/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"

/* ///////////////////////////////////////////////////////////////////////
 * callback
 */
static tb_bool_t tb_http_test_hfunc(tb_http_option_t* option, tb_char_t const* line)
{
	tb_print("[demo]: response: %s", line);
	return TB_TRUE;
}
/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// create stream
	tb_gstream_t* ist = tb_gstream_init_from_url(argv[1]);
	tb_gstream_t* ost = tb_gstream_init_from_url(argv[2]);
	if (!ist || !ost) goto end;
	
	// ioctl
	if (tb_gstream_type(ist) == TB_GSTREAM_TYPE_HTTP) 
	{
		tb_http_option_t* option = TB_NULL;
		tb_gstream_ctrl1(ist, TB_HSTREAM_CMD_GET_OPTION, &option);
		if (option) option->hfunc = tb_http_test_hfunc;
	}
	if (tb_gstream_type(ost) == TB_GSTREAM_TYPE_FILE) tb_gstream_ctrl1(ost, TB_FSTREAM_CMD_SET_FLAGS, TB_FILE_WO | TB_FILE_CREAT | TB_FILE_TRUNC);

	// open stream
	tb_int64_t itime = tb_mclock();
	if (!tb_gstream_bopen(ist)) goto end;
	itime = tb_mclock() - itime;
	tb_print("[gst]: open ist: %llu ms", itime);

	tb_int64_t otime = tb_mclock();
	if (!tb_gstream_bopen(ost)) goto end;
	otime = tb_mclock() - otime;
	tb_print("[gst]: open ost: %llu ms", otime);

#if 0
	// save stream
	tb_uint64_t size = tb_gstream_save(ist, ost);
	tb_print("save: %llu bytes", size);
#else
	// read data
	tb_byte_t 		data[TB_GSTREAM_BLOCK_MAXN];
	tb_uint64_t 	read = 0;
	tb_bool_t 		wait = TB_FALSE;
	tb_uint64_t 	left = tb_gstream_left(ist);
	tb_int64_t 		base = tb_mclock();
	tb_int64_t 		basc = tb_mclock();
	do
	{
		// read data
		tb_long_t n = tb_gstream_aread(ist, data, TB_GSTREAM_BLOCK_MAXN);
//		tb_trace("read: %d, offset: %llu, left: %llu, size: %llu", n, tb_gstream_offset(ist), tb_gstream_left(ist), tb_gstream_size(ist));
		if (n > 0)
		{
			// no waiting
			wait = TB_FALSE;

			// writ data
			if (!tb_gstream_bwrit(ost, data, n)) break;

			// update read
			read += n;
		}
		else if (!n) 
		{
			// no end?
			tb_check_break(!wait);

			// wait
			tb_long_t e = tb_gstream_wait(ist, TB_AIOO_ETYPE_READ, tb_gstream_timeout(ist));
			tb_assert_and_check_break(e >= 0);

			// timeout?
			tb_check_break(e);

			// has read?
			tb_assert_and_check_break(e & TB_AIOO_ETYPE_READ);

			// be waiting
			wait = TB_TRUE;
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

	tb_print("[gst]: load: %llu bytes, size: %llu bytes, time: %llu ms", read, tb_gstream_size(ist), tb_mclock() - base);
#endif

end:

	// destroy stream
	tb_gstream_exit(ist);
	tb_gstream_exit(ost);

	tb_exit();
	return 0;
}
