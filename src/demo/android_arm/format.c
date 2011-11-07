#include "tbox.h"

int main(int argc, char** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;
	
	// create stream
	tb_gstream_t* gst = tb_gstream_create_from_url(argv[1]);
	if (!gst || !tb_gstream_open(gst))
	{
		tb_trace("failed to open url: %s", argv[1]);
		return 0;
	}


	tb_format_t const* format =	tb_format_probe(gst, TB_FORMAT_FLAG_ALL);
	if (!format)
	{
		tb_trace("unsupported format");
		return 0;
	}

	tb_trace("format: %s", format->name);

	tb_exit();
	return 0;
}
