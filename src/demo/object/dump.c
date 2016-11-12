/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_object_dump_main(tb_int_t argc, tb_char_t** argv)
{
    // read
    tb_oc_object_ref_t root = tb_oc_object_read_from_url(argv[1]);
    if (root)
    {
        // seek?
        tb_oc_object_ref_t object = root;
        if (argv[2]) object = tb_oc_object_seek(root, argv[2], tb_true);

        // dump object
        if (object) tb_oc_object_dump(object, TB_OBJECT_FORMAT_XML);

        // exit object
        tb_oc_object_exit(root);
    }
    
    return 0;
}

