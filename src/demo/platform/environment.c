/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_platform_environment_main(tb_int_t argc, tb_char_t** argv)
{
    // init environment
    tb_environment_ref_t environment = tb_environment_init();
    if (environment)
    {
        // load variable
        if (tb_environment_load(environment, argv[1]))
        {
            // dump it
#ifdef __tb_debug__
            tb_environment_dump(environment, argv[1]);
#endif
        }

        // save variable?
        if (argc > 2)
        {
            tb_size_t i = 0;
            for (i = 2; i < argc && argv[i]; i++)
            {
                // set value
                tb_environment_insert(environment, argv[i], tb_false);
            }

            // save variable
            tb_environment_save(environment, argv[1]);

            // load variable
            if (tb_environment_load(environment, argv[1]))
            {
                // dump it
#ifdef __tb_debug__
                tb_environment_dump(environment, argv[1]);
#endif
            }
        }

        // exit environment
        tb_environment_exit(environment);
    }
    return 0;
}
