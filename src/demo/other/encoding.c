#include "tbox.h"

static tb_char_t* load(tb_char_t const* path, tb_size_t* size)
{
	tb_handle_t hfile = tb_file_init(path, TB_FILE_RO);

	tb_byte_t* p = TB_NULL;
	if (hfile)
	{
		tb_int64_t file_size = tb_file_size(hfile);
		if  (file_size <= 0) return 0;
		
		p = (tb_byte_t*)tb_malloc(file_size + 1);
		if (!p) return TB_NULL;

		tb_int64_t read_n = 0;
		while (read_n < file_size) 
		{
			tb_long_t ret = tb_file_read(hfile, p + read_n, (tb_long_t)(file_size - read_n));
			if (ret < 0) break ;
			else read_n += ret;
		}
		tb_file_exit(hfile);
		if (read_n < file_size) 
		{
			tb_free(p);
			return TB_NULL;
		}
		p[read_n] = 0;
		if (size) *size = read_n;
	}
	return p;
}
static tb_void_t save(tb_char_t const* path, tb_byte_t const* data, tb_size_t size)
{
	tb_handle_t hfile = tb_file_init(path, TB_FILE_WO | TB_FILE_CREAT | TB_FILE_TRUNC);
	if (hfile)
	{
		tb_long_t writ_n = 0;
		while (writ_n < size) 
		{
			tb_long_t ret = tb_file_writ(hfile, data + writ_n, (tb_long_t)(size - writ_n));
			if (ret < 0) break ;
			else writ_n += ret;
		}
		tb_file_exit(hfile);
	}
}

int main(int argc, char** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;
	
	tb_size_t src_n, dst_n;

	tb_printf("load: %s\n", argv[1]);
	tb_char_t* src = load(argv[1], &src_n);

	if (src)
	{
		tb_printf("conv(%d): %s\n", src_n, src);
		tb_char_t dst[4096];

#if 1
		//dst_n = tb_encoding_convert_string(TB_ENCODING_UTF8, TB_ENCODING_GB2312, src, src_n, dst, 4096);
		dst_n = tb_encoding_convert_string(TB_ENCODING_GB2312, TB_ENCODING_UTF8, src, src_n, dst, 4096);
#else
		tb_estream_t 	est;
		tb_tstream_t* 	tst = tb_estream_open(&est, TB_ENCODING_GB2312, TB_ENCODING_UTF8);
		tb_bstream_attach(tb_tstream_src(tst), src, src_n);
		tb_bstream_attach(tb_tstream_dst(tst), dst, 4096);
		tb_bstream_t* 	ost = tb_tstream_transform(tst);
		dst_n = tb_bstream_pos(ost) - tb_bstream_beg(ost);
		tb_tstream_close(tst);
#endif


		if (dst_n > 0)
		{
			tb_printf("result(%d): %s\n", dst_n, dst);
			save(argv[2], dst, dst_n);
			tb_printf("save: %s\n", argv[2]);
		}
	}
}

