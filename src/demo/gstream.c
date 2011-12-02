/* ///////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"

/* ///////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_GSTREAM_TEST_SPEED 		(1)

/* ///////////////////////////////////////////////////////////////////
 * callback
 */
static tb_bool_t http_callback_head(tb_char_t const* line, tb_pointer_t priv)
{
	tb_print("[http]: head: %s", line);
	return TB_TRUE;
}

/* ///////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// create stream
	tb_gstream_t* ist = tb_gstream_create_from_url(argv[1]);
	tb_gstream_t* ost = tb_gstream_create_from_url(argv[2]);
	if (!ist || !ost) goto end;
	
	// open stream
#if TB_GSTREAM_TEST_SPEED
	tb_int64_t itime = tb_mclock();
	if (!tb_gstream_open(ist)) goto end;
	itime = tb_mclock() - itime;
	tb_print("[gst]: open ist: %llu ms", itime);
#else
	tb_gstream_ioctl2(ist, TB_HSTREAM_CMD_SET_HEAD_FUNC, http_callback_head, TB_NULL);
	tb_gstream_ioctl1(ost, TB_FSTREAM_CMD_SET_FLAGS, TB_FILE_WO | TB_FILE_CREAT | TB_FILE_TRUNC);

	if (!tb_gstream_open(ist)) goto end;
	if (!tb_gstream_open(ost)) goto end;
#endif

#if 0
	// save stream
	tb_uint64_t size = tb_gstream_save(ist, ost);
	tb_print("save: %llu bytes", size);
#else
	// read data
	tb_byte_t 		data[TB_GSTREAM_CACHE_MAXN];
	tb_uint64_t 	read = 0;
	tb_uint64_t 	left = tb_gstream_left(ist);
	tb_int64_t 		time = tb_mclock();
	tb_int64_t 		base = tb_mclock();
	tb_int64_t 		basc = tb_mclock();
	do
	{
		// read data
		tb_long_t n = tb_gstream_read(ist, data, TB_GSTREAM_CACHE_MAXN);
//		tb_trace("read: %d, offset: %llu, left: %llu, size: %llu", n, tb_gstream_offset(ist), tb_gstream_left(ist), tb_gstream_size(ist));
		if (n > 0)
		{
			// update clock
			time = tb_mclock();

#if !TB_GSTREAM_TEST_SPEED
			// writ data
			if (n != tb_gstream_bwrit(ost, data, n)) break;
#endif

			// update read
			read += n;
		}
		else if (!n) 
		{
			// timeout?
			if (tb_mclock() - time > ist->timeout) break;

			// sleep some time
			tb_usleep(100);
		}
		else break;

		// is end?
		if (left && read >= left) break;

#if TB_GSTREAM_TEST_SPEED
		// print info
		if (tb_mclock() - basc > 5000) 
		{
			tb_print("[gst]: load: %llu bytes, speed: %llu bytes / s", tb_gstream_offset(ist), (tb_gstream_offset(ist) * 1000) / (tb_mclock() - base));
			basc = tb_mclock();
		}
#endif

	} while(1);

	tb_print("[gst]: load: %llu bytes, size: %llu bytes", read, tb_gstream_size(ist));
#endif

end:

	// destroy stream
	tb_gstream_destroy(ist);
	tb_gstream_destroy(ost);

	tb_exit();
	return 0;
}
