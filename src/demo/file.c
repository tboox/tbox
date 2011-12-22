/* ///////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"

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
	tb_eobject_t o;
	o.otype = TB_EOTYPE_FILE;
	o.etype = TB_ETYPE_READ;
	o.handle = ifile;

	// read file
	tb_byte_t 	data[4096];
	tb_uint64_t read = 0;
	while (read < size)
	{
		// try to read data
		tb_long_t n = tb_file_read(ifile, data, 4096);
		if (n > 0)
		{
			// writ data


			// update read
			read += n;
		}
		else if (!n)
		{
			// waiting read...
			if (!tb_eobject_wait(&o, -1)) break;
		}
		else break;
	}

end:

	// trace
	tb_print("size: %llu, read: %llu, writ: %llu", size, read, writ);

	// exit file
	if (ifile) tb_file_exit(ifile);
	if (ofile) tb_file_exit(ofile);

	// exit
	tb_exit();
	return 0;
}
