#include "tbox.h"
#include "stdio.h"

static tb_bool_t http_callback_head(tb_char_t const* line, tb_pointer_t priv)
{
	tb_print("[http]: head: %s", line);
	return TB_TRUE;
}

int main(int argc, char** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// create stream
	tb_gstream_t* ist = tb_gstream_create_from_url(argv[1]);
	tb_gstream_t* ost = tb_gstream_create_from_url(argv[2]);
	if (!ist || !ost) goto end;
	
	// init option
	tb_gstream_ioctl2(ist, TB_HSTREAM_CMD_SET_HEAD_FUNC, http_callback_head, TB_NULL);
	tb_gstream_ioctl1(ost, TB_FSTREAM_CMD_SET_FLAGS, TB_FILE_WO | TB_FILE_CREAT | TB_FILE_TRUNC);

	// open stream
	if (!tb_gstream_open(ist)) goto end;
	if (!tb_gstream_open(ost)) goto end;
	
	// save stream
	tb_uint64_t size = tb_gstream_save(ist, ost);
	tb_print("save: %llu bytes", size);

end:

	// destroy stream
	tb_gstream_destroy(ist);
	tb_gstream_destroy(ost);

	tb_exit();
	return 0;
}
