/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_memory_scoped_buffer_main(tb_int_t argc, tb_char_t** argv)
{
    tb_scoped_buffer_t b;
    tb_scoped_buffer_init(&b);

    tb_scoped_buffer_memncpy(&b, (tb_byte_t const*)"hello ", 6);
    tb_scoped_buffer_memncat(&b, (tb_byte_t const*)"world", 6);
    tb_trace_i("%s", tb_scoped_buffer_data(&b));

    tb_scoped_buffer_exit(&b);

    return 0;
}
