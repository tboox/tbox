/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */
typedef struct __tb_demo_loop_t
{
    // the semaphore
    tb_semaphore_ref_t  semaphore;

    // the loop
    tb_thread_ref_t     loop;

    // the index
    tb_size_t           index;

    // is stoped?
    tb_atomic_t         bstoped;

}tb_demo_loop_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * thread
 */
static tb_char_t const* tb_demo_gets(tb_char_t* line, tb_size_t maxn)
{
    // check
    tb_assert_and_check_return_val(line && maxn, tb_null);

    // done
    tb_char_t* p = line;
    tb_char_t* e = line + maxn;
    while (p < e)
    {
        // get character
        tb_char_t ch = tb_getchar(); if (ch == '\r') tb_getchar();
        tb_check_break(ch != '\r' && ch != '\n');

        // append digit
        if (tb_isdigit(ch)) *p++ = ch;
        else
        {
            // trace
            tb_trace_e("invalid character: %x, please input digit!", ch);
        }
    }

    // end
    if (p < e) *p = '\0';

    // ok?
    return line;
}
static tb_int_t tb_demo_loop(tb_cpointer_t priv)
{
    // check
    tb_demo_loop_t* loop = (tb_demo_loop_t*)priv;

    // done
    do
    {
        // check
        tb_assert_and_check_break(loop);

        // trace
        tb_trace_i("[thread: %lu]: init", loop->index);

        // loop
        while (!tb_atomic_get(&loop->bstoped))
        {
            // wait
            tb_long_t wait = tb_semaphore_wait(loop->semaphore, -1);
            tb_assert_and_check_break(wait >= 0);

            // timeout?
            tb_check_continue(wait);

            // trace
            tb_trace_i("[semaphore: %lu]: wait: ok", loop->index);
        }

    } while (0);

    // trace
    tb_trace_i("[thread: %lu]: exit", loop? loop->index : 0);

    // end
    return 0;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_platform_semaphore_main(tb_int_t argc, tb_char_t** argv)
{
    // init loop
    tb_demo_loop_t  loop[10];
    tb_size_t       i = 0;
    tb_size_t       n = tb_arrayn(loop);
    for (i = 0; i < n; i++)
    {
        // init semaphore
        loop[i].semaphore = tb_semaphore_init(0);
        tb_assert_and_check_break(loop[i].semaphore);

        // post semaphore
        tb_semaphore_post(loop[i].semaphore, 1);

        // init index
        loop[i].index = i;

        // init stoped
        loop[i].bstoped = 0;

        // init loop
        loop[i].loop = tb_thread_init(tb_null, tb_demo_loop, loop + i, 0);
        tb_assert_and_check_break(loop[i].loop);
    }

    // check
    tb_assert_and_check_return_val(i == n, 0);

    // wait some time
    tb_msleep(100);

    // post
    tb_char_t line[256];
    tb_bool_t stop = tb_false;
    while (!stop)
    {
        // get line
        tb_char_t const* p = tb_demo_gets(line, sizeof(line));
        tb_assert_and_check_break(p);

        // trace
        tb_trace_i("post: %s", p);

        // done
        while (*p && !stop)
        {
            tb_char_t ch = *p++;
            switch (ch)
            {
            case 'q':
                stop = tb_true;
                break;
            default:
                {
                    if (ch >= '0' && ch <= '9')
                    {
                        // the index
                        tb_size_t index = ch - '0';
                        tb_assert_and_check_break(index < n && index == loop[index].index);

                        // post semaphore
                        if (loop[index].semaphore) tb_semaphore_post(loop[index].semaphore, 1);
                    }
                }
                break;
            }
        }
    }

    // post loop
    for (i = 0; i < n; i++)
    {
        // quit thread
        tb_atomic_set(&loop[i].bstoped, 1);

        // post semaphore
        if (loop[i].semaphore) tb_semaphore_post(loop[i].semaphore, 1);
    }

    // exit loop
    for (i = 0; i < n; i++)
    {
        // exit loop
        if (loop[i].loop)
        {
            // wait it
            if (!tb_thread_wait(loop[i].loop, 5000, tb_null))
            {
                // trace
                tb_trace_e("wait loop[%lu]: timeout", i);
            }

            // exit it
            tb_thread_exit(loop[i].loop);
        }

        // exit semaphore
        if (loop[i].semaphore) tb_semaphore_exit(loop[i].semaphore);
    }

    // exit
    tb_trace_i("exit");
    return 0;
}
