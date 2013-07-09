/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * writ
 */
static tb_bool_t tb_test_file_writ(tb_handle_t ofile, tb_aioo_t* oo, tb_byte_t* data, tb_size_t size)
{
	tb_size_t writ = 0;
	tb_bool_t wait = tb_false;
	while (writ < size)
	{
		// try to writ data
		tb_long_t n = tb_file_writ(ofile, data + writ, size - writ);
		if (n > 0)
		{
			// update writ
			writ += n;

			// no waiting
			wait = tb_false;
		}
		else if (!n && !wait)
		{
			// waiting...
			tb_long_t etype = tb_aioo_wait(oo, 10000);

			// error?
			tb_check_break(etype >= 0);

			// timeout?
			tb_check_break(etype);

			// has writ?
			tb_assert_and_check_break(etype & TB_AIOO_ETYPE_WRIT);

			// be waiting
			wait = tb_true;
		}
		else break;
	}
	return writ == size? tb_true : tb_false;
}
/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// init file
	tb_handle_t ifile = tb_file_init(argv[1], TB_FILE_MODE_RO);
	tb_handle_t ofile = tb_file_init(argv[2], TB_FILE_MODE_WO | TB_FILE_MODE_CREAT | TB_FILE_MODE_TRUNC);
	tb_assert_and_check_goto(ifile && ofile, end);

	// file size
	tb_hize_t isize = tb_file_size(ifile);
	tb_assert_and_check_goto(isize, end);

	// init aio
	tb_aioo_t io;
	tb_aioo_t oo;
	tb_aioo_seto(&io, ifile, TB_AIOO_OTYPE_FILE, TB_AIOO_ETYPE_READ, tb_null);
	tb_aioo_seto(&oo, ofile, TB_AIOO_OTYPE_FILE, TB_AIOO_ETYPE_WRIT, tb_null);

	// read file
	tb_byte_t 	data[4096];
	tb_hize_t read = 0;
	tb_bool_t 	wait = tb_false;
	while (1)//read < isize)
	{
		// try to read data
		tb_long_t n = tb_file_read(ifile, data, 4096);
		if (n > 0)
		{
			// writ data
			if (!tb_test_file_writ(ofile, &oo, data, n)) break;;

			// update read
			read += n;

			// no waiting
			wait = tb_false;
		}
		else if (!n)
		{
			// end?
			if (wait)
			{
				tb_print("eof");
				break;
			}

			// waiting...
			tb_print("waiting...");
			tb_long_t etype = tb_aioo_wait(&io, 10000);

			// error?
			if (etype < 0)
			{
				tb_print("error");
				break;
			}

			// timeout?
			if (!etype)
			{
				tb_print("timeout");
				break;
			}

			// has read?
			tb_assert_and_check_break(etype & TB_AIOO_ETYPE_READ);

			// be waiting
			wait = tb_true;
		}
		else break;
	}

end:

	// trace
	tb_print("size: %llu, read: %llu", isize, read);

	// exit file
	if (ifile) tb_file_exit(ifile);
	if (ofile) tb_file_exit(ofile);

	// exit
	tb_exit();
	return 0;
}
