/* ///////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"

/* ///////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_TEST_ITEM_MAX 	(10)

/* ///////////////////////////////////////////////////////////////////
 * types
 */
typedef struct __tb_test_item_t
{
	tb_handle_t 	e[TB_TEST_ITEM_MAX];
	tb_handle_t 	t;
	tb_size_t 		q;

}tb_test_item_t;

/* ///////////////////////////////////////////////////////////////////
 * thread
 */
static tb_pointer_t tb_test_thread(tb_pointer_t cb_data)
{
	tb_eiop_t* 	ep = TB_NULL;
	tb_test_item_t* it = (tb_test_item_t*)cb_data;
	tb_assert_and_check_goto(it, end);
	tb_print("[thread]: init");

	// init eiop
	ep = tb_eiop_init(TB_EIO_OTYPE_EVET, TB_TEST_ITEM_MAX);
	tb_assert_and_check_goto(ep, end);

	// add event
	tb_size_t i = 0;
	for (i = 0; i < TB_TEST_ITEM_MAX; i++) 
		if (it->e[i]) tb_eiop_addo(ep, it->e[i], TB_EIO_ETYPE_SIGL);

	// loop
	while (1)
	{
		// wait
		tb_print("[event]: wait");
		tb_long_t 		r = tb_eiop_wait(ep, -1);
		tb_eio_t* 	o = tb_eiop_objs(ep);
		tb_assert_and_check_goto(r >= 0 && o, end);

		// quit?
		tb_check_goto(!it->q, end);

		// timeout?
		tb_check_continue(r);

		// check
		tb_assert_and_check_goto(o->etype & TB_EIO_ETYPE_SIGL && o->otype == TB_EIO_OTYPE_EVET, end);

		// signal
		for (i = 0; i < r; i++) 
			tb_print("[event: %x]: signal", o[i].handle);
	}

end:

	// exit eiop
	if (ep) tb_eiop_exit(ep);

	// exit thread
	tb_print("[thread]: exit");
	tb_thread_return(it? it->t : TB_NULL, TB_NULL);
	return TB_NULL;
}

/* ///////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// init item
	tb_test_item_t it = {0};

	// init event
	tb_size_t i = 0;
	for (i = 0; i < TB_TEST_ITEM_MAX; i++) it.e[i] = tb_event_init(TB_NULL, TB_FALSE);

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
							if (it.e[i]) tb_event_post(it.e[i]);
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

	// post event
	for (i = 0; i < TB_TEST_ITEM_MAX; i++) if (it.e[i]) tb_event_post(it.e[i]);

	// kill thread
	if (it.t) 
	{
		if (!tb_thread_wait(it.t, 5000))
			tb_thread_kill(it.t);
		tb_thread_exit(it.t);
	}

	// exit event
	for (i = 0; i < TB_TEST_ITEM_MAX; i++) if (it.e[i]) tb_event_exit(it.e[i]);

	tb_print("quit");
	tb_exit();
	return 0;
}
