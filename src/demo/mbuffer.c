/* /////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "tbox.h"

/* /////////////////////////////////////////////////////////////////////////
 * macros
 */ 
int main(int argc, char** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	tb_mbuffer_t b1;
	tb_mbuffer_t b2;
	tb_mbuffer_t b3;
	tb_mbuffer_init(&b1);
	tb_mbuffer_init(&b2);
	tb_mbuffer_init(&b3);

	tb_mbuffer_memncpy(&b1, "hello", 6);
	tb_mbuffer_memnsetp(&b1, 5, ' ', 1);
	tb_mbuffer_memncat(&b1, "world", 6);
	tb_print("b1: [%u]: %s", tb_mbuffer_refn(&b1), tb_mbuffer_data(&b1));
	tb_print("b2: [%u]: %s", tb_mbuffer_refn(&b2), tb_mbuffer_data(&b2));
	tb_print("b3: [%u]: %s", tb_mbuffer_refn(&b3), tb_mbuffer_data(&b3));
	tb_print("");

	tb_mbuffer_memcat(&b2, &b1);
	tb_print("b1: [%u]: %s", tb_mbuffer_refn(&b1), tb_mbuffer_data(&b1));
	tb_print("b2: [%u]: %s", tb_mbuffer_refn(&b2), tb_mbuffer_data(&b2));
	tb_print("b3: [%u]: %s", tb_mbuffer_refn(&b3), tb_mbuffer_data(&b3));
	tb_print("");

	tb_mbuffer_memcpy(&b3, &b2);
	tb_print("b1: [%u]: %s", tb_mbuffer_refn(&b1), tb_mbuffer_data(&b1));
	tb_print("b2: [%u]: %s", tb_mbuffer_refn(&b2), tb_mbuffer_data(&b2));
	tb_print("b3: [%u]: %s", tb_mbuffer_refn(&b3), tb_mbuffer_data(&b3));
	tb_print("");

	tb_mbuffer_exit(&b1);
	tb_print("b1: [%u]: %s", tb_mbuffer_refn(&b1), tb_mbuffer_data(&b1));
	tb_print("b2: [%u]: %s", tb_mbuffer_refn(&b2), tb_mbuffer_data(&b2));
	tb_print("b3: [%u]: %s", tb_mbuffer_refn(&b3), tb_mbuffer_data(&b3));
	tb_print("");

	tb_mbuffer_exit(&b2);
	tb_print("b1: [%u]: %s", tb_mbuffer_refn(&b1), tb_mbuffer_data(&b1));
	tb_print("b2: [%u]: %s", tb_mbuffer_refn(&b2), tb_mbuffer_data(&b2));
	tb_print("b3: [%u]: %s", tb_mbuffer_refn(&b3), tb_mbuffer_data(&b3));
	tb_print("");

	tb_mbuffer_exit(&b3);	
	tb_print("b1: [%u]: %s", tb_mbuffer_refn(&b1), tb_mbuffer_data(&b1));
	tb_print("b2: [%u]: %s", tb_mbuffer_refn(&b2), tb_mbuffer_data(&b2));
	tb_print("b3: [%u]: %s", tb_mbuffer_refn(&b3), tb_mbuffer_data(&b3));
	tb_print("");

	return 0;
}
