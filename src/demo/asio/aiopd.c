/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_DEMO_FILE_READ_MAXN 			(1 << 16)

/* ///////////////////////////////////////////////////////////////////////
 * types
 */
typedef struct __tb_demo_context_t
{
	// the aioo
	tb_aioo_t const* 	aioo;

	// the sock
	tb_handle_t 		sock;

	// the file
	tb_handle_t 		file;

	// the left
	tb_hize_t 			left;

	// the send
	tb_size_t 			send;

	// the real
	tb_size_t 			real;

	// the data
	tb_byte_t* 			data;

}tb_demo_context_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_demo_context_exit(tb_aiop_t* aiop, tb_demo_context_t* context)
{
	if (context)
	{
		// exit aioo
		if (context->aioo) tb_aiop_delo(aiop, context->aioo);
		context->aioo = tb_null;

		// exit sock
		if (context->sock) tb_socket_close(context->sock);
		context->sock = tb_null;

		// exit file
		if (context->file) tb_file_exit(context->file);
		context->file = tb_null;

		// exit data
		if (context->data) tb_free(context->data);
		context->data = tb_null;

		// exit it
		tb_free(context);
	}
}

/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// check
	tb_assert_and_check_return_val(argv[1], 0);

	// init tbox
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// open socket
	tb_handle_t sock = tb_socket_open(TB_SOCKET_TYPE_TCP);
	tb_assert_and_check_goto(sock, end);

	// init aiop
	tb_aiop_t* 	aiop = tb_aiop_init(16);
	tb_assert_and_check_goto(aiop, end);

	// bind 
	if (!tb_socket_bind(sock, tb_null, 9090)) goto end;

	// listen sock
	if (!tb_socket_listen(sock)) goto end;

	// addo sock
	if (!tb_aiop_addo(aiop, sock, TB_AIOE_CODE_ACPT, tb_null)) goto end;

	// accept
	tb_aioe_t list[16];
	while (1)
	{
		// wait
		tb_long_t objn = tb_aiop_wait(aiop, list, 16, -1);
		tb_assert_and_check_break(objn > 0);

		// walk list
		tb_size_t i = 0;
		for (i = 0; i < objn; i++)
		{
			// the aioo 
			tb_aioo_t const* aioo = list[i].aioo;

			// check
			tb_assert_and_check_break(aioo && aioo->handle);

			// acpt?
			if (list[i].code & TB_AIOE_CODE_ACPT)
			{
				// done acpt
				tb_bool_t 			ok = tb_false;
				tb_demo_context_t* 	context = tb_null;
				do
				{
					// make context
					context = tb_malloc0(sizeof(tb_demo_context_t));
					tb_assert_and_check_break(context);

					// init sock
					context->sock = tb_socket_accept(aioo->handle);
					tb_assert_and_check_break(context->sock);

					// init file
					context->file = tb_file_init(argv[1], TB_FILE_MODE_RO);
					tb_assert_and_check_break(context->file);

					// init data
					context->data = tb_malloc(TB_DEMO_FILE_READ_MAXN);
					tb_assert_and_check_break(context->data);

					// addo sock
					context->aioo = tb_aiop_addo(aiop, context->sock, TB_AIOE_CODE_SEND, context);
					tb_assert_and_check_break(context->aioo);

					// trace
					tb_print("acpt[%p]: ok", context->sock);

					// init left
					context->left = tb_file_size(context->file);

					// done read
					tb_long_t real = tb_file_read(context->file, context->data, tb_min(context->left, TB_DEMO_FILE_READ_MAXN));
					tb_assert_and_check_break(real > 0);

					// save size
					context->left -= real;

					// trace
//					tb_print("read[%p]: real: %ld", context->file, real);

					// done send
					context->send = real;
					real = tb_socket_send(context->sock, context->data + context->real, context->send - context->real);
					if (real >= 0)
					{
						// save real
						context->real += real;

						// trace
//						tb_print("send[%p]: real: %ld", context->sock, real);
					}
					else
					{
						// trace
						tb_print("send[%p]: closed", context->sock);
						break;
					}

					// ok
					ok = tb_true;

				} while (0);

				// failed or closed?
				if (!ok)
				{
					// exit context
					tb_demo_context_exit(aiop, context);
					break;
				}
			}
			// writ?
			else if (list[i].code & TB_AIOE_CODE_SEND)
			{
				// the context
				tb_demo_context_t* context = (tb_demo_context_t*)list[i].data;
				tb_assert_and_check_break(context);

				// continue to send it if not finished
				if (context->real < context->send)
				{
					// done send
					tb_long_t real = tb_socket_send(aioo->handle, context->data + context->real, context->send - context->real);
					if (real > 0)
					{
						// save real
						context->real += real;

						// trace
//						tb_print("send[%p]: real: %ld", aioo->handle, real);
					}
					else
					{
						// trace
						tb_print("send[%p]: closed", aioo->handle);

						// exit context
						tb_demo_context_exit(aiop, context);
						break;
					}
				}
				// finished? read file
				else if (context->left)
				{
					// init
					context->real = 0;
					context->send = 0;

					// done read
					tb_size_t tryn = 1;
					tb_long_t real = 0;
					while (!(real = tb_file_read(context->file, context->data, tb_min(context->left, TB_DEMO_FILE_READ_MAXN))) && tryn--);
					if (real > 0)
					{
						// save left
						context->left -= real;

						// trace
//						tb_print("read[%p]: real: %ld", context->file, real);

						// done send
						context->send = real;
						real = tb_socket_send(aioo->handle, context->data, context->send);
						if (real >= 0)
						{
							// save real
							context->real += real;

							// trace
//							tb_print("send[%p]: real: %ld", aioo->handle, real);
						}
						else
						{
							// trace
							tb_print("send[%p]: closed", aioo->handle);

							// exit context
							tb_demo_context_exit(aiop, context);
							break;
						}
					}
					else
					{
						// trace
						tb_print("read[%p]: closed", aioo->handle);

						// exit context
						tb_demo_context_exit(aiop, context);
						break;
					}
				}
				else 
				{
					// trace
					tb_print("read[%p]: closed", aioo->handle);

					// exit context
					tb_demo_context_exit(aiop, context);
					break;
				}
			}
			// error?
			else 
			{
				tb_print("aioe[%p]: unknown code: %lu", aioo->handle, list[i].code);
				break;
			}
		}
	}

end:

	// trace
	tb_print("end");

	// exit socket
	if (sock) tb_socket_close(sock);

	// exit aiop
	if (aiop) tb_aiop_exit(aiop);

	// exit
	tb_exit();
	return 0;
}
