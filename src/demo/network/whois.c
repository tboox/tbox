/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_WHOIS_TEST_HOST_WALK 			(1)

/* ///////////////////////////////////////////////////////////////////////
 * test
 */
static tb_void_t tb_whois_test_done(tb_char_t const* name)
{
	// init
//	tb_handle_t gst = tb_gstream_init_from_url("sock://whois.internic.net:43");
	tb_handle_t gst = tb_gstream_init_from_url("sock://199.7.51.74:43");
	tb_assert_and_check_return(gst);

	// timeout
	tb_gstream_ctrl(gst, TB_GSTREAM_CMD_SET_TIMEOUT, 1000);

	// data
	tb_char_t data[251] = {0};

	// open
	if (tb_gstream_bopen(gst))
	{
		tb_gstream_printf(gst, "%s \r\n", name);
		tb_gstream_bfwrit(gst, TB_NULL, 0);
		tb_gstream_bread(gst, data, 250);
		tb_print("%s", data);
	}

	// exit
	tb_gstream_exit(gst);
}
static tb_bool_t tb_whois_test_no_match(tb_char_t const* name)
{
	// init
//	tb_handle_t gst = tb_gstream_init_from_url("sock://whois.internic.net:43");
	tb_handle_t gst = tb_gstream_init_from_url("sock://199.7.51.74:43");
	tb_assert_and_check_return_val(gst, TB_FALSE);

	// timeout
	tb_gstream_ctrl(gst, TB_GSTREAM_CMD_SET_TIMEOUT, 1000);

	// data
	tb_char_t data[251] = {0};

	// open
	if (tb_gstream_bopen(gst))
	{
		tb_gstream_printf(gst, "%s \r\n", name);
		tb_gstream_bfwrit(gst, TB_NULL, 0);
		tb_gstream_bread(gst, data, 250);
		if (tb_strstr(data + 150, "No match")) return TB_TRUE;
	}

	// exit
	tb_gstream_exit(gst);
	return TB_FALSE;
}
static tb_bool_t tb_whois_test_walk_2()
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

		tb_print("%s: %s", p, tb_whois_test_no_match(&p[4])? "ok" : "no");
	}
}
static tb_bool_t tb_whois_test_walk_3()
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

		tb_print("%s: %s", p, tb_whois_test_no_match(&p[4])? "ok" : "no");
	}
}
static tb_bool_t tb_whois_test_walk_4()
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

		tb_print("%s: %s", p, tb_whois_test_no_match(&p[4])? "ok" : "no");
	}
}
static tb_bool_t tb_whois_test_walk_5()
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

		tb_print("%s: %s", p, tb_whois_test_no_match(&p[4])? "ok" : "no");
	}
}

static tb_bool_t tb_whois_test_walk_6()
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

		tb_print("%s: %s", p, tb_whois_test_no_match(&p[4])? "ok" : "no");
	}
}
/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// init
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

#if TB_WHOIS_TEST_HOST_WALK
//	tb_whois_test_walk_2();
	tb_whois_test_walk_3();
//	tb_whois_test_walk_4();
//	tb_whois_test_walk_5();
//	tb_whois_test_walk_6();
#else
	tb_whois_test_done(argv[1]);
#endif

	// exit
	tb_exit();
	return 0;
}
