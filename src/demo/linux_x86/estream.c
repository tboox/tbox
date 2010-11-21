#include "tplat/tplat.h"
#include "../../tbox.h"

int main(int argc, char** argv)
{
	tplat_size_t regular_block_n[TPLAT_POOL_REGULAR_CHUNCK_MAX_COUNT] = {10, 10, 10, 10, 10, 10, 10};
	tplat_pool_create(TB_CONFIG_MEMORY_POOL_INDEX, malloc(50 * 1024 * 1024), 50 * 1024 * 1024, regular_block_n);

	if (argc < 3) return 0;
	
	// input & output stream
	tb_ustream_t 	istream;
	tb_ustream_t 	ostream;;
	tb_gstream_t* 	ist = tb_gstream_open(&istream, argv[1], TB_NULL, 0, TB_GSTREAM_FLAG_RO);
	tb_gstream_t* 	ost = tb_gstream_open(&ostream, argv[2], TB_NULL, 0, TB_GSTREAM_FLAG_BLOCK | TB_GSTREAM_FLAG_WO | TB_GSTREAM_FLAG_TRUNC);

	// the src & dst data
	tb_size_t 		src_n = 20 * 1024 * 1024;
	tb_size_t 		dst_n = 20 * 1024 * 1024;
	tb_byte_t* 		src = tb_malloc(src_n);
	tb_byte_t* 		dst = tb_malloc(dst_n);

	// the tansform stream
	tb_estream_t 	est;
	tb_tstream_t* 	tst = tb_estream_open(&est, TB_ENCODING_UTF8, TB_ENCODING_GB2312);
	tb_bstream_attach(tb_tstream_src(tst), src, src_n);
	tb_bstream_attach(tb_tstream_dst(tst), dst, dst_n);

	// load stream
	tb_bstream_attach(tb_tstream_src(tst), src, tb_bstream_load(tb_tstream_src(tst), ist));

	// transform stream
	tb_tstream_transform(tst);

	// save stream
	tb_bstream_attach(tb_tstream_dst(tst), dst, tb_bstream_pos(tb_tstream_dst(tst)) - dst);
	tb_bstream_save(tb_tstream_dst(tst), ost);

	// close
	tb_tstream_close(tst);
	tb_gstream_close(ist);
	tb_gstream_close(ost);

	return 0;
}

