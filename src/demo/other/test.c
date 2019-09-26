/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_other_test_main(tb_int_t argc, tb_char_t** argv)
{
#if 0
    tb_byte_t* p = (tb_byte_t*)tb_allocator_malloc(tb_virtual_allocator(), 100 * 1024 * 1024);
    if (p)
    {
        tb_memset(p, 0, 100 * 1024 * 1024);
        tb_getchar();
        tb_allocator_free(tb_virtual_allocator(), p);
    }
#endif
    return 0;
}
