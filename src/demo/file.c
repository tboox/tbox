/* ///////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"

/* ///////////////////////////////////////////////////////////////////
 * writ
 */
static tb_bool_t tb_test_file_writ(tb_handle_t ofile, tb_byte_t* data, tb_size_t size, tb_eobject_t* oo)
{
	tb_size_t writ = 0;
	tb_bool_t wait = TB_FALSE;
	while (writ < size)
	{
		// try to writ data
		tb_long_t n = tb_file_writ(ofile, data + writ, size - writ);
		if (n > 0)
		{
			// update writ
			writ += n;

			// no waiting
			wait = TB_FALSE;
		}
		else if (!n && !wait)
		{
			// waiting...
			tb_long_t etype = tb_eobject_wait(&oo, 10000);

			// error?
			tb_check_break(etype >= 0);

			// timeout?
			tb_check_break(etype);

			// has writ?
			tb_assert_and_check_break(etype & TB_ETYPE_WRIT);

			// be waiting
			wait = TB_TRUE;
		}
		else break;
	}
	return writ == size? TB_TRUE : TB_FALSE;
}
/* ///////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// init file
	tb_handle_t ifile = tb_file_init(argv[1], TB_FILE_RO);
	tb_handle_t ofile = tb_file_init(argv[2], TB_FILE_WO | TB_FILE_CREAT | TB_FILE_TRUNC);
	tb_assert_and_check_goto(ifile && ofile, end);

	// file size
	tb_uint64_t isize = tb_file_size(ifile);
	tb_assert_and_check_goto(isize, end);

	// init eobject
	tb_eobject_t io;
	tb_eobject_t oo;
	if (!tb_eobject_init(&io, TB_EOTYPE_FILE, TB_ETYPE_READ, ifile)) goto end;
	if (!tb_eobject_init(&oo, TB_EOTYPE_FILE, TB_ETYPE_WRIT, ofile)) goto end;

	// read file
	tb_byte_t 	data[4096];
	tb_uint64_t read = 0;
	tb_bool_t 	wait = TB_FALSE;
	while (1)//read < isize)
	{
		// try to read data
		tb_long_t n = tb_file_read(ifile, data, 4096);
		if (n > 0)
		{
			// writ data
			if (!tb_test_file_writ(ofile, data, n, &oo)) break;;

			// update read
			read += n;

			// no waiting
			wait = TB_FALSE;
		}
		else if (!n)
		{
			// end?
			if (wait)
			{
				tb_print("end");
				break;
			}

			// waiting...
			tb_print("waiting...");
			tb_long_t etype = tb_eobject_wait(&io, 10000);

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

			// closed?
			if (etype == TB_ETYPE_EXIT)
			{
				tb_print("closed");
				break;
			}

			// has read?
			tb_assert_and_check_break(etype & TB_ETYPE_READ);

			// be waiting
			wait = TB_TRUE;
		}
		else break;
	}

end:

	// trace
	tb_print("size: %llu, read: %llu", isize, read);

	// exit eobject
	if (ifile) tb_eobject_exit(&io);
	if (ofile) tb_eobject_exit(&oo);

	// exit file
	if (ifile) tb_file_exit(ifile);
	if (ofile) tb_file_exit(ofile);

	// exit
	tb_exit();
	return 0;
}
