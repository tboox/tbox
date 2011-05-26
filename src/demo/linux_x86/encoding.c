#include "tbox.h"

static tb_char_t* load(tb_char_t const* path, tb_size_t* size)
{
	tb_handle_t hfile = tb_file_open(path, TB_FILE_RO);

	tb_byte_t* p = TB_NULL;
	if (hfile != TB_INVALID_HANDLE)
	{
		tb_int_t file_size = (tb_int_t)tb_file_seek(hfile, -1, TB_FILE_SEEK_SIZE);
		if  (file_size <= 0) return 0;
		
		p = (tb_byte_t*)tb_malloc(TB_CONFIG_MEMORY_POOL_INDEX, file_size + 1);
		if (!p) return TB_NULL;

		tb_int_t read_n = 0;
		while (read_n < file_size) 
		{
			tb_int_t ret = tb_file_read(hfile, p + read_n, (tb_int_t)(file_size - read_n));
			if (ret < 0) break ;
			else read_n += ret;
		}
		tb_file_close(hfile);
		if (read_n < file_size) 
		{
			tb_free(1, p);
			return TB_NULL;
		}
		p[read_n] = 0;
		if (size) *size = read_n;
	}
	return p;
}
static void save(tb_char_t const* path, tb_byte_t const* data, tb_size_t size)
{
	tb_handle_t hfile = tb_file_open(path, TB_FILE_WO | TB_FILE_CREAT | TB_FILE_TRUNC);
	if (hfile != TB_INVALID_HANDLE)
	{
		tb_int_t write_n = 0;
		while (write_n < size) 
		{
			tb_int_t ret = tb_file_write(hfile, data + write_n, (tb_int_t)(size - write_n));
			if (ret < 0) break ;
			else write_n += ret;
		}
		tb_file_close(hfile);
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

