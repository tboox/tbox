/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_platform_filelock_main(tb_int_t argc, tb_char_t** argv)
{
    tb_filelock_ref_t lock = tb_filelock_init_from_path(argv[1], tb_file_info(argv[1], tb_null)? TB_FILE_MODE_RO : TB_FILE_MODE_RW | TB_FILE_MODE_CREAT);
    if (lock)
    {
        // trace
        tb_trace_i("filelock: enter ..");

        // enter lock
        if (tb_filelock_enter(lock, (argv[2] && !tb_strcmp(argv[2], "sh"))? TB_FILELOCK_MODE_SH : TB_FILELOCK_MODE_EX))
        {
            // trace
            tb_trace_i("filelock: enter ok");

            // wait ..
            tb_getchar();

            // leave lock
            tb_filelock_leave(lock);

            // trace
            tb_trace_i("filelock: leave ok");
        }
        tb_filelock_exit(lock);
    }
    return 0;
}
