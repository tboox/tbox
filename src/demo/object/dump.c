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
    tb_object_ref_t root = tb_object_read_from_url(argv[1]);
    if (root)
    {
        // seek?
        tb_object_ref_t object = root;
        if (argv[2]) object = tb_object_seek(root, argv[2], tb_true);

        // dump object
        if (object) tb_object_dump(object, TB_OBJECT_FORMAT_JSON);

        // exit object
        tb_object_exit(root);
    }

    return 0;
}

