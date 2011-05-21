#include "tbox.h"

int main(int argc, char** argv)
{
	// init tplat
	if (TPLAT_FALSE == tplat_init(malloc(1024 * 1024), 1024 * 1024)) return 0;
	
	// create stream
	tb_gstream_t* gst = tb_gstream_create_from_url(argv[1]);
	if (!gst || !tb_gstream_open(gst))
	{
		TB_DBG("failed to open url: %s", argv[1]);
		return 0;
	}


	tb_format_t const* format =	tb_format_probe(gst, TB_FORMAT_FLAG_ALL);
	if (!format)
	{
		TB_DBG("unsupported format");
		return 0;
	}

	TB_DBG("format: %s", format->name);

	// exit tplat
	tplat_exit();
	return 0;
}
