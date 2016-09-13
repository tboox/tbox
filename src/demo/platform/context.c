/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */ 

// the contexts
static tb_byte_t        g_context0_data[8192];
static tb_byte_t        g_context1_data[8192];
static tb_byte_t        g_context2_data[8192];
static tb_byte_t        g_context1_stack[8192];
static tb_byte_t        g_context2_stack[8192];
static tb_context_ref_t g_context0 = tb_null;
static tb_context_ref_t g_context1 = tb_null;
static tb_context_ref_t g_context2 = tb_null;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */ 
static tb_void_t tb_demo_platform_context_func1(tb_uint32_t priv_hi, tb_uint32_t priv_lo)
{
    // check
    tb_cpointer_t priv = tb_context_priv(priv_hi, priv_lo);
    tb_assert_and_check_return(priv);

    // trace
    tb_trace_i("%s: enter", (tb_char_t const*)priv);

    // switch to the context2
    tb_context_swap(g_context1, g_context2);

    // trace
    tb_trace_i("%s: leave", (tb_char_t const*)priv);

    // switch to the context0
    tb_context_switch(g_context0);
}
static tb_void_t tb_demo_platform_context_func2(tb_uint32_t priv_hi, tb_uint32_t priv_lo)
{
    // check
    tb_cpointer_t priv = tb_context_priv(priv_hi, priv_lo);
    tb_assert_and_check_return(priv);

    // trace
    tb_trace_i("%s: enter", (tb_char_t const*)priv);

    // switch to the context1
    tb_context_swap(g_context2, g_context1);

    // trace
    tb_trace_i("%s: leave", (tb_char_t const*)priv);

    // switch the context1
    tb_context_switch(g_context1);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_platform_context_main(tb_int_t argc, tb_char_t** argv)
{
    // init contexts
    g_context0 = tb_context_init(g_context0_data, sizeof(g_context0_data));
    g_context1 = tb_context_init(g_context1_data, sizeof(g_context1_data));
    g_context2 = tb_context_init(g_context2_data, sizeof(g_context2_data));

    // make context1
    tb_context_make(g_context1, g_context1_stack, sizeof(g_context1_stack), tb_demo_platform_context_func1, "context1");

    // make context2
    tb_context_make(g_context2, g_context2_stack, sizeof(g_context2_stack), tb_demo_platform_context_func2, "context2");

    // trace
    tb_trace_i("context0: enter");

    // switch to context2
    tb_context_swap(g_context0, g_context2);

    // trace
    tb_trace_i("context0: leave");

    // exit contexts
    tb_context_exit(g_context0);
    tb_context_exit(g_context1);
    tb_context_exit(g_context2);
    return 0;
}
