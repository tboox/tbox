#include "eplat/eplat.h"
#include "../../tbox.h"

static eplat_char_t* load(eplat_char_t const* path, eplat_size_t* size)
{
	eplat_handle_t hfile = eplat_file_open(path, EPLAT_FILE_RO);

	eplat_byte_t* p = EPLAT_NULL;
	if (hfile != EPLAT_INVALID_HANDLE)
	{
		eplat_int_t file_size = (eplat_int_t)eplat_file_seek(hfile, -1, EPLAT_FILE_SEEK_SIZE);
		if  (file_size <= 0) return 0;
		
		p = (eplat_byte_t*)eplat_malloc(TB_CONFIG_MEMORY_POOL_INDEX, file_size + 1);
		if (!p) return EPLAT_NULL;

		eplat_int_t read_n = 0;
		while (read_n < file_size) 
		{
			eplat_int_t ret = eplat_file_read(hfile, p + read_n, (eplat_int_t)(file_size - read_n));
			if (ret < 0) break ;
			else read_n += ret;
		}
		eplat_file_close(hfile);
		if (read_n < file_size) 
		{
			eplat_free(1, p);
			return EPLAT_NULL;
		}
		p[read_n] = 0;
		if (size) *size = read_n;
	}
	return p;
}
static void save(eplat_char_t const* path, eplat_byte_t const* data, eplat_size_t size)
{
	eplat_handle_t hfile = eplat_file_open(path, EPLAT_FILE_WO | EPLAT_FILE_CREAT | EPLAT_FILE_TRUNC);
	if (hfile != EPLAT_INVALID_HANDLE)
	{
		eplat_int_t write_n = 0;
		while (write_n < size) 
		{
			eplat_int_t ret = eplat_file_write(hfile, data + write_n, (eplat_int_t)(size - write_n));
			if (ret < 0) break ;
			else write_n += ret;
		}
		eplat_file_close(hfile);
	}
}

int main(int argc, char** argv)
{
	eplat_size_t regular_block_n[EPLAT_POOL_REGULAR_CHUNCK_MAX_COUNT] = {10, 10, 10, 10, 10, 10, 10};
	eplat_pool_create(TB_CONFIG_MEMORY_POOL_INDEX, malloc(1024 * 1024), 1024 * 1024, regular_block_n);
	
	eplat_size_t src_n, dst_n;

	eplat_printf("load: %s\n", argv[1]);
	eplat_char_t* src = load(argv[1], &src_n);

	if (src)
	{
		eplat_printf("conv(%d): %s\n", src_n, src);
		eplat_char_t dst[4096];
		dst_n = tb_encoding_convert_string(TB_ENCODING_UTF8, TB_ENCODING_GB2312, src, src_n, dst, 4096);
		if (dst_n > 0)
		{
			eplat_printf("result(%d): %s\n", dst_n, dst);
			save(argv[2], dst, dst_n);
			eplat_printf("save: %s\n", argv[2]);
		}
		eplat_free(TB_CONFIG_MEMORY_POOL_INDEX, src);
	}
}

