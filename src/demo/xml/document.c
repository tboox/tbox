/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_xml_document_main(tb_int_t argc, tb_char_t** argv)
{
    // init reader and writer
    tb_xml_reader_ref_t reader = tb_xml_reader_init();
    tb_xml_writer_ref_t writer = tb_xml_writer_init();
    if (reader && writer)
    {
        // open reader and writer
        if (    tb_xml_reader_open(reader, tb_stream_init_from_url(argv[1]), tb_true)
            &&  tb_xml_writer_open(writer, tb_true, tb_stream_init_from_url(argv[2]), tb_true))
        {
            // goto
            tb_bool_t ok = tb_true;
            if (argv[3]) ok = tb_xml_reader_goto(reader, argv[3]);

            // load & save
            tb_xml_node_ref_t root = tb_null;
            if (ok) tb_xml_writer_save(writer, root = tb_xml_reader_load(reader));

            // exit root
            if (root) tb_xml_node_exit(root);
        }
    }

    // exit reader
    if (reader) tb_xml_reader_exit(reader);
    reader = tb_null;

    // exit writer
    if (writer) tb_xml_writer_exit(writer);
    writer = tb_null;

    return 0;
}

