/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdio.h>
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_TEST_ITEM_MAX 	(10)

/* ///////////////////////////////////////////////////////////////////////
 * types
 */
typedef struct __tb_test_item_t
{
	tb_handle_t 	e;
	tb_handle_t 	t;
	tb_size_t 		q;

}tb_test_item_t;

/* ///////////////////////////////////////////////////////////////////////
 * thread
 */
static tb_pointer_t tb_test_thread(tb_pointer_t cb_data)
{
	tb_test_item_t* it = (tb_test_item_t*)cb_data;
	tb_assert_and_check_goto(it, end);
	tb_print("[thread]: init");

	// loop
	tb_eobject_t o[TB_TEST_ITEM_MAX];
	while (1)
	{
		// wait
		tb_print("[event]: wait");
		tb_long_t 		r = tb_epool_wait(it->e, o, TB_TEST_ITEM_MAX, -1);
		tb_assert_and_check_goto(r >= 0, end);

		// quit?
		tb_check_goto(!it->q, end);

		// timeout?
		tb_check_continue(r);

		// signal
		tb_size_t i = 0;
		for (i = 0; i < r; i++) 
			tb_print("[event: %u]: signal", o[i].data);
	}

end:

	// exit thread
	tb_print("[thread]: exit");
	tb_thread_return(TB_NULL);
	return TB_NULL;
}

/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// init item
	tb_test_item_t it = {0};

	// init epool
	it.e = tb_epool_init(TB_TEST_ITEM_MAX + 10);
	tb_assert_and_check_return_val(it.e, 0);

	// init event
	tb_size_t 	i = 0;
	tb_handle_t e[TB_TEST_ITEM_MAX] = {TB_NULL};
	for (i = 0; i < TB_TEST_ITEM_MAX; i++) e[i] = tb_epool_adde(it.e, i);

	// init thread
	it.t = tb_thread_init(TB_NULL, tb_test_thread, &it, 0);
	tb_assert_and_check_goto(it.t, end);
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
							tb_epool_post(it.e, e[i]);
						}
					}
					break;
				}
			}
		}
	}

end:
	// quit thread
	it.q = 1;

	// kill events
	tb_epool_kill(it.e);

	// kill thread
	if (it.t) 
	{
		if (!tb_thread_wait(it.t, 5000))
			tb_thread_kill(it.t);
		tb_thread_exit(it.t);
	}

	// exit event
	tb_epool_exit(it.e);

	tb_print("quit");
	tb_exit();
	return 0;
}
