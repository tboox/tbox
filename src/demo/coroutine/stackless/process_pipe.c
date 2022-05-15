/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define COUNT   (50)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the process type
typedef struct __tb_demo_lo_process_t
{
    // the process
    tb_process_ref_t    proc;

    // the arguments
    tb_char_t**         argv;

    // the pipe files
    tb_pipe_file_ref_t  file[2];

    // the process attributes
    tb_process_attr_t   attr;

    // id
    tb_size_t           id;

    // read variables
    tb_size_t           read;
    tb_byte_t           data[8192];
    tb_size_t           size;
    tb_bool_t           wait;
    tb_long_t           real;

}tb_demo_lo_process_t, *tb_demo_lo_process_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_demo_lo_coroutine_func(tb_lo_coroutine_ref_t coroutine, tb_cpointer_t priv)
{
    // get arguments
    tb_demo_lo_process_ref_t process = (tb_demo_lo_process_ref_t)priv;
    tb_assert_and_check_return(process && process->argv);

    // enter coroutine
    tb_lo_coroutine_enter(coroutine)
    {
        // init pipe files
        if (tb_pipe_file_init_pair(process->file, tb_null, 0))
        {
            // init process
            process->attr.out.pipe = process->file[1];
            process->attr.outtype = TB_PROCESS_REDIRECT_TYPE_PIPE;
            process->proc = tb_process_init(process->argv[1], (tb_char_t const**)(process->argv + 1), &process->attr);
            if (process->proc)
            {
                // read pipe data
                while (process->read < process->size)
                {
                    process->real = tb_pipe_file_read(process->file[0], process->data + process->read, process->size - process->read);
                    if (process->real > 0)
                    {
                        process->read += process->real;
                        process->wait = tb_false;
                    }
                    else if (!process->real && !process->wait)
                    {
                        // wait pipe
                        tb_lo_coroutine_wait_pipe(process->file[0], TB_PIPE_EVENT_READ, 1000);
                        tb_check_break(tb_lo_coroutine_wait_result() > 0);
                        process->wait = tb_true;
                    }
                    else break;
                }

                // dump data
                tb_trace_i("[%lu]: read pipe: %ld", process->id, process->read);

                // wait process
                tb_lo_coroutine_wait_proc(process->proc, -1);

                // trace
                tb_trace_i("[%lu]: run: %s, status: %ld", process->id, process->argv[1], tb_lo_coroutine_proc_status());

                // exit process
                tb_process_exit(process->proc);
            }

            // exit pipe files
            tb_pipe_file_exit(process->file[0]);
            tb_pipe_file_exit(process->file[1]);
        }
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_lo_coroutine_process_pipe_main(tb_int_t argc, tb_char_t** argv)
{
    // init scheduler
    tb_lo_scheduler_ref_t scheduler = tb_lo_scheduler_init();
    if (scheduler)
    {
        // start coroutines
        tb_size_t            count = COUNT;
        tb_demo_lo_process_t processes[COUNT];
        tb_memset(processes, 0, sizeof(processes));
        while (count--)
        {
            processes[count].argv = argv;
            processes[count].id = count;
            processes[count].size = sizeof(processes[count].data);
            tb_lo_coroutine_start(scheduler, tb_demo_lo_coroutine_func, &processes[count], 0);
        }

        // run scheduler
        tb_lo_scheduler_loop(scheduler, tb_true);

        // exit scheduler
        tb_lo_scheduler_exit(scheduler);
    }
    return 0;
}
