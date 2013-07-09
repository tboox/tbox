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
	if (ost) tb_gstream_ctrl(ost, TB_FSTREAM_CMD_SET_MODE, TB_FILE_MODE_WO | TB_FILE_MODE_CREAT | TB_FILE_MODE_TRUNC);
	if (ist && ost && tb_gstream_bopen(ist) && tb_gstream_bopen(ost))
	{
		// init reader & writer
		tb_handle_t reader = tb_xml_reader_init(ist);
		tb_handle_t writer = tb_xml_writer_init(ost, tb_true);
		if (reader && writer)
		{
			// goto
			tb_bool_t ok = tb_true;
			if (argv[3]) ok = tb_xml_reader_goto(reader, argv[3]);

			// load & save
			tb_xml_node_t* root = tb_null;
			if (ok) tb_xml_writer_save(writer, root = tb_xml_reader_load(reader));

			// exit root
			if (root) tb_xml_node_exit(root);

			// exit reader & writer 
			tb_xml_reader_exit(reader);
			tb_xml_writer_exit(writer);
		}
	
		// exit stream
		tb_gstream_exit(ist);
		tb_gstream_exit(ost);
	}
	
end:
	// exit tbox
	tb_exit();
	return 0;
}

