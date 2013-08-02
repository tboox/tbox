/* ///////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "tbox.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t test_cc_main(tb_int_t argc, tb_char_t** argv);
tb_int_t test_cpp_main(tb_int_t argc, tb_char_t** argv);
tb_int_t test_m_main(tb_int_t argc, tb_char_t** argv);
tb_int_t test_mm_main(tb_int_t argc, tb_char_t** argv);

tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// init
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// cpp
	test_cpp_main(argc, argv);

	// cc
	test_cc_main(argc, argv);

#if defined(TB_CONFIG_OS_IOS) || defined(TB_CONFIG_OS_MAC)
	// m
	test_m_main(argc, argv);
	// mm
	test_mm_main(argc, argv);
#endif

	// exit
	tb_exit();
	return 0;
}
