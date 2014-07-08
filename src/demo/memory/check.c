/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"
 
/* //////////////////////////////////////////////////////////////////////////////////////
 * test
 */
tb_void_t tb_demo_overflow(tb_noarg_t);
tb_void_t tb_demo_overflow()
{
    tb_pointer_t data = tb_malloc0(10);
    if (data)
    {
        tb_memset(data, 0, 11);
        tb_free(data);
    }
}
tb_void_t tb_demo_cstring(tb_noarg_t);
tb_void_t tb_demo_cstring()
{
    tb_char_t* data = tb_malloc0_cstr(10);
    if (data)
    {
        tb_memset(data, 'c', 10);
        tb_strlen(data);
        tb_free(data);
    }
}
tb_void_t tb_demo_free2(tb_noarg_t);
tb_void_t tb_demo_free2()
{
    tb_pointer_t data = tb_malloc0(10);
    if (data)
    {
        tb_free(data);
        tb_free(data);
    }
}
tb_void_t tb_demo_leak(tb_noarg_t);
tb_void_t tb_demo_leak()
{
    tb_pointer_t data = tb_malloc0(10);
    tb_used(data);
}
tb_void_t tb_demo_stack(tb_noarg_t);
tb_void_t tb_demo_stack()
{
    __tb_volatile__ tb_size_t   head[10] = {0};
    __tb_volatile__ tb_size_t   data[10] = {0};
    __tb_volatile__ tb_size_t*  p = (__tb_volatile__ tb_size_t*)data;
    *(p + 11) = 0;
    tb_used(head);
}
tb_void_t tb_demo_overlap(tb_noarg_t);
tb_void_t tb_demo_overlap()
{
    tb_pointer_t data = tb_malloc(10);
    if (data)
    {
        tb_memcpy(data, (tb_byte_t const*)data + 1, 5);
        tb_free(data);
    }
}
 
/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_memory_check_main(tb_int_t argc, tb_char_t** argv)
{
    // done 
//  tb_demo_leak();
//  tb_demo_free2();
    tb_demo_stack();
//  tb_demo_overflow();
//  tb_demo_cstring();
//  tb_demo_overlap();

    return 0;
}
