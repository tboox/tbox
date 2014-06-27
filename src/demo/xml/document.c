/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_xml_document_main(tb_int_t argc, tb_char_t** argv)
{
    // init stream
    tb_stream_ref_t istream = tb_stream_init_from_url(argv[1]);
    tb_stream_ref_t ostream = tb_stream_init_from_url(argv[2]);
    if (ostream) tb_stream_ctrl(ostream, TB_STREAM_CTRL_FILE_SET_MODE, TB_FILE_MODE_WO | TB_FILE_MODE_CREAT | TB_FILE_MODE_TRUNC);
    if (istream && ostream && tb_stream_open(istream) && tb_stream_open(ostream))
    {
        // init reader & writer
        tb_xml_reader_ref_t reader = tb_xml_reader_init(istream);
        tb_xml_writer_ref_t writer = tb_xml_writer_init(ostream, tb_true);
        if (reader && writer)
        {
            // goto
            tb_bool_t ok = tb_true;
            if (argv[3]) ok = tb_xml_reader_goto(reader, argv[3]);

            // load & save
            tb_xml_node_ref_t root = tb_null;
            if (ok) tb_xml_writer_save(writer, root = tb_xml_reader_load(reader));

            // exit root
            if (root) tb_xml_node_exit(root);

            // exit reader & writer 
            tb_xml_reader_exit(reader);
            tb_xml_writer_exit(writer);
        }
    
        // exit stream
        tb_stream_exit(istream);
        tb_stream_exit(ostream);
    }
    
    return 0;
}

