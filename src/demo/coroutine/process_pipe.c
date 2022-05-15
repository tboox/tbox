/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define COUNT   (50)

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_void_t tb_demo_coroutine_proc(tb_cpointer_t priv)
{
    // get arguments
    tb_char_t** argv = (tb_char_t**)priv;
    tb_assert_and_check_return(argv);

    // init id
    static tb_size_t count = 0;
    tb_size_t id = count++;

    // init pipe files
    tb_pipe_file_ref_t file[2] = {0};
    if (tb_pipe_file_init_pair(file, tb_null, 0))
    {
        // init process
        tb_process_attr_t attr = {0};
        attr.out.pipe = file[1];
        attr.outtype = TB_PROCESS_REDIRECT_TYPE_PIPE;
        tb_process_ref_t process = tb_process_init(argv[1], (tb_char_t const**)(argv + 1), &attr);
        if (process)
        {
            // read pipe data
            tb_size_t read = 0;
            tb_byte_t data[8192];
            tb_size_t size = sizeof(data);
            tb_bool_t wait = tb_false;
            while (read < size)
            {
                tb_long_t real = tb_pipe_file_read(file[0], data + read, size - read);
                if (real > 0)
                {
                    read += real;
                    wait = tb_false;
                }
                else if (!real && !wait)
                {
                    // wait pipe
                    tb_long_t ok = tb_pipe_file_wait(file[0], TB_PIPE_EVENT_READ, 1000);
                    tb_check_break(ok > 0);
                    wait = tb_true;
                }
                else break;
            }

            // dump data
            tb_trace_i("[%lu]: read pipe: %ld", id, read);

            // wait process
            tb_long_t status = 0;
            tb_process_wait(process, &status, -1);

            // trace
            tb_trace_i("[%lu]: run: %s, status: %ld", id, argv[1], status);

            // exit process
            tb_process_exit(process);
        }

        // exit pipe files
        tb_pipe_file_exit(file[0]);
        tb_pipe_file_exit(file[1]);
    }

    // end
    tb_trace_i("[%lu]: end", id);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_coroutine_process_pipe_main(tb_int_t argc, tb_char_t** argv)
{
    tb_co_scheduler_ref_t scheduler = tb_co_scheduler_init();
    if (scheduler)
    {
        // start coroutines
        tb_size_t count = COUNT;
        while (count--)
            tb_coroutine_start(scheduler, tb_demo_coroutine_proc, argv, 0);

        // do loop
        tb_co_scheduler_loop(scheduler, tb_true);
        tb_co_scheduler_exit(scheduler);
    }

    return 0;
}
