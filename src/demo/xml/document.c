/* ///////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "tbox.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// init tbox
	if (!tb_init(malloc(10 * 1024 * 1024), 10 * 1024 * 1024)) return 0;

	// init stream
	tb_gstream_t* ist = tb_gstream_init_from_url(argv[1]);
	tb_gstream_t* ost = tb_gstream_init_from_url(argv[2]);
	if (ist && ost && tb_gstream_bopen(ist) && tb_gstream_bopen(ost))
	{
		// init reader & writer
		tb_handle_t reader = tb_xml_reader_init(ist);
		tb_handle_t writer = tb_xml_writer_init(ost, TB_TRUE);
		if (reader && writer)
		{
			// load & save;
			tb_xml_writer_save(writer, tb_xml_reader_load(reader));

			// exit reader & writer 
			tb_xml_reader_exit(reader);
			tb_xml_writer_exit(writer);
		}
	
		// exit stream
		tb_gstream_exit(ist);
		tb_gstream_exit(ost);
	}
	
	// exit tbox
	tb_exit();
	return 0;
}

