/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */ 


/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_coroutine_http_server_main(tb_int_t argc, tb_char_t** argv)
{
    // init scheduler
    tb_scheduler_ref_t scheduler = tb_scheduler_init();
    if (scheduler)
    {
        // start coroutines

        // run scheduler
        tb_scheduler_loop(scheduler);

        // exit scheduler
        tb_scheduler_exit(scheduler);
    }
    return 0;
}
