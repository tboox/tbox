/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_WHOIS_TEST_HOST_WALK             (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * test
 */
#if !TB_WHOIS_TEST_HOST_WALK
static tb_void_t tb_whois_test_done(tb_char_t const* name)
{
    // init
//  tb_stream_ref_t stream = tb_stream_init_from_url("sock://whois.internic.net:43");
    tb_stream_ref_t stream = tb_stream_init_from_url("sock://199.7.51.74:43");
//  tb_stream_ref_t stream = tb_stream_init_from_url("sock://whois.cnnic.net.cn:43");
    tb_assert_and_check_return(stream);

    // timeout
    tb_stream_ctrl(stream, TB_STREAM_CTRL_SET_TIMEOUT, 1000);

    // data
    tb_char_t data[251] = {0};

    // open
    if (tb_stream_open(stream))
    {
        tb_stream_printf(stream, "%s \r\n", name);
        tb_stream_sync(stream, tb_true);
        tb_stream_bread(stream, data, 250);
        tb_trace_i("%s", data);
    }

    // exit
    tb_stream_exit(stream);
}
#else
static tb_bool_t tb_whois_test_no_match_com(tb_char_t const* name)
{
    // init
//  tb_stream_ref_t stream = tb_stream_init_from_url("sock://whois.internic.net:43");
    tb_stream_ref_t stream = tb_stream_init_from_url("sock://199.7.51.74:43");
    tb_assert_and_check_return_val(stream, tb_false);

    // timeout
    tb_stream_ctrl(stream, TB_STREAM_CTRL_SET_TIMEOUT, 1000);

    // data
    tb_char_t data[251] = {0};

    // open
    if (tb_stream_open(stream))
    {
        tb_stream_printf(stream, "%s \r\n", name);
        tb_stream_sync(stream, tb_true);
        tb_stream_bread(stream, (tb_byte_t*)data, 250);
        if (tb_strstr(data + 150, "No match")) return tb_true;
    }

    // exit
    tb_stream_exit(stream);
    return tb_false;
}
static tb_bool_t tb_whois_test_no_match_cn(tb_char_t const* name)
{
    // init
//  tb_stream_ref_t stream = tb_stream_init_from_url("sock://whois.cnnic.net.cn:43");
    tb_stream_ref_t stream = tb_stream_init_from_url("sock://218.241.97.14:43");
    tb_assert_and_check_return_val(stream, tb_false);

    // timeout
    tb_stream_ctrl(stream, TB_STREAM_CTRL_SET_TIMEOUT, 1000);

    // data
    tb_char_t data[21] = {0};

    // open
    if (tb_stream_open(stream))
    {
        tb_stream_printf(stream, "%s \r\n", name);
        tb_stream_sync(stream, tb_true);
        tb_stream_bread(stream, (tb_byte_t*)data, 20);
        if (tb_strstr(data, "no matching")) return tb_true;
    }

    // exit
    tb_stream_exit(stream);
    return tb_false;
}
static tb_void_t tb_whois_test_walk_2()
{
    // table
    tb_char_t const* t = "abcdefghijklmnopqrstuvwxyz";
    tb_char_t p[] = {'w', 'w', 'w', '.', 'x', 'x', '.', 'c', 'o', 'm', '\0'};

    // walk
    tb_size_t i = 0;
    tb_size_t j = 0;
    tb_size_t b = 0;
    for (i = 26; i < 27 * 26; i++)
    {
        j = i;
        b = j % 26; j /= 26; p[5] = t[b];
        b = j % 26; p[4] = t[b];

        tb_trace_i("%s: %s", p, tb_whois_test_no_match_com(&p[4])? "ok" : "no");
    }
}
static tb_void_t tb_whois_test_walk_3()
{
    // table
    tb_char_t const* t = "abcdefghijklmnopqrstuvwxyz";
    tb_char_t p[] = {'w', 'w', 'w', '.', 'x', 'x', 'x', '.', 'c', 'o', 'm', '\0'};

    // walk
    tb_size_t i = 0;
    tb_size_t j = 0;
    tb_size_t b = 0;
    for (i = 26 * 26; i < 27 * 26 * 26; i++)
    {
        j = i;
        b = j % 26; j /= 26; p[6] = t[b];
        b = j % 26; j /= 26; p[5] = t[b];
        b = j % 26; p[4] = t[b];

        tb_trace_i("%s: %s", p, tb_whois_test_no_match_com(&p[4])? "ok" : "no");
    }
}
static tb_void_t tb_whois_test_walk_4()
{
    // table
    tb_char_t const* t = "abcdefghijklmnopqrstuvwxyz";
    tb_char_t p[] = {'w', 'w', 'w', '.', 'x', 'x', 'x', 'x', '.', 'c', 'o', 'm', '\0'};

    // walk
    tb_size_t i = 0;
    tb_size_t j = 0;
    tb_size_t b = 0;
    for (i = 26 * 26 * 26; i < 27 * 26 * 26 * 26; i++)
    {
        j = i;
        b = j % 26; j /= 26; p[7] = t[b];
        b = j % 26; j /= 26; p[6] = t[b];
        b = j % 26; j /= 26; p[5] = t[b];
        b = j % 26; p[4] = t[b];

        tb_trace_i("%s: %s", p, tb_whois_test_no_match_com(&p[4])? "ok" : "no");
    }
}
static tb_void_t tb_whois_test_walk_5()
{
    // table
    tb_char_t const* t = "abcdefghijklmnopqrstuvwxyz";
    tb_char_t p[] = {'w', 'w', 'w', '.', 'x', 'x', 'x', 'x', 'x', '.', 'c', 'o', 'm', '\0'};

    // walk
    tb_size_t i = 0;
    tb_size_t j = 0;
    tb_size_t b = 0;
    for (i = 26 * 26 * 26 * 26; i < 27 * 26 * 26 * 26 * 26; i++)
    {
        j = i;
        b = j % 26; j /= 26; p[8] = t[b];
        b = j % 26; j /= 26; p[7] = t[b];
        b = j % 26; j /= 26; p[6] = t[b];
        b = j % 26; j /= 26; p[5] = t[b];
        b = j % 26; p[4] = t[b];

        tb_trace_i("%s: %s", p, tb_whois_test_no_match_com(&p[4])? "ok" : "no");
    }
}

static tb_void_t tb_whois_test_walk_6()
{
    // table
    tb_char_t const* t = "abcdefghijklmnopqrstuvwxyz";
    tb_char_t p[] = {'w', 'w', 'w', '.', 'x', 'x', 'x', 'x', 'x', 'x', '.', 'c', 'o', 'm', '\0'};

    // walk
    tb_size_t i = 0;
    tb_size_t j = 0;
    tb_size_t b = 0;
    for (i = 26 * 26 * 26 * 26 * 26; i < 27 * 26 * 26 * 26 * 26 * 26; i++)
    {
        j = i;
        b = j % 26; j /= 26; p[9] = t[b];
        b = j % 26; j /= 26; p[8] = t[b];
        b = j % 26; j /= 26; p[7] = t[b];
        b = j % 26; j /= 26; p[6] = t[b];
        b = j % 26; j /= 26; p[5] = t[b];
        b = j % 26; p[4] = t[b];

        tb_trace_i("%s: %s", p, tb_whois_test_no_match_com(&p[4])? "ok" : "no");
    }
}

static tb_bool_t tb_whois_test_walk_ping_2(tb_char_t const* file)
{
    // init stream
    tb_stream_ref_t stream = tb_stream_init_from_url(file);
    tb_assert_and_check_return_val(stream, tb_false);

    // init ping
    tb_char_t* ping = tb_malloc0_cstr(1000 * 16);
    tb_assert_and_check_return_val(stream, tb_false);

    // open
    tb_size_t n = 0;
    if (tb_stream_open(stream))
    {
        while (tb_stream_bread_line(stream, &ping[n * 16], 15) > 0)
            n++;
    }

    // exit stream
    tb_stream_exit(stream);

    // walk
    tb_size_t i = 0;
    tb_size_t j = 0;
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            tb_char_t p[41] = {0};
            tb_snprintf(p, 40, "%s%s.cn", &ping[i * 16], &ping[j * 16]);
            tb_trace_i("%s: %s", p, tb_whois_test_no_match_cn(p)? "ok" : "no");
        }
    }

    // exit ping
    tb_free(ping);
    return tb_true;
}
static tb_bool_t tb_whois_test_walk_ping_3(tb_char_t const* file)
{
    // init stream
    tb_stream_ref_t stream = tb_stream_init_from_url(file);
    tb_assert_and_check_return_val(stream, tb_false);

    // init ping
    tb_char_t* ping = tb_malloc0_cstr(1000 * 16);
    tb_assert_and_check_return_val(stream, tb_false);

    // open
    tb_size_t n = 0;
    if (tb_stream_open(stream))
    {
        while (tb_stream_bread_line(stream, &ping[n * 16], 15) > 0)
            n++;
    }

    // exit stream
    tb_stream_exit(stream);

    // walk
    tb_size_t i = 0;
    tb_size_t j = 0;
    tb_size_t k = 0;
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            for (k = 0; k < n; k++)
            {
                tb_char_t p[61] = {0};
                tb_snprintf(p, 60, "%s%s%s.cn", &ping[i * 16], &ping[j * 16], &ping[k * 16]);
                tb_trace_i("%s: %s", p, tb_whois_test_no_match_cn(p)? "ok" : "no");
            }
        }
    }

    // exit ping
    tb_free(ping);
    return tb_true;
}
static tb_void_t tb_whois_test_walk_num_1()
{
    // table
    tb_char_t const* t = "0123456789";
    tb_char_t p[] = {'w', 'w', 'w', '.', 'x', '.', 'c', 'o', 'm', '\0'};

    // walk
    tb_size_t i = 0;
    tb_size_t j = 0;
    tb_size_t b = 0;
    for (i = 0; i < 10; i++)
    {
        j = i;
        b = j % 10; p[4] = t[b];

        tb_trace_i("%s: %s", p, tb_whois_test_no_match_com(&p[4])? "ok" : "no");
    }
}
static tb_void_t tb_whois_test_walk_num_2()
{
    // table
    tb_char_t const* t = "0123456789";
    tb_char_t p[] = {'w', 'w', 'w', '.', 'x', 'x', '.', 'c', 'o', 'm', '\0'};

    // walk
    tb_size_t i = 0;
    tb_size_t j = 0;
    tb_size_t b = 0;
    for (i = 10; i < 100; i++)
    {
        j = i;
        b = j % 10; j /= 10; p[5] = t[b];
        b = j % 10; p[4] = t[b];

        tb_trace_i("%s: %s", p, tb_whois_test_no_match_com(&p[4])? "ok" : "no");
    }
}
static tb_void_t tb_whois_test_walk_num_3()
{
    // table
    tb_char_t const* t = "0123456789";
    tb_char_t p[] = {'w', 'w', 'w', '.', 'x', 'x', 'x', '.', 'c', 'o', 'm', '\0'};

    // walk
    tb_size_t i = 0;
    tb_size_t j = 0;
    tb_size_t b = 0;
    for (i = 100; i < 1000; i++)
    {
        j = i;
        b = j % 10; j /= 10; p[6] = t[b];
        b = j % 10; j /= 10; p[5] = t[b];
        b = j % 10; p[4] = t[b];

        tb_trace_i("%s: %s", p, tb_whois_test_no_match_com(&p[4])? "ok" : "no");
    }
}
static tb_void_t tb_whois_test_walk_num_4()
{
    // table
    tb_char_t const* t = "0123456789";
    tb_char_t p[] = {'w', 'w', 'w', '.', 'x', 'x', 'x', 'x', '.', 'c', 'o', 'm', '\0'};

    // walk
    tb_size_t i = 0;
    tb_size_t j = 0;
    tb_size_t b = 0;
    for (i = 1000; i < 10000; i++)
    {
        j = i;
        b = j % 10; j /= 10; p[7] = t[b];
        b = j % 10; j /= 10; p[6] = t[b];
        b = j % 10; j /= 10; p[5] = t[b];
        b = j % 10; p[4] = t[b];

        tb_trace_i("%s: %s", p, tb_whois_test_no_match_com(&p[4])? "ok" : "no");
    }
}
static tb_void_t tb_whois_test_walk_num_5()
{
    // table
    tb_char_t const* t = "0123456789";
    tb_char_t p[] = {'w', 'w', 'w', '.', 'x', 'x', 'x', 'x', 'x', '.', 'c', 'o', 'm', '\0'};

    // walk
    tb_size_t i = 0;
    tb_size_t j = 0;
    tb_size_t b = 0;
    for (i = 10000; i < 100000; i++)
    {
        j = i;
        b = j % 10; j /= 10; p[8] = t[b];
        b = j % 10; j /= 10; p[7] = t[b];
        b = j % 10; j /= 10; p[6] = t[b];
        b = j % 10; j /= 10; p[5] = t[b];
        b = j % 10; p[4] = t[b];

        tb_trace_i("%s: %s", p, tb_whois_test_no_match_com(&p[4])? "ok" : "no");
    }
}
static tb_void_t tb_whois_test_walk_num_6()
{
    // table
    tb_char_t const* t = "0123456789";
    tb_char_t p[] = {'w', 'w', 'w', '.', 'x', 'x', 'x', 'x', 'x', 'x', '.', 'c', 'o', 'm', '\0'};

    // walk
    tb_size_t i = 0;
    tb_size_t j = 0;
    tb_size_t b = 0;
    for (i = 100000; i < 1000000; i++)
    {
        j = i;
        b = j % 10; j /= 10; p[9] = t[b];
        b = j % 10; j /= 10; p[8] = t[b];
        b = j % 10; j /= 10; p[7] = t[b];
        b = j % 10; j /= 10; p[6] = t[b];
        b = j % 10; j /= 10; p[5] = t[b];
        b = j % 10; p[4] = t[b];

        tb_trace_i("%s: %s", p, tb_whois_test_no_match_com(&p[4])? "ok" : "no");
    }
}
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_network_whois_main(tb_int_t argc, tb_char_t** argv)
{
#if TB_WHOIS_TEST_HOST_WALK
    tb_whois_test_walk_2();
    tb_whois_test_walk_3();
    tb_whois_test_walk_4();
    tb_whois_test_walk_5();
    tb_whois_test_walk_6();
    tb_whois_test_walk_num_1();
    tb_whois_test_walk_num_2();
    tb_whois_test_walk_num_3();
    tb_whois_test_walk_num_4();
    tb_whois_test_walk_num_5();
    tb_whois_test_walk_num_6();
    tb_whois_test_walk_ping_2(argv[1]);
    tb_whois_test_walk_ping_3(argv[1]);
#else
    tb_whois_test_done(argv[1]);
#endif

    return 0;
}
