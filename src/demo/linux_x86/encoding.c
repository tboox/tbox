#include "tplat/tplat.h"
#include "../../tbox.h"

static tplat_char_t* load(tplat_char_t const* path, tplat_size_t* size)
{
	tplat_handle_t hfile = tplat_file_open(path, TPLAT_FILE_RO);

	tplat_byte_t* p = TPLAT_NULL;
	if (hfile != TPLAT_INVALID_HANDLE)
	{
		tplat_int_t file_size = (tplat_int_t)tplat_file_seek(hfile, -1, TPLAT_FILE_SEEK_SIZE);
		if  (file_size <= 0) return 0;
		
		p = (tplat_byte_t*)tplat_malloc(TB_CONFIG_MEMORY_POOL_INDEX, file_size + 1);
		if (!p) return TPLAT_NULL;

		tplat_int_t read_n = 0;
		while (read_n < file_size) 
		{
			tplat_int_t ret = tplat_file_read(hfile, p + read_n, (tplat_int_t)(file_size - read_n));
			if (ret < 0) break ;
			else read_n += ret;
		}
		tplat_file_close(hfile);
		if (read_n < file_size) 
		{
			tplat_free(1, p);
			return TPLAT_NULL;
		}
		p[read_n] = 0;
		if (size) *size = read_n;
	}
	return p;
}
static void save(tplat_char_t const* path, tplat_byte_t const* data, tplat_size_t size)
{
	tplat_handle_t hfile = tplat_file_open(path, TPLAT_FILE_WO | TPLAT_FILE_CREAT | TPLAT_FILE_TRUNC);
	if (hfile != TPLAT_INVALID_HANDLE)
	{
		tplat_int_t write_n = 0;
		while (write_n < size) 
		{
			tplat_int_t ret = tplat_file_write(hfile, data + write_n, (tplat_int_t)(size - write_n));
			if (ret < 0) break ;
			else write_n += ret;
		}
		tplat_file_close(hfile);
	}
}

int main(int argc, char** argv)
{
	tplat_size_t regular_block_n[TPLAT_POOL_REGULAR_CHUNCK_MAX_COUNT] = {10, 10, 10, 10, 10, 10, 10};
	tplat_pool_create(TB_CONFIG_MEMORY_POOL_INDEX, malloc(1024 * 1024), 1024 * 1024, regular_block_n);
	
	tplat_size_t src_n, dst_n;

	tplat_printf("load: %s\n", argv[1]);
	tplat_char_t* src = load(argv[1], &src_n);

	if (src)
	{
		tplat_printf("conv(%d): %s\n", src_n, src);
		tplat_char_t dst[4096];
		dst_n = tb_encoding_convert_string(TB_ENCODING_UTF8, TB_ENCODING_GB2312, src, src_n, dst, 4096);
		//dst_n = tb_encoding_convert_string(TB_ENCODING_GB2312, TB_ENCODING_UTF8, src, src_n, dst, 4096);
		if (dst_n > 0)
		{
			tplat_printf("result(%d): %s\n", dst_n, dst);
			save(argv[2], dst, dst_n);
			tplat_printf("save: %s\n", argv[2]);
		}
		tplat_free(TB_CONFIG_MEMORY_POOL_INDEX, src);
	}
}

