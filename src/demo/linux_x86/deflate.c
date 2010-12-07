#include "tplat/tplat.h"
#include "../../tbox.h"

//#define TB_ZSTREAM_TEST_RLC
#define TB_ZSTREAM_TEST_LZSW

int main(int argc, char** argv)
{
	tplat_size_t regular_block_n[TPLAT_POOL_REGULAR_CHUNCK_MAX_COUNT] = {10, 10, 10, 10, 10, 10, 10};
	tplat_pool_create(TB_CONFIG_MEMORY_POOL_INDEX, malloc(50 * 1024 * 1024), 50 * 1024 * 1024, regular_block_n);

	if (argc < 2 || !argv[1]) return 0;

	// get output path
	tb_char_t opath[4096];
	snprintf(opath, 4095, "%s.z", argv[1]);

	// input & output stream
	tb_ustream_t 	istream;
	tb_ustream_t 	ostream;;
	tb_gstream_t* 	ist = tb_gstream_open(&istream, argv[1], TB_NULL, 0, TB_GSTREAM_FLAG_RO);
	tb_gstream_t* 	ost = tb_gstream_open(&ostream, opath, TB_NULL, 0, TB_GSTREAM_FLAG_BLOCK | TB_GSTREAM_FLAG_WO | TB_GSTREAM_FLAG_TRUNC | TB_GSTREAM_FLAG_CREATE);

	// the src & dst data
	tb_size_t 		maxn = 20 * 1024 * 1024;
	tb_size_t 		srcn = maxn;
	tb_size_t 		dstn = maxn;
	tb_byte_t* 		src = tb_malloc(srcn);
	tb_byte_t* 		dst = tb_malloc(dstn);

	// the zstream
#if defined(TB_ZSTREAM_TEST_RLC)
	tb_rlc_zstream_t 	zst;
	tb_tstream_t* 		tst = tb_zstream_open_rlc(&zst, TB_ZSTREAM_ACTION_DEFLATE);
#elif defined(TB_ZSTREAM_TEST_LZSW)
	tb_lzsw_zstream_t 	zst;
	tb_tstream_t* 		tst = tb_zstream_open_lzsw(&zst, TB_ZSTREAM_ACTION_DEFLATE);
#endif

	// attach data
	tb_bstream_attach(tb_tstream_src(tst), src, srcn);
	tb_bstream_attach(tb_tstream_dst(tst), dst, dstn);

	// load stream
	tb_bstream_attach(tb_tstream_src(tst), src, tb_bstream_load(tb_tstream_src(tst), ist));

	// check size
	TB_ASSERT(tb_bstream_size(tb_tstream_src(tst)) < maxn);
	if (tb_bstream_size(tb_tstream_src(tst)) > maxn) return 0;

	// start time
	tb_size_t dt = (tb_size_t)tplat_clock();
	
	// transform stream
	tb_tstream_transform(tst);

	// stop time
	dt = ((tb_size_t)tplat_clock()) - dt;

	// dump
	tb_size_t offset = tb_bstream_offset(tb_tstream_dst(tst));
	tb_size_t size = tb_bstream_size(tb_tstream_src(tst));
	tplat_printf("========================================================\n");
	tplat_printf("%s %s: %d(%d kb) => %d(%d kb), rate: %0.02f%%, time: %d ms\n"
		, tb_zstream_name(tst)
		, tb_zstream_action(tst) == TB_ZSTREAM_ACTION_INFLATE? "inflate" : "deflate"
		, size
		, size >> 10
		, offset
		, offset >> 10
		, size? ((offset * 100.) / size) : 0
		, dt);
	tplat_printf("========================================================\n");

	// save stream
	tb_bstream_attach(tb_tstream_dst(tst), dst, tb_bstream_offset(tb_tstream_dst(tst)));
	tb_bstream_save(tb_tstream_dst(tst), ost);

	// close
	tb_tstream_close(tst);
	tb_gstream_close(ist);
	tb_gstream_close(ost);

	return 0;
}

