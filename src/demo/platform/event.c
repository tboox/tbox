/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_TEST_ITEM_MAX    (10)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */
typedef struct __tb_test_item_t
{
    tb_handle_t     e;
    tb_handle_t     t;
    tb_size_t       i   : 24;
    tb_size_t       q   : 8;

}tb_test_item_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */
tb_char_t* gets(tb_char_t*);

/* //////////////////////////////////////////////////////////////////////////////////////
 * thread
 */
static tb_pointer_t tb_test_thread(tb_cpointer_t priv)
{
    tb_test_item_t* it = (tb_test_item_t*)priv;
    tb_assert_and_check_goto(it, end);
    tb_trace_i("[thread: %u]: init", it->i);

    // loop
    while (1)
    {
        // wait
        tb_trace_i("[event: %u]: wait", it->i);
        tb_long_t r = tb_event_wait(it->e, -1);
        tb_assert_and_check_goto(r >= 0, end);

        // quit?
        tb_check_goto(!it->q, end);

        // timeout?
        tb_check_continue(r);

        // signal
        tb_trace_i("[event: %u]: signal", it->i);
    }

end:
    tb_trace_i("[thread: %u]: exit", it? it->i : 0);
    tb_thread_return(tb_null);
    return tb_null;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_platform_event_main(tb_int_t argc, tb_char_t** argv)
{
    // init item
    tb_test_item_t it[TB_TEST_ITEM_MAX] = {{0}};

    // init thread
    tb_size_t i = 0;
    for (i = 0; i < TB_TEST_ITEM_MAX; i++)
    {
        it[i].i = i;
        it[i].e = tb_event_init(); tb_event_post(it[i].e);
        it[i].t = tb_thread_init(tb_null, tb_test_thread, it + i, 0);
        tb_assert_and_check_goto(it[i].t, end);
    }
    tb_msleep(100);

    // post
    tb_char_t s[256];
    while (1)
    {
        tb_char_t const* p = gets(s);
        if (p)
        {
            while (*p)
            {
                tb_char_t ch = *p++;
                switch (ch)
                {
                case 'q':
                    goto end;
                default:
                    {
                        if (ch >= '0' && ch <= '9')
                        {
                            // post event
                            tb_size_t i = ch - '0';
                            if (it[i].e) tb_event_post(it[i].e);
                        }
                    }
                    break;
                }
            }
        }
    }

end:
    // exit thread
    for (i = 0; i < TB_TEST_ITEM_MAX; i++)
    {
        // quit thread
        it[i].q = 1;

        // post event
        if (it[i].e) tb_event_post(it[i].e);

        // kill thread
        if (it[i].t) 
        {
            tb_thread_wait(it[i].t, 5000);
            tb_thread_exit(it[i].t);
        }

        // exit event
        if (it[i].e) tb_event_exit(it[i].e);
    }

    tb_trace_i("quit");

    return 0;
}
