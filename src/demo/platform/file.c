/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* ///////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_platform_file_main(tb_int_t argc, tb_char_t** argv)
{
#if 0
	// init file
//	tb_handle_t file = tb_file_init(argv[1], TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_BINARY | TB_FILE_MODE_TRUNC);
	tb_handle_t file = tb_file_init(argv[1], TB_FILE_MODE_DIRECT | TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_BINARY | TB_FILE_MODE_TRUNC);
	if (file)
	{
		// done
		tb_size_t 	align = TB_FILE_DIRECT_ASIZE;
		tb_size_t 	writ = 0;
		tb_size_t 	size = 512 * 1024 * 1024;
		tb_size_t 	maxn = 8096;
//		tb_size_t 	maxn = (1 << 17);
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
//				tb_trace_i("real: %ld, size: %lu", real, tb_min(maxn, size - writ));
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
			tb_trace_i("writ: %lld s", time / 1000);
			time = tb_mclock();
		}

		// exit file
		tb_file_exit(file);
	}
#elif 0
	// init file
	tb_handle_t file = tb_file_init(argv[1], TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_BINARY | TB_FILE_MODE_TRUNC);
	if (file)
	{
		// done
		tb_size_t 	writ = 0;
		tb_size_t 	size = 512 * 1024 * 1024;
		tb_size_t 	maxn = 8096;
		tb_iovec_t 	list[4];
		tb_hong_t 	time = tb_mclock();
		tb_byte_t* 	data = tb_malloc(maxn << 2);

		// init iovec
		list[0].data = tb_malloc(maxn);
		list[1].data = tb_malloc(maxn);
		list[2].data = tb_malloc(maxn);
		list[3].data = tb_malloc(maxn);
		list[0].size = maxn;
		list[1].size = maxn;
		list[2].size = maxn;
		list[3].size = maxn;

		// writ file
		while (writ < size)
		{
			// size
			tb_memcpy(data, list[0].data, list[0].size);
			tb_memcpy(data + maxn, list[1].data, list[1].size);
			tb_memcpy(data + maxn + maxn, list[2].data, list[2].size);
			tb_memcpy(data + maxn + maxn + maxn, list[3].data, list[3].size);

			// writ
			tb_long_t real = tb_file_writ(file, data, tb_min((size - writ), (maxn << 2)));
			if (real > 0) writ += real;
			else if (!real) ;
			else break;
		}

		// exit data
		tb_free(list[0].data);
		tb_free(list[1].data);
		tb_free(list[2].data);
		tb_free(list[3].data);
		tb_free(data);

		// sync
		tb_file_sync(file);

		// ok
		if (writ == size)
		{
			// trace
			time = tb_mclock() - time;
			tb_trace_i("writ: %lld s", time / 1000);
			time = tb_mclock();
		}

		// exit file
		tb_file_exit(file);
	}
#elif 0
	// init file
	tb_handle_t file = tb_file_init(argv[1], TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_BINARY | TB_FILE_MODE_TRUNC);
	if (file)
	{
		// done
		tb_size_t 	writ = 0;
		tb_size_t 	size = 512 * 1024 * 1024;
		tb_size_t 	maxn = 8096;
		tb_iovec_t 	list[4];
		tb_hong_t 	time = tb_mclock();

		// init iovec
		list[0].data = tb_malloc(maxn);
		list[1].data = tb_malloc(maxn);
		list[2].data = tb_malloc(maxn);
		list[3].data = tb_malloc(maxn);
		list[0].size = maxn;
		list[1].size = maxn;
		list[2].size = maxn;
		list[3].size = maxn;

		// writ file
		while (writ < size)
		{
			// size
			tb_size_t left = size - writ;
			if (left < (maxn << 2))
			{
				list[0].size = left >= maxn? maxn : left; left -= list[0].size;
				list[1].size = left >= maxn? maxn : left; left -= list[1].size;
				list[2].size = left >= maxn? maxn : left; left -= list[2].size;
				list[3].size = left >= maxn? maxn : left; left -= list[3].size;
			}

			// writ
			tb_long_t real = tb_file_writv(file, list, 4);
			if (real > 0) writ += real;
			else if (!real) ;
			else break;
		}

		// exit data
		tb_free(list[0].data);
		tb_free(list[1].data);
		tb_free(list[2].data);
		tb_free(list[3].data);

		// sync
		tb_file_sync(file);

		// ok
		if (writ == size)
		{
			// trace
			time = tb_mclock() - time;
			tb_trace_i("writ: %lld s", time / 1000);
			time = tb_mclock();
		}

		// exit file
		tb_file_exit(file);
	}
#elif 0
	tb_handle_t ifile = tb_file_init(argv[1], TB_FILE_MODE_RW | TB_FILE_MODE_BINARY);
	tb_handle_t ofile = tb_file_init(argv[2], TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_BINARY | TB_FILE_MODE_TRUNC);
	if (ifile && ofile)
	{
		tb_hize_t writ = 0;
		tb_hize_t size = tb_file_size(ifile);
		while (writ < size)
		{
			tb_long_t real = tb_file_writf(ofile, ifile, writ, size - writ);
			if (real > 0) writ += real;
			else break;
		}
	}
		
	// exit file
	if (ifile) tb_file_exit(ifile);
	if (ofile) tb_file_exit(ofile);

#else
	// copy
	tb_file_copy(argv[1], argv[2]);
#endif

	return 0;
}
