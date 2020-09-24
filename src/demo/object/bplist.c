/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_object_bplist_main(tb_int_t argc, tb_char_t** argv)
{
    // read object
    tb_object_ref_t object = tb_object_read_from_url(argv[1]);

    // writ
    if (object)
    {
        // writ object
        tb_object_writ_to_url(object, argv[2], TB_OBJECT_FORMAT_BPLIST);

        // exit object
        tb_object_exit(object);
    }

    return 0;
}

