/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */ 

// the stacks
static tb_byte_t g_stack1[8192];
static tb_byte_t g_stack2[8192];

// the contexts
static tb_byte_t        g_contexts[8192];
static tb_context_ref_t g_context0 = (tb_context_ref_t)(g_contexts);
static tb_context_ref_t g_context1 = (tb_context_ref_t)(g_contexts + 512);
static tb_context_ref_t g_context2 = (tb_context_ref_t)(g_contexts + 1024);

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */ 
static tb_void_t tb_demo_platform_context_func1(tb_uint32_t priv_hi, tb_uint32_t priv_lo)
{
    // check
    tb_cpointer_t priv = tb_context_priv(priv_hi, priv_lo);
    tb_assert_and_check_return(priv);

    // trace
    tb_trace_i("func1: enter: %s", (tb_char_t const*)priv);

    // swap it
    tb_context_swap(g_context1, g_context2);

    // trace
    tb_trace_i("func1: leave: %s", (tb_char_t const*)priv);
}
static tb_void_t tb_demo_platform_context_func2(tb_uint32_t priv_hi, tb_uint32_t priv_lo)
{
    // check
    tb_cpointer_t priv = tb_context_priv(priv_hi, priv_lo);
    tb_assert_and_check_return(priv);

    // trace
    tb_trace_i("func2: enter: %s", (tb_char_t const*)priv);

    // swap it
    tb_context_swap(g_context2, g_context1);

    // trace
    tb_trace_i("func2: leave: %s", (tb_char_t const*)priv);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_platform_context_main(tb_int_t argc, tb_char_t** argv)
{
    // check
    tb_assert_and_check_return_val(tb_context_size() < 512, -1);

    // make context1
    tb_context_get(g_context1);
    tb_context_make(g_context1, g_context0, g_stack1, sizeof(g_stack1), tb_demo_platform_context_func1, "context1");

    // make context2
    tb_context_get(g_context2);
    tb_context_make(g_context2, g_context1, g_stack2, sizeof(g_stack2), tb_demo_platform_context_func2, "context2");

    // swap it
    tb_context_swap(g_context0, g_context2);
    return 0;
}
