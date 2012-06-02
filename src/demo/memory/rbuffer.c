/* ///////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "tbox.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */ 
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	tb_rbuffer_t b1;
	tb_rbuffer_t b2;
	tb_rbuffer_t b3;
	tb_rbuffer_init(&b1);
	tb_rbuffer_init(&b2);
	tb_rbuffer_init(&b3);

	tb_rbuffer_memncpy(&b1, "hello", 6);
	tb_rbuffer_memnsetp(&b1, 5, ' ', 1);
	tb_rbuffer_memncat(&b1, "world", 6);
	tb_print("b1: [%u]: %s", tb_rbuffer_refn(&b1), tb_rbuffer_data(&b1));
	tb_print("b2: [%u]: %s", tb_rbuffer_refn(&b2), tb_rbuffer_data(&b2));
	tb_print("b3: [%u]: %s", tb_rbuffer_refn(&b3), tb_rbuffer_data(&b3));
	tb_print("");

	tb_rbuffer_memcat(&b2, &b1);
	tb_print("b1: [%u]: %s", tb_rbuffer_refn(&b1), tb_rbuffer_data(&b1));
	tb_print("b2: [%u]: %s", tb_rbuffer_refn(&b2), tb_rbuffer_data(&b2));
	tb_print("b3: [%u]: %s", tb_rbuffer_refn(&b3), tb_rbuffer_data(&b3));
	tb_print("");

	tb_rbuffer_memcpy(&b3, &b2);
	tb_print("b1: [%u]: %s", tb_rbuffer_refn(&b1), tb_rbuffer_data(&b1));
	tb_print("b2: [%u]: %s", tb_rbuffer_refn(&b2), tb_rbuffer_data(&b2));
	tb_print("b3: [%u]: %s", tb_rbuffer_refn(&b3), tb_rbuffer_data(&b3));
	tb_print("");

	tb_rbuffer_exit(&b1);
	tb_print("b1: [%u]: %s", tb_rbuffer_refn(&b1), tb_rbuffer_data(&b1));
	tb_print("b2: [%u]: %s", tb_rbuffer_refn(&b2), tb_rbuffer_data(&b2));
	tb_print("b3: [%u]: %s", tb_rbuffer_refn(&b3), tb_rbuffer_data(&b3));
	tb_print("");

	tb_rbuffer_exit(&b2);
	tb_print("b1: [%u]: %s", tb_rbuffer_refn(&b1), tb_rbuffer_data(&b1));
	tb_print("b2: [%u]: %s", tb_rbuffer_refn(&b2), tb_rbuffer_data(&b2));
	tb_print("b3: [%u]: %s", tb_rbuffer_refn(&b3), tb_rbuffer_data(&b3));
	tb_print("");

	tb_rbuffer_exit(&b3);	
	tb_print("b1: [%u]: %s", tb_rbuffer_refn(&b1), tb_rbuffer_data(&b1));
	tb_print("b2: [%u]: %s", tb_rbuffer_refn(&b2), tb_rbuffer_data(&b2));
	tb_print("b3: [%u]: %s", tb_rbuffer_refn(&b3), tb_rbuffer_data(&b3));
	tb_print("");

	return 0;
}
