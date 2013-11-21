/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// init
	if (!tb_init(malloc(10 * 1024 * 1024), 10 * 1024 * 1024)) return 0;

	// copy
//	tb_file_copy(argv[1], argv[2]);

	// init file
	tb_handle_t file = tb_file_init(argv[1], TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_BINARY | TB_FILE_MODE_TRUNC);
//	tb_handle_t file = tb_file_init(argv[1], TB_FILE_MODE_DIRECT | TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_BINARY | TB_FILE_MODE_TRUNC);
	if (file)
	{
		// done
		tb_size_t 	align = TB_FILE_DIRECT_ASIZE;
		tb_size_t 	writ = 0;
		tb_size_t 	size = 512 * 1024 * 1024;
		tb_size_t 	maxn = (1 << 17);
		tb_byte_t* 	data = tb_malloc(maxn + align);
		tb_hong_t 	time = tb_mclock();
		if (data)
		{
			// align
			tb_byte_t* buff = (tb_byte_t*)tb_align((tb_hize_t)data, align);

			// writ file
			while (writ < size)
			{
				tb_long_t real = tb_file_writ(file, buff, tb_min(maxn, size - writ));
//				tb_print("real: %ld, size: %lu", real, tb_min(maxn, size - writ));
				if (real > 0) writ += real;
				else if (!real) ;
				else break;
			}

			// exit data
			tb_free(data);
		}

		// sync
		tb_file_sync(file);

		// ok
		if (writ == size)
		{
			// trace
			time = tb_mclock() - time;
			tb_print("writ: %lld s", time / 1000);
			time = tb_mclock();
		}

		// exit file
		tb_file_exit(file);
	}

	// exit
	tb_exit();
	return 0;
}
